//
// Created by Rafael Zulli De Gioia Paiva on 11/12/15.
//

#include "SegmentationEvaluator.h"
#include "WatershedImage.h"




double SegmentationEvaluator::evaluatePair(Region *rPR,Region *rGT,int i)
{
    Mat prMask = rPR->getMask();
    Mat gtMask = rGT->getMask();

    int rPRPos = countNonZero(prMask);
    int rGTPos = countNonZero(gtMask);
   // cout<<rPRPos<<" "<<rGTPos<<endl;


    Mat result(rPR->getMask().rows,rPR->getMask().cols,CV_8UC1);
    result = Scalar::all(0);

    //cout<< rPR->getMask().channels()<< rGT->getMask().channels()<<endl;
    result = (prMask/2 + gtMask/2);

    //double min, max;
    //cv::minMaxLoc(result, &min, &max);


    int totalCases = countNonZero(result);

    int imageSize = rPR->getMask().cols*rPR->getMask().rows;

    // Regiao que ambos concordam
    int agreeRegion = countNonZero(result-128);

    // Nao regiao que ambos concordam
    int agreeNotRegion = abs(imageSize - totalCases) ;



    int rPRNeg = abs(imageSize - rPRPos);
    int rGTNeg = abs(imageSize - rGTPos);

    // regiao que pred fala sim mas gt fala nao
    int rPRPos_rGTNeg = abs(rPRPos-agreeRegion);

    // regiao que pred fala nao mas gt fala sim
    int rGTPos_rPRNeg = abs(rGTPos-agreeRegion);



    /*                        rPR
     *                SIM               NAO
     *  rGT  SIM  agreeRegion       rGTPos_rPRNeg    | rGTPOs
     *       NAO  rPRPos_rGTNeg     agreeNotRegion   | rGTNeg
     *       ________________________________________|________
     *              rPRPos              rPRNeg       |  imageSize
     *
     */


    double observedArgree =  (((double)agreeRegion + (double)agreeNotRegion)/(double)imageSize);

    double chanceAgree = ((double)rPRPos/imageSize)*  ((double)rGTPos/imageSize) + ((double)rPRNeg/imageSize) * ((double)rGTNeg/imageSize);


    double kappa = ((double)observedArgree- chanceAgree)/(1.0-chanceAgree);
    /* cout<<i<<" - Total: "<< totalCases <<" image size: "<< imageSize<<" Agree: "<< agreeRegion<< " Agree Not: "<< agreeNotRegion << endl <<
            "\t rPR Pos "<<rPRPos<<" rPR Neg "<<rPRNeg <<" rGT Pos "<<rGTPos<<" rGT Neg "<<rGTNeg <<endl<<
            "\t OA: " << observedArgree <<  " AC: "<<chanceAgree<<" Kappa: " <<kappa<<endl<<endl;
    //imshow("evaluate ", result);
    //int k = waitKey(0);
    cout<<kappa<<endl;
    cout<<kappa<<endl;*/
    return kappa;
}


double SegmentationEvaluator::evaluate(Segmentation *pr, Segmentation *gt)
{
    int i=0;
    double maxEvaluation;
    double finalEvaluation = 0.0;

    for( auto& pPR : pr->regions) {

        Region rPR = pPR.second;

        maxEvaluation = 0;

        //imshow("alo",rPR.getMask());


        i++;

        for(auto& pGT : gt->regions)
        {
            Region rGT = pGT.second;

            bool intersects = rPR.getBoundaryRect().intersects(rGT.getBoundaryRect());
            //bool intersects = rPR.getMask().getBoundary().intersected(rGT.getMask().getBoundary()).isEmpty();
            //cout<<intersects;
            if(intersects)
            {
                double ev = evaluatePair(&rPR,&rGT,i);

                if(ev>maxEvaluation)
                    maxEvaluation = ev;
                //cout<<"\t"<<ev<<endl;
            }

        }
        //cout<<maxEvaluation<<endl;
        finalEvaluation += maxEvaluation;
        pr->notas[pPR.first] = maxEvaluation;
        //evaluation.insert(evaluation.end(),make_pair(rPR,maxEvaluation));
    }

    return finalEvaluation/(double)i;



}
