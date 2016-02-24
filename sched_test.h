/*
 * Linux Scheduler Test: This is a test project created to check how long
 * different threads of different scheduler class of Linux gets opportunity to
 * execute.
 *
 * The software come with absolutely no warrantly and licensed under MPL 2.0
 *
 * Copyrights: Visteon Technical and Services Center - Chennai
 */
#ifndef SCHED_TEST_H
#define SCHED_TEST_H


struct thread_arg {
	int core;
	int tid;
	int max_threads;
	char class[32];
};

enum sched_class {
	SC_NORMAL,
	SC_FIFO,
	SC_RR,
	SC_MAX
};


#endif
