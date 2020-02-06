#include "videocap.h"

VideoCap::VideoCap()
{
    capture = new cv::VideoCapture();
}

VideoCap::~VideoCap()
{
    delete capture;
}

bool VideoCap::open(int device)
{
    return capture->open(device);
}

double VideoCap::getProperty(int propIdx)
{
    return capture->get(propIdx);
}

bool VideoCap::setProperty(int propIdx,double propVal)
{
    return capture->set(propIdx,propVal);
}

bool VideoCap::grabFrame()
{
    return capture->grab();
}

unsigned char* VideoCap::retrieveFrame()
{
    capture->retrieve(rawImage);
    return rawImage.ptr();
}

cv::Mat VideoCap::retrieveMat()
{
    capture->retrieve(rawFrame);
    return rawFrame;
}

bool VideoCap::isOpened() const
{
    return capture->isOpened();
}

cv::Mat VideoCap::saveImage()
{
    cv::Mat saved = retrieveMat();
    std::stringstream filename;
    filename << "home"<< "/" << "pcorrea"  <<"/" <<"Pictures" <<"/" <<"savedOP" << ".png";
    cv::imwrite(filename.str(), saved);
}



