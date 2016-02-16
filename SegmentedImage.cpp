//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

#include "SegmentedImage.h"

SegmentedImage::SegmentedImage(QString imagePath, QString supervisedPath)  : SupervisedImage(imagePath,supervisedPath){

    SupervisedImage::getImage();
    SupervisedImage::getRegions();

    parseRegions(false);

}




void SegmentedImage::parseRegions(bool debug)
{

    RNG rnd = theRNG();
    rnd.state = time(NULL);
    regionsMask = Mat(getImage()->height(),getImage()->width(),CV_8UC3);
    regionsMask = Scalar::all(0);
    Mat supportImg(getImage()->height(),getImage()->width(),CV_8UC3);


    for(int i = 0; i < getRegions().size(); i++ ) {

        int b = rnd.uniform(0, 255);
        int g = rnd.uniform(0, 255);
        int r = rnd.uniform(0, 255);

        //Utils::QPolygon2Mask(regionsMask,getRegions().at(i).getMask().getBoundary(),getRegions().at(i).getMask().getHoles());

        supportImg = Scalar::all(0);
        vector<vector<cv::Point> > points;

        Utils::QPolygon2CvPointArray(getRegions().at(i).getMask().getBoundary(), points);

        cv::fillPoly(supportImg, points, Scalar(r,g,b), 4);

        for(const QPolygon& hole : getRegions().at(i).getMask().getHoles()){
            Utils::QPolygon2CvPointArray(hole, points);
            cv::fillPoly(supportImg, points, cvScalarAll(0),4 );
        }

        regionsMask += supportImg;


    }
    if(debug)
    imshow("debug parseRegions",regionsMask);


}

QList<Region> SegmentedImage::getRegions() {
    return SupervisedImage::getRegions();

}
Mat SegmentedImage::getMask()
{
    return regionsMask;
}