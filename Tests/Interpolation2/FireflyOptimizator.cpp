//
// Created by Rafael Zulli De Gioia Paiva on 12/12/15.
//

#include "FireflyOptimizator.h"


FireflyOptimizator::FireflyOptimizator( QString imagePath,  QString imageXml, float threshold, int MaxGenerations, int PopulationSize, float gamma)
: MaxGenerations(MaxGenerations), PopulationSize(PopulationSize), imagePath(imagePath) , imageXml(imageXml), gamma(gamma),threshold(threshold)
{
    SegmentedImage im2("/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.jpg","/Users/zulli/Documents/city/sun_abnjbjjzwfckjhyx.xml");

    listaCor = vector<Vec3b*>(0);
    groundTruth = Segmentation(im2,threshold,listaCor);

    regions = *groundTruth.getRegions();
    init();
}

void FireflyOptimizator::init() {
    generationRanked = false;
    optmized=false;

    groundTruth.getRegions();
    normalizeDis = sqrt( groundTruth.getMask().rows*groundTruth.getMask().rows + groundTruth.getMask().cols*groundTruth.getMask().cols);

    createPopulation();


}

void FireflyOptimizator::createPopulation()
{
    cout<< "Creating population ";
    for(int i=0;i<PopulationSize;i++)
    {
        Segmentation temp(imagePath,threshold,listaCor);
        population.push_back(temp);
    }
    cout<< " - Ok "<<endl;

}


void FireflyOptimizator::rankGeneration() {

    std::sort(generationRank.begin(), generationRank.end(), [](pair<Segmentation,double> &left, pair<Segmentation,double> &right) {
        return left.second > right.second;
    });

}

pair<Segmentation,double> FireflyOptimizator::getGenerationBest() {
    if(!generationRanked)
        rankGeneration();
    return generationRank[0];
}

pair<Segmentation,double> FireflyOptimizator::getBestOverall() {
    if(!optmized)
        run();
    return generationRank[0];
}

void FireflyOptimizator::startGeneration()
{

    int idx_ffI =0;
    vector<Segmentation>::iterator ffI;
    for( ffI = population.begin(),idx_ffI=0; ffI != population.end(); idx_ffI++,ffI++)
    {

        markers[idx_ffI] = ffI->matrixAdj;
        //cout<<markers[i].size()<<endl;
        ranks[idx_ffI] =  ranker.evaluate(ffI->getRegions(),&regions);
        generationRank.push_back(make_pair( *ffI, ranks[idx_ffI]));

    }
}

void FireflyOptimizator::computeFirefly(Segmentation ffI, int idx_ffI, int verbose = 0)
{
    if(verbose >=1 ) {
        cout << "------------------------------" << endl;
        cout << "Starting " << idx_ffI << endl;
    }

    double rank1 = ranks[idx_ffI];
    int j = 0,idx_ffJ =0 ;

    vector<Segmentation>::iterator ffJ;


    for( ffJ = population.begin(),idx_ffJ=0; ffJ != population.end();idx_ffJ++,ffJ++) {

        if (idx_ffI == idx_ffJ) {
            j++;
            continue;
        }

        double rank2 = ranks[idx_ffJ];

        if(verbose >=2 )
            cout << "\tComparing " << idx_ffI << " with " << j << " (" << rank1 << " x " << rank2 << " ) ";

        if (rank2 > rank1) {

            double distance = getDistance(ffI, *ffJ);
            double amount = beta * exp(-gamma * distance * distance);

            if(verbose >= 3)
              cout << "  -  Dis: " << distance << " It: " << amount;

            ffI.matrixAdj = ffI.interpolateMatrix(*ffJ, amount);

            j++;

        }
        else
        {  if(verbose >=3 )
             cout<<" - "<<idx_ffI << " greater, no update.";
        }
        //cout<<endl;
    }


}

void FireflyOptimizator::updateGeneration()
{
    vector<Segmentation>::iterator ffI;
    int idx_ffI =0;

    for( ffI = population.begin(),idx_ffI=0; ffI != population.end(); idx_ffI++,ffI++)
    {
        *ffI = Segmentation(imagePath,markers[idx_ffI],threshold,listaCor);//ranks[idx_ffI] =  ranker.evaluate(&ffI->getRegions(),&regions);


    }
}

void FireflyOptimizator::finishGeneration()
{
    rankGeneration();

    pair<Segmentation,double> r = getGenerationBest();

    imshow("generationBest",r.first.getMask());
    waitKey(1);

    generationBest.push_back(r);
    cout<<" BEST of " << gen<< ": "<<r.second<<endl;
    cout<<">>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<-"<<endl;
    generationRank.clear();
    generationRanked = false;

}

