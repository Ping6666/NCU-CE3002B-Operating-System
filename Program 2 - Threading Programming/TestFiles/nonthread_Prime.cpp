#include <iostream>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

#define primeNumber 100000

// primeNumber 100000 take about 43 s

void time_diff(struct timeval x, struct timeval y);

int main()
{
    struct timeval stop, start;

    bool primeTable[primeNumber];

    for (long long i = 2; i <= primeNumber + 1; i++)
    {
        primeTable[i - 2] = true;
    }

    gettimeofday(&start, NULL);

    for (long long computeNow = 2; computeNow <= primeNumber + 1; computeNow++)
    {
        long long sqrt_ = sqrt(computeNow);
        for (long long testNum = 2; testNum <= sqrt_; testNum++)
        {
            if (computeNow % testNum == 0)
            {
                primeTable[computeNow - 2] = false;
                break;
            }
        }
        std::cout << "\rStatus : current / total : " << computeNow - 1 << " / " << primeNumber;
    }

    gettimeofday(&stop, NULL);
    std::cout << "\n";

    /*
    for (long long i = 2; i <= primeNumber + 1; i++)
    {
        if (primeTable[i - 2] == true)
        {
            printf("%d\n", i);
        }
    }
    */

    std::cout << "Finish all compute in ";
    time_diff(start, stop);

    return 0;
}

void time_diff(struct timeval X, struct timeval Y)
{
    double Xs, Ys;

    Xs = (double)X.tv_usec / 1000000 + (double)X.tv_sec;
    Ys = (double)Y.tv_usec / 1000000 + (double)Y.tv_sec;

    printf("%f s\n", Ys - Xs);
}