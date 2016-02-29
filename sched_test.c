/*
 * Linux Scheduler Test: This is a test project created to check how long
 * different threads of different scheduler class of Linux gets opportunity to
 * execute.
 *
 * The software come with absolutely no warrantly and licensed under MPL 2.0
 *
 * Copyrights: Visteon Technical and Services Center - Chennai
 */
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>
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


#define MAX_JOBS	(0xFF)
#define MAX_LOOP	(0xFFFFF)
#define MAX_THREADS	(60)


/******************************************************************************
 * Globals
 *****************************************************************************/
pthread_barrier_t Barrier;
unsigned long MaxJobs = MAX_JOBS;
unsigned long MaxLoops = MAX_LOOP;


/******************************************************************************
 * Functions start here
 *****************************************************************************/
void *rr_thread(void *arg)
{
	int i, j;
	cpu_set_t cpuset;
	struct thread_arg targ;
	struct sched_param schp;
	unsigned long start, end;
	volatile unsigned long rr_count = 0;
	struct timeval time;
	unsigned long lt1, lt2, jt1, jt2; /* in millisec */

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));

	/* prepare and set cpu affinity the core passed as arg */
	CPU_ZERO(&cpuset);
	CPU_SET(targ.core, &cpuset);
	assert(sched_setaffinity(0, sizeof cpuset, &cpuset) == 0);

	/* set realtime priority to lowest */
	schp.sched_priority = 1;
	assert(pthread_setschedparam(pthread_self(), SCHED_RR, &schp) == 0);
	printf("Thread %x with %s class started in CPU %d\n", targ.tid,
	       targ.class, targ.core);
	pthread_barrier_wait(&Barrier);

	/* do the main job */
	gettimeofday(&time, NULL);
	jt1 = time.tv_sec*1000 + time.tv_usec/1000;
	for (i = 0; i < MaxJobs; i++) {
		/* do a loop */
		gettimeofday(&time, NULL);
		lt1 = time.tv_sec*1000 + time.tv_usec/1000;
		for (j = 0; j < MaxLoops; j++) {
			rr_count++;
			report_loop_increment(SC_RR);
		}
		gettimeofday(&time, NULL);
		lt2 = time.tv_sec*1000 + time.tv_usec/1000;
		report_loop_complete(SC_RR, lt2 - lt1);
	}
	gettimeofday(&time, NULL);
	jt2 = time.tv_sec*1000 + time.tv_usec/1000;
	report_job_complete(SC_RR, jt2 - jt1);

	printf("Thread %x with %s class is terminated on CPU %d\n", targ.tid,
	       targ.class, targ.core);
	return NULL;
}

void *fifo_thread(void *arg)
{
	int i, j;
	cpu_set_t cpuset;
	struct thread_arg targ;
	struct sched_param schp;
	unsigned long start, end;
	volatile unsigned long fifo_count = 0;
	struct timeval time;
	unsigned long lt1, lt2, jt1, jt2; /* in millisec */

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));

	/* prepare and set cpu affinity the core passed as arg */
	CPU_ZERO(&cpuset);
	CPU_SET(targ.core, &cpuset);
	assert(sched_setaffinity(0, sizeof cpuset, &cpuset) == 0);

	/* set realtime priority to lowest */
	schp.sched_priority = 1;
	assert(pthread_setschedparam(pthread_self(), SCHED_FIFO, &schp) == 0);
	printf("Thread %x with %s class started in CPU %d\n", targ.tid,
	       targ.class, targ.core);
	pthread_barrier_wait(&Barrier);

	/* do the main job */
	gettimeofday(&time, NULL);
	jt1 = time.tv_sec*1000 + time.tv_usec/1000;
	for (i = 0; i < MaxJobs; i++) {
		/* do a loop */
		gettimeofday(&time, NULL);
		lt1 = time.tv_sec*1000 + time.tv_usec/1000;
		for (j = 0; j < MaxLoops; j++) {
			fifo_count++;
			report_loop_increment(SC_FIFO);
		}
		gettimeofday(&time, NULL);
		lt2 = time.tv_sec*1000 + time.tv_usec/1000;
		report_loop_complete(SC_FIFO, lt2 - lt1);
	}
	gettimeofday(&time, NULL);
	jt2 = time.tv_sec*1000 + time.tv_usec/1000;
	report_job_complete(SC_FIFO, jt2 - jt1);

	printf("Thread %x with %s class is terminated on CPU %d\n", targ.tid,
	       targ.class, targ.core);
	return NULL;
}

