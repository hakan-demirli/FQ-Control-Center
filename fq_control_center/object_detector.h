#ifndef OBJECT_DETECTOR_H
#define OBJECT_DETECTOR_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include "fps.h"
#include "json.hpp"


using json = nlohmann::json;

class ObjectDetector: public QObject {
    Q_OBJECT
private:
    const json cfg;
    explicit ObjectDetector(json cfg,
                            QWaitCondition& all_done_cv,
                            QMutex& flag_mutex,
                            bool& object_detector_done_bool,
                            QObject *parent = nullptr);

public:
    static ObjectDetector& getInstance(json cfg,
                                       QWaitCondition& all_done_cv,
                                       QMutex& flag_mutex,
                                       bool& object_detector_done_bool,
                                       QObject *parent = nullptr);
    void operator=(ObjectDetector const&) = delete;
    ObjectDetector(ObjectDetector const&) = delete;
    ~ObjectDetector();
    void run();

    bool keep_running;
    bool toggle_object_detection;

    cv::Mat * detecting_results;
    cv::Mat * detecting_frame;

private:
    QThread m_thread;
    QWaitCondition& all_done_cv;
    QMutex& flag_mutex;
    bool& object_detector_done_bool;
    const std::string all_models_path;
    const std::string model_folder;
    const std::string model_file;
    const std::string model_config_file;
    cv::dnn::dnn4_v20191202::Net net;

public slots:
    void main_loop();
};

#endif // OBJECT_DETECTOR_H
