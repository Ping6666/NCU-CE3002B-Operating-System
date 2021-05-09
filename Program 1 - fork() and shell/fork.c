#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void forkImplement(int mode)
{
    // initialize fork
    pid_t pid = fork();

    // initialize variable
    int sum = 0;
    int len = 35000;

    if (pid == 0) // Child process
    {
        if (mode == 2) // set wait
        {
            wait(NULL);
        }
        // calculate with formula
        sum = len * (len + 1) / 2;
        printf("sum : %d. ", sum);
        // print name and pid
        printf("Child process! ");
        printf("pid : %d. ppid : %d.\n", getpid(), getppid());
    }
    else if (pid > 0) // Parent process
    {
        if (mode == 1) // set wait
        {
            wait(NULL);
        }
        // calculate with loop
        for (int i = 0; i <= len; i++)
        {
            sum += i;
        }
        printf("sum : %d. ", sum);
        // print name and pid
        printf("Parent process! ");
        printf("pid : %d. ppid : %d.\n", getpid(), getppid());
    }
    else // Error
    {
        printf("Error!\n");
    }
}

int main(const int argc, char **argv)
{
    int commandOption;
    if ((commandOption = getopt(argc, argv, "hdcp")) != -1)
    {
        switch (commandOption)
        {
        case 'h': // Print help message
            printf("-h : Print this help.\n");
            printf("-d : Implement fork in default mode.\n");
            printf("-c : Implement fork in child first mode.\n");
            printf("-p : Implement fork in parent first mode.\n");
            break;
        case 'd': // fork with default mode without wait
            forkImplement(0);
            break;
        case 'c': // fork with child first mode with wait in parent
            forkImplement(1);
            break;
        case 'p': // fork with parent first mode with wait in child
            forkImplement(2);
            break;

        default:
            break;
        }
    }
    else // wrong input argc
    {
        printf("Incorrect argc numebr.\n");
        return 1;
    }
}
