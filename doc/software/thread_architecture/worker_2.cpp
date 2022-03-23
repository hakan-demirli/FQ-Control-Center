#include "worker_2.h"


Worker_2::Worker_2(QWaitCondition& w2_done_cv,
                   QWaitCondition& all_done_cv,
                   QMutex& flag_mutex,
                   bool& w1_done_bool,
                   QObject *parent) :
    QObject(parent),
    w2_done_cv(w2_done_cv),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    w1_done_bool(w1_done_bool)
{
    qDebug() << "Creating Worker_2 Object";
    keep_running.test_and_set();
    moveToThread(&m_thread);
    m_thread.start();
}

Worker_2::~Worker_2(){

    qDebug() << "Destroying Worker_2 Object";

    if( m_thread.isRunning() )
    {
        try {w2_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

Worker_2& Worker_2::getInstance(QWaitCondition& w2_done_cv,
                                QWaitCondition& all_done_cv,
                                QMutex& flag_mutex,
                                bool& w1_done_bool,
                                QObject *parent)
{
    static Worker_2 instance(w2_done_cv,all_done_cv,flag_mutex,w1_done_bool,parent);
    return instance;
}

void Worker_2::main_loop(){

    qDebug() << "Worker_2::main_loop thread id:" << QThread::currentThreadId();

    while(keep_running.test_and_set()){
        for(const auto& fr : *tracking_frames){
            qDebug() << "Worker_2:" << fr;
        }
        tracking_frames->clear();
        //upside is the real work
        flag_mutex.lock();
        w2_done_cv.wakeAll();
        all_done_cv.wait(&flag_mutex);
        flag_mutex.unlock();
    }
}

void Worker_2::run(int* dbg_int){
    this->dbg_int = dbg_int;
    QMetaObject::invokeMethod( this, "main_loop");
}
