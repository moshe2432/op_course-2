#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/petersonlock.h"
#include "user/user.h"

#include <math.h>

int id_counter = 0;

struct tournament_tree
{
    int id;
    int num_processes;
    struct tournament_tree *next;
    int *treeNodes;
    int *processes;
};

struct tournament_tree *trees;

#define MAX_LEVELS 4 // log2(16)

struct lock_path_entry
{
    int lock_id;
    int role; // 0 or 1
};

// Each process holds this:
struct lock_path_entry path[MAX_LEVELS];
int path_length = 0;

int log2_int(unsigned int x)
{
    int res = 0;
    while (x >>= 1)
    {
        res++;
    }
    return res;
}

int get_role(int index, int L, int l)
{
    int role;
    role = (index & (1 << (L - l - 1))) >> (L - l - 1);
    return role;
}

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
    int pid = 0;
    if (processes != 2 && processes != 4 && processes != 8 && processes != 16)
        return -1;

    int L = log2_int(processes);

    struct tournament_tree *tree = malloc(sizeof(struct tournament_tree));
    tree->treeNodes = malloc(sizeof(int) * (processes - 1));
    tree->processes = malloc(sizeof(int) * processes);
    tree->id = id_counter++;
    tree->num_processes = processes;

    for (int i = 0; i < processes - 1; i++)
    {
        tree->treeNodes[i] = peterson_create();
    }

    for (int i = 1; i < processes; i++)
    {
        pid = fork();
        if (pid < 0)
            return -1;

        if (pid == 0)
        {
            // In child
            for (int l = 0; l < L; l++)
            {
                int parent = (i >> (l + 1)) + (1 << (L - l - 1)) - 1;
                int role = (i >> l) & 1;
                path[l].lock_id = tree->treeNodes[parent];
                path[l].role = role;
            }
            path_length = L;
            break;
        }
        else
        {
            tree->processes[i] = pid;
        }
    }

    tree->processes[0] = getpid();
    if (pid != 0)
    {
        // In parent
        for (int l = 0; l < L; l++)
        {
            int role = get_role(0, L, l);
            path[l].lock_id = tree->treeNodes[(1 << (L - l - 1)) - 1];
            path[l].role = role;
        }
        path_length = L;
    }

    tree->next = trees;
    trees = tree;
    return tree->id;
}

// Attempts to complete the tournament to acquire the lock at the root
// of the tree for the calling process. This function uses the lock and role
// assigned to the calling process by tournament_create. Returns 0 on success and -1 on error.
int tournament_acquire(void)
{

    for (int i = 0; i < path_length; i++)
    {
        peterson_acquire(path[i].lock_id, path[i].role);
    }
    return 0;
}

// Releases all locks held by the calling process in the reverse order of
// acquisition. Returns 0 on success and -1 on error.
int tournament_release(void)
{
    for (int i = path_length - 1; i >= 0; i--)
    {
        peterson_release(path[i].lock_id, path[i].role);
    }
    return 0;
}

int tournament_wait(int id)
{
    struct tournament_tree *tree = trees;
    int pid = getpid();
    while (tree->id != id)
    {
        if (tree->next == 0)
        {
            return -1;
        }
        tree = tree->next;
    }

    for (int i = 0; i < tree->num_processes; i++)
    {
        if (pid != tree->processes[i])
        {
            wait(tree->processes + i);
        }
    }
    return 0;
}

int tournament_delete(int id)
{
    struct tournament_tree *tree = trees;

    while (tree->id != id)
    {
        if (tree->next == 0)
        {
            return -1;
        }
        tree = tree->next;
    }

    for (int i = 0; i < tree->num_processes - 1; i++)
    {
        peterson_destroy(tree->treeNodes[i]);
    }
    return 0;
}