void  FireflyOptimizator::startClock()
{
    begin = clock();
}
void  FireflyOptimizator::endClock()
{
    end = clock();
    elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
}


void FireflyOptimizator::run() {

    optmized = false;


    beta = 1.0;

    ranks.clear();
    ranks.resize(population.size());
    markers.clear();
    markers.resize(population.size());

    vector<Segmentation>::iterator ffI;
    int idx_ffI =0;


    for (gen = 1; gen <= MaxGenerations; gen++)
    {
        cout<<">>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        cout<<"GENERATION "<<gen<<endl;

        cout<< "Starting generation. Updating population ";
        startClock();

        startGeneration();

        endClock();
        cout << " - end ( "<<  elapsed_secs<< " sec )" << endl;

        for( ffI = population.begin(),idx_ffI=0; ffI != population.end(); idx_ffI++,ffI++)
        {

            startClock();

            computeFirefly(*ffI,idx_ffI);

            endClock();
            cout << " - end ( "<<  elapsed_secs<< " sec )" << endl;


        }
        cout<< "Generation done. Updating population ";
        startClock();

        updateGeneration();

        endClock();
        cout<< " - OK "<<" ( "<<  elapsed_secs << " sec )" << endl;;


        finishGeneration();

    }

    cout<<"Final result "<<(generationBest.end()-1)->second<<endl;

    optmized = true;

}


double FireflyOptimizator::getDistance(Segmentation pr, Segmentation gt)
{
    double dis = 0.0;


    for(int i=0;i<pr.size;i++) {

        int top = (i-pr.width)<0?-1:i-pr.width;
        int down = (i+pr.width) >= pr.width* pr.height ? -1 : i+pr.width;
        int left = (i-1)<0?-1:i-1;
        int right = (i+1)>= pr.height*pr.width?-1:i+1;


        if (top!=-1)
        {

            dis += abs(pr.findWeight(i, top) - gt.findWeight(i, top));

        }
        if (down!=-1)
        {

            dis += abs(pr.findWeight(i, down) - gt.findWeight(i, down));

        }
        if (right!=-1)
        {

            dis += abs(pr.findWeight(i, right) - gt.findWeight(i, right));

        }
        if (left!=-1) {

            dis += abs(pr.findWeight(i, left) - gt.findWeight(i, left));

        }

    }

    return dis;
}

/////old
/*
void FireflyOptimizator::run() {

    bool debug = false,debug2=false;
    optmized = false;
    vector<WatershedImage>::iterator ffI,ffJ;
    SegmentationEvaluator ranker;

    double beta = 1.0;
    int i=0,j=0;
    clock_t begin,end;
    double elapsed_secs;

    for (int gen = 0; gen < MaxGenerations; gen++)
    {
        cout<<">>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<-"<<endl;
        cout<<"GENERATION "<<gen<<endl;
        for( ffI = population.begin(); ffI != population.end(); ffI++)
        {
            if(debug) {
                cout << "------------------------------" << endl;
                cout << "Starting " << i << endl;
            }
            cout << "Starting " << i ;
            begin = clock();

            double rank1 = ranker.evaluate(&ffI->getRegions(),&regions);
            if(debug2)
                imshow(to_string(long(&(*ffI))),ffI->getMaskC1()*255);

            if(debug)
                cout<<"Evaluation of "<<i<<": "<<rank1<<endl;
            for( ffJ = population.begin(); ffJ != population.end(); ffJ++)
            {
                if(ffI == ffJ){ j++;continue;}
                if(debug2)
                    imshow(to_string(long(&(*ffJ))),ffJ->getMaskC1()*255);

                if(debug)
                    cout<<" comparing with "<<j<<endl;

                SegmentationEvaluator ranker2;
                double rank2 = ranker.evaluate(&ffJ->getRegions(), &regions);

                if(debug)
                    cout<<" Evaluation of"<<j<<": "<<rank2<<endl<<endl;
                j++;

                if(rank2 > rank1)
                {
                    vector< pair< Point2i , Point2i > >  pairs = findMatch(*ffJ,*ffI);
                    double distance = getDistance(pairs);
                    double amount = beta * exp( - gamma * distance*distance);

                    //cout<<distance<<" "<<amount<<endl;

                    if(debug) {
                        showPoints(&(*ffJ), &(*ffI));
                        waitKey(0);
                    }
                    WatershedImage res = moveToward(pairs,amount);
                    res.watershed(false);


                    *ffI = res;


                    if(debug2)
                        imshow(to_string(long(&(*ffI))),ffI->getMaskC1()*255);
                    if(debug) {
                        showPoints(&(*ffJ), &(*ffI));
                        waitKey(10);
                    }

                }

            }
            end = clock();

            cout << " - end ( "<<  double(end - begin) / CLOCKS_PER_SEC<< " sec )" << endl;
            if(debug)
                cout<<"------------------------------"<<endl;
            j=0;
            i++;

            generationRank.push_back(make_pair( *ffI, rank1));
        }
        i=0;

        rankGeneration();
        pair<WatershedImage,double> r = getGenerationBest();
        imshow("generationBest",r.first.getMaskC1()*255);
        waitKey(1);
        generationBest.push_back(r);
        cout<<" BEST of " << gen<< ": "<<r.second<<endl;
        cout<<">>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<-"<<endl;
        generationRank.clear();
        generationRanked = false;
    }
    cout<<"result "<<(generationBest.end()-1)->second<<endl;
    optmized = true;
    waitKey(5000);
}
*/
void FireflyOptimizator::showPoints(vector<Point3i> gt, vector<Point3i> pr)
{
    Mat img(groundTruth.getMask().rows,groundTruth.getMask().cols,CV_8UC1);
    img = Scalar::all(0);

    for (Point3i p1 : gt) {
        circle(img, Point2i(p1.x, p1.y), 3, 255);
    }
    for (Point3i p1 : pr) {
        circle(img, Point2i(p1.x, p1.y), 3, 127);
    }

    imshow("points",img);
}

