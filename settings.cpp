#include "settings.h"


Settings::Settings(QObject *parent):
    QObject(parent),
    camera_stream(CAMERA_CFG_FILE,std::ofstream::in | std::ofstream::out),
    ui_stream(UI_CFG_FILE,std::ofstream::in | std::ofstream::out)
{
    camera_stream >> camera_cfg;
    ui_stream >> ui_cfg;
}

void Settings::save_cfg_files(){
    camera_stream << camera_cfg;
    ui_stream << ui_cfg;
}
