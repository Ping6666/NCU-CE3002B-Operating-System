#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void callerSequence(char **commands, char *buffer, int check);

void pwdPrint()
{
    char cwd[1024], hostname[128];
    printf("%s@", getlogin());
    gethostname(hostname, sizeof(hostname));
    printf("%s:", hostname);

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s$ ", cwd);
    else
        perror("Getcwd failed.\n");
}

void commandPrint(char **param)
{
    int i = 0;
    while (*param)
    {
        printf("Param_%02d = %s.\n", ++i, *(param++));
    }
    printf("\n");
}

void tokenizeBuffer(char **param, char *str, char *delim)
{
    int counter = 0;
    char *token = strtok(str, delim);

    while (token != NULL)
    {
        while (token[strlen(token) - 1] == ' ' || token[strlen(token) - 1] == '\n')
        {
            token[strlen(token) - 1] = '\0';
        }
        if (*token != '\0' && strlen(token) > 0)
        {
            param[counter] = (char *)malloc(sizeof(char *) * strlen(token) + 1);
            strcpy(param[counter++], token);
        }
        token = strtok(NULL, delim);
    }
    param[counter] = NULL;

    // commandPrint(param);
}

void executeCommands(char **param)
{
    pid_t pid = fork();

    if (pid < 0) // error
    {
        printf("Fork error.");
        exit(1);
    }
    else if (pid == 0) // child
    {
        execvp(*param, param);
        perror("Invalid commands.\n");
        exit(1);
    }
    else // parent
        wait(NULL);
}

void executeCommandsPiped(char **param)
{

    int i = 0;
    int fd[2][2];

    while (*(param++))
    {
        if (*param)
        {
            if (pipe(fd[!(!(i))]) < 0)
            {
                perror("Pipe error.\n");
                exit(1);
            }
        }

        pid_t pid = fork();

        if (pid < 0) // error
        {
            printf("Fork error.");
            exit(1);
        }
        else if (pid == 0) // child
        {
            if (*param)
            {
                dup2(fd[!(!(i))][1], STDOUT_FILENO);
                close(fd[!(!(i))][0]);
                close(fd[!(!(i))][1]);
            }

            if (!(!(i)))
            {
                dup2(fd[0][0], STDIN_FILENO);
                close(fd[0][0]);
                close(fd[0][1]);
            }
            char *paramNow[512];
            callerSequence(paramNow, *(param - 1), 1);
            exit(0);
        }
        else
        {
            if (!(!(i)))
            {
                close(fd[0][0]);
                close(fd[0][1]);

                fd[0][0] = fd[1][0];
                fd[0][1] = fd[1][1];
            }
            wait(NULL);
        }
        i = 1;
    }
}

void executeCommandsRedirected(char **param, int type) // type : 0 RD, 1 WR, 2 APP
{
    char *paramNow[512], *fileNow[512];
    pid_t pid = fork();

    if (pid < 0) // error
    {
        printf("Fork error.");
        exit(1);
    }
    else if (pid == 0) // child
    {
        int i = 0;
        while (*(param + (++i)))
        {
            tokenizeBuffer(fileNow, *(param + i), " ");
            if (fileNow == NULL)
                break;

            pid_t pid_ = fork();

            if (pid_ < 0) // error
            {
                printf("Fork error.");
                exit(1);
            }
            else if (pid_ == 0) // child
            {
                int fd;
                switch (type)
                {
                case 0:
                    fd = open(fileNow[0], O_RDONLY);
                    break;
                case 1:
                    fd = open(fileNow[0], O_WRONLY | O_CREAT | O_TRUNC, 0664);
                    break;
                case 2:
                    fd = open(fileNow[0], O_WRONLY | O_CREAT | O_APPEND, 0664);
                    break;

                default:
                    break;
                }

                if (fd < 0)
                {
                    perror("File error.\n");
                    exit(1);
                }

                switch (type)
                {
                case 0:
                    dup2(fd, STDIN_FILENO);
                    break;
                case 1:
                    dup2(fd, STDOUT_FILENO);
                    break;
                case 2:
                    dup2(fd, STDOUT_FILENO);
                    break;

                default:
                    break;
                }

                callerSequence(paramNow, *param, 2 + type);
                exit(0);
            }
            else // parent
                wait(NULL);
        }
    }
    else // parent
        wait(NULL);
}

void callerSequence(char **commands, char *buffer, int check)
{
    if (strstr(buffer, ">>") && check > 4) // check = 5
    {
        tokenizeBuffer(commands, buffer, ">>");
        executeCommandsRedirected(commands, 2);
    }
    else if (strchr(buffer, '>') && check > 3) // check = 4
    {
        tokenizeBuffer(commands, buffer, ">");
        executeCommandsRedirected(commands, 1);
    }
    else if (strchr(buffer, '<') && check > 2) // check = 3
    {
        tokenizeBuffer(commands, buffer, "<");
        executeCommandsRedirected(commands, 0);
    }
    else if (strchr(buffer, '|') && check > 1) // check = 2
    {
        tokenizeBuffer(commands, buffer, "|");
        executeCommandsPiped(commands);
    }
    else if (check > 0) // check = 1 or 9
    {
        if (check == 9)
        {
            if (*buffer == '\n')
                return;

            tokenizeBuffer(commands, buffer, " ");

            if (!(*commands))
                return;

            if (strstr(*commands, "cd"))
                chdir(*(commands + 1));
            else if (strstr(*commands, "exit"))
                exit(0);
            else
                executeCommands(commands);
        }
        else
        {
            tokenizeBuffer(commands, buffer, " ");
            executeCommands(commands);
        }
    }
}

int main()
{
    char buffer[1024], *commands[512];

    while (1)
    {
        pwdPrint();
        fgets(buffer, 1024, stdin);
        callerSequence(commands, buffer, 9);
    }
}
