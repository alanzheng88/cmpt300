/*
 * NAME, etc.
 *
 * sync.c
 *
 *
 */

#define _REENTRANT

#include "sync.h"
#include <stdio.h>

/*
 * Spinlock routines
 */

int my_spinlock_init(my_spinlock_t *lock)
{
	lock->lock = 0;
  lock->count = 0;
	return 0;
}

int my_spinlock_destroy(my_spinlock_t *lock)
{
	return 0;
}

int my_spinlock_unlock(my_spinlock_t *lock)
{ 
  if (fas(&(lock->count)) == 1) {
	  unlock(&(lock->lock));
  }
	return 0;
}

int my_spinlock_lockTAS(my_spinlock_t *lock)
{
  volatile pthread_t currentThread = pthread_self();
  volatile int isLocked = lock->lock != 0;
  volatile int hasLocks = lock->count > 0;
  volatile int isOwner = pthread_equal(lock->owner, currentThread);

  if (isLocked && hasLocks && isOwner) { 
    lock->count++;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
  } else {
	  while (tas(&(lock->lock)) != 0);
    lock->owner = currentThread;
    lock->count = 1;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
  }
	return 0;
}


int my_spinlock_lockTTAS(my_spinlock_t *lock)
{
  volatile pthread_t currentThread = pthread_self();
  volatile int isLocked = lock->lock != 0;
  volatile int hasLocks = lock->count > 0;
  volatile int isOwner = pthread_equal(lock->owner, currentThread);

  if (isOwner && isLocked && hasLocks) { 
    lock->count++;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
  } else {
    while (1) {
      while (lock->lock) { /* keep spinning */ }
      if (tas(&(lock->lock)) == 0) {
        // entered critical section - perform task after returning
        lock->owner = currentThread;
        lock->count = 1;
        return 0;
      }
    }
  }

  return 0;
}

// returns 0 if successfully acquired lock, otherwise returns -1
int my_spinlock_trylock(my_spinlock_t *lock)
{
  volatile pthread_t currentThread = pthread_self();
  volatile int isLocked = lock->lock != 0;
  volatile int hasLocks = lock->count > 0;
  volatile int isOwner = pthread_equal(lock->owner, currentThread);

  if (isOwner && isLocked && hasLocks) { 
    lock->count++;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
    return 0;
  } else {
	  if (tas(&(lock->lock)) != 0) { return -1; }
    lock->owner = currentThread;
    lock->count = 1;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
    return 0;
  }
}


/*
 * Mutex routines
 */

int my_mutex_init(my_mutex_t *lock)
{
  lock->lock = 0; 
  lock->count = 0;
	return 0;
}

int my_mutex_destroy(my_mutex_t *lock)
{
	return 0;
}

int my_mutex_unlock(my_mutex_t *lock)
{
  if (fas(&(lock->count)) == 1) {
    unlock(&(lock->lock));
  }
	return 0;
}

int my_mutex_lock(my_mutex_t *lock)
{
  useconds_t delay = 0;
  volatile pthread_t currentThread = pthread_self();
  volatile int isLocked = lock->lock != 0;
  volatile int hasLocks = lock->count > 0;
  volatile int isOwner = pthread_equal(lock->owner, currentThread);

  if (isOwner && isLocked && hasLocks) { 
    lock->count++;
//    printf("\n\t\t\tpid[%d] current count: %d", pthread_self(), lock->count);  
    return 0;
  } else {
    while (1) {
      while (lock->lock) {
        delay += rand() % 10 + 1;
        usleep(delay);
      }
      if (!tas(&(lock->lock))) {
        lock->owner = currentThread;
        lock->count = 1;
        return 0;
      }
    }
  }
  return 0;
}

int my_mutex_trylock(my_mutex_t *lock)
{
	return !tas(&(lock->lock));
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

