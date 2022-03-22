#include "worker_0.h"


Worker_0::Worker_0(QWaitCondition& w0_done_cv,
                   QWaitCondition& all_done_cv,
                   QMutex& flag_mutex,
                   bool& w1_done_bool,
                   QObject *parent) :
    QObject(parent),
    w0_done_cv(w0_done_cv),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    w1_done_bool(w1_done_bool)
{
    qDebug() << "Creating Worker_0 Object";
    keep_running.test_and_set();
    moveToThread(&m_thread);
    m_thread.start();
}

Worker_0::~Worker_0(){

    qDebug() << "Destroying Worker_0 Object";
    if( m_thread.isRunning() )
    {
        try {w0_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

Worker_0& Worker_0::getInstance(QWaitCondition& w0_done_cv,
                                QWaitCondition& all_done_cv,
                                QMutex& flag_mutex,
                                bool& w1_done_bool,
                                QObject *parent)
{
    static Worker_0 instance(w0_done_cv,all_done_cv,flag_mutex,w1_done_bool,parent);
    return instance;
}

void Worker_0::main_loop(){

    qDebug() << "Worker_0::main_loop thread id:" << QThread::currentThreadId();
    while(keep_running.test_and_set()){
        QThread::msleep(50);
        *dbg_int = (*dbg_int) + 1;
        new_frames->push_back(*dbg_int);
        // upside is the real work

        flag_mutex.lock();
        //qDebug() << "Worker_0:" << *dbg_int;
        if (w1_done_bool){
            w1_done_bool = false;
            w0_done_cv.wakeAll();
            all_done_cv.wait(&flag_mutex);
        }
        flag_mutex.unlock();
    }
}

void Worker_0::run(int* dbg_int){
    this->dbg_int = dbg_int;
    QMetaObject::invokeMethod( this, "main_loop");
}
