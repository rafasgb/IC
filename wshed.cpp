//
// Created by Rafael Zulli De Gioia Paiva on 13/11/15.
//


#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <cstdio>
#include <iostream>
using namespace cv;
using namespace std;


Mat markerMask, img;

QPolygon extractPolygon(QString Xml) {
    QRegularExpression exp("<pt>.?<x>\\D?(\\d*)\\D?</x>.?<y>\\D?(\\d*)\\D?</y>.?</pt>",
                           QRegularExpression::DotMatchesEverythingOption);
    auto matches = exp.globalMatch(Xml);
    QPolygon res;
    while (matches.hasNext()) {
        auto match = matches.next();
        int x = match.captured(1).toInt();
        int y = match.captured(2).toInt();
        cout<< x<<" "<<y<<endl;
        res << QPoint(x, y);
    }
    return res;
}



QString extractLabel(QString Xml) {

    QRegularExpression exp("<name>(.*?)</name>", QRegularExpression::DotMatchesEverythingOption);
    auto matched = exp.globalMatch(Xml);
    return matched.next().captured(1).remove("\n");

}


void parse_xml(const QImageCV image,  QList<Region> *regions)  {


    QString supervisedPath = "/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml";
    QRegularExpression exp("<object>.*?</object>", QRegularExpression::DotMatchesEverythingOption);

    QFile f(supervisedPath);

    f.open(QFile::ReadOnly);
    QByteArray contents = f.readAll().replace('\r', '\n');
    f.close();
    auto matches = exp.globalMatch(contents);
    while (matches.hasNext()) {
        QString regionXml = matches.next().captured(0);
        cout<< regionXml.toStdString()<<endl;
        QPolygon polygon = extractPolygon(regionXml);
        polygon = polygon.intersected(QPolygon(QRect(0, 0, image.width() - 1, image.height() - 1)));
        QString label = extractLabel(regionXml);
        label.prepend('\"').append('\"');
        if (polygon.size() != 0) {
            (*regions) << (Region(&image, polygon, label));
            cout<< label.toStdString()<<" : ";

        }
    }


}



double getPosRate(Mat * ground, Mat * pred)
{
    int area_total = countNonZero((*ground)+(*pred));

    int area_intersec = countNonZero((*ground)*(*pred));

    int area_notInterset = area_total-area_intersec;

    return ((double)area_intersec/(double)area_total);


}
void wshed(Mat * input, vector<Point3i> &marcadores)
{

    SupervisedImage supImg("/Users/zulli/Documents/out/sun_aghqpdrnitevyedj.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");

    while(supImg.hasError());
    supImg.getImage();

    QList<Region> regions2;

    parse_xml(*supImg.getImage(),&regions2);



    Mat alo(supImg.getImage()->size().height(),supImg.getImage()->size().width(),CV_8UC3);
    regions2 = supImg.getRegions();
    cout<<supImg.getRegions().at(0).getMask().size().width<<endl;
    RNG rnd = theRNG();
    rnd.state = time(NULL);

    for(int i=0;i<supImg.getRegions().size();i++) {
        alo = Scalar::all(0);
        Utils::QPolygon2Mask(alo,supImg.getRegions().at(i).getMask().getBoundary(),supImg.getRegions().at(i).getMask().getHoles());

        vector<vector<cv::Point> > points;

        Utils::QPolygon2CvPointArray(supImg.getRegions().at(i).getMask().getBoundary(), points);

        cout<< supImg.getRegions().at(i).getMask().size().width<<endl;
        for (QPoint p : supImg.getRegions().at(i).getMask().getBoundary())
        {
            //cout<<p.x()<<" "<<p.y()<<endl;
        }
        cv::fillPoly(alo, points, cvScalarAll(rnd.uniform(0,255)), 4);

        for(const QPolygon& hole : supImg.getRegions().at(i).getMask().getHoles()){
            Utils::QPolygon2CvPointArray(hole, points);
            cv::fillPoly(alo, points, cvScalarAll(0),4 );
        }

        imshow("alo",alo);
        int c= waitKey(0);
        if ((char) c == 27)
            break;
    }

    imshow("alo2",alo);



    vector<Vec3b> cores;

    Mat marImg( input->size(),CV_32SC1);

    marImg = Scalar::all(0);

    int imax = std::numeric_limits<int>::max();
    Scalar b(255,255,255);

    imshow( "watershed ma2rkers", *input );
    for(int i = 1; i <= marcadores.size(); i++ )
    {
        int b = rnd.uniform(0, 255);
        int g = rnd.uniform(0, 255);
        int r = rnd.uniform(0, 255);
        cores.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));

        Point3i pos = marcadores[i-1];
        cout<< pos<<endl;

        circle(marImg, Point(pos.x,pos.y),pos.z, Scalar(i,i,i),CV_FILLED, 8,0);
        //marImg.at<int>(pos.x,pos.y) = imax;

    }

    imshow( "watershed markers", marImg );

    watershed( *input, marImg );

    imshow( "watershed markers", *input );


    Mat wshed(input->size(), CV_8UC3);
    wshed = Scalar::all(0);
    for( int i = 0; i < marImg.rows; i++ )
        for( int j = 0; j < marImg.cols; j++ )
        {
            int index = marImg.at<int>(i,j);
            if( index == -1 )
                wshed.at<Vec3b>(i,j) = Vec3b(255,255,255);
            else if( index <= 0 || index > marcadores.size() )
                wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
            else
                wshed.at<Vec3b>(i,j) = cores[index - 1];
        }

    wshed = wshed;// + (*input)*0.5;

    imshow( "watershed transform", wshed*0.5  + (*input)*0.5 );

    Mat temp(input->size(),CV_8UC1);
    QImageCV* image = new QImageCV(*input);

    QList<Region> regions;

    for(int i=0;i< cores.size();i++)
    {
        inRange(wshed, cores[i],cores[i],temp);
        cout<< countNonZero(temp)<<endl;
        /*vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        temp = Scalar::all(0);
        cout<< cores[i];

        imshow("alo",temp);

        findContours( temp, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );*/
        Region r(image, temp);
        regions.append(r);




        /*
        cout<< contours.size();
        for( int j = 0; i< contours.size(); i++ ) {
            temp = Scalar::all(0);
            drawContours(temp, contours, j, 255, CV_FILLED, 8, hierarchy);
        }*/

        imshow("alo",temp);
        int c= waitKey(0);
        if ((char) c == 27)
            break;
    }

