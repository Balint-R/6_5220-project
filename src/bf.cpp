#include <bits/stdc++.h>
using namespace std;

#pragma GCC target "avx2"
#pragma GCC optimize "Ofast"

const int MN = 10 << 20;
int n, m;
char s1[MN], s2[MN];
int ans[MN];

int main(){
    scanf("%d %d\n", &n, &m);
    assert(n < MN && m <= n);
    fread(s1, 1, n+1, stdin);
    fread(s2, 1, m+1, stdin);
    s1[n] = s2[m] = 0;

    for(int i = 0; i < n-m+1; i++){
        int res = 0;
        for(int j = 0; j < m; j++) res += s1[i+j] != s2[j];
        ans[i] = res;
    }

    for(int i = 0; i < n-m+1; i++) printf("%d%c", ans[i], " \n"[i == n-m]);
}
