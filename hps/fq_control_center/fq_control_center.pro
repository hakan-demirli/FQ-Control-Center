QT += core gui
QT += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES -= QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera_loop.cpp \
    fps.cpp \
    gas_sensors.cpp \
    main.cpp \
    mainwindow.cpp \
    object_detector.cpp \
    object_tracker.cpp \
    settings.cpp

HEADERS += \
    camera_loop.h \
    constants.h \
    fps.h \
    gas_sensors.h \
    hps.h \
    json.hpp \
    mainwindow.h \
    object_detector.h \
    object_tracker.h \
    settings.h

FORMS += \
    mainwindow.ui

# message($$QMAKESPEC)

QMAKE_CXXFLAGS += -Wno-deprecated-copy
#QMAKE_CXXFLAGS += -Wdouble-promotion
#QMAKE_CXXFLAGS += -Wfloat-conversion

# [TODO] Remove unused opencv libraries

linux-arm-gnueabihf-g++: {
    message( "found arm g++" )
    QMAKE_CXXFLAGS_RELEASE += -Wno-expansion-to-defined
    QMAKE_CXXFLAGS_RELEASE += -Wno-unused-parameter
    QMAKE_CXXFLAGS_RELEASE += -isystem $$PWD/opencv/include/opencv4
    QMAKE_CXXFLAGS_RELEASE += -isystem /usr/arm-linux-gnueabihf/include

    INCLUDEPATH += $$PWD/opencv/include/opencv4
    LIBS += -L$$PWD/opencv/lib \
    -lopencv_stitching\
    -lopencv_superres\
    -lopencv_videostab\
    -lopencv_aruco\
    -lopencv_bgsegm\
    -lopencv_bioinspired\
    -lopencv_ccalib\
    -lopencv_dnn\
    -lopencv_dpm\
    -lopencv_fuzzy\
    -lopencv_line_descriptor\
    -lopencv_optflow\
    -lopencv_plot\
    -lopencv_reg\
    -lopencv_saliency\
    -lopencv_stereo\
    -lopencv_structured_light\
    -lopencv_rgbd\
    -lopencv_surface_matching\
    -lopencv_tracking\
    -lopencv_datasets\
    -lopencv_text\
    -lopencv_face\
    -lopencv_shape\
    -lopencv_video\
    -lopencv_ximgproc\
    -lopencv_calib3d\
    -lopencv_features2d\
    -lopencv_flann\
    -lopencv_xobjdetect\
    -lopencv_objdetect\
    -lopencv_ml\
    -lopencv_xphoto\
    -lopencv_highgui\
    -lopencv_videoio\
    -lopencv_imgcodecs\
    -lopencv_photo\
    -lopencv_imgproc\
    -lopencv_core\
}

linux-g++: {
    message( "found x86 g++" )
    INCLUDEPATH += opencv_x86/
    LIBS += -Lopencv_x86/ \
    -ltbb\
    -lopencv_stitching\
    -lopencv_superres\
    -lopencv_videostab\
    -lopencv_aruco\
    -lopencv_bgsegm\
    -lopencv_bioinspired\
    -lopencv_ccalib\
    -lopencv_dnn\
    -lopencv_dpm\
    -lopencv_fuzzy\
    -lopencv_line_descriptor\
    -lopencv_optflow\
    -lopencv_plot\
    -lopencv_reg\
    -lopencv_saliency\
    -lopencv_stereo\
    -lopencv_structured_light\
    -lopencv_rgbd\
    -lopencv_surface_matching\
    -lopencv_tracking\
    -lopencv_datasets\
    -lopencv_text\
    -lopencv_face\
    -lopencv_shape\
    -lopencv_video\
    -lopencv_ximgproc\
    -lopencv_calib3d\
    -lopencv_features2d\
    -lopencv_flann\
    -lopencv_xobjdetect\
    -lopencv_objdetect\
    -lopencv_ml\
    -lopencv_xphoto\
    -lopencv_highgui\
    -lopencv_videoio\
    -lopencv_imgcodecs\
    -lopencv_photo\
    -lopencv_imgproc\
    -lopencv_core\
}


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


QMAKE_CXXFLAGS+= -std=gnu++14

QMAKE_CXXFLAGS_RELEASE += -Ofast

DISTFILES += \
    config/bench_records.txt \
    config/camera_cfg.json \
    config/compile_commands.json \
    config/gas_sensors_cfg.json \
    config/info.json \
    data/gas_sensor_data_0.log \
    data/gas_sensor_data_1.log \
    data/gas_sensor_data_2.log \
    data/gas_sensor_data_3.log

