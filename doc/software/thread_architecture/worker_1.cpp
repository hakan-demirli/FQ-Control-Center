#include "worker_1.h"


Worker_1::Worker_1(QWaitCondition& w1_done_cv,
                   QWaitCondition& all_done_cv,
                   QMutex& flag_mutex,
                   bool& w1_done_bool,
                   QObject *parent) :
    QObject(parent),
    w1_done_cv(w1_done_cv),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    w1_done_bool(w1_done_bool)
{
    qDebug() << "Creating Worker_1 Object";
    keep_running.test_and_set();
    moveToThread(&m_thread);
    m_thread.start();
}

Worker_1::~Worker_1(){

    qDebug() << "Destroying Worker_1 Object";
    if( m_thread.isRunning() )
    {
        try {w1_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

Worker_1& Worker_1::getInstance(QWaitCondition& w1_done_cv,
                                QWaitCondition& all_done_cv,
                                QMutex& flag_mutex,
                                bool& w1_done_bool,
                                QObject *parent)
{
    static Worker_1 instance(w1_done_cv,all_done_cv,flag_mutex,w1_done_bool,parent);
    return instance;
}

void Worker_1::main_loop(){

    qDebug() << "Worker_1::main_loop thread id:" << QThread::currentThreadId();
    while(keep_running.test_and_set()){
        QThread::msleep(1000);
        *detecting_frame = *detecting_frame*(-1);
        // upside is the real work

        flag_mutex.lock();
        //qDebug() << "Worker_1:" << *detecting_frame;
        w1_done_bool = true;
        w1_done_cv.wakeAll();
        all_done_cv.wait(&flag_mutex);
        flag_mutex.unlock();
    }
}

void Worker_1::run(int* dbg_int){
    this->dbg_int = dbg_int;
    QMetaObject::invokeMethod( this, "main_loop");
}
