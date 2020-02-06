#include "camerathread.h"

#include <iostream>
#include <string>

CameraTask::CameraTask(QObject *parent) : QObject(parent)
{

}

CameraTask::CameraTask(VideoCap* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    this->running = true;
    this->camera = camera;
    this->videoFrame = videoFrame;
    this->cvImageBuf = cvImageBuf;
    this->width = width;
    this->height = height;
}

CameraTask::~CameraTask()
{

}

void CameraTask::stop()
{
    running = false;
}

void CameraTask::doWork()
{
    QElapsedTimer timer;
    float fps = 0.0f;
    int millisElapsed = 0;
    int millis;
    timer.start();

    if(videoFrame)
        videoFrame->map(QAbstractVideoBuffer::ReadOnly);

    cv::Mat screenImage;
    if(videoFrame)
        screenImage = cv::Mat(height,width,CV_8UC4,videoFrame->bits());

    while(running && videoFrame != NULL && camera != NULL){
        if(!camera->grabFrame())
            continue;
        unsigned char* cameraFrame = camera->retrieveFrame();

        //Get camera image into screen frame buffer
        if(videoFrame){
            cv::Mat tempMat(height,width,CV_8UC3,cameraFrame);
            cv::cvtColor(tempMat,screenImage,cv::COLOR_RGB2RGBA);

            if (camera->grabFrame()) {
                screenImage.copyTo(capImg);
                //                m_capture = false;
            }
        }

        //Export camera image
        if(cvImageBuf){
            memcpy(cvImageBuf,cameraFrame,height*width*3);
        }

        emit imageReady();


        millis = (int)timer.restart();
        millisElapsed += millis;
        fps = CAM_FPS_RATE*fps + (1.0f - CAM_FPS_RATE)*(1000.0f/millis);
        if(millisElapsed >= CAM_FPS_PRINT_PERIOD){
            qDebug("Camera is running at %f FPS",fps);
            millisElapsed = 0;
        }
    }
}

void CameraTask::capFrame()
{


    auto time = std::time(nullptr);
    auto locaTime = *std::localtime(&time);
    char buffer[256];
    strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &locaTime);

    QString fileName;
    fileName.append("frame_");
    fileName.append(buffer);
    fileName.append(".png");

    if (!capImg.empty()) {
        QImage image(capImg.data, capImg.cols, capImg.rows, capImg.step, QImage::Format::Format_RGB888);
        image.save(fileName);
        emit imageFrame();
        qDebug() << "Frame is ok";
    } else {
        qDebug() << "Frame is empty";

    }

    qDebug() << "cap frame";
}

//thread

CameraThread::CameraThread(VideoCap* camera, QVideoFrame* videoFrame, unsigned char* cvImageBuf, int width, int height)
{
    task = new CameraTask(camera,videoFrame,cvImageBuf,width,height);
    task->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(started()), task, SLOT(doWork()));
    connect(task, SIGNAL(imageReady()), this, SIGNAL(imageReady()));
}

CameraThread::~CameraThread()
{
    stop();
    delete task;
}

void CameraThread::start()
{
    workerThread.start();
}

void CameraThread::stop()
{
    if(task != NULL)
        task->stop();
    workerThread.quit();
    workerThread.wait();
}

