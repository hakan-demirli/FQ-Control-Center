QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera_loop.cpp \
    fps.cpp \
    main.cpp \
    mainwindow.cpp \
    settings.cpp \
    webcam.cpp

HEADERS += \
    camera_loop.h \
    fps.h \
    json.hpp \
    mainwindow.h \
    settings.h \
    webcam.h

FORMS += \
    mainwindow.ui

# message($$QMAKESPEC)

linux-arm-gnueabihf-g++: {
    message( "found arm g++" )
    INCLUDEPATH += /home/emre/cross_compile/OpenCV/install/include/opencv4
    LIBS += -L/home/emre/cross_compile/OpenCV/install/lib \
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

linux-g++: {
    message( "found x86 g++" )
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -L/usr/lib \
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

