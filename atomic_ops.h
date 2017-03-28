#ifndef __ATOMIC_OPS_H__
#define __ATOMIC_OPS_H__


static inline unsigned long faa(volatile int * ptr) // Fetch and Add
{
	return __sync_fetch_and_add(ptr, 1);
}

static inline unsigned long fas(volatile int * ptr) // Fetch and Subtract
{
	return __sync_fetch_and_sub(ptr, 1);
}

static inline unsigned long tas(volatile unsigned long* ptr) //Test and Set
{
	return __sync_lock_test_and_set(ptr, 1);
}


static void unlock(volatile unsigned long* ptr)
{
  __sync_lock_release(ptr);
}


static inline unsigned long cas(volatile unsigned long* ptr, unsigned long old, unsigned long _new) //Compare and Swap
{
    return __sync_val_compare_and_swap(ptr, old, _new);

}


#endif // __ATOMIC_OPS_H__
