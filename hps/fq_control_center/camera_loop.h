#ifndef CAMERA_LOOP_H
#define CAMERA_LOOP_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include <opencv2/core/utility.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QWaitCondition>
#include <QSemaphore>
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QDebug>
#include <QMutex>

#include "fps.h"
#include "json.hpp"
#include "object_tracker.h"
#include "object_detector.h"


using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
private:
    const json cfg;
public:
    bool keep_running;
private:
    explicit CameraLoop(json cfg, QObject *parent = nullptr);
    void detect_objects(void);
    void create_bounding_boxes_confidences_and_trackers(void);
    void remove_duplicate_bounding_boxes_confidences_and_trackers(void);
    void tracking_loop();

    QThread m_thread;
    bool object_detector_done_bool;
    bool object_tracker_done_bool;
    QWaitCondition all_done_cv;
    QMutex flag_mutex;

    std::vector<cv::Mat> frames_0;
    std::vector<cv::Mat> frames_1;
    std::vector<cv::Mat> frames_2;
    std::vector<cv::Mat>* new_frames;
    std::vector<cv::Mat>* detecting_frames;
    std::vector<cv::Mat>* tracking_frames;
    cv::Mat results_0;
    cv::Mat results_1;
    cv::Mat* tracking_results;
    cv::Mat* detecting_results;

public:
    static CameraLoop& getInstance(json cfg, QObject *parent = nullptr);
    CameraLoop(CameraLoop const&) = delete;
    void operator=(CameraLoop const&) = delete;
    ~CameraLoop();
    void run(void);

    cv::VideoCapture cap;
    cv::Mat frame;
    fps cro;

    ObjectTracker& object_tracker;
    ObjectDetector& object_detector;

public slots:
    void main_loop(void);

};



#endif // CAMERA_LOOP_H
