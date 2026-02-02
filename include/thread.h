#ifndef KLIB_THREAD_H
#define KLIB_THREAD_H

#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>

/* run function on thread */
static pthread_t run_in_thread(void *data, 
                               void (*function)(void *data));
	
/* IMPLIMATION */
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

struct thr{
	void *data;
	void (*function)(void *data);
};

static void * 
_thread_routine(void * data)
{
	struct thr *thr = (struct thr *)data;
	// run callback
	if (thr->function)
		thr->function(thr->data);
	// return value for pthread_join
	void *retval = thr->data;
	// no need struct thr - free
	free(data);
	pthread_exit(retval);
}

static pthread_t 
_thread_create(struct thr *thr)
{
	pthread_t tid = 0;
	//create new thread
	pthread_create(
			&tid, 
			NULL, 
			_thread_routine, 
			thr);

	return tid;
}

pthread_t 
run_in_thread(void *data,
              void (*function)(void *data))
{
	pthread_t tid = 0;
	// allocate struct thr - to run in thread
	struct thr *thr = 
		(struct thr *)malloc(sizeof(struct thr));
	if (thr == NULL)
		return tid;

	thr->data = data;
	thr->function = function;

	return _thread_create(thr);
}


#ifdef __cplusplus
}
#endif

#endif /* ifndef KLIB_THREAD_H */

// vim:ft=c
