#include "camera_loop.h"

// for convenience
using json = nlohmann::json;

CameraLoop::CameraLoop(QObject *parent) :
    QObject(parent),
    toggle_stream(true)
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

void CameraLoop::read_json(const std::string json_file, json& j){
    std::ifstream i(json_file);
    i >> j;
}

void CameraLoop::read_model_labels(const std::string label_list_file,std::vector<std::string>& labels){
    std::ifstream i(std::string(label_list_file).c_str());
    std::string line;

    while (getline(i, line))
        labels.push_back(line);
}

void CameraLoop::receiveValue(int new_value){
    if (value != new_value) {
        value = new_value;
        emit sendValue(value);
    }
}

void CameraLoop::main_loop(void) {

    json cfg_data;
    json info;
    read_json(CFG_FILE,cfg_data);
    read_json(INFO_FILE,info);

    const std::string all_models_path = cfg_data["all_models_path"];
    const std::string model_folder = cfg_data["model_folder"];
    const std::string model_file = all_models_path + "/" + model_folder + "/" + cfg_data["model_file"].get<std::string>();
    const std::string label_list_file = all_models_path + "/" + model_folder + "/" + cfg_data["label_list_file"].get<std::string>();
    const std::string model_config_file = all_models_path + "/" + model_folder + "/" + cfg_data["model_config_file"].get<std::string>();

    std::vector<std::string> labels;
    read_model_labels(label_list_file,labels);
    
    Webcam& usb_webcam = Webcam::getInstance(cfg_data["source"],cfg_data["width"],cfg_data["height"]);

    auto net = cv::dnn::readNet(model_file, model_config_file, "TensorFlow");

    fps cro;

    cv::Mat image;
    cv::Scalar mean_blob = cv::Scalar(127.5, 127.5, 127.5);

    usb_webcam.run();

    while (toggle_stream) {
        cro.tic();

        image = usb_webcam.read();

        cv::Mat blob = cv::dnn::blobFromImage(image, 1.0, cv::Size(cfg_data["width"],cfg_data["height"]), mean_blob, true, false);

        net.setInput(blob);

        cv::Mat output = net.forward();

        // Matrix with all the detections
        cv::Mat results(output.size[2], output.size[3], CV_32F, output.ptr<float>());

        // Run through all the predictions
        for (int i = 0; i < results.rows; i++){
            int class_id = int(results.at<float>(i, 1));
            float confidence = results.at<float>(i, 2);

            // Check if the detection is over the min threshold and then draw bbox
            if (confidence > cfg_data["min_confidence_score"] &&  class_id == 1){
                int bboxX = int(results.at<float>(i, 3) * image.cols);
                int bboxY = int(results.at<float>(i, 4) * image.rows);
                int bboxWidth = int(results.at<float>(i, 5) * image.cols - bboxX);
                int bboxHeight = int(results.at<float>(i, 6) * image.rows - bboxY);
                cv::rectangle(image, cv::Point(bboxX, bboxY), cv::Point(bboxX + bboxWidth, bboxY + bboxHeight), cv::Scalar(0,0,255), 2);
                std::string class_name = labels[class_id-1];
            }
        }

        //cv::putText(image, "us: " + std::to_string(cro.toc()), cv::Point(50, 50), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 255, 0), 2, false);
        //cv::imshow("image", image);

        emit sendFrame(image,cro.toc());
    }
}
