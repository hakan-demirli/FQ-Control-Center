#ifndef PLAY_GROUND_H
#define PLAY_GROUND_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include <thread>
#include <atomic>

#include "worker_0.h"
#include "worker_1.h"
#include "worker_2.h"


class PlayGround: public QObject {
    Q_OBJECT
private:
    explicit PlayGround(QObject *parent = nullptr);

    QThread m_thread;
    bool w1_done_bool;
    QWaitCondition w0_done_cv;
    QWaitCondition w1_done_cv;
    QWaitCondition w2_done_cv;
    QWaitCondition all_done_cv;
    QMutex flag_mutex;
    QMutex w0_done_mutex;
    QMutex w1_done_mutex;
    QMutex w2_done_mutex;

    std::vector<int> frames_0;
    std::vector<int> frames_1;
    std::vector<int> frames_2;
    std::vector<int>* new_frames;
    std::vector<int>* detecting_frames;
    std::vector<int>* tracking_frames;

    int dbg_int;

    Worker_0& w0;
    Worker_1& w1;
    Worker_2& w2;

public:
    static PlayGround& getInstance(QObject *parent = nullptr);
    void operator=(PlayGround const&) = delete;
    PlayGround(PlayGround const&) = delete;
    ~PlayGround();

    void run();

    std::atomic_flag keep_running;

public slots:
    void main_loop(void);
};

#endif // PLAY_GROUND_H
