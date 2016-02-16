//
// Created by Rafael Zulli De Gioia Paiva on 27/01/16.
//
#include <opencv2/core/core.hpp>
#include <vector>

#include <FeatureExtractors/QImageCV.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <map>
#include <utility>
#include <opencv2/imgproc.hpp>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "../../SegmentedImage.h"
#include <sys/stat.h>
using namespace cv;
using namespace std;
int DISCRE = 1000;
int DEBUG = true ;



class Segmentation {
private:
    typedef pair<unsigned int, unsigned int> Index;
    typedef map<Index, float> Matrix;

    vector<char> color;
    vector<Vec3b*> listaCor;
    RNG rng;

    //vector<vector< float>> matrixAdj;

    int width, height;
    int size;
    QImageCV image;
    QString file;
    Mat mask;
    float threshold;
public:

    Mat getMask()
    {
        return mask;
    }
    Matrix matrixAdj;
    void dfs_visit(int i)
    {
        color[i]='g';
        //   cout << "Setting " << i << " to gray" << endl;
        int top = (i-width)<0?-1:i-width;
        int down = (i+width) >= height*width ? -1 : i+width;
        int left = (i-1)<0?-1:i-1;
        int right = (i+1)>= width*height  ?-1:i+1;

        for (int j=0; j<4;j++)
        {
            int k=-1;
            switch(j)
            {
                case 0:
                    k= right;
                    break;
                case 1:
                    k=down;
                    break;
                case 2:
                    k=top;
                    break;
                case 3:
                    k=left;
                    break;


            }
            if(k==-1)
                continue;

            //cout << "Checking " << V[i].adj[j] << " color" << endl;
            if(color[k]=='w' && findWeight(k,i)>=threshold)
                dfs_visit(k);

        }
        color[i]='b';
        //cout << "Setting " << i << " to BLACK" << endl;
    }
    void showImageMask(const string s)
    {
        //cout<<mask.cols<<image.getCvBGRImage().cols<< " "<<image.getCvBGRImage().channels()<<endl;
        //Mat tm(image.height(),image.width(),CV_8UC3);
        //cvtColor(mask,tm,CV_GRAY2RGB);
        Mat temp = (mask+image.getCvBGRImage())*0.5;// (image.getCvBGRImage()*0.5);// + (image.getCvBGRImage()*0.5);

        imshow(s,mask);
        imshow("oi",image.getCvBGRImage());

    }
    void setWeight(Matrix *m,unsigned int x, unsigned int y,float amount)
    {
        (*m)[Index(x,y)] = (*m)[Index(y,x)] = amount;
    }
    void setWeight(unsigned int x, unsigned int y,float amount)
    {
        matrixAdj[Index(x,y)] = matrixAdj[Index(y,x)] = amount;
    }
    float findWeight(unsigned int x, unsigned int y)
    {
        Matrix::iterator a = matrixAdj.find(Index(x,y));

        if(a != matrixAdj.end())
            return a->second;

        return 0;

    }
    Segmentation(SegmentedImage im, float threshold,vector<Vec3b*>&listaCor)
    {

        this->listaCor = listaCor;

        this->file = im.getImagePath();
        init();

        this->threshold = threshold;

        Mat out;
        resize(im.getMask(), out, Size(), 0.25,0.25);
        makeRegions(out);
        makeMask();
    }
    Segmentation(QString file, Matrix matrixAdj,float threshold,vector<Vec3b*>&listaCor)
    {
        this->matrixAdj = matrixAdj;
        this->listaCor = listaCor;
        this->file = file;
        init();
        this->threshold = threshold;
        makeMask();
    }
    Segmentation(QString file, float threshold,vector<Vec3b*>&listaCor) {

        this->file = file;
        this->listaCor = listaCor;
        init();


        this->threshold = threshold;
        makeRegions();
        makeMask();

    }
    void init()
    {
        rng = RNG(time(NULL));
        Mat a = imread(file.toStdString().c_str());

        //Mat out(a.rows/4, a.cols/4, CV_8UC3);
        Mat out;
        resize(a, out, Size(), 0.25,0.25);

        this->image = QImageCV(out);



        mask = Mat(image.height(), image.width(), CV_8UC3);
        mask = Scalar::all(0.0);


        width = image.height();
        height = image.width();

        color.resize(width*height);

        size = width * height;
        //cout << width << " " << height << " " << size << endl;



    }
    void makeRegions(Mat m) {

        for(int y =0;y<height;y++)
        {
            for(int x =0;x<width;x++)
            {

                Vec3b cor = m.at<Vec3b>(x,y);
                int i= x + y*width;
                int top = (i-width)<0?-1:i-width;
                int down = (i+width) >= height*width ? -1 : i+width;
                int left = (i-1)<0?-1:i-1;
                int right = (i+1)>= width*height  ?-1:i+1;

                Vec3b corViz;
                for (int j=0; j<4;j++) {
                    int k = -1;
                    switch (j) {
                        case 0:
                            k = right;
                            break;
                        case 1:
                            k = down;
                            break;
                        case 2:
                            k = top;
                            break;
                        case 3:
                            k = left;
                            break;


                    }
                    if (k == -1)
                        continue;

                    switch (j) {
                        case 0:
                            corViz = m.at<Vec3b>(x+1,y);
                            break;
                        case 1:
                            corViz = m.at<Vec3b>(x,y+1);
                            break;
                        case 2:
                            corViz = m.at<Vec3b>(x,y-1);
                            break;
                        case 3:
                            corViz = m.at<Vec3b>(x-1,y);
                            break;


                    }


                    if(cor == corViz)
                        setWeight(i,k,1.0);
                    else
                        setWeight(i,k,0.0);

                }
            }
        }
    }
    void makeRegions() {

        for(int i=0;i<width* height;i++)
        {
            int top = (i-width)<0?-1:i-width;
            int down = (i+width) >= width* height ? -1 : i+width;
            int left = (i-1)<0?-1:i-1;
            int right = (i+1)>= height*width?-1:i+1;


            if(top != -1)
            {
                setWeight(i,top,rng.uniform((double)0,(double)1));//((float)(rand()%DISCRE)/DISCRE);
            }
            if(left != -1)
            {
                setWeight(i,left,rng.uniform((double)0,(double)1));//((float)(rand()%DISCRE)/DISCRE);
            }
            if(down != -1)
            {
                setWeight(i,down,rng.uniform((double)0,(double)1));//((float)(rand()%DISCRE)/DISCRE);
            }
            if(right != -1)
            {
                setWeight(i,right,rng.uniform((double)0,(double)1));//((float)(rand()%DISCRE)/DISCRE);
            }

        }

    }
    void makeMask()
    {
        int corIdx=0;
        for(int i=0;i<size;i++)
        {
            color[i]='w';
        }
        int j=0;
        for(int i=0;i<size;i++)
        {
            if(i>=1)
            {
                if(listaCor.size() > corIdx)
                {
                    if(listaCor[corIdx] == NULL)
                    {

                        listaCor[corIdx] = new Vec3b(rand()%254+1,rand()%254+1,rand()%254+1);

                    }
                } else
                {
                    listaCor.resize(corIdx+1);
                    listaCor[corIdx] = new Vec3b(rand()%254+1,rand()%254+1,rand()%254+1);
                }
               // listaCor[corIdx] = Vec3b(rand()%254+1,rand()%254+1,rand()%254+1);


                Vec3b * cor = (listaCor[corIdx]);
                corIdx++;
                //cout<<"Connected component "<<i-1<<" :";

                for(int i=0;i<size;i++)
                {
                    if(color[i]=='b'){
                        int x=i%width,y=i/width;
                        mask.at<Vec3b>(x,y)= *cor;

                        color[i]='z';
                    }
                }
                //cout<<"\n";
            }
            if(color[i]=='w') {

                dfs_visit(i);j++;
            }
        }
        //cout<<j<<endl;

    }
    void print()
    {
        for (int i = 0; i < width*height; i++) {
            for (int j = 0; j < width * height; j++) {

                cout << findWeight(i,j) << "\t";
            }
            cout<<endl;
        }


    }

