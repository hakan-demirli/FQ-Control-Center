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

#include "json.hpp"
#include "fps.h"
#include "webcam.h"

using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
public:
    explicit CameraLoop(QObject *parent = nullptr);
    ~CameraLoop();
    void run(void);

    const std::string INFO_FILE = "./config/info.json";
    const std::string CFG_FILE = "./config/cfg.json";
    const std::string BENCH_FILE = "./config/bench_records.txt";

    bool toggle_stream;

private:
    void read_json(const std::string json_file, json& j);
    void read_model_labels(const std::string label_list_file,std::vector<std::string>& labels);


    QThread m_thread;
    int value;

public slots:
    void receiveValue(int value);
    void main_loop(void);

signals:
    void sendValue(int newValue);
    void sendFrame(cv::Mat frame, long inference_time);
};



#endif // CAMERA_LOOP_H
