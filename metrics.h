/*
 * Linux Scheduler Test: This is a test project created to check how long
 * different threads of different scheduler class of Linux gets opportunity to
 * execute.
 *
 * The software come with absolutely no warrantly and licensed under MPL 2.0
 *
 * Copyrights: Visteon Technical and Services Center - Chennai
 */
#ifndef METRICS_H
#define METRICS_H

struct thread_metrics {
	unsigned long count;
	int jobs;
	int loops;
	int tof; /* time of finish */
	int loop_period;
	int job_period;
	int total_period;
};

extern struct thread_metrics	Metrics[];

void report_loop_increment(enum sched_class sc);
void report_loop_complete(enum sched_class sc, unsigned long period);
void report_job_complete(enum sched_class sc, unsigned long period);
void report_sclass_complete(enum sched_class sc);


#endif
