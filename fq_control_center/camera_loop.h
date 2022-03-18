#ifndef CAMERA_LOOP_H
#define CAMERA_LOOP_H

#include <iostream>
#include <fstream>

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

#include "fps.h"
#include "webcam.h"
#include "json.hpp"

#include <QDebug>


using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
private:
    json cfg;
public:
    explicit CameraLoop(json cfg, QObject *parent = nullptr);
    ~CameraLoop();
    void run(void);

    bool toggle_stream;
    bool toggle_video;
    bool toggle_stats;
    bool toggle_object_detection;
    bool toggle_tracking;
    bool toggle_bounding_boxes;

private:
    void detect_objects(void);
    void create_bounding_boxes_confidences_and_trackers(void);
    void remove_duplicate_bounding_boxes_confidences_and_trackers(void);

    QThread m_thread;
    const std::string all_models_path;
    const std::string model_folder;
    const std::string model_file;
    const std::string model_config_file;
    Webcam& usb_webcam;

    cv::Scalar mean_blob = cv::Scalar(127.5, 127.5, 127.5);
    cv::Mat image;
    cv::Mat results;

    QVector<cv::Ptr<cv::Tracker>> bunch_of_trackers;
    std::vector<unsigned int> tracker_total_missed_frames;
    std::vector<cv::Rect2d> rois;
    std::vector<float> confidences;
    unsigned int total_frames;
    unsigned int tracker_last_index;
    std::vector<int> indices;
    fps cro_all;

public slots:
    void main_loop(void);

signals:
    void sendValue(int newValue);
    void sendFrame(cv::Mat frame);
    void sendStats(long compute_time);
};



#endif // CAMERA_LOOP_H