double FireflyOptimizator::pointDis(Point2i point, Point2i other) {
    return sqrt( (point.x - other.x) *(point.x - other.x) +
                 (point.y - other.y) *(point.y - other.y));
}


int FireflyOptimizator::findClosestPoint(Point3i pointTemp, vector< Point3i > pointList) {

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


vector< pair< Point2i , Point2i > > FireflyOptimizator::findMatch(vector< Point3i > points1,vector< Point3i > points2) {

   // vector< Point3i > points1  = im1.getMarkers();
   // vector< Point3i > points2  = im2.getMarkers();

    sort(points1.begin(),points1.end(),sortByXAxis);
    sort(points1.begin(),points1.end(),sortByXAxis);

    vector< pair< Point2i, Point2i > > result;


    //cout<<points1.size()<<" "<<points2.size()<<endl;
    for( Point3i p1 : points1) {
        int winnerPos = findClosestPoint(p1,points2);
        //cout<<winnerPos<<endl;
        Point2i a(p1.x,p1.y),b(points2[winnerPos].x,points2[winnerPos].y);
        result.push_back(make_pair(a,b));
        points2.erase(points2.begin()+winnerPos);

    }

    return result;

}

Point2i FireflyOptimizator::interpolate(pair<Point2i,Point2i> p, float amount)
{
    Point2i p1 = p.first;
    Point2i p2 = p.second;

    double angle = atan2(p1.y-p2.y,p1.x-p2.x);
    double dis = pointDis(p1,p2);
   // cout<<dis<<" "<<angle<<" "<<max(dis*amount,1.0)<<" "<< cos(angle)<< " "<<sin(angle)<<endl;
    if(dis<2.0)
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


vector< Point3i > FireflyOptimizator::moveToward(vector<pair<Point2i, Point2i>> vec,float amount)
{
    vector< Point3i > markerC2;

    for( pair<Point2i, Point2i> p  : vec)
    {
        p.second = interpolate(p,amount);

        markerC2.push_back(Point3i(p.second.x,p.second.y,0));


    }
    //WatershedImage r(imagePath,markerC2);
    return markerC2;

}
/*Begin
   1) Objective function: f(\mathbf {x} ),\quad \mathbf {x} =(x_{1},x_{2},...,x_{d});
   2) Generate an initial population of fireflies \mathbf {x} _{i}\quad (i=1,2,\dots ,n);.
   3) Formulate light intensity I so that it is associated with f(\mathbf {x} )
      (for example, for maximization problems, I\propto f(\mathbf {x} ) or simply I=f(\mathbf {x} );
   4) Define absorption coefficient \gamma

   While (t < MaxGeneration)
      for i = 1 : n (all n fireflies)
         for j = 1 : n (n fireflies)
            if (I_{j}>I_{i}),
               move firefly i towards j;
               Vary attractiveness with distance r via \exp(-\gamma \;r);
               Evaluate new solutions and update light intensity;
            end if
         end for j
      end for i
      Rank fireflies and find the current best;
   end while

   Post-processing the results and visualization;

end*/