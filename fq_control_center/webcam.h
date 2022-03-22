#ifndef WEBCAM_H
#define WEBCAM_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

#include "fps.h"
#include "json.hpp"


using json = nlohmann::json;

class Webcam: public QObject {
    Q_OBJECT
private:
    json cfg;
    explicit Webcam(json cfg,
                    QWaitCondition& webcam_done_cv,
                    QWaitCondition& all_done_cv,
                    QMutex& flag_mutex,
                    bool& object_detector_done_bool,
                    QObject *parent = nullptr);

public:
    static Webcam& getInstance(json cfg,
                               QWaitCondition& webcam_done_cv,
                               QWaitCondition& all_done_cv,
                               QMutex& flag_mutex,
                               bool& object_detector_done_bool,
                               QObject *parent = nullptr);
    void operator=(Webcam const&) = delete;
    Webcam(Webcam const&) = delete;
    ~Webcam();
    void run();

    std::vector<cv::Mat>* new_frames;
    unsigned long int* new_per_frame_period;
    bool keep_running;

private:
    QThread m_thread;

    QWaitCondition& webcam_done_cv;
    QWaitCondition& all_done_cv;
    QMutex& flag_mutex;
    bool& object_detector_done_bool;

    cv::VideoCapture cap;
    cv::Mat frame;
    fps cro;

public slots:
    void main_loop(void);
};

#endif// WEBCAM_H
