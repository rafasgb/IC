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
#include "../../SegmentationEvaluator.h"
#include "../../SegmentedImage.h"
#include "Segmentation.h"

class FireflyOptimizator {


public:
    FireflyOptimizator( QString imagePath,  QString imageXml, float threshold, int MaxGenerations, int PopulationSize, float gamma);
    void init();
    void run();
    void rankGeneration();
    void createPopulation();
    pair<Segmentation,double> getGenerationBest();
    pair<Segmentation,double> getBestOverall();

private:
    vector< double > ranks;
    vector< Matrix > markers;
    double beta = 1.0;
    int gen;
    void startGeneration();
    void  endClock();
    void  startClock();
    clock_t begin,end;
    double elapsed_secs;

    void computeFirefly(Segmentation , int , int );
    void updateGeneration();
    void finishGeneration();
    SegmentationEvaluator ranker;
    vector<  Vec3b* > listaCor;
    Segmentation groundTruth;
    QList<Region> regions;
    QString imagePath, imageXml;
    int MaxGenerations;
    int PopulationSize;
    float threshold;
    float gamma;
    bool generationRanked,optmized;
    vector<Segmentation> population;
    vector< pair<Segmentation,double> > generationRank,generationBest;

    double normalizeDis;
    void showPoints(vector<Point3i> gt, vector<Point3i> pr);

    bool SortbyXaxis(const Point3i & a, const Point3i &b);
    vector<Point3i> moveToward(vector<pair<Point2i, Point2i>> vec, float amount);

    double pointDis(Point2i point, Point2i other);

    int findClosestPoint(Point3i pointTemp, vector< Point3i > pointList);
    vector< pair< Point2i , Point2i > > findMatch(Segmentation im1, Segmentation im2);

    Point2i interpolate(pair<Point2i, Point2i> p, float amount);

    //vector<Point3i> moveToward(WatershedImage *from, WatershedImage *to);
    vector< pair< Point2i , Point2i > > findMatch(vector< Point3i > points1,vector< Point3i > points2);

    double getDistance(Segmentation pr , Segmentation gt );
};

struct {


    bool operator()(const Point3i & a, const Point3i &b) const {
        return a.x < b.x;
    }
} sortByXAxis;

#endif //WATERSHED_FIREFLYOPTIMIZATOR_H
