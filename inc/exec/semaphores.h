#pragma once

#include <stdio.h>
#include <pthread.h>

struct SignalSemaphore
{
	pthread_mutex_t sem;
};

static inline void InitSemaphore(struct SignalSemaphore* sem)
{
	pthread_mutexattr_t Attr;
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&sem->sem, &Attr);
}

static inline void ObtainSemaphore(struct SignalSemaphore* sem)
{
	pthread_mutex_lock(&sem->sem);
}

static inline void ReleaseSemaphore(struct SignalSemaphore* sem)
{
	pthread_mutex_unlock(&sem->sem);
}

// No way to delete a semaphore in Amiga APIs, but pthread_mutex_destroy is a no-op on Haiku anyway.
