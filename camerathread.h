#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QDebug>
#include <QThread>
#include <QObject>
#include <QElapsedTimer>
#include <QVideoFrame>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include <vector>

#include "videocap.h"

#ifdef QT_DEBUG
#define DPRINT(...) qDebug(__VA_ARGS__)
#else
#define DPRINT(...) while(0);
#endif


class CameraTask : public QObject{
Q_OBJECT

public:
    explicit CameraTask(QObject *parent = nullptr);

    CameraTask(VideoCap* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);

    virtual ~CameraTask();

    void stop();

private:

    const float CAM_FPS_RATE = 0.9f;            ///< Rate of using the older FPS estimates
    const int CAM_FPS_PRINT_PERIOD = 500;

    int width;                                  ///< Width of the camera image
    int height;                                 ///< Height of the camera image
    VideoCap* camera;                           ///< The camera to get data from
    bool running = false;                       ///< Whether the worker thread is running
    QVideoFrame* videoFrame;                    ///< Place to draw camera image to
    unsigned char* cvImageBuf;                  ///< Place to export camera image to

    bool mCapture;//
    bool m_capture;//
    cv::Mat capImg;//

public slots:
    void doWork();
    void capFrame();

signals:

void imageReady();
void imageFrame();
};


class CameraThread : public QObject{
Q_OBJECT

public:

    CameraThread(VideoCap* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height);
    virtual ~CameraThread();
    void start();
    void stop();

private:

    QThread workerThread;               ///< The thread that the camera will work in
    CameraTask* task = NULL;            ///< The camera loop method and parameter container

signals:
    void imageReady();
    void imageFrame();
};

#endif /* CAMERATHREAD_H */

