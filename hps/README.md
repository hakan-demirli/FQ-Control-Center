FQ-Control-Center is a utility software that combines ARM and FPGA controls together. It provides a simple GUI interface for use and highly customizable config files for development.

FQ-Control-Center consists of 3 main parts:
Camera Unit
Sensor Aggregation Unit
Webserver Unit

Camera Unit:
Camera unit detects people by using deep neural networks and tracks them. It records the statistics of human traffic.

Software architecture:
[insert diagram]
Neural networks are computationaly expensive. DE10-Nano ARM HPS can only process 1 frame every 1.2 seconds. Hence, to obtain a worthwile FPS, detection, tracking ,and acquisition parts has to operate concurrently.





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
