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

int main()
{
    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    QString f(filename);
    SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");

    FireflyOptimizator ff("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml",0.7,30,50,0.000000009);
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
            float por2 = seg.evaluate(d.getRegions(),objetivo.getRegions())*100;
            cout<<st<<" Objetivo x "<<amount<<" - "<<por<<"% igual"<<endl;
            cout<<"Segmentation evaluator "<<por2<<"%"<<endl;
            imwrite(path+st+"/"+c+" - "+ to_string(por)+" - "+to_string(por2)+".png",d.getMask());


        }
    }
    waitKey(0);
    return 0;
}

