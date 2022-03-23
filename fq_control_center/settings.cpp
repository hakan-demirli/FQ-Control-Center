#include "settings.h"


Settings::Settings(QObject *parent):
    QObject(parent),
    camera_stream(CAMERA_CFG_FILE, std::ios::in | std::ios::out),
    gas_sensors_stream(GAS_SENSORS_CFG_FILE, std::ios::in | std::ios::out)
{
    camera_stream.open(CAMERA_CFG_FILE, std::ios::in | std::ios::out);
    gas_sensors_stream.open(GAS_SENSORS_CFG_FILE, std::ios::in | std::ios::out);

    camera_stream >> camera_cfg;
    gas_sensors_stream >> gas_sensors_cfg;
}

void Settings::save_cfg_files(){
    camera_stream << std::setw(4) << camera_cfg;
    gas_sensors_stream << std::setw(4) << gas_sensors_cfg;
}
