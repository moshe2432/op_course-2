#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/petersonlock.h"
#include "user/user.h"
#include "proc.h"
#include <math.h>

struct tournament_node
{
    struct tournament_node *parent;

    struct tournament_node *right;
    struct tournament_node *left;

    int lock_id;    // or this
    int process_id; // or this
    int index;
};

struct tournament_node *root;

// Creates a new tournament tree with the given number of processes.
// This function accepts the number of processes that will participate in
// the tournament, which must be a power of 2 up to 16. It will create all
// locks needed for the tree, fork the processes, and assign each process
// a lock and a role in each tree level. Returns the tournament ID assigned
// to the process on success and -1 on error, such as an invalid number of
// processes or if the tree cannot be created. If a fork operation fails, the
// function should return -1, but does not attempt to clean up the tree.
int tournament_create(int processes)
{
    int height;
    if (processes != 2 && processes != 4 && processes != 8 || processes != 16)
    {
        return -1;
    }

    height = log2(processes) - 1;

    // create root

    /// [processes -1] number of lock nodes
    /// we have [processes] number of leaves and

    // for height - 1

    return 0;
}

// Attempts to complete the tournament to acquire the lock at the root
// of the tree for the calling process. This function uses the lock and role
// assigned to the calling process by tournament_create. Returns 0 on success and -1 on error.
int tournament_acquire(void)
{
    // from the child
    int process_id, role, index, L, l;
    struct proc *p = myproc();
    process_id = p->pid;

    // Find the starting node of the current process (leaf)
    L = 0;
    struct tournament_node *current_node = root;
    // list of nodes to check
    // number of children to check = 2 ^ L
    while (current_node->process_id != process_id && current_node != 0)
    {
        
        L++;
        // BFS
    }

    if (current_node == 0)
        return -1;

    index = current_node->index;
    l = 0;

    // Play the tournament
    // Go up a node, try to catch, continue until finished

    while (current_node != 0)
    {
        role = get_role(index, L, l);
        if (current_node->lock_id != 0)
        {
            if (peterson_acquire(current_node->lock_id, role) < 0)
            {
                printf("Failed to acquire lock\n");
                exit(1);
            }
        }

        if (current_node->parent == 0)
            return 0;

        current_node = current_node->parent;
        l++;
        // sleep
    }
    // run bfs until node with same process_id
    // maybe reverse try to catch the locks
    // if catch root lock success

    return 0;
}

int get_role(int index, int L, int l)
{
    int role;
    role = (index & (1 << (L - l - 1))) >> (L - l - 1);
    return role;
}

// Releases all locks held by the calling process in the reverse order of
// acquisition. Returns 0 on success and -1 on error.
int tournament_release(void)
{
    // starting from root
    int process_id;
    struct proc *p = myproc();
    process_id = p->pid;

    // Find the starting node of the current process (leaf)
    struct tournament_node *current_node = root;
    while (current_node->process_id != process_id && current_node != 0)
    {
        // BFS
    }

    if (current_node == 0)
        return -1;

    // if (peterson_release(lock_id, role) < 0)
    // {
    //     printf("Failed to release lock\n");
    //     exit(1);
    // }
    return 0;
}