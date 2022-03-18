#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

#include <iostream>
#include <fstream>

#include "json.hpp"


using json = nlohmann::json;

class Settings: public QObject {
    Q_OBJECT

public:
    explicit Settings(QObject *parent = nullptr);

    json camera_cfg;
    json camera_ui_cfg;

private:
    std::fstream camera_stream;
    std::fstream camera_ui_stream;

    const std::string CAMERA_UI_CFG_FILE = "./config/camera_ui_cfg.json";
    const std::string CAMERA_CFG_FILE = "./config/camera_cfg.json";

public slots:
    void save_cfg_files();
};

#endif // SETTINGS_H
