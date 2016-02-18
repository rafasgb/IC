//
// Created by Rafael Zulli De Gioia Paiva on 16/02/16.
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
#include <sys/stat.h>



#ifndef WATERSHED_SEGMENTATION_H
#define WATERSHED_SEGMENTATION_H

using namespace cv;
using namespace std;



typedef pair<unsigned int, unsigned int> Index;
typedef map<Index, float> Matrix;

class Segmentation {
private:

    QList<Region> regions;
    vector<char> color;
    vector<Vec3b*> listaCor;
    RNG rng;

    //vector<vector< float>> matrixAdj;


    QImageCV image;
    QString file;
    Mat mask;
    float threshold;
public:

    Matrix matrixAdj;
    int width, height;
    int size;


    Segmentation(){};
    Segmentation(SegmentedImage im, float threshold,vector<Vec3b*>& listaCor );

    Segmentation(QString , Matrix ,float ,vector<Vec3b*>&);
    Segmentation(QString , float ,vector<Vec3b*>&) ;

    void init();

    void makeRegions(Mat);
    void makeRegions();

    void makeMask();

    void print();

    Segmentation interpolate(Segmentation , float );

    Matrix interpolateMatrix(Segmentation , float );

    float compara(Segmentation , float );

    void dfs_visit(int);
    void showImageMask(const string);

    void setWeight(Matrix *,unsigned int , unsigned int ,float );
    void setWeight(unsigned int, unsigned int ,float);
    float findWeight(unsigned int , unsigned int );
    Mat getMask();
    QList<Region> *getRegions();

};



#endif //WATERSHED_SEGMENTATION_H
