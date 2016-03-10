//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

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

#ifndef WATERSHED_SEGMENTATIONEVALUATOR_H
#define WATERSHED_SEGMENTATIONEVALUATOR_H

#include "Tests/Interpolation2/Segmentation.h"

class SegmentationEvaluator {

private:
    //vector< pair<Region,float> > evaluation;

    double evaluatePair(Region *,Region *,int);


public:
    SegmentationEvaluator(){};

    double evaluate(Segmentation *pr, Segmentation *gt);



};


#endif //WATERSHED_SEGMENTATIONEVALUATOR_H
