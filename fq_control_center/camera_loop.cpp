#include "camera_loop.h"


CameraLoop::CameraLoop(json cfg, QObject *parent) :
    QObject(parent),
    cfg(cfg),
    toggle_stream(true),
    toggle_video(true),
    toggle_stats(true),
    toggle_object_detection(true),
    toggle_tracking(true),
    toggle_bounding_boxes(true),
    all_models_path(cfg["all_models_path"]),
    model_folder(cfg["model_folder"]),
    model_file(all_models_path + "/" + model_folder + "/" + cfg["model_file"].get<std::string>()),
    model_config_file(all_models_path + "/" + model_folder + "/" + cfg["model_config_file"].get<std::string>()),
    usb_webcam(Webcam::getInstance(cfg["source"],cfg["width"],cfg["height"])),
    total_frames(20),
    tracker_last_index(0)
{
    moveToThread(&m_thread);
    m_thread.start();
}

CameraLoop::~CameraLoop(){

    toggle_stream = false;

    if( m_thread.isRunning() )
    {
        m_thread.quit();
        m_thread.wait();
    }
}

void CameraLoop::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}

inline void CameraLoop::detect_objects(){
    static auto net = cv::dnn::readNet(model_file, model_config_file, "TensorFlow");
    cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(cfg["width"],cfg["height"]), mean_blob, true, false);
    net.setInput(blob);
    cv::Mat output = net.forward();
    results = cv::Mat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
}

inline void CameraLoop::create_bounding_boxes_confidences_and_trackers(){
    for (int i = 0; i < results.rows; i++){
        int class_id = int(results.at<float>(i, 1));
        float confidence = results.at<float>(i, 2);

        // Check if the detection is over the min threshold and then draw bbox
        if (confidence > cfg["min_confidence_score"] &&  class_id == 1){
            int bboxX = int(results.at<float>(i, 3) * image.cols);
            int bboxY = int(results.at<float>(i, 4) * image.rows);
            int bboxWidth = int(results.at<float>(i, 5) * image.cols - bboxX);
            int bboxHeight = int(results.at<float>(i, 6) * image.rows - bboxY);
            rois.push_back(cv::Rect2d(bboxX,bboxY,bboxWidth,bboxHeight));
            confidences.push_back(confidence);
            bunch_of_trackers.push_back(cv::TrackerMOSSE::create());
            bunch_of_trackers.back()->init(image,rois.back());
            tracker_total_missed_frames.push_back(0);
        }
    }
}

inline void CameraLoop::remove_duplicate_bounding_boxes_confidences_and_trackers(){
    cv::dnn::NMSBoxes(rois, confidences, cfg["min_confidence_score"], cfg["nmsThreshold"], indices);

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

void CameraLoop::main_loop(void) {

    QThread::currentThread()->setPriority(QThread::HighPriority);
    usb_webcam.run();
    tracker_last_index = confidences.size();

    while (toggle_stream) {
        try {
            cro_all.tic();
            image = usb_webcam.read();
            if (image.empty())
                continue;
            if (toggle_object_detection && (total_frames >= cfg["object_detection_wait_frames"])){
                total_frames = 0;
                indices.clear();
                detect_objects();
                create_bounding_boxes_confidences_and_trackers();
                if(!rois.empty())
                    remove_duplicate_bounding_boxes_confidences_and_trackers();

                for(const auto& roi : rois){
                    if(toggle_bounding_boxes){
                        cv::rectangle(image, roi, cv::Scalar(0,0,255), 2);
                    }
                }
            }else if(toggle_tracking && !rois.empty()){
                for(int i = rois.size()-1; i >= 0; --i) {
                    bool alive = bunch_of_trackers[i]->update(image,rois[i]);
                    if(alive)
                        cv::rectangle(image, rois[i], cv::Scalar(0,0,255), 2);
                    else if(tracker_total_missed_frames[i] > cfg["tracker_max_missed_frames"]){
                        bunch_of_trackers.erase(bunch_of_trackers.begin() + i);
                        confidences.erase(confidences.begin() + i);
                        tracker_total_missed_frames.erase(tracker_total_missed_frames.begin() + i);
                        rois.erase(rois.begin() + i);
                    }else{
                        tracker_total_missed_frames[i] += 1;
                    }
                }
            }else{
                //Emmitting frames non-stop suffocates the GUI. Hence, the delay.
                QThread::msleep(5);
            }
            total_frames += 1;
            //cv::putText(image, "us: " + std::to_string(cro.toc()), cv::Point(50, 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
            //cv::imshow("image", image);

            if (toggle_video)
                emit sendFrame(image);
            if (toggle_stats)
                emit sendStats(cro_all.toc());
        }  catch (int) {
            qDebug() << "ignoring an exception";
        }

    }
}