 #include "kernel/types.h"
#include "kernel/stat.h"
 #include "user.h"


 int main(int argc, char *argv[])
 {
     int id = tournament_create(16);
     if (id < 0)
     {
         printf("Failed to create tournament\n");
         exit(-1);
     }
 
     //printf("Tournament created with ID: %d\n", id);
 
     // Acquire the lock
     if (tournament_acquire() < 0)
     {
         printf("Failed to acquire lock\n");
         exit(-1);
     }
 
     printf("Process %d: Lock acquired\n", getpid());
 
     // Simulate some work
     sleep(1);
     printf("Process %d: Lock release\n", getpid());
     // Release the lock
     if (tournament_release() < 0)
     {
         printf("Failed to release lock\n");
         exit(-1);
     }
 
     
 
     exit(0);
 }