    Segmentation interpolate(Segmentation objetivo, float amount)
    {

        Matrix novo;
        for(int i=0;i<size;i++) {

            int top = (i - width) < 0 ? -1 : i - width;
            int down = (i + width) >= width * height ? -1 : i + width;
            int left = (i - 1) < 0 ? -1 : i - 1;
            int right = (i + 1) >= height * width ? -1 : i + 1;
            float w1 = 0, w2 = 0, dif = 0;
            if (down != -1) {
                w1 = matrixAdj[Index(i, down)];
                w2 = objetivo.matrixAdj[Index(i, down)];

                dif = (w2 - w1) * amount;
                // cout<<i<<" "<<j<<" "<<w1<<" "<<w2<<""<<endl;
                setWeight(&novo, i, down, w1 + dif);
            }

            if (right != -1) {

                w1 = matrixAdj[Index(i, right)];
                w2 = objetivo.matrixAdj[Index(i, right)];

                dif = (w2 - w1) * amount;
                // cout<<i<<" "<<j<<" "<<w1<<" "<<w2<<""<<endl;
                setWeight(&novo, i, right, w1 + dif);
            }

            if (top != -1) {
                w1 = matrixAdj[Index(i, top)];
                w2 = objetivo.matrixAdj[Index(i, top)];

                dif = (w2 - w1) * amount;
                // cout<<i<<" "<<j<<" "<<w1<<" "<<w2<<""<<endl;
                setWeight(&novo, i, top, w1 + dif);

            }
            if(left != -1) {

                w1 = matrixAdj[Index(i, left)];
                w2 = objetivo.matrixAdj[Index(i, left)];

                dif = (w2 - w1) * amount;
                // cout<<i<<" "<<j<<" "<<w1<<" "<<w2<<""<<endl;
                setWeight(&novo, i, left, w1 + dif);
            }




        }
        return Segmentation(file,novo,threshold,listaCor);
    }
    float compara(Segmentation b, float tol)
    {
        float igual = 0;
        float total = 0;
        for(int i=0;i<size;i++) {

                int top = (i-width)<0?-1:i-width;
                int down = (i+width) >= width* height ? -1 : i+width;
                int left = (i-1)<0?-1:i-1;
                int right = (i+1)>= height*width?-1:i+1;


                if (top!=-1)
                {
                    total++;
                    if (abs(b.findWeight(i, top) - findWeight(i, top))< tol)
                        igual++;
                }
                if (down!=-1)
                {
                    total++;
                    if (abs(b.findWeight(i, down) - findWeight(i, down))< tol)
                        igual++;
                }
                if (right!=-1)
                {
                    total++;
                    if (abs(b.findWeight(i, right) - findWeight(i, right))< tol)
                        igual++;
                }
                if (left!=-1) {
                    total++;
                    if (abs(b.findWeight(i, left) - findWeight(i, left))<tol)
                        igual++;
                }

        }

        return igual/total;
    }

};


