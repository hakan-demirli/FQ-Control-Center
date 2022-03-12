#ifndef CAMERA_LOOP_H
#define CAMERA_LOOP_H

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QThread>

#include "fps.h"
#include "webcam.h"
#include "json.hpp"

using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
public:
    explicit CameraLoop(json cfg, QObject *parent = nullptr);
    ~CameraLoop();
    void run(void);

    bool toggle_stream;
    json cfg;

private:
    QThread m_thread;
    int value;
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
    void sendFrame(cv::Mat frame, long inference_time);
};



#endif // CAMERA_LOOP_H
