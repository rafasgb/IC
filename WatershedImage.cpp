//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

#include "WatershedImage.h"


WatershedImage::WatershedImage(QString path, vector<Point3i> mark) : markers(mark) {

    loadImage(path);
}

WatershedImage::WatershedImage(Mat imageCV, vector<Point3i> mark) : markers(mark), image(imageCV) {

}

WatershedImage::WatershedImage(Mat imageCV, int markerQt) :image(imageCV)
{
    generateRandomMarkers(markerQt);
}

WatershedImage::WatershedImage(QString path, int markerQt)
{
    loadImage(path);
    generateRandomMarkers(markerQt);
}


void WatershedImage::loadImage(QString path) {

    if( !image.load(path) )
    {
        printf("Error reading image: %s\n", path.toStdString().c_str());

    }
}


void WatershedImage::watershed(bool debug) {

    RNG rnd = theRNG();
    rnd.state = time(NULL);

    Mat input(image.height(),image.width(),CV_8UC3);
    input = image.getCvBGRImage();
    cvtColor(input, input, COLOR_BGR2GRAY);
    cvtColor(input, input, COLOR_GRAY2BGR);
    //image.getCvBGRImage().copyTo(input);

    if(debug)
    imshow( "input", input );

    regionsMaskC1 = Mat(input.size(),CV_32SC1);
    regionsMaskC1 = Scalar::all(0);

    vector<Vec3b> colors;

    for(int i = 1; i <= markers.size(); i++ )
    {
        int b = rnd.uniform(0, 255);
        int g = rnd.uniform(0, 255);
        int r = rnd.uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));

        Point3i pos = markers[i-1];
        //cout<< pos<<endl;

        circle(regionsMaskC1, Point(pos.x,pos.y),pos.z, Scalar(i,i,i),CV_FILLED, 8,0);

    }

    //if(debug)
    //imshow( "watershed markers", regionsMask );



    cv::watershed( input, regionsMaskC1 );


    if(debug) {
        imshow("watershed transform", input);


        Mat wshed(input.size(), CV_8UC3);
        for (int i = 0; i < regionsMaskC1.rows; i++)
            for (int j = 0; j < regionsMaskC1.cols; j++) {
                int index = regionsMaskC1.at<int>(i, j);
                if (index == -1)
                    wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                else if (index <= 0 || index > markers.size())
                    wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
                else
                    wshed.at<Vec3b>(i, j) = colors[index - 1];
            }

        regionsMaskC3 = wshed;


        if (debug)
            imshow("watershed transform", regionsMaskC3 * 0.5 + (input) * 0.5);
    }



    for(int i=1;i<= colors.size();i++) {

        Mat temp(input.size(),CV_8UC1);

        temp = Scalar::all(0);
        inRange(regionsMaskC1, i, i, temp);

        Region r(&image, temp);
        if (!r.getMask().getBoundary().isEmpty()) {
            regions <<r;


        }


    }
    regionsMaskC1.convertTo(regionsMaskC1,CV_8UC1);



}

void WatershedImage::generateRandomMarkers(int markerQt) {

    RNG rnd = theRNG();
    rnd.state = getTickCount();

    for (int i = 0; i < markerQt; i++)
    {
        Point3f a;
        a.x = rnd.uniform(0, image.width()-1);
        a.y = rnd.uniform(0, image.height()-1);
        a.z = rnd.uniform(1, 10);

        markers.push_back(a);
    }

}

QList<Region>& WatershedImage::getRegions() {
    return regions;
}

Mat WatershedImage::getMaskC1()
{
    return regionsMaskC1;
}

Mat WatershedImage::getMaskC3()
{
    return regionsMaskC3;
}
vector<Point3i> WatershedImage::getMarkers()
{
    return markers;
}
