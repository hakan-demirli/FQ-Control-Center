#ifndef OBJECT_TRACKER_H
#define OBJECT_TRACKER_H

#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>

#include "fps.h"
#include "json.hpp"


using json = nlohmann::json;

class ObjectTracker: public QObject {
    Q_OBJECT
private:
    const json cfg;
    explicit ObjectTracker(json cfg,
                           QWaitCondition& object_tracker_done_cv,
                           QWaitCondition& all_done_cv,
                           QMutex& object_tracker_done_mutex,
                           QObject *parent = nullptr);
public:
    static ObjectTracker& getInstance(json cfg,
                                      QWaitCondition& object_tracker_done_cv,
                                      QWaitCondition& all_done_cv,
                                      QMutex& object_tracker_done_mutex,
                                      QObject *parent = nullptr);
    void operator=(ObjectTracker const&) = delete;
    ObjectTracker(ObjectTracker const&) = delete;
    ~ObjectTracker();
    void run();

    bool keep_running;
    bool toggle_video;
    bool toggle_boundary_line;
    bool toggle_object_tracking;
    bool toggle_bounding_boxes;
    bool toggle_middle_point;
    bool toggle_stats;
    bool object_tracker_done;

    std::vector<cv::Mat>* tracking_frames;
    cv::Mat* tracking_results;

private:
    void create_bounding_boxes_confidences_and_trackers();
    void remove_duplicate_bounding_boxes_confidences_and_trackers();
    void track_and_emit();

    QThread m_thread;
    QWaitCondition& object_tracker_done_cv;
    QWaitCondition& all_done_cv;
    QMutex& object_tracker_done_mutex;

    QVector<cv::Ptr<cv::Tracker>> bunch_of_trackers;
    std::vector<unsigned int> tracker_total_missed_frames;
    std::vector<cv::Rect2d> rois;
    std::vector<float> confidences;
    std::vector<int> indices;
    std::vector<unsigned int> stats;
    fps cro_tracking;

public slots:
    void main_loop();

signals:
    void sendFrame(cv::Mat frame);
    void sendStats(std::vector<unsigned int>);
};

#endif // OBJECT_TRACKER_H
