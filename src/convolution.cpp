#include "atcoder/convolution.hpp"
#include "atcoder/modint.hpp"
#include <vector>
using namespace std;

void convolution(int n, int m, vector<int> &arr, vector<int> &brr){
    // First n elements of arr and first m elements of brr are input.
    // First n+m-1 elements of arr are output.
    // arr and brr must have size that's a power of 2 and >= n+m-1.
    assert(arr.size() == brr.size());
    int sz = arr.size();
    assert(!(sz & (sz-1)));
    assert(sz >= n+m-1);

    if(m < 60){
        for(int i = n+m-1; i >= 0; i--){
            int res = 0;
            for(int j = max(0, i-n); j < min(m, i+1); j++){
                res += arr[i-j] * brr[j];
            }
            arr[i] = res;
        }
        return;
    }

    for(int i = n; i < sz; i++) arr[i] = 0;
    for(int i = m; i < sz; i++) brr[i] = 0;

    typedef atcoder::modint998244353 mint;
    static_assert(sizeof(int) == sizeof(mint));
    mint *arr_mint = (mint*) &arr[0];
    mint *brr_mint = (mint*) &brr[0];
    atcoder::internal::butterfly(sz, arr_mint);
    atcoder::internal::butterfly(sz, brr_mint);
    for(int i = 0; i < sz; i++) arr_mint[i] *= brr_mint[i];
    atcoder::internal::butterfly_inv(sz, arr_mint);
    mint inv_val = mint(sz).inv();
    for(int i = 0; i < n+m-1; i++) arr_mint[i] *= inv_val;
}
