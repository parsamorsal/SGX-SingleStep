#include "simstep.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

    pid_t pid;
    int ret = 1;
    int status;
    pid = fork();
    if (pid == -1)
    {
        // pid == -1 means error occured
        printf("can't fork, error occured\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        printf("child process, pid = %u\n", getpid());

        char *argv_list[] = {"app", NULL};
        system("../SampleEnclave/app");
        //execv("/home/parsa/Desktop/SampleEnclave/app", argv_list);
        exit(0);
    }
    else
    {
        printf("parent process, pid = %u\n", getppid());

        if (waitpid(pid, &status, 0) > 0)
        {
            if (WIFEXITED(status) && !WEXITSTATUS(status))
                printf("program execution successfull\n");

            else if (WIFEXITED(status) && WEXITSTATUS(status))
            {
                if (WEXITSTATUS(status) == 127)
                {
                    // execv failed
                    printf("execv failed\n");
                }
                else
                    printf("program terminated normally,"
                           " but returned a non-zero status\n");
            }
            else
                printf("program didn't terminate normally\n");
        }
        else
        {
            printf("waitpid() failed\n");
        }
        exit(0);
    }
    // single_step_init(1);

    // start_single_stepping();
    
    // stop_single_stepping();

    
}