#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>

#define V 10
#define E 20
#define INF 1000000

void generateGraph(int graph[V][V])
{
    int j, u, v, w;
    int count = 0;

    for (int i = 0; i < V; i++)
        for (j = 0; j < V; j++)
            graph[i][j] = 0;

    /* 각 정점을 앞의 정점 중 하나와 연결하여 연결성을 보장한다. */
    for (int v = 1; v < V; v++)
    {
        u = rand() % v;
        w = rand() % 100 + 1;
        graph[u][v] = graph[v][u] = w;
        count++;
    }

    /* 자기 루프와 중복 간선을 제외하고 총 20개까지 추가한다. */
    while (count < E)
    {
        u = rand() % V;
        v = rand() % V;
        if (u == v || graph[u][v] != 0)
            continue;
        w = rand() % 100 + 1;
        graph[u][v] = graph[v][u] = w;
        count++;
    }
}

void printGraph(int graph[V][V])
{
    int u, v;
    printf("Graph\nVertices : %d\nEdges    : %d\n\n", V, E);
    printf("Edge       Weight\n");
    for (u = 0; u < V; u++)
        for (v = u + 1; v < V; v++)
            if (graph[u][v] != 0)
                printf("(%d, %d)     %3d\n", u, v, graph[u][v]);
}

void floydWarshall(int graph[V][V], int dist[V][V], int next[V][V])
{
    int i, j, k;
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < V; j++)
        {
            next[i][j] = -1;
            if (i == j)
            {
                dist[i][j] = 0;
                next[i][j] = j;
            }
            else if (graph[i][j] != 0)
            {
                dist[i][j] = graph[i][j];
                next[i][j] = j;
            }
            else
            {
                dist[i][j] = INF;
            }
        }
    }

    /* k를 중간 정점으로 사용하는 경로와 기존 경로를 비교한다. */
    for (k = 0; k < V; k++)
    {
        for (i = 0; i < V; i++)
        {
            for (j = 0; j < V; j++)
            {
                if (dist[i][k] != INF && dist[k][j] != INF &&
                    dist[i][k] + dist[k][j] < dist[i][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

void printShortestPaths(int dist[V][V], int next[V][V])
{
    int u, v, current;
    int count = 0;
    printf("\nShortest Paths\n\n");
    for (u = 0; u < V; u++)
    {
        for (v = u + 1; v < V; v++)
        {
            printf("%d -> %d\n", u, v);
            if (next[u][v] == -1)
            {
                printf("Path     : No path\nDistance : INF\n\n");
            }
            else
            {
                current = u;
                printf("Path     : %d", current);
                while (current != v)
                {
                    current = next[current][v];
                    printf(" -> %d", current);
                }
                printf("\nDistance : %d\n\n", dist[u][v]);
            }
            count++;
        }
    }
    printf("Total vertex pairs : %d\n", count);
}

void verifyPaths(int graph[V][V], int dist[V][V], int next[V][V])
{
    int u, v, cur, nxt, sum, steps;
    int ok = 0, total = 0;

    for (u = 0; u < V; u++)
    {
        for (v = u + 1; v < V; v++)
        {
            total++;
            sum = 0;
            steps = 0;
            cur = u;
            while (cur != v && steps < V) /* 순환 방지 */
            {
                nxt = next[cur][v];
                if (nxt == -1 || graph[cur][nxt] == 0) /* 존재하지 않는 간선 */
                    break;
                sum += graph[cur][nxt];
                cur = nxt;
                steps++;
            }
            if (cur == v && sum == dist[u][v])
                ok++;
            else
                printf("MISMATCH: %d -> %d (path sum %d, dist %d)\n",
                       u, v, sum, dist[u][v]);
        }
    }
    printf("Verified : %d/%d pairs OK\n", ok, total);
}

int main(int argc, char *argv[])
{
    int graph[V][V], dist[V][V], next[V][V];
    unsigned int seed = (unsigned int)time(NULL);
    char *end;
    unsigned long value;

    if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [seed]\n", argv[0]);
        return 1;
    }
    if (argc == 2)
    {
        errno = 0;
        value = strtoul(argv[1], &end, 10);
        if (argv[1][0] < '0' || argv[1][0] > '9' ||
            *end != '\0' || errno != 0 || value > UINT_MAX)
        {
            fprintf(stderr, "Seed must be an unsigned integer.\n");
            return 1;
        }
        seed = (unsigned int)value;
    }

    srand(seed);
    printf("seed = %u\n\n", seed);
    generateGraph(graph);
    printGraph(graph);
    floydWarshall(graph, dist, next);
    printShortestPaths(dist, next);
    verifyPaths(graph, dist, next);
    return 0;
}
