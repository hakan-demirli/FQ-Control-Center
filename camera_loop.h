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

# include <QDebug>

using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
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

    json cfg;

private:
    QThread m_thread;
    const int t_delta_detection = 500000; //micro seconds
    const std::string all_models_path;
    const std::string model_folder;
    const std::string model_file;
    const std::string model_config_file;
    Webcam& usb_webcam;

    fps cro;
    cv::Scalar mean_blob = cv::Scalar(127.5, 127.5, 127.5);
    cv::Mat image;

public slots:
    void main_loop(void);

signals:
    void sendValue(int newValue);
    void sendFrame(cv::Mat frame);
    void sendStats(long compute_time);
};



#endif // CAMERA_LOOP_H
