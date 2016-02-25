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
	printf("\tNo of loops    : %lu\n", p->count);
	printf("\tNo of jobs     : %d\n", p->loops);
	printf("\tNo of threads  : %d\n", p->jobs);
#if 1
	printf("\tCPU usage (ms) : %d\n",
	       p->loop_period);
#endif
	if(full == 0)
		return;

	printf("\tCPU usage (ms) : %d\n",
	       p->job_period);
	printf("\tDuration (ms)  : %d\n",
	       p->total_period);
}

void metrics_print(int fifo, int rr, int other)
{
	int snapshot_cnt;
	int f,r,o;
	double f_l, r_l, o_l;
	double f_status, r_status, o_status;

	f = SC_FIFO;
	r = SC_RR;
	o = SC_NORMAL;

	/* Check if criteria for printing snapshot is met */
	snapshot_cnt  = (other) ? 1 : 0;
	snapshot_cnt += (fifo) ? 1 : 0;
	snapshot_cnt += (rr) ? 1 : 0;
	if(snapshot_cnt < 2)
		goto print_summary;

	/* compute data for printing job status in % */
	o_l = (double) other * MaxLoops * MaxJobs;
	f_l = (double) fifo * MaxLoops * MaxJobs;
	r_l = (double) rr * MaxLoops * MaxJobs;

	printf("\n\nJobs Snapshots:\n===============\n");
	if(fifo > 0) {
		f_status = (MetSnapShot[f][f].count * 100.0) / f_l;
		r_status = o_status = 0.0;

		printf("\n Status when SCHED_FIFO completed \n");
		printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
		if(rr > 0) {
			printf("  > SCHED_FIFO.RR\n");
			print_item(&MetSnapShot[f][r], 0);
			r_status = (MetSnapShot[f][r].count * 100.0) / r_l;
		}
		if(other > 0) {
			printf("  > SCHED_FIFO.OTHERS\n");
			print_item(&MetSnapShot[f][o], 0);
			o_status = (MetSnapShot[f][o].count * 100.0) / o_l;
		}
		printf(" ... \n");
		printf(" Job Status ==> FIFO: %.2f%%, RR: %.2f%%, OTHER: %.2f%%\n",
		       f_status, r_status, o_status);
	}

	if(rr > 0) {
		r_status = (MetSnapShot[r][r].count * 100.0) / r_l;
		f_status = o_status = 0.0;

		printf("\n Status when SCHED_RR completed \n");
		printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
		if(fifo > 0) {
			printf("  > SCHED_RR.FIFO\n");
			print_item(&MetSnapShot[r][f], 0);
			f_status = (MetSnapShot[r][f].count * 100.0) / f_l;
		}
		if(other > 0) {
			printf("  > SCHED_RR.OTHERS\n");
			print_item(&MetSnapShot[r][o], 0);
			o_status = (MetSnapShot[r][o].count * 100.0) / o_l;
		}
		printf(" ... \n");
		printf(" Job Status ==> FIFO: %.2f%%, RR: %.2f%%, OTHER: %.2f%%\n",
		       f_status, r_status, o_status);
	}

	if(other > 0) {
		o_status = (MetSnapShot[o][o].count * 100.0) / o_l;
		r_status = f_status = 0.0;

		printf("\n Status when SCHED_OTHER completed \n");
		printf(" ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n");
		if(fifo > 0) {
			printf("  > SCHED_OTHER.FIFO\n");
			print_item(&MetSnapShot[o][f], 0);
			f_status = (MetSnapShot[o][f].count * 100.0) / f_l;
		}
		if(rr > 0) {
			printf("  > SCHED_OTHER.RR\n");
			print_item(&MetSnapShot[o][r], 0);
			r_status = (MetSnapShot[o][r].count * 100.0) / r_l;
		}
		printf(" ... \n");
		printf(" Job Status ==> FIFO: %.2f%%, RR: %.2f%%, OTHER: %.2f%%\n",
		       f_status, r_status, o_status);
	}

print_summary:
	printf("\n\n\nOverall summary:\n================\n");
	if(fifo > 0) {
		printf("\n  SCHED_FIFO\n  ~~~~~~~~~~\n");
		print_item(&Metrics[SC_FIFO], 1);
	}
	if(rr > 0) {
		printf("\n  SCHED_RR\n  ~~~~~~~~\n");
		print_item(&Metrics[SC_RR], 1);
	}
	if(other > 0) {
		printf("\n  SCHED_OTHERS\n  ~~~~~~~~~~~~\n");
		print_item(&Metrics[SC_NORMAL], 1);
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
