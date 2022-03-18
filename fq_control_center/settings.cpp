#include "settings.h"


Settings::Settings(QObject *parent):
    QObject(parent)
{
    camera_stream.open(CAMERA_CFG_FILE, std::ios::in | std::ios::out);
    camera_ui_stream.open(CAMERA_UI_CFG_FILE, std::ios::in | std::ios::out);
    camera_stream >> camera_cfg;
    camera_ui_stream >> camera_ui_cfg;
}

void Settings::save_cfg_files(){
    camera_stream << camera_cfg;
    camera_ui_stream << camera_ui_cfg;
}
