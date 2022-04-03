#ifndef WORKER_0_H
#define WORKER_0_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include <thread>
#include <atomic>
#include <deque>


class Worker_0: public QObject {
    Q_OBJECT
private:
    explicit Worker_0(QWaitCondition& w0_done_cv,
                      QWaitCondition& all_done_cv,
                      QMutex& flag_mutex,
                      bool& w1_done_bool,
                      QObject *parent = nullptr);

public:
    static Worker_0& getInstance(QWaitCondition& w0_done_cv,
                                 QWaitCondition& all_done_cv,
                                 QMutex& flag_mutex,
                                 bool& w1_done_bool,
                                 QObject *parent = nullptr);
    void operator=(Worker_0 const&) = delete;
    Worker_0(Worker_0 const&) = delete;
    ~Worker_0();

public:
    void run(int* dbg_int);
    std::vector<int>* new_frames;

    std::atomic_flag keep_running;
private:
    QThread m_thread;

    QWaitCondition& w0_done_cv;
    QWaitCondition& all_done_cv;
    QMutex& flag_mutex;
    bool& w1_done_bool;
    int* dbg_int;


public slots:
    void main_loop(void);
};

#endif// WORKER_0_H
