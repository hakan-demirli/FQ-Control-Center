#include "object_tracker.h"


ObjectTracker::ObjectTracker(json cfg,
                             QWaitCondition& all_done_cv,
                             QMutex& flag_mutex,
                             bool& object_tracker_done_bool,
                             QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    toggle_video(cfg["Run"]=="||"),
    toggle_boundary_line(cfg["Bondary Line"]=="||"),
    toggle_object_tracking(cfg["Object Tracking"]=="||"),
    toggle_bounding_boxes(cfg["Bounding Boxes"]=="||"),
    toggle_middle_point(cfg["Middle Point"]=="||"),
    toggle_stats(cfg["Stats"]=="||"),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    object_tracker_done_bool(object_tracker_done_bool),
    inside(0)
{
    moveToThread(&m_thread);
    m_thread.start();
}

ObjectTracker& ObjectTracker::getInstance(json cfg,
                                          QWaitCondition& all_done_cv,
                                          QMutex& flag_mutex,
                                          bool& object_tracker_done_bool,
                                          QObject *parent)
{
    static ObjectTracker instance(cfg,all_done_cv,flag_mutex,object_tracker_done_bool,parent);
    return instance;
}

ObjectTracker::~ObjectTracker(){

    qDebug() << "Destroying ObjectTracker Object";
    if( m_thread.isRunning() )
    {
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

    //const int ydif = (int(cfg["Boundary End Point"][1])-int(cfg["Boundary Start Point"][1]));
    //const int xdif = (int(cfg["Boundary End Point"][0])-int(cfg["Boundary Start Point"][0]));
    //const float boundary_slope = ((float)(ydif))/xdif;

    if(toggle_object_tracking){
        create_bounding_boxes_confidences_and_trackers();

        if(!rois.empty()){
            indices.clear();
            remove_duplicate_bounding_boxes_confidences_and_trackers();
        }
    }
    // process every frame
    //    update all trackers
    //    emit processed frames
    for(auto& fr : *tracking_frames){
        cro_tracking.tic();
        if(toggle_object_tracking && !rois.empty()){
            for(int i = rois.size()-1; i >= 0; --i) {
                // check if the object is still on frame
                auto middle_point_old = (rois[i].br() + rois[i].tl())*0.5;
                bool alive = bunch_of_trackers[i]->update(fr,rois[i]);
                if(alive){
                    auto middle_point = (rois[i].br() + rois[i].tl())*0.5;
                    qDebug() << "oldxy: " << middle_point_old.x << " " << middle_point_old.y ;
                    qDebug() << "newxy: " << middle_point.x << " " << middle_point.y ;
                    if((middle_point_old.x > int(cfg["Boundary Start Point"][0])) && (middle_point.x < int(cfg["Boundary Start Point"][0]))){
                        inside += 1;
                    }

                    if((middle_point_old.x < int(cfg["Boundary Start Point"][0])) && (middle_point.x > int(cfg["Boundary Start Point"][0]))){
                        inside -= 1;
                    }

                    if(toggle_bounding_boxes)
                        cv::rectangle(fr,
                                      rois[i],
                                      cv::Scalar(cfg["Color"][0],cfg["Color"][1],cfg["Color"][2]),
                                      cfg["Rectangle Thickness"]);
                    if(toggle_middle_point){
                        cv::circle(fr,
                                   middle_point,
                                   cfg["Circle Radius"],
                                   cv::Scalar(cfg["Color"][0],cfg["Color"][1],cfg["Color"][2]),
                                   cfg["Circle Thickness"]);
                    }
                }
                else if(tracker_total_missed_frames[i] > cfg["Tracker Max Missed Frames"]){
                    bunch_of_trackers.erase(bunch_of_trackers.begin() + i);
                    confidences.erase(confidences.begin() + i);
                    tracker_total_missed_frames.erase(tracker_total_missed_frames.begin() + i);
                    rois.erase(rois.begin() + i);
                }else{
                    tracker_total_missed_frames[i] += 1;
                }

                //sleep for the remaining time for consistent frame rate
                //QThread::usleep(1);
            }
        }
        if (toggle_video){
            if(toggle_boundary_line){
                cv::line(fr,
                         cv::Point(int(cfg["Boundary Start Point"][0]),int(cfg["Boundary Start Point"][1])),
                         cv::Point(int(cfg["Boundary End Point"][0]),int(cfg["Boundary End Point"][1])),
                         cv::Scalar(cfg["Color"][2],cfg["Color"][1],cfg["Color"][0]),
                         cfg["Boundary Thickness"]);
            }
            emit sendFrame(fr);
        }
        if (toggle_stats){
            stats.push_back(bunch_of_trackers.size());
            stats.push_back(cro_tracking.toc());
            stats.push_back((inside));
            emit sendStats(stats);
            stats.clear();
        }
    }
}

void ObjectTracker::main_loop(void) {

    qDebug() << "ObjectTracker::main_loop thread id:" << QThread::currentThreadId();
    // track and emit all frames then wait for all done
    cpu_set_t my_set;        /* Define your cpu_set bit mask. */
    CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
    CPU_SET(0, &my_set);     /* set the bit that represents core 0. */
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set); /* Set affinity of tihs process to */
                                                      /* the defined mask, i.e. only 0. */
    while(keep_running){
        track_and_emit();
        tracking_frames->clear();

        flag_mutex.lock();
        object_tracker_done_bool = true;
        all_done_cv.wait(&flag_mutex);
        flag_mutex.unlock();
    }
}

void ObjectTracker::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}
