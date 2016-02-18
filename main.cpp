#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cstdio>
#include <iostream>
#include <time.h>
#include <limits>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "WatershedImage.h"
#include <cstdio>
#include <iostream>
#include <time.h>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/core/core.hpp>
#include <Utilities/FeaturesComplexNetwork/FeaturesComplexNetwork.hpp>
#include <FeatureExtractors/LabelFeature.hpp>
#include <Utilities/ComplexNetworkConstructor/ComplexNetworkConstructor.hpp>
#include <Utilities/DatabaseReader/SunDatabaseReader.hpp>
#include <FeatureExtractors/LabelFeatureFactory.hpp>
#include <Utilities/ComplexNetworkConstructor/ComplexNetworkConstructorP.hpp>
#include <FeatureExtractors/HsvFeatureFactory.hpp>
#include <FeatureExtractors/Region.hpp>
#include <FeatureExtractors/QImageCV.hpp>

#include <Utilities/SupervisedImage.hpp>
#include <qset.h>
#include <Utilities/Utils.hpp>
#include <qregularexpression.h>
#include "SegmentedImage.h"
#include "SegmentationEvaluator.h"
#include "OldFireflyOptimizator.h"

using namespace cv;
using namespace std;

int main2()
{
    FireflyOptimizator ff("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml",50,20,25,0.4);

    ff.run();
    cout<<" Best overall : "<<ff.getBestOverall().second<<endl;

    return 0;
}

int main() {

    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";

    SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");


    imshow("ground thruth",im2.getMask());


    vector< WatershedImage > vc;
    clock_t begin,end;
    double elapsed_secs;

    double wsCreate=0,ws=0,segCreate=0,segEvaluate=0;


    double max = -10.0;
    int size =100;
    for(int i=0;i<size;i++)
    {

        begin = clock();
        WatershedImage c(QString(filename),100);
        end = clock();


        wsCreate += double(end - begin) / CLOCKS_PER_SEC;

        begin = clock();
        c.watershed(false);
        end = clock();
        //imshow("wshed",c.getMask()*1000);

        ws += double(end - begin) / CLOCKS_PER_SEC;

        //------
        vc.push_back(c);
        cout<<i<<endl;
        //------

        begin = clock();
        //SegmentationEvaluator a(vc[i].getRegions(),im2.getRegions());
        end = clock();

        segCreate += double(end - begin) / CLOCKS_PER_SEC;

        begin = clock();
        //double vl = a.evaluate();
        end = clock();

        segEvaluate += double(end - begin) / CLOCKS_PER_SEC;


      //  cout<<vl<<endl;
       // if(vl>max)
         //   max=vl;
    }


    cout<<"wsCreate \t"<<wsCreate/size<<endl<<
            "ws \t"<<ws/size<<endl<<
            "segCreate \t"<<segCreate/size<<endl<<
            "segEvaluate \t"<<segEvaluate/size<<endl<<
            "max \t"<<    max;






    int k = waitKey(0);

}
