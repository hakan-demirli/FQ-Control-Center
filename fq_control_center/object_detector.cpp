#include "object_detector.h"


ObjectDetector::ObjectDetector(json cfg,
                               QWaitCondition& all_done_cv,
                               QMutex& flag_mutex,
                               bool& object_detector_done_bool,
                               QObject *parent) :
    QObject(parent),
    cfg(cfg),
    keep_running(true),
    toggle_object_detection(cfg["Object Detection"]=="||"),
    all_done_cv(all_done_cv),
    flag_mutex(flag_mutex),
    object_detector_done_bool(object_detector_done_bool),
    all_models_path(cfg["All Models Path"]),
    model_folder(cfg["Model Folder"]),
    model_file(all_models_path + "/" + model_folder + "/" + cfg["Model File"].get<std::string>()),
    model_config_file(all_models_path + "/" + model_folder + "/" + cfg["Model Config File"].get<std::string>()),
    net(cv::dnn::readNet(model_file, model_config_file, "TensorFlow"))
{
    moveToThread(&m_thread);
    m_thread.start();
}

ObjectDetector& ObjectDetector::getInstance(json cfg,
                                            QWaitCondition& all_done_cv,
                                            QMutex& flag_mutex,
                                            bool& object_detector_done_bool,
                                            QObject *parent)
{
    static ObjectDetector instance(cfg,all_done_cv,flag_mutex,object_detector_done_bool,parent);
    return instance;
}

ObjectDetector::~ObjectDetector(){

    qDebug() << "Destroying ObjectDetector Object";
    if( m_thread.isRunning() )
    {
        try {all_done_cv.wakeAll();} catch(int t){}
        m_thread.quit();
    }
}

void ObjectDetector::main_loop(){

    qDebug() << "ObjectDetector::main_loop thread id:" << QThread::currentThreadId();


    cv::Mat blob;
    cv::Mat output;
    // detect objects and wait for all_done
    while(keep_running){
        if(toggle_object_detection){
            blob = cv::dnn::blobFromImage(*detecting_frame,
                                                  cfg["Scale Factor"],
                                                  cv::Size(cfg["Size"][0],cfg["Size"][1]),
                                                  cv::Scalar(cfg["Mean Blob"][0],cfg["Mean Blob"][1],cfg["Mean Blob"][2]),
                                                  cfg["Swap RB"],
                                                  cfg["Crop"],
                                                  CV_32F);
            net.setInput(blob);
            output = net.forward();
            *detecting_results = cv::Mat(output.size[2], output.size[3], CV_32F, output.ptr<float>());
        }else{
            //QThread::msleep(20);
        }
        flag_mutex.lock();
        object_detector_done_bool = true;
        all_done_cv.wait(&flag_mutex);
        flag_mutex.unlock();
    }
}

void ObjectDetector::run(){
    QMetaObject::invokeMethod( this, "main_loop");
}
