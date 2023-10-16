#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MaxNum 2000
#define P1 0.001
#define P2 1
// #define beta1 0.6
// #define beta2 1.0
#define miu 0.5
#define gamma 0.5
#define q 0.1
#define w 0.1
#define ini_node 800
#define cycle 1

#define Data_R "F:\\SISRS\\Code\\data_put\\line\\2023test.txt" // d是传播率那个，a是alpha
// #define Data_1d "F:\\SISRS\\Code\\network\\network_d1_3.txt"
// #define Data_2d_1 "F:\\SISRS\\Code\\network\\network_d2_1_3.txt"
// #define Data_2d_2 "F:\\SISRS\\Code\\network\\network_d2_2_3.txt"
// #define Data_2d_3 "F:\\SISRS\\Code\\network\\network_d2_3_3.txt"
#define Data_1d "F:\\SISRS\\Code\\network\\network_d1_test2.txt"
#define Data_2d_1 "F:\\SISRS\\Code\\network\\network_d2_1_test2.txt"
#define Data_2d_2 "F:\\SISRS\\Code\\network\\network_d2_2_test2.txt"
#define Data_2d_3 "F:\\SISRS\\Code\\network\\network_d2_3_test2.txt"


/*Create an adjacency list*/
typedef struct node
{
    int adjvex_1;
    int adjvex_2[2];
    struct node *next;
} EdgeNode;

typedef struct
{
    int vertex;            // ID node
    int state, last_state; // s0,i1,r2,q3
    int k;
    EdgeNode *firstedge;
} VertexNode;

typedef VertexNode AdjList[MaxNum];

typedef struct
{
    AdjList hyper_node;
    AdjList graph;
    double ave_k, ave_k2;
    double I;
} ALGraph;

void creatnetwork(ALGraph *);
void spread_sir(ALGraph *, double beta_in, double beta_on);
void spread_edge(ALGraph *, int node, double beta_in, double beta_on);


int main()
{
    
    ALGraph *G = (ALGraph *)malloc(sizeof(ALGraph));
    creatnetwork(G);
    srand(time(NULL));

   

    printf("%lf %lf\n", G->ave_k, G->ave_k2);

    double count[31];
    double Rs[31];

    for (size_t i = 0; i < 31; i++)
    {
        //count[i] = i * 0.01;
        // count[i]=0.2;
        //count[i] = i * 0.1 / G->ave_k * miu;
        count[i] = i * 0.15 / G->ave_k * miu;
        spread_sir(G, count[i], 4.0 * 1.0 / G->ave_k2 * miu);
        //spread_sir(G, count[i], 1.0);
        Rs[i] = G->I;
        printf("%d %lf\n", i+1, Rs[i]);
        break;
    }

    // FILE *fp;
    // fp = fopen(Data_R, "w");
    // if (fp)
    // {
    //     for (int i = 0; i < 31; i++)
    //     {
    //         fprintf(fp, "%lf\n", Rs[i]);
    //     }
    // }
    // fclose(fp);

    system("pause");
    return 0;
}

