/*
    Copyright (c) 2008-2020 Jan W. Krieger (<jan@jkrieger.de>) (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-06-10 19:19:10 +0200 (Mi, 10 Jun 2015) $  (revision $Rev: 3976 $)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <cmath>
#include <cstdlib>
#include <iostream>
#include <chrono>

#include "jkqtcommon_imexport.h"

#ifndef __WINDOWS__
# if defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32)
#  define __WINDOWS__
# endif
#endif

#ifndef __LINUX__
# if defined(linux)
#  define __LINUX__
# endif
#endif

#if defined(__WINDOWS__)
  #include<windows.h>
#elif defined(__LINUX__)
  #include <sys/time.h>
#else
  #include <sys/time.h>
#endif



#ifndef JKQTPHIGHRESTIMER_H
#define JKQTPHIGHRESTIMER_H


/**
 * \brief This class implements a high resolution timer capable of measuring time intervals with a resolution
 *        of some microseconds
 * \ingroup jkqtptools_debugging
 *
 * \attention Note that this is a MS Windows specific implementation od a high-resolution timer using some of
 * windows' API methods (namely \c QueryPerformanceCounter() and \c QueryPerformanceFrequency() ). So if you want
 * to use this class on non-win32 systems you will have to find a way to implement it for your system!!!
 *
 * \attention Also note that a standard windows system is NOT a real time OS. So do not expect to get a high accuracy
 * when timing operations using this timer. It gives you an accurate time stamp, but it can not guarantee when
 * the code is executed!
 *
 *
 * The timer works very simple:
 *   - you can start the timer with start() which means you set a time=0
 *   - then you can query the time difference to the last call of start() by using getTime().
 * .
 *
 * This is the result of a test on a WinXP system, Athlon64 X2 3800+ (2GHz):
 * \image html highrestimer.png
 * Note that this histogram tells you more about how your system works than about the timer itself.
 * What we can find out about the timer is that it really provides a resolution in the microsecond region. This can
 * be seen, as there is a non-zero minimal measured interval (1.67619 usec). This basically tells us the run-time of
 * the measurement loop. There are also bigger run-times which are produced, as this program ran on a multi-process
 * (not process<b>or</b> !!!) OS, which means that the current process may be stalled to allow othe rprocesses to
 * execute. If this would run on a single-process system we should only get one run-time, if we assume that all
 * methods need a fixed amount of time. So we can use this method to test whether our system provides a sufficiently
 * accurate time. The 1.68 usec give an upper bound for the timer interval and thus the resolution they do <b>not</b>
 * represent the timer resolution. To get this nominal resolution in a windows system you could use the WinAPI method
 * \c QueryPerformanceFrequency() which is used in the method getTime(). On my system the timer frequency is 3.57955 MHz
 * which corresponds to a resolution of 0.2794 usec.
 *
 * \par win32 implementation issues:
 * To implement this timer on windows systems I use two API calls from the windows kernel. They are:
 *   - <a href="http://msdn2.microsoft.com/en-us/library/ms644904.aspx">QueryPerformanceCounter()</a>
 *   - <a href="http://msdn2.microsoft.com/en-us/library/ms644905.aspx">QueryPerformanceFrequency()</a>
 * .
 * \n
 * The first one is used to read times: In the start() method we simply save the current counter value to a variable.
 * In getTime() we can then again use QueryPerformanceCounter() to get the current counter value and then calculate
 * the difference between these two. Using QueryPerformanceFrequency() we can calculate the time difference in usecs
 * from the counter value difference using:
 *   \f[ \Delta t=\frac{N_{\mbox{now}}-N_{\mbox{start}}}{\mbox{\texttt{QueryPerformanceFrequency()}}}\cdot 10^{6} \f]
 *
 * \par why not standard POSIX?
 * The standard POSIX time functions available in a MinGW environment have a resolution that is in the range of
 * some 10 milliseconds. This may be enough for many purposes, but for exact time measurement, as you will want
 * to do it on measurement and control systems this is not sufficient. So I tried to find a possibility to
 * implement a system-independent high-resolution timer. As most timer stuff depends on very low-level kernel
 * calls on every operating system this seems to be nearly impossible. So I implemented this timer for win32 only,
 * as this is the system we use in the lab.
 *
 * \par linux implementation possibility:
 * This class does not implement a timer for Linux/Unix systems but I
 * will provide you with a small hint how you could implement one here:
 * The <a href="http://www.gnu.org/software/libc/manual/html_node/High_002dResolution-Calendar.html">\c gettimeofday()
 * method</a> from the libc seems to have enough tim resolution for our pourposes, so try to use this for the high-
 * resolution timer. As mentioned above this method is not available for MinGW in win32 which is the main development
 * platform for this project.
 *
 * \test you can find an example application in the file \link test_hrestimer.cpp \endlink.
 *
 */
class JKQTCOMMON_LIB_EXPORT JKQTPHighResTimer {
  protected:
#ifdef __WINDOWS__
        /** \brief internal: time stamp of the last call of start() */
        LARGE_INTEGER last;
#else
        std::chrono::system_clock::time_point last;
#endif
        /** \brief internal: timer frequency */
        double freq;
    public:
        /** \brief class constructor. */
        JKQTPHighResTimer();
        /** \brief class destructor */
        ~JKQTPHighResTimer();
        /** \brief start the timer */
        void start();
        /** \brief get the time since the last call of start() in microseconds */
        double getTime();
};





#endif // JKQTPHIGHRESTIMER_H
