#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MaxNum 10000
#define ave_k 10
#define ave_k2 6
// #define beta1 0.6
// #define beta2 0.8
#define miu 0.4
#define gamma 0.1
#define q 0.0
#define w 0.0
#define ini_node 0.4
#define cycle 5000

#define Data_R "F:\\SISRS\\Code\\data_put\\Time\\hot.txt" // d是传播率那个，a是alpha

int main()
{
    srand(time(NULL));
    double Rs[51][51];
    for (size_t i = 1; i < 51; i++)
    {
        for (size_t j = 1; j < 51; j++)
        {
            double beta2 = 0.02 * i;
            //double v = 0.2 * i;
            double beta1 = 0.002 * j;
            double S = 1 - ini_node, I = ini_node, Q = 0, R = 0;

            double x = (miu + gamma) / gamma * (q * (1 - miu) + w * (1 - miu) + miu) / (w * (1 - miu) + miu);

            double A = -beta2 * ave_k2 * x;
            double B = beta2 * ave_k2 - beta1 * ave_k * x;
            double C = beta1 * ave_k - miu - q * (1 - miu) + q * w * pow(1 - miu, 2) / (w * (1 - miu) + miu);
            // double A = -v*beta1 * ave_k2 * x;
            // double B = v*beta1 * ave_k2 - beta1 * ave_k * x;
            // double C = beta1 * ave_k - miu - q * (1 - miu) + q * w * pow(1 - miu, 2) / (w * (1 - miu) + miu);

            A = -A;
            B = -B;
            C = -C;

            double delta = pow(B, 2) - 4 * A * C;
            // Rs[i][j] = delta;

            if (delta < 0)
            {
                Rs[i][j] = 0;
            }
            else
            {
                Rs[i][j] = (-B + sqrt(delta)) / (2 * A);
                if (Rs[i][j]<0)
                {
                    Rs[i][j] = 0;
                }
            }
        }
    }

    FILE *fp;
    fp = fopen(Data_R, "w");
    if (fp)
    {
        for (int i = 1; i < 51; i++)
        {
            for (size_t j = 1; j < 51; j++)
            {
                fprintf(fp, "%lf ", Rs[i][j]);
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);

    // system("pause");
    return 0;
}
