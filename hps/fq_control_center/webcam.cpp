#include "webcam.h"


Webcam::Webcam(json cfg,
               QWaitCondition& webcam_done_cv,
               QMutex& flag_mutex,
               bool& object_detector_done_bool,
               bool& object_tracker_done_bool,
               QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    webcam_done_cv(webcam_done_cv),
    flag_mutex(flag_mutex),
    object_detector_done_bool(object_detector_done_bool),
    object_tracker_done_bool(object_tracker_done_bool)
{
    cap = cv::VideoCapture((int)cfg["Source"]);
    cap.set(cv::CAP_PROP_BUFFERSIZE, int(cfg["Camera Buffer Size"]));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, (int)cfg["Size"][1]);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, (int)cfg["Size"][0]);
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
                            QMutex& flag_mutex,
                            bool& object_detector_done_bool,
                            bool& object_tracker_done_bool,
                            QObject *parent)
{
    static Webcam instance(cfg,webcam_done_cv,flag_mutex,object_detector_done_bool,object_tracker_done_bool,parent);
    return instance;
}

void Webcam::main_loop(){

    qDebug() << "Webcam::main_loop thread id:" << QThread::currentThreadId();
    cpu_set_t my_set;        /* Define your cpu_set bit mask. */
    CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
    CPU_SET(0, &my_set);     /* set the bit that represents core 0. */
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of tihs process to */
                                                      /* the defined mask, i.e. only 0. */
    // read and store frames until object detection is done
    // then wait for the all_done signal
    while(keep_running){
        cap.read(frame);
        new_frames->push_back(frame);
        flag_mutex.lock();
        if (object_detector_done_bool && object_tracker_done_bool){
            object_detector_done_bool = false;
            object_tracker_done_bool = false;
            webcam_done_cv.wakeAll();
        }
        flag_mutex.unlock();
    }
}

void Webcam::run(){
    cap.read(frame);
    new_frames->push_back(frame);
    QMetaObject::invokeMethod( this, "main_loop");
}
