FQ-Control-Center is a utility software that combines ARM and FPGA controls together. It provides a simple GUI interface for use and highly customizable config files for development.

* Table of Contents
    * Overview
        * Camera Unit
        * Sensor Aggregation unit
        * Webserver Unit
    * Development

FQ-Control-Center consists of 3 main parts:
* Camera Unit
* Sensor Aggregation Unit
* Webserver Unit

# Camera Unit:
Camera unit detects people by using deep neural networks and tracks them. It records the statistics of human traffic.

## Software architecture:
Object detection and tracking is a sequential operation by nature. First it has to be continious in time. Webcam thread acquires frames. Object detection thread detects people from frames. Tracker thread tracks the detected people. One part of the operation can't be started without the previous one is finished. So, the simplest algorithm for object tracking is:
```
forever:
    read frame
    detect objects every couple of frames
    track objects
    display
```
Assuming one turn with the object detection takes less than the frame priod this is indeed a viable solution. Unfortunately for DE10-Nano one turn takes 1.2 seconds. Which is 24 times longer than our 20FPS camera period. There are two viable solutions to speed up this process: FPGA OpenCL object detection accelerator or a multicore algorithm.

The first option is the best choice from performance perspective. We are guaranteed to solve FPS issue. But, It is the most challenging and time consuming choice. Porting a state of art object detection algorithm to OpenCL and debugging it sounds like a nightmare. And as someone who knows about deep neural networks for the couple months it adds another layer of doubt.

The second option is the easiest one. We can directy use pretrained and tested Deep Neural Network. Also, one of the threads can be easily replaced by an FPGA accelerator down the line and combine with the first method. But, there is a chance that hps alone will not be enough to solve our FPS problem.

I obviously went for the second option. Object detection takes 1 seconds for a single ARM core without tracking and webcam. If I can offload tracking and detection to the second core, we can achieve a real-time tracking and a detection update every second. Sounds plausable.

New algorithm:
![alt-image](./doc/thread_architecture.drawio.svg)    

There are three threads. Object detection thread runs non-stop and detects objects. During this heavy and long computation Webcam thread stores the frames. As soon as object detection ends, recorded frames and detected object results are sent to the Tracker thread and the most recent frame is fed into the detector.

Our new algorithm intorduces couple of new challenges; Consumer-Producer problem and data duplication problem.
Webcam and Object detection thread are both consumer and producer, tracker thread is only a consumer.




QTCharts can't be cross compiled for qt5.5.1.
Charts were added in Qt 5.7 to open source version, and QtCharts from 5.12 definitely won't compile on Qt 5.5.
In 5.5, they were still a commercial add-on. So you need to get in touch with Qt Company to get the 5.5 source code for QtCharts.
[SOURCE](https://forum.qt.io/topic/104170/qtconfig-is-not-a-recognized-test-function/5)

# How to Deploy
- Copy OpenCV shared libraries into a folder. In this case its the plugins folder. And export it.
    - ```export LD_LIBRARY_PATH=/root/nn/plugins``` 
- Select display.
    - ```export DISPLAY=:0```
- Run the app.
    - ```./fq_control_center```

TrackerMIL -> slow and inaccurate. A lot of false positives
TrackerMOSSE -> really fast. But it takes time to lock on to a box
TrackerKCF -> kinda slow but accurate
TrackerBoosting -> buggy and slow
TrackerTLD -> like KCF but a bit buggy?
TrackerGOTURN -> Deep learning based tracker. Buggy as hell.
TrackerMedianFlow -> Fast but there are some false positives
TrackerCSRT -> Accurate but slow

OpenCV 4.5+ removed tracker algorithms.
downgrading to 4.2 for the arm

CAMERA [TODO]

- Try HOG detection.
    ``` hog = cv2.HOGDescriptor()
        hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
    ```
- Implement OpenCL accelerator
- More camera paramater for camera_cfg
    - Tracker algorithm
    - Video file as a source
# -------------------------------------------------
Requirements:
Ubuntu == 18.04
OpenCV == 4.2.0
QT >= 5.5.0