/*

    RNG rng(12345);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours( wshed, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    Draw contours
    Mat drawing = Mat::zeros( wshed.size(), CV_8UC3 );

    vector<Region>
    for( int i = 0; i< contours.size(); i++ ) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        for (int j = 0; j < contours.at(i).size(); j++)
        {
            circle(drawing, contours.at(i).at(j),1, color,CV_FILLED, 1,0);

        }

       // drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );
    }
    imshow("alo", drawing);*/

}

int main2();



int main2()
{


    char* filename = (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    Mat img0 = imread(filename, 1), imgGray;
    if( img0.empty() )
    {
        cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
        return 0;
    }

    img0.copyTo(img);
    cvtColor(img, markerMask, COLOR_BGR2GRAY);
    cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);
    markerMask = Scalar::all(0);
    vector<Point3i> marcadores;

    RNG rnd = theRNG();
    rnd.state = time(NULL);

    cout<< img0.size()<<endl;
    int num = 10;
    for (int i = 0; i < num; i++)
    {
        Point3f a;
        a.x = rnd.uniform(0, img0.rows);
        a.y = rnd.uniform(0, img0.cols);
        a.z = rnd.uniform(1, 10);

        marcadores.push_back(a);
    }

    wshed(&imgGray, marcadores);

    /*for(;;) {
        int c = waitKey(0);
        if ((char) c == 27)
            break;
    }*/
    return 0;
}


static void help()
{
    cout << "\nThis program demonstrates the famous watershed segmentation algorithm in OpenCV: watershed()\n"
            "Usage:\n"
            "./watershed [image_name -- default is ../data/fruits.jpg]\n" << endl;
    cout << "Hot keys: \n"
            "\tESC - quit the program\n"
            "\tr - restore the original image\n"
            "\tw or SPACE - run watershed segmentation algorithm\n"
            "\t\t(before running it, *roughly* mark the areas to segment on the image)\n"
            "\t  (before that, roughly outline several markers on the image)\n";
}
Mat markerMask, img;
Point prevPt(-1, -1);
static void onMouse( int event, int x, int y, int flags, void* )
{
    if( x < 0 || x >= img.cols || y < 0 || y >= img.rows )
        return;
    if( event == EVENT_LBUTTONUP || !(flags & EVENT_FLAG_LBUTTON) )
        prevPt = Point(-1,-1);
    else if( event == EVENT_LBUTTONDOWN )
        prevPt = Point(x,y);
    else if( event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON) )
    {
        Point pt(x, y);
        if( prevPt.x < 0 )
            prevPt = pt;
        line( markerMask, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        line( img, prevPt, pt, Scalar::all(255), 5, 8, 0 );
        prevPt = pt;
        imshow("image", img);
    }
}
int main( int argc, char** argv )
{
    char* filename = argc >= 2 ? argv[1] : (char*)"/Users/zulli/ClionProjects/Watershed/sun.jpg";
    Mat img0 = imread(filename, 1), imgGray;
    if( img0.empty() )
    {
        cout << "Couldn'g open image " << filename << ". Usage: watershed <image_name>\n";
        return 0;
    }
    help();
    namedWindow( "image", 1 );
    img0.copyTo(img);
    cvtColor(img, markerMask, COLOR_BGR2GRAY);
    cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);
    markerMask = Scalar::all(0);
    imshow( "image", img );
    setMouseCallback( "image", onMouse, 0 );
    for(;;)
    {
        int c = waitKey(0);
        if( (char)c == 27 )
            break;
        if( (char)c == 'r' )
        {
            markerMask = Scalar::all(0);
            img0.copyTo(img);
            imshow( "image", img );
        }
        if( (char)c == 'w' || (char)c == ' ' )
        {
            int i, j, compCount = 0;
            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
            if( contours.empty() )
                continue;
            Mat markers(markerMask.size(), CV_32S);
            markers = Scalar::all(0);
            int idx = 0;
            for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
                drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);
            if( compCount == 0 )
                continue;
            vector<Vec3b> colorTab;
            for( i = 0; i < compCount; i++ )
            {
                int b = theRNG().uniform(0, 255);
                int g = theRNG().uniform(0, 255);
                int r = theRNG().uniform(0, 255);
                colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
            }
            double t = (double)getTickCount();
            watershed( img0, markers );
            t = (double)getTickCount() - t;
            printf( "execution time = %gms\n", t*1000./getTickFrequency() );
            Mat wshed(markers.size(), CV_8UC3);
            // paint the watershed image
            for( i = 0; i < markers.rows; i++ )
                for( j = 0; j < markers.cols; j++ )
                {
                    int index = markers.at<int>(i,j);
                    if( index == -1 )
                        wshed.at<Vec3b>(i,j) = Vec3b(255,255,255);
                    else if( index <= 0 || index > compCount )
                        wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
                    else
                        wshed.at<Vec3b>(i,j) = colorTab[index - 1];
                }
            wshed = wshed*0.5 + imgGray*0.5;
            imshow( "watershed transform", wshed );
        }
    }
    return 0;
}