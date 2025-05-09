#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/petersonlock.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if (argc != 1) {
        printf("Usage: lock_check\n");
        exit(1);
    }

    // Create a Peterson lock
    int lock_id = peterson_create();
    if (lock_id < 0) {
        printf("Failed to create lock\n");
        exit(1);
    }

    // Fork a child process
    int fork_ret = fork();
    if (fork_ret < 0) {
        printf("Fork failed\n");
        exit(1);
    }

    // Determine the role of the process
    int role = fork_ret > 0 ? 0 : 1;

    // Loop to acquire and release the lock
    for (int i = 0; i < 100; i++) {
        if (peterson_acquire(lock_id, role) < 0) {
            printf("Failed to acquire lock\n");
            exit(1);
        }

        // Critical section
        if (role == 0)
            printf("Parent process in critical section\n");
        else
            printf("Child process in critical section\n");

        if (peterson_release(lock_id, role) < 0) {
            printf("Failed to release lock\n");
            exit(1);
        }
    }

    // Parent process waits for child to finish
    if (fork_ret > 0) {
        wait(0);
        printf("Parent process destroying lock\n");
        if (peterson_destroy(lock_id) < 0) {
            printf("Failed to destroy lock\n");
            exit(1);
        }
    }

    exit(0);
}