void *normal_thread(void *arg)
{
	int i, j;
	int new_nice;
	cpu_set_t cpuset;
	struct thread_arg targ;
	struct sched_param schp;
	volatile unsigned long normal_count = 0;
	struct timeval time;
	unsigned long lt1, lt2, jt1, jt2; /* in millisec */

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));

	CPU_ZERO(&cpuset);
	CPU_SET(targ.core, &cpuset);
	assert(sched_setaffinity(0, sizeof cpuset, &cpuset) == 0);

	/* let's renice it to highest priority level */
	new_nice = nice(-20);
	printf("Thread %x with %s class started in CPU %d\n", targ.tid,
	       targ.class, targ.core);
	pthread_barrier_wait(&Barrier);

	/* do the main job */
	gettimeofday(&time, NULL);
	jt1 = time.tv_sec*1000 + time.tv_usec/1000;
	for (i = 0; i < MaxJobs; i++) {
		/* do a loop */
		gettimeofday(&time, NULL);
		lt1 = time.tv_sec*1000 + time.tv_usec/1000;
		for (j = 0; j < MaxLoops; j++) {
			normal_count++;
			report_loop_increment(SC_NORMAL);
		}
		gettimeofday(&time, NULL);
		lt2 = time.tv_sec*1000 + time.tv_usec/1000;
		report_loop_complete(SC_NORMAL, lt2 - lt1);
	}
	gettimeofday(&time, NULL);
	jt2 = time.tv_sec*1000 + time.tv_usec/1000;
	report_job_complete(SC_NORMAL, jt2 - jt1);

	printf("Thread %x with %s class is terminated on CPU %d\n", targ.tid,
	       targ.class, targ.core);
	return NULL;
}

void * handle_normals(void *arg)
{
	int tmax, i;
	pthread_t *tid_n;
	pthread_attr_t attr_n;
	struct thread_arg targ;

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));
	tmax = targ.max_threads;
	if (tmax <= 0) {
		return 0;
	}

	/* create thread id objects dynamically */
	tid_n = malloc(tmax * sizeof(pthread_t));
	if (tid_n == NULL) {
		printf("tid_n: malloc failure!\n");
		return 0;
	}

	/* initialize thread attributes for all types */
	pthread_attr_init(&attr_n);
	assert(pthread_attr_setschedpolicy(&attr_n, SCHED_OTHER) == 0);

	/* create different classes of threads */
	strcpy(targ.class, "SCHED_OTHER");
	for (i = 0; i < tmax; i++) {
		targ.tid = i;
		assert(pthread_create(&tid_n[i], &attr_n, normal_thread,
				      (void *)&targ) == 0);

		/* sleep to allow new thread to execute */
		usleep(1000);
	}
	printf("Created %d SCHED_OTHER threads on CPU %d\n", i, targ.core);

	/* wait for all normal threads to complete */
	for (i = 0; i < tmax; i++) {
		pthread_join(tid_n[i], NULL);
	}

	/* wait till all threads report job complete */
	while(Metrics[SC_NORMAL].jobs < tmax);

	if (tid_n != NULL)
		free(tid_n);

	report_sclass_complete(SC_NORMAL);

	return 0;
}


void * handle_rrs(void *arg)
{
	int tmax, i;
	pthread_t *tid_r;
	pthread_attr_t attr_r;
	struct thread_arg targ;

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));
	tmax = targ.max_threads;
	if (tmax <= 0) {
		return 0;
	}

	/* create thread id objects dynamically */
	tid_r = malloc(tmax * sizeof(pthread_t));
	if (tid_r == NULL) {
		printf("tid_r: malloc failure!\n");
		return 0;
	}

	/* initialize thread attributes for all types */
	pthread_attr_init(&attr_r);
	assert(pthread_attr_setschedpolicy(&attr_r, SCHED_RR) == 0);

	/* create threads */
	strcpy(targ.class, "SCHED_RR");
	for (i = 0; i < tmax; i++) {
		targ.tid = i;
		assert(pthread_create(&tid_r[i], &attr_r, rr_thread,
				      (void *)&targ) == 0);

		/* sleep to allow new thread to execute */
		usleep(1000);
	}
	printf("Created %d SCHED_RR threads on CPU %d\n", i, targ.core);


	/* wait for all rr threads to complete */
	for (i = 0; i < tmax; i++) {
		pthread_join(tid_r[i], NULL);
	}

	/* wait till all threads report job complete */
	while(Metrics[SC_RR].jobs < tmax);

	if (tid_r != NULL)
		free(tid_r);

	report_sclass_complete(SC_RR);

	return 0;
}


