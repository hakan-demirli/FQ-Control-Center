#include "camera_loop.h"


CameraLoop::CameraLoop(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    object_detector_done_bool(true),
    object_tracker_done_bool(true),
    new_frames(&frames_0),
    detecting_frames(&frames_1),
    tracking_frames(&frames_2),
    tracking_results(&results_0),
    detecting_results(&results_1),
    object_tracker(ObjectTracker::getInstance(cfg,all_done_cv,flag_mutex,object_tracker_done_bool,parent)),
    object_detector(ObjectDetector::getInstance(cfg,all_done_cv,flag_mutex,object_detector_done_bool,parent))
{
    qDebug() << "Creating CameraLoop Object";

    cap = cv::VideoCapture((int)cfg["Source"]);
    cap.set(cv::CAP_PROP_BUFFERSIZE, int(cfg["Camera Buffer Size"]));
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, (int)cfg["Size"][1]);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, (int)cfg["Size"][0]);

    moveToThread(&m_thread);
    m_thread.start();

    cap.read(frame);
    new_frames->push_back(frame);

    // swap frame packets so object detection can start with a frame
    std::swap(tracking_frames,detecting_frames);
    std::swap(new_frames,detecting_frames);

    object_detector.detecting_frame = &detecting_frames->front();
    object_detector.detecting_results = detecting_results;

    object_tracker.tracking_frames = tracking_frames;
    object_tracker.tracking_results = tracking_results;

    object_detector.run();

    object_tracker.run();
}

CameraLoop::~CameraLoop(){


    qDebug() << "Destroying PlayGround Object";
    object_detector.keep_running = false;
    object_tracker.keep_running = false;

    if( m_thread.isRunning() )
    {
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
    cpu_set_t my_set;        /* Define your cpu_set bit mask. */
    CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
    CPU_SET(0, &my_set);     /* set the bit that represents core 0. */
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of tihs process to */
                                                      /* the defined mask, i.e. only 0. */
    // This thread controls the all other threads. Give highpriority.
    //QThread::currentThread()->setPriority(QThread::HighPriority);
    // wait until webcam is done
    // if it is done object detection is also done
    // swap all containers
    // tell everyone to wake-up

    //    No need to wait for tracking
    //    In worst case jumping couple of frames ahead won't cause serious problems
    while (keep_running) {
        cap.read(frame);
        new_frames->push_back(frame);
        flag_mutex.lock();
        if (object_detector_done_bool && object_tracker_done_bool){
            object_detector_done_bool = false;
            object_tracker_done_bool = false;

            std::swap(tracking_frames,detecting_frames);
            std::swap(new_frames,detecting_frames);
            std::swap(tracking_results,detecting_results);

            object_detector.detecting_frame = &detecting_frames->front();
            object_detector.detecting_results = detecting_results;

            object_tracker.tracking_frames = tracking_frames;
            object_tracker.tracking_results = tracking_results;

            all_done_cv.wakeAll();
        }
        flag_mutex.unlock();
    }
}
