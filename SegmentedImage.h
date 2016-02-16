//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

#include <Utilities/SupervisedImage.hpp>

#ifndef WATERSHED_SEGMENTEDIMAGE_H
#define WATERSHED_SEGMENTEDIMAGE_H
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

class SegmentedImage : public SupervisedImage {
private:
    Mat regionsMask;

    void parseRegions(bool=false);
public:
    SegmentedImage(QString imagePath, QString supervisedPath);
    QList<Region> getRegions();
    Mat getMask();


};


#endif //WATERSHED_SEGMENTEDIMAGE_H