int main()
{
    string exp;
    string base = "/Users/zulli/Documents/exp/";
    cout<< "Nome do experimento: ";
    getline(cin,exp);

    exp += "/";
    string path = base+exp;

    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    vector<  Vec3b* > listaCor = vector<Vec3b*>(0);
    srand (time(NULL));


    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    QString f(filename);
    SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");



    for(float t = 0.0;t<=1.1;t+=0.1)
    {
        string st = to_string(t);
        mkdir((path+st).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        Segmentation j(im2, t,listaCor);
        j.showImageMask(st+" objetivo ");

        //Segmentation a(f, 0.8,listaCor);
        //a.showImageMask("Objetivo");

        imwrite(path+st+"/objetivo.png",j.getMask());
        Segmentation b(f, t,listaCor);
        b.showImageMask(st+"Atual");
        imwrite(path+st+"/inicial.png",b.getMask());
        for(int i=1;i<=20;i++)
        {
            float amount = i/20.0;
            string c = to_string(amount);

            Segmentation d = b.interpolate(j,amount);

            d.showImageMask(st+" - "+c);
            float por = d.compara(j,1.0/DISCRE)*100;
            cout<<st<<" Objetivo x "<<amount<<" - "<<por<<"% igual"<<endl;
            imwrite(path+st+"/"+c+" - "+ to_string(por)+".png",d.getMask());
        }
    }
    waitKey(0);
    return 0;
}


/*class Pixel
{
public:
    float left =-1,right=-1,down=-1,top=-1;
};


class Segmentation
{
private:
    vector<vector<Pixel>> pixels;
    int width,height;
    int size;
    QImageCV image;
    Mat mask;
    float threshold;
public:
    Segmentation(QString file, float threshold)
    {
        this->image = QImageCV(file);
        this->image.load(file);

        mask =  Mat(image.height(),image.width(),CV_8UC4);
        mask = Scalar::all(0.0);


        width = image.height();
        height = image.width();

        size = width*height*4;
        cout<< width<<" "<<height<<" "<<size<<endl;

        makeRegions();
        this->threshold = threshold;
        //makeMask();
    }

    void makeRegions()
    {

        for (int y = 0; y < height; y++)
        {
            for(int x=0;x<width; x++) {

                if (pixels[x][y].top == -1) {
                    if(y>0)
                    {
                        if (pixels[x][y - 1].down == -1) {
                            pixels[x][y - 1].down = pixels[x][y].top = ((float)(rand() % DISCRE) / DISCRE);
                        } else
                        {
                            pixels[x][y].top = pixels[x][y-1].down;
                        }
                    } else
                    {
                        pixels[x][y ].top = ((float)(rand() % DISCRE) / DISCRE);
                    }
                }

                if (pixels[x][y].down == -1) {
                    if(y<height)
                    {
                        if (pixels[x][y + 1].top == -1) {
                            pixels[x][y + 1].top = pixels[x][y].down = ((float)(rand() % DISCRE) / DISCRE);
                        } else
                        {
                            pixels[x][y].down = pixels[x][y+1].top;
                        }
                    } else
                    {
                        pixels[x][y].top = ((float)(rand() % DISCRE) / DISCRE);
                    }
                }

                if (pixels[x][y].left == -1) {
                    if(x>0)
                    {
                        if (pixels[x-1][y].right == -1) {
                            pixels[x-1][y].right = pixels[x][y].left = ((float)(rand() % DISCRE) / DISCRE);
                        } else
                        {
                            pixels[x][y].left = pixels[x][y].right;
                        }
                    } else
                    {
                        pixels[x][y].left = ((float)(rand() % DISCRE) / DISCRE);
                    }
                }
                if (pixels[x][y].right == -1) {
                    if(x<width)
                    {
                        if (pixels[x+1][y].left == -1) {
                            pixels[x+1][y].left = pixels[x][y].right = ((float)(rand() % DISCRE) / DISCRE);
                        } else
                        {
                            pixels[x][y].right = pixels[x+1][y].left;
                        }
                    } else
                    {
                        pixels[x][y].right = ((float)(rand() % DISCRE) / DISCRE);
                    }
                }


            }
        }
    }
    void makeMask(){


        for (int y = 0; y < height; y++) {
            for(int x=0;x<width; x++) {
                int i = y * width + x;
                //int x = i % width, y = (int) floor(i / height);
                if (DEBUG)
                    cout << x << " " << y;

                if(pixels[x][y].left > threshold || pixels[x][y].top > threshold)
                {
                    mask.at<Vec4b>(x, y) = mask.at<Vec4b>(x, y)
                }



                if (mask.at<Vec4b>(x, y) == Vec4b(0,0,0)) {
                    mask.at<Vec4b>(x, y) = Vec4b((rand() % 254) + 1, (rand() % 254) + 1, (rand() % 254) + 1, 255);

                }
                if (DEBUG)
                    cout << " - " << mask.at<Vec4b>(x, y);
                int dX = 0, dY = 0;
                if (DEBUG)
                    cout << " (";

                for (int pI = 0; pI < 4; pI++) {
                    switch (pI) {
                        case 0:
                            dX = 0;
                            dY = -1;
                            break;
                        case 1:
                            dX = -1;
                            dY = 0;
                            break;
                        case 2:
                            dX = 1;
                            dY = 0;
                            break;
                        case 3:
                            dX = 0;
                            dY = 1;
                            break;

                    }
                    if (DEBUG)
                        cout << "[dx: "<<dX<<" dy: "<<dY<<" "<<pixels[i * 4 + pI] << "] ";
                    if (pixels[i pI] >= threshold) {
                        if (x + dX > 0 &&
                                x + dX < width &&
                                y + dY > 0 &&
                                y + dY < height)
                            mask.at<Vec4b>(x + dX, y + dY) = mask.at<Vec4b>(x, y);
                    }

                };
                if (DEBUG) {
                    cout << " )";
                    cout << endl;
                }

            }
        }
    }

    void showImageMask(const string s)
    {
        cout<<mask.cols<<image.getCvBGRImage().cols<< " "<<image.getCvBGRImage().channels()<<endl;
        //Mat tm(image.height(),image.width(),CV_8UC3);
        //cvtColor(mask,tm,CV_GRAY2RGB);
        Mat temp = (mask+image.getCvBGRImage())*0.5;// (image.getCvBGRImage()*0.5);// + (image.getCvBGRImage()*0.5);

        imshow(s,mask);
        imshow("oi",temp);
        waitKey(0);
    }

};

int main()
{
    srand (time(NULL));
    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    QString f(filename);


    Segmentation a(f, -1);
    a.showImageMask("alo");
    return 0;
}

*/