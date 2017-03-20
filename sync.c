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
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock->locked = 0;
  lock->ownerId = -1;
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{
  printf("unlocked function called!\n");
	unlock(&(lock->locked));
	return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
	while (my_spinlock_trylock(lock));
	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
	while (1) {
		while (lock->locked) { /* keep spinning */ }
		if (!my_spinlock_trylock(lock)) {
			// entered critical section - perform task after returning
			return 0;
		}
	}
}

int my_spinlock_trylock(my_spinlock_t *lock)
{
  // returns 0 if successfully acquired lock, otherwise returns 1
  if (tas(&(lock->locked))) {
    return 1;
  }
 
  return 0;
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
  lock->locked = 0; 
	return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
	return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
  unlock(&(lock->locked));
	return 0;
}

int my_mutex_lock(my_mutex_t *lock)
{
  useconds_t delay = 0;
  while (1) {
    while (lock->locked) {
      delay += rand() % 10 + 1;
      usleep(delay);
    }
    if (!tas(&(lock->locked))) {
      return 0;
    }
  }
}

int my_mutex_trylock(my_mutex_t *lock)
{
	return !tas(&(lock->locked));
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

