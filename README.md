# timetest
Unix C program to measure duration of high-resolution timer.

## Introduction

Unix C programs can use several different APIs to get timestamps.

Some of them, e.g. time(), gettimeofday(), return "wall time", which is to say the time that you would see on your wall clock (or your watch or most likely of all, your cell phone).  Wall time is great for things like timestamps in log files, but it is not what one should be using for measuring elapsed time.  In other words, if you take two time samples and subtract them to get a delta time, wall time functions risk giving you inaccurate results.  What if daylight savings time starts or ends between the two samples?  What if the user sets the system time or changes the timezone, or what if NNTP performs an adjustment?  Or what if the [IERS](http://www.iers.org/) schedules a [leap second](https://en.wikipedia.org/wiki/Leap_second)?  All of these events can result in discontinuities in the time values returned by time(), gettimeofday(), and other wall clock APIs.

The most-portable way to get high-resolution timestamps is with [clock_gettime()](http://linux.die.net/man/3/clock_gettime).  Unfortunately for us Mac users, "most-portable" does not extend to us; that function is not available in OS X.  Mac users must use clock_get_time(), which not only has a different calling sequence and implementation approach, it also requires initialization prior to the first use, and cleanup.  But oh well, it's still better than gettimeofday.

The timetest program measures the duration of a call to either clock_gettime() or clock_get_time(), for linux or OS X respectively.  One might wish that simply sampling the high-resolution time has no overhead, but unfortunately it does.  On a second-generation Macbook Air, clock_get_time() takes about 1.8 microseconds to execute.  On a Linux virtual machine (using VirtualBox) running on the same Mac, clock_gettime() only takes .25 microseconds, over 7 times faster.  On a [CHIP single-board computer](http://getchip.com/), which runs Linux on a 1GHz R8 proessor), clock_gettime() takes about 1 microsecond, still almost twice as fast as OS X's clock_get_time().

If the construction of the program seems a bit odd, it's because there are a variety of factors which can affect timing.  For example, the first time through any peice of code results in cache misses for both the code and the data, forcing access to main memory.  Subsequent executions of the code is much faster due to the use of the processor cache.  Another factor is having background processing happening.  For example, an earlier version of this program called printf() at various points, but never between two samples.  I forgot that printf does buffered I/O, which means that some of the processing happens asynchronously, mostly via interrupt handling.  These interrupts interfere with accurate measureemnts.  So this version of the code saves up all its prints to the end.  Similarly, the act of loading the program results in disk I/O, which can have asynchronous aspects to it.  So a sleep() is called before any measurement is done.

Even with these precautions taken, the timings have a high degree of variability.  This is largely due to the fact that Unix might interrupt the test program to any number of things, from handling clock ticks to running a cron job.  Not only do these interrupts steal time, they also tend to flush the processor caches.  There are techniques to isolate a program from virtually all of these variables, but it's too much trouble for my purposes.  Generally you should be able to take the lowest measure from several runs.

You can find timetest at:

* User documentation (this README): https://github.com/fordsfords/timetest/tree/gh-pages

Note: the "gh-pages" branch is considered to be the current stable release.  The "master" branch is the development cutting edge.

## Quick Start

1. If you haven't set up your CHIP to be able to compile C programs, perform [these instructions](http://wiki.geeky-boy.com/w/index.php?title=CHIP_do_once) up to and including installing gcc.

2. Get the files onto CHIP:

        mkdir timetest
        cd timetest
        wget http://fordsfords.github.io/timetest/timetest.c
        wget http://fordsfords.github.io/timetest/bld.sh

3. Build the package:

        ./bld.sh

4. Test the package:

        ./timetest

5. Sample output:

        chip@chip:~$ gcc -Wall -o3 -std=c99 -o timetest timetest.c
        chip@chip:~$ ./timetest
        timetest.c
        clock_getres: 0 1
        delta1=1.015612e-06 s
        delta2=1.079100e-06 s
        delta3=1.000000e-06 s
        delta1=1.008201e-06 s
        delta2=1.075000e-06 s
        delta3=1.000000e-06 s
        delta1=1.006053e-06 s
        delta2=1.079200e-06 s
        delta3=1.000000e-06 s
        chip@chip:~$

There's a main loop which performs 3 measurements, delta1, delta2, and delta3.  That main loop is performed three times.  Delta1 represents an average over one million samples.  Delta2 represents an average over ten samples.  Delta3 is a single sample.  The delta1 measure has the greatest variability because a million calls takes a while, which greatly increases the opportunity for the kernel to interrupt it multiple times.

## Release Notes

* 0.1 (31-Jan-2016)

    Initial pre-release.

## License

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

This work is dedicated to the public domain under CC0 1.0 Universal:
http://creativecommons.org/publicdomain/zero/1.0/

To the extent possible under law, Steven Ford has waived all copyright
and related or neighboring rights to this work. In other words, you can 
use this code for any purpose without any restrictions.
This work is published from: United States.
Project home: https://github.com/fordsfords/timetest