void * handle_fifos(void *arg)
{
	int tmax, i;
	pthread_t *tid_f;
	pthread_attr_t attr_f;
	struct thread_arg targ;

	/* copy args to local stack */
	memcpy((void*)&targ, (const void*)arg, sizeof(targ));
	tmax = targ.max_threads;
	if (tmax <= 0) {
		return 0;
	}

	/* create thread id objects dynamically */
	tid_f = malloc(tmax * sizeof(pthread_t));
	if (tid_f == NULL) {
		printf("tid_f: malloc failure!\n");
	}

	/* initialize thread attributes for all types */
	pthread_attr_init(&attr_f);
	assert(pthread_attr_setschedpolicy(&attr_f, SCHED_FIFO) == 0);

	/* create threads */
	strcpy(targ.class, "SCHED_FIFO");
	for (i = 0; i < tmax; i++) {
		targ.tid = i;
		assert(pthread_create(&tid_f[i], &attr_f, fifo_thread,
				      (void *)&targ) == 0);

		/* sleep to allow new thread to execute */
		usleep(1000);
	}
	printf("Created %d SCHED_FIFO threads on CPU %d\n", i, targ.core);

	/* wait for all fifo threads to complete */
	for (i = 0; i < tmax; i++) {
		pthread_join(tid_f[i], NULL);
	}

	/* wait till all threads report job complete */
	while(Metrics[SC_FIFO].jobs < tmax);

	if (tid_f != NULL)
		free(tid_f);

	report_sclass_complete(SC_FIFO);

	return 0;
}

