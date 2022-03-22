#include "camera_loop.h"


CameraLoop::CameraLoop(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    object_detector_done_bool(true),
    new_frames(&frames_0),
    detecting_frames(&frames_1),
    tracking_frames(&frames_2),
    tracking_results(&results_0),
    detecting_results(&results_1),
    new_per_frame_period(&per_frame_period_0),
    tracking_per_frame_period(&per_frame_period_0),
    usb_webcam(Webcam::getInstance(cfg,webcam_done_cv,all_done_cv,flag_mutex,object_detector_done_bool,parent)),
    object_tracker(ObjectTracker::getInstance(cfg,object_tracker_done_cv,all_done_cv,object_tracker_done_mutex,parent)),
    object_detector(ObjectDetector::getInstance(cfg,object_detector_done_cv,all_done_cv,flag_mutex,object_detector_done_bool,parent))
{
    qDebug() << "Creating CameraLoop Object";
    moveToThread(&m_thread);
    m_thread.start();

    usb_webcam.new_frames = new_frames;
    usb_webcam.new_per_frame_period = new_per_frame_period;
    usb_webcam.run();

    webcam_done_mutex.lock();
    webcam_done_cv.wait(&webcam_done_mutex);

    std::swap(tracking_frames,detecting_frames);
    std::swap(new_frames,detecting_frames);

    usb_webcam.new_frames = new_frames;

    object_detector.detecting_frame = &detecting_frames->front();
    object_detector.detecting_results = detecting_results;

    object_tracker.tracking_frames = tracking_frames;
    object_tracker.tracking_results = tracking_results;
    object_tracker.tracking_per_frame_period = tracking_per_frame_period;

    all_done_cv.wakeAll();
    webcam_done_mutex.unlock();

    object_detector.run();

    object_tracker.run();
}

CameraLoop::~CameraLoop(){


    qDebug() << "Destroying PlayGround Object";
    usb_webcam.keep_running = false;
    object_detector.keep_running = false;
    object_tracker.keep_running = false;

    if( m_thread.isRunning() )
    {
        try {webcam_done_cv.wakeAll();} catch(int t){}
        try {object_detector_done_cv.wakeAll();} catch(int t){}
        try {object_tracker_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

CameraLoop& CameraLoop::getInstance(json cfg, QObject *parent)
{
    static CameraLoop instance(cfg, parent);
    return instance;
}

void CameraLoop::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}

void CameraLoop::main_loop(void) {

    qDebug() << "CameraLoop::main_loop thread id:" << QThread::currentThreadId();

    while (keep_running) {
        webcam_done_mutex.lock();
        webcam_done_cv.wait(&webcam_done_mutex);

        std::swap(tracking_frames,detecting_frames);
        std::swap(new_frames,detecting_frames);

        std::swap(tracking_results,detecting_results);

        std::swap(new_per_frame_period,tracking_per_frame_period);

        usb_webcam.new_frames = new_frames;

        object_detector.detecting_frame = &detecting_frames->front();
        object_detector.detecting_results = detecting_results;

        object_tracker.tracking_frames = tracking_frames;
        object_tracker.tracking_results = tracking_results;
        object_tracker.tracking_per_frame_period = tracking_per_frame_period;

        all_done_cv.wakeAll();
        webcam_done_mutex.unlock();
    }
}
