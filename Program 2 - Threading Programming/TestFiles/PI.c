#include <stdio.h>
/*
int main()
{
    int step = 1000000;
    double PI = 3;
    // Nilakantha's series
    for (double i = 2; i <= 2 * step; i += 2)
    {
        double tmp = (4 / (i * (i + 1) * (i + 2)));
        PI += ((((int)i % 4) - 1) * tmp);
    }
    printf("%.16f\n", PI);

    return 0;
}
*/
// /*
int main()
{
    // https://crypto.stanford.edu/pbc/notes/pi/code.html
    int r[280 + 1];
    int i, k;
    int b, d;
    int c = 0;

    for (i = 0; i < 280; i++)
    {
        r[i] = 2000;
    }

    for (k = 280; k > 0; k -= 14)
    {
        d = 0;

        i = k;
        for (;;)
        {
            d += r[i] * 10000;
            b = 2 * i - 1;

            r[i] = d % b;
            d /= b;
            i--;
            if (i == 0)
                break;
            d *= i;
        }
        printf("%.4d", c + d / 10000);
        c = d % 10000;
    }

    return 0;
}
// */