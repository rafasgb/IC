//
// Created by Rafael Zulli De Gioia Paiva on 16/12/15.
//
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
#include "../../WatershedImage.h"
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
#include "../../SegmentedImage.h"
#include "../../SegmentationEvaluator.h"
#include "../../FireflyOptimizator.h"
#include <math.h>
using namespace std;

bool SortbyXaxis(const Point3i & a, const Point3i &b)
{
    return a.x < b.x;
}

double pointDis(Point2i point, Point2i other)
{
    return sqrt( (point.x - other.x) *(point.x - other.x) +
                 (point.y - other.y) *(point.y - other.y));
}

int findClosestPoint(Point3i pointTemp, vector< Point3i > pointList)
{

    Point2i point(pointTemp.x,pointTemp.y);
    int closest= -1,i=0;
    double closestDis = 1<<15;

    for( Point3i otherTemp : pointList)
    {
        Point2i other(otherTemp.x,otherTemp.y);
        double dis = pointDis(point,other);
        //cout<<dis<<endl;
        if(dis < closestDis){
            closest = i;
            closestDis =  dis;
        }
        i++;
    }

    return closest;

}


vector< pair< Point2i , Point2i > > findMatch(WatershedImage im1, WatershedImage im2) {

    vector< Point3i > points1  = im1.getMarkers();
    vector< Point3i > points2  = im2.getMarkers();

    sort(points1.begin(),points1.end(),SortbyXaxis);
    sort(points1.begin(),points1.end(),SortbyXaxis);

    vector< pair< Point2i, Point2i > > result;


    //cout<<points1.size()<<" "<<points2.size()<<endl;
    for( Point3i p1 : points1) {
        int winnerPos = findClosestPoint(p1,points2);
        //cout<<winnerPos<<endl;
        Point2i a(p1.x,p1.y),b(points2[winnerPos].x,points2[winnerPos].y);
        result.push_back(make_pair(a,b));
        //points2.erase(points2.begin()+winnerPos);

    }

    return result;

}
Point2i interpolate(pair<Point2i,Point2i> p, float amount)
{
    Point2i p1 = p.first;
    Point2i p2 = p.second;

    double angle = atan2(p1.y-p2.y,p1.x-p2.x);
    double dis = pointDis(p1,p2);
    cout<<dis<<" "<<angle<<" "<<max(dis*amount,1.0)<<" "<< cos(angle)<< " "<<sin(angle)<<endl;
    if(dis<3.0)
        p2 = p1;
    else {
        double cosAngle = cos(angle);

        int cosSign = (cosAngle)<0?-1:1;

        double sinAngle = sin(angle);

        int sinSign = (sinAngle)<0?-1:1;

        p2.x = (int) (p2.x + cosSign * ( round(abs(cosAngle ) * max(dis * amount, 1.0) )));
        p2.y = (int) (p2.y + sinSign * ( round(abs(sinAngle ) * max(dis * amount, 1.0) )));
    }

    return p2;
}

int main()
{

    FireflyOptimizator f(QString("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg"),QString("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml"),5,10,100,0.0015);
    f.run();

    return 0;
}

int moveToward()
{
    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    WatershedImage c(QString(filename),200);
    WatershedImage c2(QString(filename),200);
    c.watershed(false);
    c2.watershed(false);

    Mat img(c.getMaskC1().rows,c.getMaskC1().cols,CV_8UC1);

    for(int i=0;i<100;i++)
    {
        img = Scalar::all(0);

        for( Point3i p1 : c.getMarkers()) {
            circle(img,Point2i(p1.x,p1.y),3,255);
        }
        for( Point3i p1 : c2.getMarkers()) {
            circle(img,Point2i(p1.x,p1.y),3,127);
        }
        c.watershed(false);
        c2.watershed(false);

        imshow("wshed",img);
        imshow("gt",c.getMaskC1());
        imshow("pd",c2.getMaskC1());
        waitKey(1);

        vector< Point3i > markerC , markerC2;


        vector< pair< Point2i , Point2i > >  vec = findMatch(c,c2);
        for( pair<Point2i, Point2i> p  : vec)
        {
            p.second = interpolate(p,0.0002);

            markerC.push_back(Point3i(p.first.x,p.first.y,0));
            markerC2.push_back(Point3i(p.second.x,p.second.y,0));


        }
        c = WatershedImage(QString(filename),markerC);
        c2 = WatershedImage(QString(filename),markerC2);

    }


    return 0;
}

void testElapsedTimeRegion()
{


    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";

   //SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");


   // imshow("ground thruth",im2.getMask());


    vector< WatershedImage > vc;
    clock_t begin,end;
    double elapsed_secs;

    double wsCreate=0,ws=0,segCreate=0,segEvaluate=0;


    double max = -10.0;
    int size =1000;
    for(int i=990;i<size;i++)
    {

        begin = clock();
        WatershedImage c(QString(filename),i);
        WatershedImage c2(QString(filename),i);
        end = clock();
        cout<< "for "<<i<<" regions create() "<< double(end - begin) / CLOCKS_PER_SEC<<endl;


        wsCreate += double(end - begin) / CLOCKS_PER_SEC;

        begin = clock();
        c.watershed(false);
        c2.watershed(false);
        end = clock();
        cout<< "for "<<i<<" regions watershed() "<< double(end - begin) / CLOCKS_PER_SEC<<endl;

        imshow("wshed",c.getMaskC1());
        imshow("wshed2",c2.getMaskC1());
        waitKey(1000);
        ws += double(end - begin) / CLOCKS_PER_SEC;

        //------

        //cout<<i<<endl;
        //------

        begin = clock();
        findMatch(c,c2);
        end = clock();

        segCreate += double(end - begin) / CLOCKS_PER_SEC;
        cout<< "for "<<i<<" regions findMatch() "<< double(end - begin) / CLOCKS_PER_SEC<<endl;
        cout<<"-------------------------------------------"<<endl<<endl;
    }


    cout<<"wsCreate \t"<<wsCreate/size<<endl<<
    "ws \t"<<ws/size<<endl;

}