//
// Created by Rafael Zulli De Gioia Paiva on 12/12/15.
//

#ifndef WATERSHED_FIREFLYOPTIMIZATOR_H
#define WATERSHED_FIREFLYOPTIMIZATOR_H


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
#include <qstring.h>
#include "WatershedImage.h"
#include "SegmentedImage.h"

class FireflyOptimizator {


public:
    FireflyOptimizator( QString imagePath,  QString imageXml, int markerNumber, int MaxGenerations, int PopulationSize, float gamma)
            : MaxGenerations(MaxGenerations), PopulationSize(PopulationSize), imagePath(imagePath) , imageXml(imageXml), gamma(gamma),markerNumber(markerNumber) , groundTruth(imagePath, imageXml){ groundTruth = SegmentedImage(imagePath, imageXml); regions = groundTruth.getRegions();init(); }
    void init();
    void run();
    void rankGeneration();
    void createPopulation();
    pair<WatershedImage,double> getGenerationBest();
    pair<WatershedImage,double> getBestOverall();

private:
    SegmentedImage groundTruth;
    QList<Region> regions;
    QString imagePath, imageXml;
    int MaxGenerations;
    int PopulationSize;
    int markerNumber;
    float gamma;
    bool generationRanked,optmized;
    vector<WatershedImage> population;
    vector< pair<WatershedImage,double> > generationRank,generationBest;

    double normalizeDis;
    void showPoints(vector<Point3i> gt, vector<Point3i> pr);

    bool SortbyXaxis(const Point3i & a, const Point3i &b);
    vector<Point3i> moveToward(vector<pair<Point2i, Point2i>> vec, float amount);

    double pointDis(Point2i point, Point2i other);

    int findClosestPoint(Point3i pointTemp, vector< Point3i > pointList);
    vector< pair< Point2i , Point2i > > findMatch(WatershedImage im1, WatershedImage im2);

    Point2i interpolate(pair<Point2i, Point2i> p, float amount);

    //vector<Point3i> moveToward(WatershedImage *from, WatershedImage *to);
    vector< pair< Point2i , Point2i > > findMatch(vector< Point3i > points1,vector< Point3i > points2);

    double getDistance(vector<pair<Point2i, Point2i>> vec);
};

struct {


    bool operator()(const Point3i & a, const Point3i &b) const {
        return a.x < b.x;
    }
} sortByXAxis;

#endif //WATERSHED_FIREFLYOPTIMIZATOR_H
