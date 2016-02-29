Intro
=====
This project will help developers analyzing scheduling impact on different 
threads running under different classes (such as SCHED_RR, SCHED_FIFO, 
SCHED_OTHERS) by modifying /proc/sys/kernel/sched_rt_runtime_us


Usage:
======
    ./sched_test -n NX -f NF -r NR -C CC -d DD [-F FF]
        -f	Creates FX number of SCHED_FIFO threads
        -r	Creates RX number of SCHED_RR threads
        -n	Creates NX number of SCHED_OTHER threads
        -C	Runs all the above threads in the CPU core 'CC'
        :::::O::P::T::I::O::N::A::L::::A::R::G::U::M::E::N::T::S:::::
        -F	Duplicates and distribute SCHED_FIFO to CPU Core 'FF'
        -R	Duplicates and distribute SCHED_RR to CPU Core 'RR'
        -N	Duplicates and distribute SCHED_OTHER to CPU Core 'NN'
        -L	Number of loops each Job to make
        -J	Number of jobs each threads to do before exit

        Note:
        -----
        =>  '-c' option is mandatory
        =>  at least one of '-r' '-f' '-n' option is mandatory
        =>  do not use more than 60 threads in total

	
How to build
============
Just give a "make" and you get the binary.

Note: a shell script sched_test.sh is provided as an example on how you can use
the generated binary
