#include <bits/stdc++.h>
using namespace std;

mt19937 rng(430298584);
int randInt(int a, int b){return uniform_int_distribution(a, b)(rng);}

int main(int argc, char **argv){
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    printf("%d %d\n", n, m);
    for(int i = 0; i < n; i++) putchar(randInt('a', 'f'));
    putchar('\n');
    for(int i = 0; i < m; i++) putchar(randInt('a', 'f'));
    putchar('\n');
}
