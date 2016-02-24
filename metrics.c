/*
 * Linux Scheduler Test: This is a test project created to check how long
 * different threads of different scheduler class of Linux gets opportunity to
 * execute.
 *
 * The software come with absolutely no warrantly and licensed under MPL 2.0
 *
 * Copyrights: Visteon Technical and Services Center - Chennai
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include <utmpx.h>
#include <limits.h>

#include "sched_test.h"
#include "metrics.h"



/******************************************************************************
 * Globals
 *****************************************************************************/
struct thread_metrics	Metrics[SC_MAX];
struct thread_metrics	MetSnapShot[SC_MAX][SC_MAX];
pthread_mutex_t		MetMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t	MutexAttr;
int			StartTime; /* in millisecond */



/******************************************************************************
 * Functions start here
 *****************************************************************************/
static print_item(struct thread_metrics *p, int full)
{
	printf("\tNo of atomic loops: %lu\n", p->count);
	printf("\tNo of for-loops: %d\n", p->loops);
	printf("\tNo of jobs (N for-loops): %d\n", p->jobs);
	printf("\tSum of all loop duration (msec): %d\n",
	       p->loop_period);

	if(full == 0)
		return;

	printf("\tSum of all job duration (msec) : %d\n",
	       p->job_period);
	printf("\tTotal duration (ms) from start to end (incl.wait time) : %d\n",
	       p->total_period);
}

void metrics_print(int fifo, int rr, int other)
{
	printf("\n\nOverall summary:\n================\n");
	if(fifo > 0) {
		printf("  SCHED_FIFO\n  ----------\n");
		print_item(&Metrics[SC_FIFO], 1);
	}
	if(rr > 0) {
		printf("  SCHED_RR\n  --------\n");
		print_item(&Metrics[SC_RR], 1);
	}
	if(other > 0) {
		printf("  SCHED_OTHERS\n  ------------\n");
		print_item(&Metrics[SC_NORMAL], 1);
	}

	printf("\n\nSnapshots:\n=========\n");
	if(fifo > 0) {
		printf(" The duration and count when FIFO completed \n");
		printf("  > SCHED_FIFO.RR\n");
		print_item(&MetSnapShot[SC_FIFO][SC_RR], 0);
		printf("  > SCHED_FIFO.OTHERS\n");
		print_item(&MetSnapShot[SC_FIFO][SC_NORMAL], 0);
		printf(" ... \n");
	}

	if(rr > 0) {
		printf(" The duration and count when RR completed \n");
		printf("  > SCHED_RR.FIFO\n");
		print_item(&MetSnapShot[SC_RR][SC_FIFO], 0);
		printf("  > SCHED_RR.OTHERS\n");
		print_item(&MetSnapShot[SC_RR][SC_NORMAL], 0);
		printf(" ... \n");
	}

	if(other > 0) {
		printf(" The duration and count when OTHER completed \n");
		printf("  > SCHED_OTHERS.FIFO\n");
		print_item(&MetSnapShot[SC_NORMAL][SC_FIFO], 0);
		printf("  > SCHED_OTHERS.RR\n");
		print_item(&MetSnapShot[SC_NORMAL][SC_RR], 0);
		printf(" ... \n");
	}
}

void report_loop_increment(enum sched_class sc)
{
	if(0 != pthread_mutex_lock(&MetMutex)) {
		printf("%s(): Error in getting the lock! Data rejected!!\n",
		       __func__);
		return;
	}

	Metrics[sc].count++;

	pthread_mutex_unlock(&MetMutex);
}

void report_loop_complete(enum sched_class sc, unsigned long period)
{
	if(0 != pthread_mutex_lock(&MetMutex)) {
		printf("%s(): Error in getting the lock! Data rejected!!\n",
		       __func__);
		return;
	}

	Metrics[sc].loop_period += period;
	Metrics[sc].loops++;

	pthread_mutex_unlock(&MetMutex);
}

void report_job_complete(enum sched_class sc, unsigned long period)
{
	if(0 != pthread_mutex_lock(&MetMutex)) {
		printf("%s(): Error in getting the lock! Data rejected!!\n",
		       __func__);
		return;
	}

	Metrics[sc].job_period += period;
	Metrics[sc].jobs++;

	pthread_mutex_unlock(&MetMutex);
}

void report_sclass_complete(enum sched_class sc)
{
	int i, t2;
	struct timeval time;

	if(0 != pthread_mutex_lock(&MetMutex)) {
		printf("%s(): Error in getting the lock! Data rejected!!\n",
		       __func__);
		return;
	}

	/* capture the period taken by this scheduler class */
	gettimeofday(&time, NULL);
	t2 = time.tv_sec*1000 + time.tv_usec/1000;
	Metrics[sc].total_period = t2 - StartTime;

	/* take snapshot of metrics by other classes at this time */
	for(i = 0; i < SC_MAX; i++) {
		MetSnapShot[sc][i].count = Metrics[i].count;
		MetSnapShot[sc][i].jobs = Metrics[i].jobs;
		MetSnapShot[sc][i].loops = Metrics[i].loops;
		MetSnapShot[sc][i].tof = Metrics[i].tof;
		MetSnapShot[sc][i].loop_period = Metrics[i].loop_period;
		MetSnapShot[sc][i].job_period = Metrics[i].job_period;
		MetSnapShot[sc][i].total_period = Metrics[i].total_period;
	}

	pthread_mutex_unlock(&MetMutex);
}


int metrics_exit(void)
{
	if(0 != pthread_mutexattr_destroy(&MutexAttr)) {
		printf("%s(): Unable to destroy mutex att\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int metrics_init(void)
{
	struct timeval time;

	/* init global array */
	memset(Metrics, 0, sizeof(Metrics));

	/* init the lock for the global array */
	if (0 != pthread_mutexattr_init(&MutexAttr)) {
		return -1;
	}
	if (0 != pthread_mutex_init(&MetMutex, &MutexAttr)) {
		return -1;
	}

	/* capture start time */
	gettimeofday(&time, NULL);
	StartTime = time.tv_sec*1000 + time.tv_usec/1000;
}
