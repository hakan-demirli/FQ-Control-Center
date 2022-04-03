#include "play_ground.h"


PlayGround::PlayGround(QObject *parent) :
    QObject(parent),
    w1_done_bool(false),
    new_frames(&frames_0),
    detecting_frames(&frames_1),
    tracking_frames(&frames_2),
    w0(Worker_0::getInstance(w0_done_cv,all_done_cv,flag_mutex,w1_done_bool,nullptr)),
    w1(Worker_1::getInstance(w1_done_cv,all_done_cv,flag_mutex,w1_done_bool,nullptr)),
    w2(Worker_2::getInstance(w2_done_cv,all_done_cv,flag_mutex,w1_done_bool,nullptr))
{
    qDebug() << "Creating PlayGround Object";
    keep_running.test_and_set();
    moveToThread(&m_thread);
    m_thread.start();


    w0.new_frames = new_frames;
    detecting_frames->push_back(0); // uninitialized access bypass
    w1.detecting_frame = &detecting_frames->front();
    w2.tracking_frames = tracking_frames;

    dbg_int = 0;
    w0.run(&dbg_int);
    w1.run(&dbg_int);
    w2.run(&dbg_int);
}

PlayGround::~PlayGround(){
    qDebug() << "Destroying PlayGround Object";
    w0.keep_running.clear();
    w1.keep_running.clear();
    w2.keep_running.clear();

    if( m_thread.isRunning() )
    {
        try {w0_done_cv.wakeAll();} catch(int t){}
        try {w1_done_cv.wakeAll();} catch(int t){}
        try {w2_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

PlayGround& PlayGround::getInstance(QObject *parent)
{
    static PlayGround instance(parent);
    return instance;
}

void PlayGround::main_loop(){

    qDebug() << "PlayGround::main_loop thread id:" << QThread::currentThreadId();

    while(keep_running.test_and_set()){

        flag_mutex.lock();
        w0_done_cv.wait(&flag_mutex);
        qDebug() << "PlayGround:";
        std::swap(tracking_frames,detecting_frames);
        std::swap(new_frames,detecting_frames);
        w0.new_frames = new_frames;
        w1.detecting_frame = &detecting_frames->front();
        w2.tracking_frames = tracking_frames;
        all_done_cv.wakeAll();
        flag_mutex.unlock();
    }
}

void PlayGround::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}
