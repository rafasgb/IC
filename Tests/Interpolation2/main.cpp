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
#include "../../SegmentationEvaluator.h"
#include "FireflyOptimizator.h"
#include "Segmentation.h"
#include <sys/stat.h>

#ifndef GLOBALS
#define GLOBALS
int DISCRE = 1000;
bool DEBUG = true ;
#endif


using namespace cv;
using namespace std;

Segmentation gerarMask(int k,QString flm,float t,vector< Vec3b* > listaCor)
{
    Mat m(80,80,CV_8UC3);
    Vec3b cor(255,0,255);

    for(int i =0;i<80;i++)
        for(int j=0;j<80;j++)
        {
            bool ruido = true;
            switch (k) {
                case 0:
                    if (i < 40 && j < 40)
                        ruido = false;
                    break;
                case 1:
                    if (i >= 40 && j < 40)
                        ruido = false;
                    break;
                case 2:
                    if (i < 40 && j >= 40)
                        ruido = false;
                    break;
                case 3:
                    if (i >= 40 && j >= 40)
                        ruido = false;
                    break;


            }
            if (ruido == true) {
                int a = rand() % 255;
                m.at<Vec3b>(i, j) = Vec3b(a,a, a);
            }
            else
                m.at<Vec3b>(i,j) = cor;

        }
    Segmentation nova(flm,m,t,listaCor);

    Vec3b n;
    switch (k) {
        case 0:
            n = nova.idRegiao[0];
            break;
        case 1:
            n = nova.idRegiao[41];
            break;
        case 2:
            n = nova.idRegiao[6321];
            break;
        case 3:
            n = nova.idRegiao[6399];
            break;

    }
    cout<< n<<endl;

    nova.notas[n.val[0]+n.val[1]+n.val[2]]= 1.0;
    return nova;
}

int main3() {

    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    QString f(filename);
    vector<  Vec3b* > listaCor = vector<Vec3b*>(0);
    srand (time(NULL));

    float t = 0.5;


   /* Segmentation *vec[4];
    for (int img = 1; img <= 4; img++)
    {
        string flm = "/Users/zulli/Documents/exp/Quadrantes/"+to_string(img)+".png";
        Mat mask = imread(flm.c_str());

        vec[img-1] = new Segmentation(QString(flm.c_str()),mask,t,listaCor);
        vec[img-1]->showImageMask("mask : "+to_string(img));
    }
    */
    Segmentation atual(QString("/Users/zulli/Documents/exp/Quadrantes/1.png"), t,listaCor);
    int num = 20;
    float it = 0.3;
    string flm = "/Users/zulli/Documents/exp/Quadrantes/1.png";

    for(int i=0; i<num; i++) {
        cout << i << endl;
        for (int img = 0; img < 4; img++)
        {
            Segmentation nova = gerarMask(img,QString(flm.c_str()),t,listaCor);

            nova.showImageMask("img "+to_string(img));

            cout<<"interpolar - b"<<endl;
            atual = atual.interpolate(nova,it);
            atual.showImageMask("img "+to_string(img)+" it "+to_string(i));
            cout<<"interpolar - e"<<endl;
            waitKey(1);
        }

    }
    atual.showImageMask("img");
    waitKey(0);
    return  0;
}

int main()
{
    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    QString f(filename);
    SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");

    FireflyOptimizator ff("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml",0.5,30,100,0.000000005);
    ff.run();
    return 0;
}

int main2()
{
    string exp,base ;
    cout<< "Root dir: ";
    getline(cin,base);

    if(base=="")
        base = "/Users/zulli/Documents/exp/";

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



    for(float t = 0.1;t<=1.1;t+=0.1)
    {
        string st = to_string(t);
        mkdir((path+st).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        Segmentation objetivo(im2, t,listaCor);
        objetivo.showImageMask(st+" objetivo ");

        //Segmentation a(f, 0.8,listaCor);
        //a.showImageMask("Objetivo");

        imwrite(path+st+"/objetivo.png",objetivo.getMask());

        Segmentation atual(f, t,listaCor);
        atual.showImageMask(st+"Atual");
        imwrite(path+st+"/inicial.png",atual.getMask());
        for(int i=1;i<=20;i++)
        {
            float amount = i/20.0;
            string c = to_string(amount);

            Segmentation d = atual.interpolate(objetivo,amount);

            d.showImageMask(st+" - "+c);
            float por = d.compara(objetivo,1.0/DISCRE)*100;
            SegmentationEvaluator seg;
            float por2 = seg.evaluate(&d,&objetivo)*100;
            cout<<st<<" Objetivo x "<<amount<<" - "<<por<<"% igual"<<endl;
            cout<<"Segmentation evaluator "<<por2<<"%"<<endl;
            imwrite(path+st+"/"+c+" - "+ to_string(por)+" - "+to_string(por2)+".png",d.getMask());


        }
    }
    waitKey(0);
    return 0;
}

