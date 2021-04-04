#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void forkImplement(int mode)
{
    // 建立子行程
    pid_t pid = fork();

    if (pid == 0) // 子行程
    {
        if (mode == 2)
        {
            wait(NULL);
        }
        int len = 100;
        int sum = len * (len + 1) / 2;
        printf("sum : %d.  ", sum);
        printf("Child process!.  pid number : %d\n", getpid());
    }
    else if (pid > 0) // 父行程
    {
        if (mode == 1)
        {
            wait(NULL);
        }
        int sum = 0;
        int len = 100;
        for (int i = 0; i <= len; i++)
        {
            sum += i;
        }
        printf("sum : %d.  ", sum);
        printf("Parent process!. pid number : %d\n", getpid());
    }
    else // 錯誤
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
        case 'h':
            printf("-h : Print this help.\n");
            printf("-d : Implement fork in default mode.\n");
            printf("-c : Implement fork in child first mode.\n");
            printf("-p : Implement fork in parent first mode.\n");
            break;
        case 'd':
            forkImplement(0);
            break;
        case 'c':
            forkImplement(1);
            break;
        case 'p':
            forkImplement(2);
            break;

        default:
            break;
        }
    }
    else
    {
        printf("Incorrect argc numebr.\n");
        return 1;
    }
}