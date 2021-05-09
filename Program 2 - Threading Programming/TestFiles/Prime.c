#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#define primeNumber 1000000
#define pthreadNumber 16

// windows primeNumber 10000000000 about  5.5 s when using inplace  loop
// windows primeNumber 10000000    about 14.0 s when using function loop

// linux   primeNumber 100000      about  8.5 s when using function loop with pthread
// linux   primeNumber 1000000     about 80.0 s when using function loop with pthread

void *checkPrime(void *i);
void time_diff(struct timeval x, struct timeval y);

int main()
{
    struct timeval stop, start;

    gettimeofday(&start, NULL);

    // bool primeTable[primeNumber] = {false};

    for (long long i = 2; i <= primeNumber; i++)
    {

        pthread_t thread_id;
        void *checkResult;
        pthread_create(&thread_id, NULL, checkPrime, (void *)i);
        pthread_join(thread_id, &checkResult);

        if ((bool)checkResult)
        {
            // primeTable[i - 1] == false;
        }
    }

    gettimeofday(&stop, NULL);

    /*
    int len = sizeof(primeTable) / sizeof(bool);
    for (int i = 2; i <= len; i++)
    {
        if (primeTable[i - 1] == false)
        {
            // printf("%d\n", i);
        }
    }
    */

    time_diff(start, stop);

    return 0;
}

void *checkPrime(void *i)
{
    long long inputNum = (long long)i;
    long long sqrt_ = sqrt(inputNum);
    for (long long testNum = 2; testNum <= sqrt_; testNum++)
    {
        if (inputNum % testNum == 0)
        {
            return (void *)false;
        }
    }

    return (void *)true;
}

void time_diff(struct timeval X, struct timeval Y)
{
    double Xs, Ys;

    Xs = (double)X.tv_usec / 1000000 + (double)X.tv_sec;
    Ys = (double)Y.tv_usec / 1000000 + (double)Y.tv_sec;

    printf("%f s\n", Ys - Xs);
}