#ifndef CAMERA_H
#define CAMERA_H

#include <QQuickItem>
#include <QCameraInfo>
#include <QList>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QSize>
#include <QMutex>
#include <QWaitCondition>
#include <QVariant>

#include <opencv2/highgui/highgui.hpp>

#include "camerathread.h"

Q_DECLARE_METATYPE(cv::Mat)

class Camera : public QQuickItem {
Q_OBJECT
    Q_DISABLE_COPY(Camera)
    Q_PROPERTY(int device READ getDevice WRITE setDevice)
    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ getVideoSurface WRITE setVideoSurface)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QStringList deviceList READ getDeviceList NOTIFY deviceListChanged)
    Q_PROPERTY(QVariant cvImage READ getCvImage NOTIFY cvImageChanged)

public:
    Camera(QQuickItem* parent = 0);

    ~Camera();

    int getDevice() const;

    void setDevice(int device);

    QSize getSize() const;

    void setSize(QSize size);
    QStringList getDeviceList() const;

    QAbstractVideoSurface* getVideoSurface() const;

    void setVideoSurface(QAbstractVideoSurface* videoSurface);

    QVariant getCvImage();

signals:
    void sizeChanged();
    void deviceListChanged();
    void cvImageChanged();

private:
    const QVideoFrame::PixelFormat VIDEO_OUTPUT_FORMAT = QVideoFrame::PixelFormat::Format_ARGB32;

    int device = 0;                                         ///< The camera device number
    QStringList deviceList;                                 ///< The list of available devices, indices corresponding to device indices
    QSize size;                                             ///< The desired camera resolution
    VideoCap* camera = NULL;                                ///< The camera object
    CameraThread* thread = NULL;                            ///< Thread to run camera image acquisition

    QVideoFrame* videoFrame = NULL;                         ///< Object that contains the image buffer to be presented to the video surface
    QAbstractVideoSurface* videoSurface = NULL;             ///< Object that presents the videoFrame to the outside world

    bool exportCvImage = false;                             ///< Whether to export the CV image
    cv::Mat cvImage;                                        ///< Buffer to export the camera image to
    unsigned char* cvImageBuf = NULL;                       ///< Actual physical buffer for cv::Mat, it is helpless without it
    void update();
    void allocateCvImage();
    void allocateVideoFrame();

    CameraTask *frame = NULL;


public slots:
    void changeParent(QQuickItem* parent);
    void saveFrameFromCamera();

private slots:
    void imageReceived();
};

#endif /* CAMERA_H */

