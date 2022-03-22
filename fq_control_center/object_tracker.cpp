#include "object_tracker.h"


ObjectTracker::ObjectTracker(json cfg,
                             QWaitCondition& object_tracker_done_cv,
                             QWaitCondition& all_done_cv,
                             QMutex& object_tracker_done_mutex,
                             QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    toggle_video(cfg["Run"]=="||"),
    toggle_object_tracking(cfg["Object Tracking"]=="||"),
    toggle_bounding_boxes(cfg["Bounding Boxes"]=="||"),
    toggle_middle_point(cfg["Middle Point"]=="||"),
    toggle_stats(cfg["Stats"]=="||"),
    object_tracker_done(true),
    object_tracker_done_cv(object_tracker_done_cv),
    all_done_cv(all_done_cv),
    object_tracker_done_mutex(object_tracker_done_mutex)
{
    moveToThread(&m_thread);
    m_thread.start();
}

ObjectTracker& ObjectTracker::getInstance(json cfg,
                                          QWaitCondition& object_tracker_done_cv,
                                          QWaitCondition& all_done_cv,
                                          QMutex& object_tracker_done_mutex,
                                          QObject *parent)
{
    static ObjectTracker instance(cfg, object_tracker_done_cv,all_done_cv,object_tracker_done_mutex,parent);
    return instance;
}

ObjectTracker::~ObjectTracker(){

    qDebug() << "Destroying ObjectTracker Object";
    if( m_thread.isRunning() )
    {
        try {object_tracker_done_cv.wakeAll();} catch(int t){}
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

inline void ObjectTracker::create_bounding_boxes_confidences_and_trackers(){

    // create bounding boxes, thresholds and confidences from object detection results
    int bboxX;
    int bboxY;
    int bboxWidth;
    int bboxHeight;
    int class_id;
    float confidence;

    for (int i = 0; i < tracking_results->rows; i++){
        class_id = int(tracking_results->at<float>(i, 1));
        confidence = tracking_results->at<float>(i, 2);

        if (confidence > cfg["Minimum Confidence Score"] &&  class_id == 1){
            bboxX = int(tracking_results->at<float>(i, 3) * tracking_frames->front().cols);
            bboxY = int(tracking_results->at<float>(i, 4) * tracking_frames->front().rows);
            bboxWidth = int(tracking_results->at<float>(i, 5) * tracking_frames->front().cols - bboxX);
            bboxHeight = int(tracking_results->at<float>(i, 6) * tracking_frames->front().rows - bboxY);
            rois.push_back(cv::Rect2d(bboxX,bboxY,bboxWidth,bboxHeight));
            confidences.push_back(confidence);
            bunch_of_trackers.push_back(cv::TrackerMOSSE::create());
            bunch_of_trackers.back()->init(tracking_frames->front(),rois.back());
            tracker_total_missed_frames.push_back(0);
        }
    }
}

inline void ObjectTracker::remove_duplicate_bounding_boxes_confidences_and_trackers(){
    // apply non maximum suppresion to find duplicate bounding boxes
    cv::dnn::NMSBoxes(rois, confidences, cfg["Minimum Confidence Score"], cfg["NMS Threshold"], indices);

    // remove duplicate bounding boxes and their respective trackers and confidences
    // indices returns the ones that should be alive.
    // Algorithm:
    //  start from the bottom and search if the bounding box is in index
    //    if it is; don't delete
    //    if not; remove it
    for(int i = rois.size()-1; i >= 0; --i) {
        if(std::find(indices.begin(), indices.end(), i) != indices.end()) {
        } else {
            bunch_of_trackers.erase(bunch_of_trackers.begin() + i);
            confidences.erase(confidences.begin() + i);
            tracker_total_missed_frames.erase(tracker_total_missed_frames.begin() + i);
            rois.erase(rois.begin() + i);
        }
    }
}

inline void ObjectTracker::track_and_emit(void) {

    create_bounding_boxes_confidences_and_trackers();

    if(!rois.empty()){
        indices.clear();
        remove_duplicate_bounding_boxes_confidences_and_trackers();
    }

    for(auto& fr : *tracking_frames){
        cro_tracking.tic();
        if(toggle_object_tracking && !rois.empty()){
            for(int i = rois.size()-1; i >= 0; --i) {
                // check if the object is still on frame
                bool alive = bunch_of_trackers[i]->update(fr,rois[i]);
                if(alive){
                    if(toggle_bounding_boxes)
                        cv::rectangle(fr,
                                      rois[i],
                                      cv::Scalar(cfg["Color"][0],cfg["Color"][1],cfg["Color"][2]),
                                      cfg["Rectangle Thickness"]);
                    if(toggle_middle_point)
                        cv::circle(fr,
                                   (rois[i].br() + rois[i].tl())*0.5,
                                    cfg["Circle Radius"],
                                    cv::Scalar(cfg["Color"][0],cfg["Color"][1],cfg["Color"][2]),
                                    cfg["Circle Thickness"]);
                }
                else if(tracker_total_missed_frames[i] > cfg["Tracker Max Missed Frames"]){
                    bunch_of_trackers.erase(bunch_of_trackers.begin() + i);
                    confidences.erase(confidences.begin() + i);
                    tracker_total_missed_frames.erase(tracker_total_missed_frames.begin() + i);
                    rois.erase(rois.begin() + i);
                }else{
                    tracker_total_missed_frames[i] += 1;
                }
                //sleep for the remaining time to achieve a constant frame rate
                long long int t = *tracking_per_frame_period - cro_tracking.toc();
                QThread::usleep(std::max<long long int>(t, 0));
            }
        }
        if (toggle_video)
            emit sendFrame(fr);
        if (toggle_stats){
            stats.push_back(bunch_of_trackers.size());
            stats.push_back(cro_tracking.toc());
            emit sendStats(stats);
        }
    }
}

void ObjectTracker::main_loop(void) {

    qDebug() << "ObjectTracker::main_loop thread id:" << QThread::currentThreadId();
    while(keep_running){
        track_and_emit();
        tracking_frames->clear();
        //upside is the real work
        object_tracker_done_mutex.lock();
        object_tracker_done_cv.wakeAll();
        all_done_cv.wait(&object_tracker_done_mutex);
        object_tracker_done_mutex.unlock();
    }
}

void ObjectTracker::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}
