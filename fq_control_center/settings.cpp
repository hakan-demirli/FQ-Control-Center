#include "settings.h"


Settings::Settings(QObject *parent):
    QObject(parent)
{
    camera_stream.open(CAMERA_CFG_FILE, std::ios::in | std::ios::out);
    ui_stream.open(UI_CFG_FILE, std::ios::in | std::ios::out);
    camera_stream >> camera_cfg;
    ui_stream >> ui_cfg;
}

void Settings::save_cfg_files(){
    camera_stream << camera_cfg;
    ui_stream << ui_cfg;
}
