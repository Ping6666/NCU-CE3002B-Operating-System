#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    // 建立子行程
    pid_t pid = fork();

    if (pid == 0) // 子行程
    {
        // wait(NULL);
        printf("Child process!.  pid number : %d\n", getpid());
    }
    else if (pid > 0) // 父行程
    {
        // wait(NULL);
        printf("Parent process!. pid number : %d\n", getpid());
    }
    else // 錯誤
    {
        printf("Error!\n");
    }

    return 0;
}