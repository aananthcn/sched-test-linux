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



/******************************************************************************
 * Functions start here
 *****************************************************************************/
static print_item(struct thread_metrics *p, int full)
{
	printf("\tNo of loops: %d\n", p->loops);
	printf("\tNo of jobs (one or more loops): %d\n", p->jobs);
	printf("\tTotal duration (ms) by adding loop times: %d\n",
	       p->loop_period);

	if(full == 0)
		return;

	printf("\tTotal duration (ms) by adding job times (incl. wait time) : %d\n",
	       p->job_period);
	printf("\tTotal duration (ms) from start to end (incl.wait time) : %d\n",
	       p->total_period);
}

void metrics_print(void)
{
	printf("\n\nOverall summary:\n================\n");
	printf("  SCHED_FIFO\n  ----------\n");
	print_item(&Metrics[SC_FIFO], 1);
	printf("  SCHED_RR\n  --------\n");
	print_item(&Metrics[SC_RR], 1);
	printf("  SCHED_OTHERS\n  ------------\n");
	print_item(&Metrics[SC_NORMAL], 1);

	printf("\n\nSnapshot:\n========\n");

	printf(" The duration and count when FIFO completed \n");
	printf("  > SCHED_FIFO.RR\n");
	print_item(&MetSnapShot[SC_FIFO][SC_RR], 0);
	printf("  > SCHED_FIFO.NORMAL\n");
	print_item(&MetSnapShot[SC_FIFO][SC_NORMAL], 0);
	printf(" ... \n");

	printf(" The duration and count when RR completed \n");
	printf("  > SCHED_RR.FIFO\n");
	print_item(&MetSnapShot[SC_RR][SC_FIFO], 0);
	printf("  > SCHED_RR.OTHERS\n");
	print_item(&MetSnapShot[SC_RR][SC_NORMAL], 0);
	printf(" ... \n");

	printf(" The duration and count when OTHERS completed \n");
	printf("  > SCHED_OTHERS.FIFO\n");
	print_item(&MetSnapShot[SC_NORMAL][SC_FIFO], 0);
	printf("  > SCHED_OTHERS.RR\n");
	print_item(&MetSnapShot[SC_NORMAL][SC_RR], 0);
	printf(" ... \n");
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

void report_sclass_complete(enum sched_class sc, unsigned long period)
{
	int i;

	if(0 != pthread_mutex_lock(&MetMutex)) {
		printf("%s(): Error in getting the lock! Data rejected!!\n",
		       __func__);
		return;
	}

	/* capture the period taken by this scheduler class */
	Metrics[sc].total_period += period;

	/* take snapshot of metrics by other classes at this time */
	for(i = 0; i < SC_MAX; i++) {
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
	/* init global array */
	memset(Metrics, 0, sizeof(Metrics));

	/* init the lock for the global array */
	if (0 != pthread_mutexattr_init(&MutexAttr)) {
		return -1;
	}
	if (0 != pthread_mutex_init(&MetMutex, &MutexAttr)) {
		return -1;
	}
}
