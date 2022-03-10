#ifndef CAMERA_LOOP_H
#define CAMERA_LOOP_H

#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>

#include "json.hpp"
#include "fps.h"
#include "webcam.h"

#include <QObject>

using json = nlohmann::json;

class CameraLoop: public QObject {
    Q_OBJECT
public:
    void tic();
    long toc();
    const std::string INFO_FILE = "./info.json";
    const std::string CFG_FILE = "./cfg.json";
    const std::string BENCH_FILE = "./bench_records.txt";
    explicit CameraLoop(QObject *parent = nullptr);

private:
    void read_json(const std::string json_file, json& j);
    void read_model_labels(const std::string label_list_file,std::vector<std::string>& labels);
    void main_loop(void);

    int value;

public slots:
    void receiveValue(int value);

signals:
    void sendValue(int newValue);
};



#endif // CAMERA_LOOP_H
