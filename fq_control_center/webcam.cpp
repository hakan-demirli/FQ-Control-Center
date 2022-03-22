#include "webcam.h"


Webcam::Webcam(json cfg,
               QWaitCondition& webcam_done_cv,
               QWaitCondition& all_done_cv,
               QMutex& flag_mutex,
               bool& object_detector_done_bool,
               QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    webcam_done_cv(webcam_done_cv),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    object_detector_done_bool(object_detector_done_bool)
{
    cap = cv::VideoCapture((int)cfg["Source"]);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1); // [TODO] EXPERIMENT WITH THE CONSTANT BUFFERSIZE
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, (int)cfg["Size"][0]);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, (int)cfg["Size"][1]);
    moveToThread(&m_thread);
    m_thread.start();
}

Webcam::~Webcam(){

    if( m_thread.isRunning() )
    {
        m_thread.quit();
        m_thread.wait();
    }
    try {
        cap.release();
    }  catch (int a) {

    }

}

Webcam& Webcam::getInstance(json cfg,
                            QWaitCondition& webcam_done_cv,
                            QWaitCondition& all_done_cv,
                            QMutex& flag_mutex,
                            bool& object_detector_done_bool,
                            QObject *parent)
{
    static Webcam instance(cfg,webcam_done_cv,all_done_cv,flag_mutex,object_detector_done_bool,parent);
    return instance;
}

void Webcam::main_loop(){

    qDebug() << "Webcam::main_loop thread id:" << QThread::currentThreadId();
    while(keep_running){
        cap.read(frame);
        new_frames->push_back(frame);
        flag_mutex.lock();
        if (object_detector_done_bool){
            object_detector_done_bool = false;
            webcam_done_cv.wakeAll();
            all_done_cv.wait(&flag_mutex);
        }
        flag_mutex.unlock();
    }
}

void Webcam::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}
