#include "camera_loop.h"


CameraLoop::CameraLoop(json cfg, QObject *parent) :
    QObject(parent),
    toggle_stream(true),
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
    fps cro;
    auto net = cv::dnn::readNet(model_file, model_config_file, "TensorFlow");
    usb_webcam.run();

    while (toggle_stream) {
        cro.tic();

        image = usb_webcam.read();

        cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(cfg["width"],cfg["height"]), mean_blob, true, false);
        net = cv::dnn::readNetFromTensorflow(model_file, model_config_file);
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
                cv::rectangle(image, cv::Point(bboxX, bboxY), cv::Point(bboxX + bboxWidth, bboxY + bboxHeight), cv::Scalar(0,0,255), 2);
                //std::string class_name = labels[class_id-1];
            }
        }

        //cv::putText(image, "us: " + std::to_string(cro.toc()), cv::Point(50, 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
        //cv::imshow("image", image);

        emit sendFrame(image,cro.toc());
    }
    usb_webcam.stop();
}
