#include "fps.h"


void fps::tic(){
        gettimeofday(&this->start_time, 0);
}

long fps::toc(){
    gettimeofday(&this->end_time, 0);
    long t = (end_time.tv_sec*1e6 + end_time.tv_usec) - (start_time.tv_sec*1e6 + start_time.tv_usec);
    return t;
}
