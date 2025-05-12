
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "petersonlock.h"

struct petersonlock locks[NPETERSONLOCKS];

/// THOUGHTS
// - maybe change the locks list to struct that has a spin lock to protect it
// they said to MAYBE use __sync_lock_test_and_set and __sync_lock_release which need a spinlock



// ------------------------------------------
//     Documentations from the assignment    
// ------------------------------------------
// • __sync_synchronize is used to ensure all memory operations before
// the call are completed before any memory operations after the call are
// started and that the memory accessed is synchronized between cores.
// This does NOT mean that __sync_synchronize is a mutual exclusion
// mechanism, but rather deals only with the memory system. 
//
// It should be used before reading state that is expected to be shared between cores,
// and after writing shared state.


// • __sync_lock_test_and_set is an atomic exchange operation that sets a
// new value and returns the old value. It can be used to atomically set a
// value and check the old value in a single operation. However, this does
// not guarantee that other cores will see the new value immediately. To
// enforce that, we need to use __sync_synchronize before reading the
// value and after writing it.

// • __sync_lock_release is used to set a value to 0 atomically. Other cores
// may not see the new value immediately, like __sync_lock_test_and_set.


// 
// -------------------------------------------
//  need to protect all the parts that update
// locks[i] because maybe more than one thread
//        are trying to update the list
// -------------------------------------------
// 

void peterson_init(void)
{
    for (int i = 0; i < NPETERSONLOCKS; i++)
    {
        locks[i].alive = 0;
        locks[i].barrier = 0;
        locks[i].interested[0] = 0;
        locks[i].interested[1] = 0;
    }
}


// Creates a new Peterson lock and returns a unique identifier for the lock.
// In case the lock cannot be created, returns -1.
int peterson_create(void)
{
    for (int i = 0; i < NPETERSONLOCKS; i++)
    {
        if(__sync_lock_test_and_set(&locks[i].alive, 1) == 0)
        {
            locks[i].barrier = 0;
            locks[i].interested[0] = 0;
            locks[i].interested[1] = 0;
            
            // ensure the change is visible to all cpus before releasing
            __sync_synchronize();
            return i;
        }
    }
    return -1;
    

    // // ensure single creation at a time
    // while(__sync_lock_test_and_set(list_lock_in_question???, 1) != 0)
    //     ;

    // __sync_synchronize();
    
    // int i;
    // for (i = 0; i < NPETERSONLOCKS; i++)
    // {
    //     if (locks[i].alive == 0)
    //     {
    //         locks[i].alive = 1;
            
    //         // ensure the change is visible to all cpus before releasing
    //         __sync_synchronize();

    //         __sync_lock_release(list_lock_in_question???);

    //         return i;
    //     }
    // }
    
    // __sync_synchronize();

    // __sync_lock_release(list_lock_in_question???);
    
    // return -1;
}

// Takes a lock identifier and the role of the process (0 or 1) and attempts
// to acquire the lock. If the lock is already held by another process, the
// calling process should yield the CPU and try again later. In either case,
// when the function returns, it is guaranteed that no other process holds
// the lock and the calling process has acquired it. Returns 0 on success
// and -1 on error (e.g., the lock identifier or the role is invalid).
int peterson_acquire(int lock_id, int role)
{

    struct petersonlock *lock = &locks[lock_id];
    if (lock->alive == 0 || role < 0 || role > 1)
    {
        // if error return -1

        return -1;
    }

    // ensure all previous pperation are truly coplete before starting the critical section
    __sync_synchronize();

    
    lock->interested[role] = 1;
    lock->barrier = role;
    int other = 1 - role;
    while (lock->barrier == role &&
           lock->interested[other] == 1)
    {
        yield();
    }

    // ensure all previous pperation are truly coplete before starting the critical section
    __sync_synchronize();
    return 0;



    // struct petersonlock *lock;
    // if (getlock(lock_id, &lock) < 0)
    // {
    //     // if error return -1

    //     return -1;
    // }

    // int other = 1 - role;
    // lock->interested[role] = 1;
    // lock->barrier = role;
    // while (lock->barrier == role &&
    //        lock->interested[other] == 1)
    // {
    //     yield();
    // }

    // // ensure all previous pperation are truly coplete before starting the critical section
    // __sync_synchronize();

    // return 0;
}

// Takes a lock identifier and the role of the process (0 or 1) and releases
// the lock. The other role may not acquire the lock until it is released by
// this call. Returns 0 on success and -1 on error (e.g., the lock identifier
// or the role is invalid).
int peterson_release(int lock_id, int role) {
    
    struct petersonlock *lock = &locks[lock_id];
    if (lock->alive == 0 || role < 0 || role > 1)
    {
        // if error return -1

        return -1;
    }
    // ensure all previous pperation are truly coplete before starting the critical section
    __sync_synchronize();

    lock->interested[role] = 0;
    __sync_synchronize();

    
    
    
    // ensure all cricitcal section work is truly complete before leaving the critical section

    // struct petersonlock *lock;
    // if (getlock(lock_id, &lock) < 0)
    // {
    //     // if error return -1

    //     return -1;
    // }

    // lock->interested[role] = 0;
    // return 0;
}

// Deletes the lock with the given identifier. Once this function returns,
// the lock identifier is invalid and neither role can acquire the lock. Either
// role can call this function. Returns 0 on success and -1 on error (e.g.,
// the lock identifier is invalid).
int peterson_destroy(int lock_id) {

    locks[lock_id].alive = 0;
    locks[lock_id].barrier = 0;
    locks[lock_id].interested[0] = 0;
    locks[lock_id].interested[1] = 0;

    __sync_synchronize();

    
    
    
    // ensure single creation at a time
    // while(__sync_lock_test_and_set(list_lock_in_question???, 1) != 0)
    //     ;   

    // struct petersonlock *lock;
    // if (getlock(lock_id, &lock) < 0)
    // {
    //     // if error return -1
    //     __sync_lock_release(list_lock_in_question???);

    //     return -1;
    // }

    // if (lock->alive == 0) {
    //     __sync_lock_release(list_lock_in_question???);

    //     return -1;
    // }

    // lock->alive = 0;


    // // ensure the change is visible to all cpus before releasing
    // __sync_synchronize();

    // __sync_lock_release(list_lock_in_question???);


    // return 0;
}

int getlock(int lock_id, struct petersonlock *lock)
{
    // __sync_synchronize();
    
    // while(__sync_lock_test_and_set(list_lock_in_question???, 1) != 0)
    // ;

    // if (locks[lock_id].alive == 0)
    // {
    //     locks[lock_id].alive = 1;

        
    //     lock = &locks[lock_id];
    //     return 0;
    // }
    // // release(lk); replace with __sync calls
    // return -1;
}