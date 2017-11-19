#include "mog.h"
#include "../settings.h"
#include <opencv2/video/background_segm.hpp>

Mog::Mog()
{
   // pMOG1 = cv::bgsegm::createBackgroundSubtractorMOG(150,13,0.1,1); //MOG approach
    pMOG2 = cv::createBackgroundSubtractorMOG2(400,16,false); //MOG2 approach /*Settings::mogHistory, Settings::mogThreshold*/
} 

cv::Mat Mog::processMat(cv::Mat &frame)
{
    cv::Mat fgMaskMOG; //fg mask generated by MOG method
    if (frame.empty()) {
        exit(EXIT_FAILURE);
    }
    pMOG2->apply(frame, fgMaskMOG);
   // pMOG1->apply(frame, fgMaskMOG);
    return fgMaskMOG;
}

