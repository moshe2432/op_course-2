#include "types.h"
// Mutual exclusion lock.

struct petersonlock
{
    uint locked; // Is the lock held?
    uint barrier;
    uint interested[2];
    uint alive; //
     

    // For debugging:
    char *name;      // Name of lock.
    struct cpu *cpu; // The cpu holding the lock.
};
