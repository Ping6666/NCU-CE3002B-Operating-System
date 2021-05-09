#include <iostream>
#include <math.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>

#define primeNumber 100000

// primeNumber 100000 take about 52 s by threadNumber 12

void checkPrime(long long inputNum, bool &outputBool);
void checkLoop(long long a, long long b, bool &outputBool);
void time_diff(struct timeval x, struct timeval y);

int main()
{
    struct timeval stop, start;
    int threadNumber = std::thread::hardware_concurrency();
    std::cout << "Maximum number of threads a system can support is : " << threadNumber << "\n";

    bool primeTable[primeNumber];

    for (long long i = 2; i <= primeNumber + 1; i++)
    {
        primeTable[i - 2] = true;
    }

    gettimeofday(&start, NULL);

    /*
    std::thread threadId[threadNumber];
    for (long long computeNow = 2; computeNow <= primeNumber + 1; computeNow++)
    {
        int idx = (computeNow - 2) % threadNumber;
        threadId[idx] = std::thread(checkPrime, computeNow, std::ref(primeTable[computeNow - 2]));

        if (idx == threadNumber - 1)
        {
            for (auto &thread : threadId)
            {
                thread.join();
            }
        }
        std::cout << "\rStatus : current / total : " << computeNow - 1 << " / " << primeNumber;
    }
    */

    std::thread threadId[threadNumber];
    for (long long computeNow = 2; computeNow <= primeNumber + 1; computeNow++)
    {
        bool primeChecking[threadNumber];
        for (int i = 0; i < threadNumber; i++)
        {
            primeChecking[i] = false;
        }
        long long sqrt_ = sqrt(computeNow);
        for (long long testNum = 2; testNum <= sqrt_; testNum++)
        {
            if (computeNow - 2 > threadNumber)
            {
                int idx = (testNum - 2) % threadNumber;

                threadId[idx] = std::thread(checkLoop, computeNow, testNum, std::ref(primeChecking[idx]));

                if (idx == threadNumber - 1)
                {
                    for (int j = 0; j < threadNumber; j++)
                    {
                        if (primeChecking[j] == true)
                        {
                            primeTable[computeNow - 2] = true;
                            break;
                        }
                    }
                    for (auto &thread : threadId)
                    {
                        if (thread.joinable())
                            thread.join();
                        else
                            thread.detach();
                    }
                }
                else if (testNum == sqrt_)
                {
                    for (int j = 0; j < threadNumber; j++)
                    {
                        if (primeChecking[j] == true)
                        {
                            primeTable[computeNow - 2] = true;
                            break;
                        }
                    }
                    for (auto &thread : threadId)
                    {
                        if (thread.joinable())
                            thread.join();
                        else
                            thread.detach();
                    }
                }
            }
            else
            {
                if (computeNow % testNum == 0)
                {
                    primeTable[computeNow - 2] = false;
                    break;
                }
            }

            std::cout << "\rStatus : current / total : " << computeNow - 1 << " / " << primeNumber;
        }
    }
    /*
    for (auto &thread : threadId)
    {
        if (thread.joinable())
            thread.join();
        else
            thread.detach();
    }
    */

    gettimeofday(&stop, NULL);
    std::cout << "\n";

    /*
    for (int i = 2; i <= primeNumber + 1; i++)
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

void checkPrime(long long inputNum, bool &outputBool)
{
    long long sqrt_ = sqrt(inputNum);
    for (long long testNum = 2; testNum <= sqrt_; testNum++)
    {
        if (inputNum % testNum == 0)
        {
            outputBool = false;
            return;
        }
    }
    outputBool = true;
    return;
}

void checkLoop(long long a, long long b, bool &outputBool)
{
    if (a % b == 0)
    {
        outputBool = false;
        return;
    }
    outputBool = true;
    return;
}

void time_diff(struct timeval X, struct timeval Y)
{
    double Xs, Ys;

    Xs = (double)X.tv_usec / 1000000 + (double)X.tv_sec;
    Ys = (double)Y.tv_usec / 1000000 + (double)Y.tv_sec;

    printf("%f s\n", Ys - Xs);
}