void creatnetwork(ALGraph *G)
{

    //普通图
    int c = 0;
    int lines = 0;
    int sum_k = 0;
    int sum_kk = 0;

    FILE *fp;
    fp = fopen(Data_1d, "r");
    if (fp)
    {
        while ((c = fgetc(fp)) != EOF)
            if (c == '\n')
                lines++;
        fclose(fp);
    }

    int **a;
    a = (int **)malloc(sizeof(int *) * lines);

    for (size_t i = 0; i < lines; i++)
    {
        a[i] = (int *)malloc(sizeof(int) * 2);
    }
    fclose(fp);

    /*把文件读为数组，并求出节点数*/
    fp = fopen(Data_1d, "r");
    for (size_t i = 0; i < lines; i++)
    {
        for (size_t j = 0; j < 2; j++)
        {
            fscanf(fp, "%d", &a[i][j]);
        }
    }
    fclose(fp);
    /*关闭文件*/

    /*置空顶点表*/
    for (size_t i = 0; i < MaxNum; i++)
    {
        EdgeNode *h = (EdgeNode *)malloc(sizeof(EdgeNode));
        h->next = NULL;
        G->graph[i].firstedge = h;
        G->graph[i].vertex = i;
    }
    /*建立边表*/
    for (size_t k = 0; k < lines; k++)
    {
        int i = a[k][0];
        int j = a[k][1];
        EdgeNode *s = (EdgeNode *)malloc(sizeof(EdgeNode));
        s->adjvex_1 = j;
        s->next = G->graph[i].firstedge->next;
        G->graph[i].firstedge->next = s;
    }

    //计算度
    for (size_t i = 0; i < MaxNum; i++)
    {
        EdgeNode *s = (EdgeNode *)malloc(sizeof(EdgeNode));
        s = G->graph[i].firstedge;
        int count = 0;
        while (G->graph[i].firstedge->next != NULL)
        {
            count++;
            G->graph[i].firstedge = G->graph[i].firstedge->next;
        }
        G->graph[i].firstedge = s;
        G->graph[i].k = count;
        sum_k += G->graph[i].k;
        if (G->graph[i].k==0)
        {
            printf("H");
        }
        
    }
    G->ave_k = 1.0 * sum_k / MaxNum;
    sum_k = 0;

    /*单纯形图*/
    int array_edgenum[MaxNum];
    int array_nodeid[10000];
    int Max;

    /*置空超图顶点表*/
    for (size_t i = 0; i < MaxNum; i++)
    {
        EdgeNode *h;
        h = (EdgeNode *)malloc(sizeof(EdgeNode));
        h->next = NULL;
        G->hyper_node[i].firstedge = h;
        G->hyper_node[i].vertex = i;
    }

    fp = fopen(Data_2d_3, "r");
    for (size_t i = 0; i < MaxNum; i++)
    {
        fscanf(fp, "%d", &array_edgenum[i]);
    }
    fclose(fp);

    fp = fopen(Data_2d_1, "r");
    for (size_t i = 0; i < MaxNum; i++)
    {
        Max = array_edgenum[i];
        G->hyper_node[i].k = Max;
        //printf("%d\n",Max);
        sum_k += G->hyper_node[i].k;
        for (size_t j = 0; j < Max; j++)
        {
            fscanf(fp, "%d", &array_nodeid[j]);
            EdgeNode *s = (EdgeNode *)malloc(sizeof(EdgeNode));
            s->adjvex_2[0] = array_nodeid[j];
            s->next = G->hyper_node[i].firstedge->next;
            G->hyper_node[i].firstedge->next = s;
        }
    }
    fclose(fp);

    G->ave_k2 = 1.0 * sum_k / MaxNum;

    fp = fopen(Data_2d_2, "r");
    for (size_t i = 0; i < MaxNum; i++)
    {
        Max = array_edgenum[i];
        for (size_t j = 0; j < Max; j++)
        {
            fscanf(fp, "%d", &array_nodeid[j]);
        }
        EdgeNode *s;
        s = G->hyper_node[i].firstedge;
        while (G->hyper_node[i].firstedge->next != NULL)
        {
            G->hyper_node[i].firstedge = G->hyper_node[i].firstedge->next;
            G->hyper_node[i].firstedge->adjvex_2[1] = array_nodeid[Max - 1];
            Max--;
        }
        G->hyper_node[i].firstedge = s;
        s = NULL;
        free(s);
    }
    fclose(fp);

}
void spread_sir(ALGraph *G, double beta_in, double beta_on)
{
    double SUM_I = 0; // 轮总的R和I个数
    double x;
    for (size_t t = 0; t < cycle; t++)
    {
        int sum_i = 0; // 轮内的R和I数量
        // 初始化节点状态为0
        for (size_t i = 0; i < MaxNum; i++)
        {
            G->hyper_node[i].state = 0;
            G->hyper_node[i].last_state = 0;
        }

        // 选择初始节点
        int array_node[ini_node];
        for (int i = 0; i < ini_node; i++) // 选择初始节点
        {
            while (1)
            {
                int t_x = rand() % MaxNum;
                int judge = 0;
                array_node[i] = t_x;
                if (i > 1)
                {
                    for (int j = 0; j < i; j++)
                    {
                        if (array_node[i] == array_node[j])
                        {
                            judge = 1;
                        }
                    }
                }
                if (judge == 0)
                    break;
            }
        }

        // 初始感染节点赋值
        for (size_t i = 0; i < ini_node; i++)
        {
            G->hyper_node[array_node[i]].last_state = 1;
            G->hyper_node[array_node[i]].state = 1;
        }
        int count = 0;
        double sum = 0;
        int count_p = 0;

        // 进入感染过程
        while (1)
        {
            for (size_t i = 0; i < MaxNum; i++)
            {
                if (G->hyper_node[i].state == 0) // S
                {
                    spread_edge(G, i, beta_in, beta_on);
                }
                else if (G->hyper_node[i].state == 1) // I
                {
                    x = (double)rand() / (RAND_MAX);
                    //printf("1=%lf ",x);
                    if (x < miu)
                    {
                        G->hyper_node[i].state = 2;
                    }
                    else
                    {
                        x = (double)rand() / (RAND_MAX);
                        //printf("2=%lf\n",x);
                        if (x < q)
                        {
                            G->hyper_node[i].state = 3;
                        }
                    }
                }
                else if (G->hyper_node[i].state == 3) // Q
                {
                    x = (double)rand() / (RAND_MAX);
                    if (x < miu)
                    {
                        G->hyper_node[i].state = 2;
                    }
                    else
                    {
                        x = (double)rand() / (RAND_MAX);
                        if (x < w)
                        {
                            G->hyper_node[i].state = 1;
                        }
                    }
                }
                else // R
                {
                    x = (double)rand() / (RAND_MAX);
                    if (x < gamma)
                    {
                        G->hyper_node[i].state = 0;
                    }
                }
            }
            // 总的感染数
            for (size_t i = 0; i < MaxNum; i++)
            {
                if (G->hyper_node[i].state == 1)
                {
                    sum_i++;
                }

            }
            //printf("%d ", sum_i);
            count++;
            if (sum_i == 0)
            {
                SUM_I += 0;
                //printf("%lf\n",0);
                break;
            }

            if (count > 1000)
            {
                //break;
                sum += (1.0 * sum_i / MaxNum);
                count_p++;
                if (count_p == 200)
                {
                    SUM_I += sum / count_p;
                    //printf("%lf\n",sum / count_p);
                    break;
                }

                // SUM_I += (1.0 * sum_i / MaxNum);
                // break;
            }
            sum_i = 0;
            for (size_t i = 0; i < MaxNum; i++)
            {
                G->hyper_node[i].last_state = G->hyper_node[i].state;
            }
        }
    }
    G->I = SUM_I / cycle;
}
void spread_edge(ALGraph *G, int node, double beta_in, double beta_on)
{
    double q_in = 1, q_on = 1; // 总的感染率
    int count_infect = 0;
    double x;
    //double x = (double)rand() / (RAND_MAX);
    int c=0;

    EdgeNode *d = G->hyper_node[node].firstedge;
    while (G->hyper_node[node].firstedge->next != NULL)
    {
        int count = 0;
        G->hyper_node[node].firstedge = G->hyper_node[node].firstedge->next;
        int n1 = G->hyper_node[node].firstedge->adjvex_2[0];
        int n2 = G->hyper_node[node].firstedge->adjvex_2[1];

        if (G->hyper_node[n1].last_state == 1)
        {
            count++;
        }
        if (G->hyper_node[n2].last_state == 1)
        {
            count++;
        }
        if (count == 2)
        {
            x = (double)rand() / (RAND_MAX);
            if (x <= beta_on)
            {
                c=1;
                //break;
            }
            
            count_infect++;
        }
    }
    G->hyper_node[node].firstedge = d;
    d = NULL;
    free(d);

    q_on = pow((1 - beta_on), count_infect);

    count_infect = 0;
    // 点对点传播
    EdgeNode *s = G->graph[node].firstedge;
    while (G->graph[node].firstedge->next != NULL)
    {
        G->graph[node].firstedge = G->graph[node].firstedge->next;
        int n = G->graph[node].firstedge->adjvex_1;
        if (G->hyper_node[n].last_state == 1)
        {
            x = (double)rand() / (RAND_MAX);
            if (x <= beta_in)
            {
                c=1;
                //break;
            }
            count_infect++;
        }
    }
    G->graph[node].firstedge = s;
    s = NULL;
    free(s);

    q_in = pow((1 - beta_in), count_infect);

    double t_x = q_in * q_on;

    if (x < t_x)
    {
        G->hyper_node[node].state = 0;
    }
    else
    {
        G->hyper_node[node].state = 1;
    }
    // if (c == 0)
    // {
    //     G->hyper_node[node].state = 0;
    // }
    // else
    // {
    //     G->hyper_node[node].state = 1;
    // }
}
