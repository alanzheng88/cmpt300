/*
 * NAME, etc.
 *
 * sync.c
 *
 *
 */

#define _REENTRANT

#include "sync.h"


/*
 * Pthread spin lock
 */

int spinlock_lock(pthread_spinlock_t *lock) {
	while (1) {
		while (pthread_spin_trylock(lock)) { /* keep spinning */ }
		if (!pthread_spin_lock(lock)) { 
			return 0;
		}
	}
}


/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock->i = 0;
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
	lock->i = 0;
	return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
	while (tas(&(lock->i))) { /* keep spinning */ }
	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	while (1) {
		while (my_spinlock_trylock(lock)) { /* keep spinning */ }
		if (!tas(&(lock->i))) {
			// entered critical section - perform task after returning
			return 0;
		}
	}
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
	return lock->i;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
	return -1;
}

int my_mutex_destroy(my_mutex_t *lock)
{
	return -1;
}

int my_mutex_unlock(my_mutex_t *lock)
{
	return -1;
}

int my_mutex_lock(my_mutex_t *lock)
{
	return -1;
}

int my_mutex_trylock(my_mutex_t *lock)
{
	return -1;
}

/*
 * Queue Lock
 */

int my_queuelock_init(my_queuelock_t *lock)
{
	return -1;
}

int my_queuelock_destroy(my_queuelock_t *lock)
{
	return -1;
}

int my_queuelock_unlock(my_queuelock_t *lock)
{
	return -1;
}

int my_queuelock_lock(my_queuelock_t *lock)
{
	return -1;
}

int my_queuelock_trylock(my_queuelock_t *lock)
{
	return -1;
}

