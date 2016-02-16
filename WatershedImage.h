//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

#ifndef WATERSHED_WATERSHEDIMAGE_H
#define WATERSHED_WATERSHEDIMAGE_H

#include <QList>
#include <FeatureExtractors/Region.hpp>
#include <QString>
#include <opencv/cv.h>
#include <FeatureExtractors/QImageCV.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
using namespace std;
using namespace cv;

class WatershedImage {

private:

    QImageCV image;
    Mat regionsMaskC1,regionsMaskC3;
    QList<Region> regions;
    vector<Point3i> markers;
    void loadImage(QString);
    void generateRandomMarkers(int);


public:
    WatershedImage(QString, vector<Point3i> mark=vector<Point3i>());
    WatershedImage(QString, int);
    WatershedImage(Mat, vector<Point3i> mark=vector<Point3i>());
    WatershedImage(Mat, int);
    void watershed(bool=false);
    QList<Region>& getRegions();
    Mat getMaskC3();
    Mat getMaskC1();
    vector<Point3i> getMarkers();

};


#endif //WATERSHED_WATERSHEDIMAGE_H
