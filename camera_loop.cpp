#include "camera_loop.h"


CameraLoop::CameraLoop(json cfg, QObject *parent) :
    QObject(parent),
    toggle_stream(true),
    toggle_video(true),
    toggle_stats(true),
    toggle_object_detection(true),
    toggle_tracking(true),
    toggle_bounding_boxes(true),
    cfg(cfg),
    all_models_path(cfg["all_models_path"]),
    model_folder(cfg["model_folder"]),
    model_file(all_models_path + "/" + model_folder + "/" + cfg["model_file"].get<std::string>()),
    model_config_file(all_models_path + "/" + model_folder + "/" + cfg["model_config_file"].get<std::string>()),
    usb_webcam(Webcam::getInstance(cfg["source"],cfg["width"],cfg["height"]))
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

void CameraLoop::main_loop(void) {

    QThread::currentThread()->setPriority(QThread::HighPriority);

    auto net = cv::dnn::readNet(model_file, model_config_file, "TensorFlow");

    //cv::Ptr<cv::Tracker> tracker = cv::TrackerKCF::create();
    //cv::Ptr<cv::MultiTracker> trackers = cv::MultiTracker::create();
    QVector<cv::Ptr<cv::Tracker>> bunch_of_trackers;

    fps cro_all;

    usb_webcam.run();
    QVector<cv::Rect2d> rois;

    unsigned int total_frames = 20;

    while (toggle_stream) {
        cro_all.tic();

        image = usb_webcam.read();

        if (toggle_object_detection && (total_frames >= 20)){
            total_frames = 0;


            cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(cfg["width"],cfg["height"]), mean_blob, true, false);

            net.setInput(blob);

            cv::Mat output = net.forward();

            // Matrix with all the detections
            cv::Mat results(output.size[2], output.size[3], CV_32F, output.ptr<float>());

            // Run through all the predictions
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
                }
            }

            for(const auto& roi : rois){
                if(toggle_bounding_boxes){
                    cv::rectangle(image, roi, cv::Scalar(0,0,255), 2);
                }
                if(toggle_tracking){
                    bunch_of_trackers.push_back(cv::TrackerMOSSE::create());
                    bunch_of_trackers.back()->init(image,roi);
                    //trackers->add(tracker, image, roi);
                }
            }
        }else if(toggle_tracking){
            //const auto& a = trackers->update(image);
            for(int i = 0; i < rois.size(); ++i) {
                if(bunch_of_trackers[i]->update(image,rois[i]))
                    cv::rectangle(image, rois[i], cv::Scalar(0,0,255), 2);
                else{
                    bunch_of_trackers.erase(bunch_of_trackers.begin() + i);
                    i--;
                }
            }
        }else{
            //Emmitting frames non-stop suffocates the GUI. Hence, the delay.
            QThread::msleep(10);
        }
        total_frames += 1;
        //cv::putText(image, "us: " + std::to_string(cro.toc()), cv::Point(50, 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
        //cv::imshow("image", image);

        if (toggle_video)
            emit sendFrame(image);
        if (toggle_stats)
            emit sendStats(cro_all.toc());
    }
}
