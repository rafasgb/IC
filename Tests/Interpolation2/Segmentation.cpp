//
// Created by Rafael Zulli De Gioia Paiva on 16/02/16.
//

#include "Segmentation.h"

Segmentation::Segmentation(QString file,Mat mask, float threshold,vector<Vec3b*>& listaCor ):listaCor(listaCor), file(file), threshold(threshold)
{
    init();
    makeRegions(mask);
    makeMask();
}

Segmentation::Segmentation(SegmentedImage im, float threshold,vector<Vec3b*>& listaCor ):listaCor(listaCor), file(im.getImagePath()), threshold(threshold)
{
        init();

        Mat out;
        resize(im.getMask(), out, Size(), 0.25,0.25);
        makeRegions(out);
        makeMask();
}
Segmentation::Segmentation(QString file, Matrix matrixAdj,float threshold,vector<Vec3b*>&listaCor): listaCor(listaCor), file(file), threshold(threshold), matrixAdj(matrixAdj)
{

    init();

    makeMask();
}
Segmentation::Segmentation(QString file, float threshold,vector<Vec3b*>&listaCor) : listaCor(listaCor), file(file), threshold(threshold)
{


    init();


    makeRegions();
    makeMask();

}


///
/// inicia
///

void Segmentation::init()
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

    idRegiao = vector<Vec3b>(size,Vec3b(-1,-1,-1));
    notas = map<int,double >();
    regions = map<int,Region >();
    //cout << width << " " << height << " " << size << endl;



}
///
/// Cria a matriz de adjacencia a partir de uma mascara
///
void Segmentation::makeRegions(Mat m) {

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
                    setWeight(i,k,1.0);//2*threshold);
                else
                    setWeight(i,k,0.0);

            }
        }
    }
}
///
/// cria uma matriz de adjacencia aleatória
///
void Segmentation::makeRegions()
{

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

///
/// cria a mascara a partir da matriz de adjacencia
///
void Segmentation::makeMask()
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


            Vec3b * cor = listaCor[corIdx];



            Mat temp(mask.size(),CV_8UC1);
            temp = Scalar::all(0);
            //cout<<"Connected component "<<i-1<<" :";
            int sz = 0;
            for(int i=0;i<size;i++)
            {
                if(color[i]=='b')
                {
                    idRegiao[i] = *cor;

                    int x=i%width,y=i/width;
                    mask.at<Vec3b>(x,y)= *cor;
                    temp.at<uchar>(x,y) = 255;
                    sz++;
                    color[i]='z';
                }
            }





            Region r(&image, temp);
            if(sz>0)
            {
                corIdx++;
                notas[cor->val[0]+cor->val[1]+cor->val[2]] = 0.0;
                regions[cor->val[0]+cor->val[1]+cor->val[2]] = r;
            }


            //cout<<"\n";
        }
        if(color[i]=='w') {

            dfs_visit(i);j++;
        }
    }
    //cout<<j<<endl;

}

///
/// implementação de DFS para encontrar componentes conexos ao fazer máscara
///
void Segmentation::dfs_visit(int i)
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

///
/// compara duas Segmentations e verifica quão próximas elas estão.
/// tol indica a tolerância da diferença entre duas conexões das imagens
///
float Segmentation::compara(Segmentation b, float tol)
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
///
/// interpola a matriz de adjacência dessa segmentação em direção
/// a interpolação objetivo.
///
/// float amount 0.0 a 1.0
Segmentation Segmentation::interpolate(Segmentation objetivo, float amount)
{
    Matrix novo = interpolateMatrix(objetivo,amount);
    return Segmentation(file,novo,threshold,listaCor);
}

void Segmentation::interpolatePixel(Segmentation *objetivo, Matrix * novo, int i, int vizinho, float amount)
{
    Vec3b corP1 = objetivo->idRegiao[i];
    double nota1 = objetivo->notas[corP1.val[0]+corP1.val[1]+corP1.val[2]];

    Vec3b corP2 = objetivo->idRegiao[vizinho];
    double nota2 = objetivo->notas[corP2.val[0]+corP2.val[1]+corP2.val[2]];

    double notaFinal = (nota1>nota2)?nota1:nota2;

    float w1 = matrixAdj[Index(i, vizinho)];

    float w2 = objetivo->matrixAdj[Index(i, vizinho)];

    float am = amount;// / (1 + ( w2-w1)*( w2-w1));
    double dif = (w2 - w1) * am * notaFinal;

    float peso = (w1+w2)/2.0;
    // cout<<i<<" "<<j<<" "<<w1<<" "<<w2<<""<<endl;
    //setWeight(&novo, i, down, peso);
    setWeight(novo, i, vizinho, w1 + dif);
}


Matrix Segmentation::interpolateMatrix(Segmentation objetivo, float amount)
{

    Matrix novo;
    for(int i=0;i<size;i++) {



        int top = (i - width) < 0 ? -1 : i - width;
        int down = (i + width) >= width * height ? -1 : i + width;
        int left = (i - 1) < 0 ? -1 : i - 1;
        int right = (i + 1) >= height * width ? -1 : i + 1;
        float w1 = 0, w2 = 0, dif = 0, am,peso;
        if (down != -1) {
            interpolatePixel(&objetivo,&novo,i,down,amount);
        }

        if (right != -1) {
            interpolatePixel(&objetivo,&novo,i,right,amount);
        }

        if (top != -1) {
            interpolatePixel(&objetivo,&novo,i,top,amount);
        }
        if(left != -1) {
            interpolatePixel(&objetivo,&novo,i,left,amount);
        }




    }
    return novo;
}

///
/// Printa todas a matriz de adjacência da segmentação
///
void Segmentation::print()
{
    for (int i = 0; i < width*height; i++) {
        for (int j = 0; j < width * height; j++) {

            cout << findWeight(i,j) << "\t";
        }
        cout<<endl;
    }


}
///
/// retorna mascara
///
Mat Segmentation::getMask()
{
    return mask;
}

///
///retorna peso de conexão
///
float Segmentation::findWeight(unsigned int x, unsigned int y)
{
    Matrix::iterator a = matrixAdj.find(Index(x,y));

    if(a != matrixAdj.end())
        return a->second;

    return 0;

}

///
/// seta peso  de conexão na matriz de adjacência
///
void Segmentation::setWeight(unsigned int x, unsigned int y,float amount)
{
    matrixAdj[Index(x,y)] = matrixAdj[Index(y,x)] = amount;
}

///
/// seta peso de conexão em uma matriz de adjacência qualquer
///
void Segmentation::setWeight(Matrix *m,unsigned int x, unsigned int y,float amount)
{
    (*m)[Index(x,y)] = (*m)[Index(y,x)] = amount;
}

///
/// exibe mascara.
///
void Segmentation::showImageMask(const string s)
{

    Mat temp = (mask+image.getCvBGRImage())*0.5;

    imshow(s,mask);

}



map<int,Region> * Segmentation::getRegions()
{
    return &regions;
}
