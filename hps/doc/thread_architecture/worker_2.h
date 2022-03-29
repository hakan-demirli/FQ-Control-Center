#ifndef WORKER_2_H
#define WORKER_2_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include <thread>
#include <atomic>
#include <deque>


class Worker_2: public QObject {
    Q_OBJECT
private:
    explicit Worker_2(QWaitCondition& w2_done_cv,
                      QWaitCondition& all_done_cv,
                      QMutex& flag_mutex,
                      bool& w1_done_bool,
                      QObject *parent = nullptr);

public:
    static Worker_2& getInstance(QWaitCondition& w2_done_cv,
                                 QWaitCondition& all_done_cv,
                                 QMutex& flag_mutex,
                                 bool& w1_done_bool,
                                 QObject *parent = nullptr);
    void operator=(Worker_2 const&) = delete;
    Worker_2(Worker_2 const&) = delete;
    ~Worker_2();

public:
    void run(int* dbg_int);
    std::vector<int>* tracking_frames;

    std::atomic_flag keep_running;

private:
    QThread m_thread;
    QWaitCondition& w2_done_cv;
    QWaitCondition& all_done_cv;
    QMutex& flag_mutex;
    bool& w1_done_bool;
    int* dbg_int;

public slots:
    void main_loop(void);
};

#endif// WORKER_2_H
