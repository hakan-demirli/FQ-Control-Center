#ifndef WEBCAM_H
#define WEBCAM_H

#include <opencv2/opencv.hpp>
#include <thread>

class Webcam{
    public:
        /* 
        * webcam is a singleton class that reads frames from a webcam asynchronously.
        */
        static Webcam& getInstance(const int,const int, const int);
        cv::Mat read();
        void run();
        void stop();
        Webcam(Webcam const&) = delete;
        ~Webcam();
        void operator=(Webcam const&) = delete;
    
    private:
        void update();
        Webcam(const int,const int, const int);

    public:

    private:
        bool thread_run;
        const int src;
        const int width;
        const int height;
        cv::VideoCapture cap;
        cv::Mat frame;
        std::thread frame_reader;
};

#endif// WEBCAM_H
