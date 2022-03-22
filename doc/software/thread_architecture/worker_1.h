#ifndef WORKER_1_H
#define WORKER_1_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include <thread>
#include <atomic>
#include <deque>


class Worker_1: public QObject {
    Q_OBJECT
private:
    explicit Worker_1(QWaitCondition& w1_done_cv,
                      QWaitCondition& all_done_cv,
                      QMutex& flag_mutex,
                      bool& w1_done_bool,
                      QObject *parent = nullptr);

public:
    static Worker_1& getInstance(QWaitCondition& w1_done_cv,
                                 QWaitCondition& all_done_cv,
                                 QMutex& flag_mutex,
                                 bool& w1_done_bool,
                                 QObject *parent = nullptr);
    void operator=(Worker_1 const&) = delete;
    Worker_1(Worker_1 const&) = delete;
    ~Worker_1();

public:
    void run(int* dbg_int);
    int* detecting_frame;

    std::atomic_flag keep_running;

private:
    QThread m_thread;
    QWaitCondition& w1_done_cv;
    QWaitCondition& all_done_cv;
    QMutex& flag_mutex;
    bool& w1_done_bool;
    int* dbg_int;

public slots:
    void main_loop(void);
};

#endif// WORKER_1_H
