#include "types.h"
// Mutual exclusion lock.

struct petersonlock
{
    uint locked; // Is the lock held?
    uint barrier;
    uint interested[2];
    uint alive; //
    uint pid[2];    // Lock identifier. 

    // For debugging:
    char *name;      // Name of lock.
    struct cpu *cpu; // The cpu holding the lock.
};
