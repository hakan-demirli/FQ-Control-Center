#ifndef FPS_H
#define FPS_H


#include <sys/time.h>

class fps{

public:
    void tic();
    long toc();

private:
    struct timeval start_time;
    struct timeval end_time;
};

#endif // FPS_H
