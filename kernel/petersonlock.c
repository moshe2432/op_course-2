
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "spinlock.h"
#include "petersonlock.h"

struct petersonlock locks[NPETERSONLOCKS];
struct spinlock *lk;
int lastcreatedindex = 0;

void initpetersonlock()
{
    initlock(&lk, "peterson lock");
}

// Creates a new Peterson lock and returns a unique identifier for the lock.
// In case the lock cannot be created, returns -1.
int peterson_create(void)
{
    acquire(lk);
    int i;

    if (lastcreatedindex < NPETERSONLOCKS)
    {
        locks[lastcreatedindex] = newlock;
    }
    else
    {
        for (i = 0; i < NPETERSONLOCKS; i++)
        {
            if (locks[i].alive == 0)
            {
                locks[i].alive = 1;

                release(lk);
                return i;
            }
        }
    }

    release(lk);
    return -1;
    // for lock in lock
    // if alive = false
    // alive = true
    // return index

    // lock
    // new plock
    // NPLOCKS ++
    // return NPLOCKS
}

// Takes a lock identifier and the role of the process (0 or 1) and attempts
// to acquire the lock. If the lock is already held by another process, the
// calling process should yield the CPU and try again later. In either case,
// when the function returns, it is guaranteed that no other process holds
// the lock and the calling process has acquired it. Returns 0 on success
// and -1 on error (e.g., the lock identifier or the role is invalid).
int peterson_acquire(int lock_id, int role)
{
    struct petersonlock *lock;
    if (getlock(lock_id, &lock) < 0)
    {
        // if error return -1

        return -1;
    }

    int other = 1 - role;
    lock->interested[0] = 1;
    lock->barrier = role;
    while (lock->barrier == role &&
           lock->interested[other] == 1)
    {
        yield();
    }

    return 0;
}

int peterson_release(int lock_id, int role) {}

int peterson_destroy(int lock_id) {}

int getlock(int lock_id, struct petersonlock *lock)
{
    acquire(lk);
    if (locks[lock_id].alive == 0)
    {
        locks[lock_id].alive = 1;

        release(lk);
        lock = &locks[lock_id];
        return 0;
    }
    release(lk);
    return -1;
}