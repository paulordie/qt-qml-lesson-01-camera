#include "camera.h"

Camera::Camera(QQuickItem* parent) :
    QQuickItem(parent)
{
    //Build the list of available devices
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for(const auto& cameraInfo : cameras){
        QString device;
        device += cameraInfo.deviceName();
        device += " - ";
        device += cameraInfo.description();
        deviceList << device;
    }
    emit deviceListChanged();

    size = QSize(640,480);
    connect(this, &QQuickItem::parentChanged, this, &Camera::changeParent);

    //Open camera right away
    update();
}

Camera::~Camera()
{
    if(thread)
        thread->stop();
    delete thread;
    delete camera;
    //Camera release is automatic when cv::VideoCapture is destroyed
}

void Camera::changeParent(QQuickItem* parent)
{
}

void Camera::saveFrameFromCamera()
{
    frame->capFrame();
}

int Camera::getDevice() const
{
    return device;
}

void Camera::setDevice(int device)
{
    if(device >= 0 && this->device != device){
        this->device = device;
        update();
    }
}

QSize Camera::getSize() const
{
    return size;
}

void Camera::setSize(QSize size)
{
    if(this->size.width() != size.width() || this->size.height() != size.height()){
        this->size = size;
        update();
        emit sizeChanged();
    }
}

QVariant Camera::getCvImage()
{
    if(!exportCvImage){
        exportCvImage = true;
        update();
    }
    QVariant container(QVariant::UserType);
    container.setValue(cvImage);
    return container;
}

QStringList Camera::getDeviceList() const
{
    return deviceList;
}

void Camera::allocateCvImage()
{
    cvImage.release();
    delete[] cvImageBuf;

    cvImageBuf = new unsigned char[size.width()*size.height()*3];
    cvImage = cv::Mat(size.height(),size.width(),CV_8UC3,cvImageBuf);
}

void Camera::allocateVideoFrame()
{
    videoFrame = new QVideoFrame(size.width()*size.height()*4,size,size.width()*4,VIDEO_OUTPUT_FORMAT);
}

void Camera::update()
{
    DPRINT("Opening camera %d, width: %d, height: %d", device, size.width(), size.height());

    //Destroy old thread, camera accessor and buffers
    delete thread;
    delete camera;
    if(videoFrame && videoFrame->isMapped())
        videoFrame->unmap();
    delete videoFrame;
    videoFrame = NULL;
    delete[] cvImageBuf;
    cvImageBuf = NULL;

    //Create new buffers, camera accessor and thread
    if(exportCvImage)
        allocateCvImage();
    if(videoSurface)
        allocateVideoFrame();
    camera = new VideoCap();
    thread = new CameraThread(camera,videoFrame,cvImageBuf,size.width(),size.height());
    connect(thread,SIGNAL(imageReady()), this, SLOT(imageReceived()));

    //Open newly created device
    try{
        if(camera->open(device)){
            camera->setProperty(CV_CAP_PROP_FRAME_WIDTH,size.width());
            camera->setProperty(CV_CAP_PROP_FRAME_HEIGHT,size.height());
            if(videoSurface){
                if(videoSurface->isActive())
                    videoSurface->stop();
                if(!videoSurface->start(QVideoSurfaceFormat(size,VIDEO_OUTPUT_FORMAT)))
                    DPRINT("Could not start QAbstractVideoSurface, error: %d",videoSurface->error());
            }
            thread->start();
            DPRINT("Opened camera %d",device);
        }
        else
            DPRINT("Could not open camera %d",device);
    }
    catch(int e){
        DPRINT("Exception %d",e);
    }
}

void Camera::imageReceived()
{
    //Update VideoOutput
    if(videoSurface)
        if(!videoSurface->present(*videoFrame))
            DPRINT("Could not present QVideoFrame to QAbstractVideoSurface, error: %d",videoSurface->error());

    //Update exported CV image
    if(exportCvImage)
        emit cvImageChanged();
}

QAbstractVideoSurface* Camera::getVideoSurface() const
{
    return videoSurface;
}

void Camera::setVideoSurface(QAbstractVideoSurface* surface)
{
    if(videoSurface != surface){
        videoSurface = surface;
        update();
    }
}

