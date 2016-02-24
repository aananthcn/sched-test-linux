Intro
=====
This project will help developers analyzing scheduling impact on different 
threads running under different classes (such as SCHED_RR, SCHED_FIFO, 
SCHED_OTHERS) by modifying /proc/sys/kernel/sched_rt_runtime_us

Usage: 
======
    ./sched_test -n NX -f NF -r NR -c CN <br>
        -n	Creates NX number of SCHED_NORMAL threads <br>
        -f	Creates FX number of SCHED_FIFO threads <br>
        -r	Creates RX number of SCHED_RR threads <br>
        -c	Runs all the above threads in the CPU core 'CN' <br>

    Note: <br>
        '-c' option is mandatory <br>
        at least one thread is mandatory <br>
        do not use more than 30 threads in total <br>
	
	
How to build
============
Just give a "make" and you get the binary.

Note: a shell script sched_test.sh is provided as an example on how you can use
the generated binary