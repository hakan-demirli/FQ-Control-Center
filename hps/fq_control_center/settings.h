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
    json gas_sensors_cfg;

private:
    std::fstream camera_stream;
    std::fstream gas_sensors_stream;

    const std::string CAMERA_CFG_FILE = "./config/camera_cfg.json";
    const std::string GAS_SENSORS_CFG_FILE = "./config/gas_sensors_cfg.json";

public slots:
    void save_cfg_files();
};

#endif // SETTINGS_H
