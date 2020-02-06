#ifndef VIDEOCAP_H
#define VIDEOCAP_H

#include <opencv2/videoio.hpp>
#include <opencv2/opencv.hpp>

class VideoCap {

public:

    VideoCap();
    virtual ~VideoCap();

    bool open(int device);
    double getProperty(int propIdx);
    bool setProperty(int propIdx, double propVal);
    bool grabFrame();
    unsigned char* retrieveFrame();

    cv::Mat retrieveMat();

    bool isOpened() const;
    cv::Mat saveImage();

private:

    cv::VideoCapture* capture;
    cv::Mat rawImage;
    cv::Mat rawFrame;
};

#endif /* VIDEOCAP_H */
