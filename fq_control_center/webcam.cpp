#include "webcam.h"


Webcam::Webcam(const int webcam_source, const int width, const int height)
    : src{webcam_source}, width{width}, height{height}
{
    this->cap = cv::VideoCapture(src);
    this->cap.set(cv::CAP_PROP_BUFFERSIZE, 1); // [TODO] EXPERIMENT WITH THE CONSTANT BUFFERSIZE
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.read(this->frame); // Read one frame to prevent uninitialized reads.
}

Webcam::~Webcam(){
    this->thread_run = false;
    if(frame_reader.joinable())
        frame_reader.join();
    cap.release();
}

Webcam& Webcam::getInstance(const int src, const int width, const int height)
{
    static Webcam instance(src,width,height);
    return instance;
}

void Webcam::update(){
    while(thread_run){
        cap.read(this->frame);
    }
}

cv::Mat Webcam::read(){
    return this->frame;
}

void Webcam::run(){
    this->thread_run = true;
    frame_reader = std::thread(&Webcam::update,this);
}

void Webcam::stop(){
    this->thread_run = false;
}