void print_usage(char *prog)
{
	if(prog == NULL)
		return;

	printf("\nUsage:\n======\n   %s -n NX -f NF -r NR -c CC -d DD [-F FF]\n", prog);
	printf("\t  -f\tCreates FX number of SCHED_FIFO threads\n");
	printf("\t  -r\tCreates RX number of SCHED_RR threads\n");
	printf("\t  -n\tCreates NX number of SCHED_OTHER threads\n");
	printf("\t  -C\tRuns all the above threads in the CPU core 'CC'\n");
	printf("\t  :::::O::P::T::I::O::N::A::L::::A::R::G::U::M::E::N::T::S:::::\n");
	printf("\t  -F\tDuplicates and distribute SCHED_FIFO to CPU Core 'FF'\n");
	printf("\t  -R\tDuplicates and distribute SCHED_RR to CPU Core 'RR'\n");
	printf("\t  -N\tDuplicates and distribute SCHED_OTHER to CPU Core 'NN'\n");
	printf("\t  -L\tNumber of loops each Job to make\n");
	printf("\t  -J\tNumber of jobs each threads to do before exit\n");
	printf("\n   Note:\n   -----");
	printf("\n\t=>  '-c' option is mandatory");
	printf("\n\t=>  at least one of '-r' '-f' '-n' option is mandatory");
	printf("\n\t=>  do not use more than %d threads in total", MAX_THREADS);
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	int core, sched_other, sched_fifo, sched_rr;
	int core_f, core_n, core_r, c, threads, max_threads;
	int dist_threads;
	unsigned long ln, lf, lr;
	pthread_t tid_n, tid_nd, tid_f, tid_fd, tid_r, tid_rd;
	pthread_attr_t attr;
	struct thread_arg targ;


	sched_other = sched_fifo = sched_rr = -1;
	core_n = core_f = core_r = core = -1;
	if(metrics_init() < 0) {
		printf("Metrics init failed!!\n");
		return -1;
	}

	while ((c = getopt(argc, argv, "f:r:n:F:R:N:C:J:L:")) != -1) {
		switch (c) {
		case 'n': /* Number of SCHED_NORMAL or SCHED_OTHER threads */
			sched_other = atoi(optarg);
			break;
		case 'f': /* Number of SCHED_FIFO threads */
			sched_fifo = atoi(optarg);
			break;
		case 'r': /* Number of SCHED_RR threads */
			sched_rr = atoi(optarg);
			break;
		case 'C': /* CPU core for all threads */
			core = atoi(optarg);
			break;
		case 'F': /* CPU core for SCHED_FIFO threads alone */
			core_f = atoi(optarg);
			break;
		case 'N': /* CPU core for SCHED_OTHER threads alone */
			core_n = atoi(optarg);
			break;
		case 'R': /* CPU core for SCHED_RR threads alone */
			core_r = atoi(optarg);
			break;
		case 'J': /* Max number of Jobs (1 job = 'L' loops) */
			MaxJobs = atoi(optarg);
			break;
		case 'L': /* Max number loops (the basic work for threads) */
			MaxLoops = atoi(optarg);
			break;
		default:
			printf("arg \'-%c\' not supported\n", c);
			print_usage(argv[0]);
			return -1;
		}
	}

	if ((core == -1) && ((sched_other == -1) || (sched_fifo == -1) ||
			     (sched_rr == -1))) {
		printf("\nInput conditions not met!! See the note below!\n");
		print_usage(argv[0]);
		return -1;
	}

	if ((core_n != -1) && (sched_other == -1)) {
		printf("\nError: -N is provided but -n option is not provided\n");
		print_usage(argv[0]);
		return -1;
	}
	if ((core_f != -1) && (sched_fifo == -1)) {
		printf("\nError: -F is provided but -f option is not provided\n");
		print_usage(argv[0]);
		return -1;
	}
	if ((core_r != -1) && (sched_rr == -1)) {
		printf("\nError: -R is provided but -r option is not provided\n");
		print_usage(argv[0]);
		return -1;
	}

	threads = 0;
	if (sched_other > 0)
		threads += sched_other * ((core_n == -1) ? 1 : 2);
	if (sched_fifo > 0)
		threads += sched_fifo * ((core_f == -1) ? 1 : 2);
	if (sched_rr > 0)
		threads += sched_rr * ((core_r == -1) ? 1 : 2);

	max_threads = MAX_THREADS;
	if (threads > max_threads) {
		printf("\nPlease limit your number of threads! %d\n", threads);
		print_usage(argv[0]);
		return -1;
	}

	/* thread attr for master thread of all classes is set as FIFO */
	pthread_attr_init(&attr);
	assert(pthread_attr_setschedpolicy(&attr, SCHED_FIFO) == 0);
	pthread_barrier_init(&Barrier, NULL, threads);

	targ.core = core;

	/* create master thread for SCHED_OTHER class */
	if (sched_other > 0) {
		strcpy(targ.class, "SCHED_OTHER");
		targ.max_threads = sched_other;
		assert(pthread_create(&tid_n, &attr, handle_normals,
				      (void *)&targ) == 0);
		/* sleep to allow new thread to execute */
		usleep(1000);

		/* duplicate SCHED_OTHER and distribute to core_n */
		if(core_n != -1) {
			targ.core = core_n;
			strcpy(targ.class, "SCHED_OTHER");
			targ.max_threads = sched_other;
			assert(pthread_create(&tid_nd, &attr, handle_normals,
					      (void *)&targ) == 0);
		}
		/* sleep to allow new thread to execute */
		usleep(1000);
	}

	/* create master thread for SCHED_FIFO class */
	if (sched_fifo > 0) {
		strcpy(targ.class, "SCHED_FIFO");
		targ.max_threads = sched_fifo;
		assert(pthread_create(&tid_f, &attr, handle_fifos,
				      (void *)&targ) == 0);
		/* sleep to allow new thread to execute */
		usleep(1000);

		/* duplicate SCHED_FIFO and distribute to core_f */
		if(core_f != -1) {
			targ.core = core_f;
			strcpy(targ.class, "SCHED_FIFO");
			targ.max_threads = sched_fifo;
			assert(pthread_create(&tid_fd, &attr, handle_fifos,
					      (void *)&targ) == 0);
		}
	}

	/* create master thread for SCHED_RR class */
	if (sched_rr > 0) {
		strcpy(targ.class, "SCHED_RR");
		targ.max_threads = sched_rr;
		assert(pthread_create(&tid_r, &attr, handle_rrs,
				      (void *)&targ) == 0);
		/* sleep to allow new thread to execute */
		usleep(1000);

		/* duplicate SCHED_RR and distribute to core_r */
		if(core_r != -1) {
			targ.core = core_r;
			strcpy(targ.class, "SCHED_RR");
			targ.max_threads = sched_rr;
			assert(pthread_create(&tid_rd, &attr, handle_rrs,
					      (void *)&targ) == 0);
		}
		/* sleep to allow new thread to execute */
		usleep(1000);
	}


	/* first wait for all fifo threads to complete */
	if (sched_fifo > 0) {
		pthread_join(tid_f, NULL);
		if (core_f != -1)
			pthread_join(tid_fd, NULL);
	}
	/* then wait for all rr threads to complete */
	if (sched_rr > 0) {
		pthread_join(tid_r, NULL);
		if (core_r != -1)
			pthread_join(tid_rd, NULL);
	}
	/* and finally wait for all normal threads to complete */
	if (sched_other > 0) {
		pthread_join(tid_n, NULL);
		if (core_n != -1)
			pthread_join(tid_nd, NULL);
	}

	metrics_print(sched_fifo, sched_rr, sched_other);
	metrics_exit();

	return 0;
}
