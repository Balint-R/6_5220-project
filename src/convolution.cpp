#include "atcoder/convolution.hpp"
#include "atcoder/modint.hpp"
#include <bit>
#include <cstdio>
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

    int small_sz = bit_ceil((unsigned) m) * 2;
    int small_n = small_sz - m;

    // fprintf(stderr, "n %d, m %d, sz %d, small_sz %d\n", n, m, sz, small_sz);

    if(small_sz <= sz/2){
        for(int i = m; i < small_sz; i++) brr[i] = 0;

        typedef atcoder::modint998244353 mint;
        static_assert(sizeof(int) == sizeof(mint));
        mint *brr_mint = (mint*) &brr[0];
        atcoder::internal::butterfly(small_sz, brr_mint);

        // arr is used for result
        vector<int> a_cp = arr;
        for(int i = 0; i < n+m-1; i++) arr[i] = 0;
        vector<mint> a_buf(small_sz);

        for(int al = 0; al < n; al += small_n){
            int ar = min(al + small_n, n);
            int num_rem = ar - al;
            for(int i = 0; i < num_rem; i++) a_buf[i] = a_cp[al+i];
            for(int i = num_rem; i < small_sz; i++) a_buf[i] = 0;

            mint *arr_mint = &a_buf[0];
            atcoder::internal::butterfly(small_sz, arr_mint);
            for(int i = 0; i < small_sz; i++) arr_mint[i] *= brr_mint[i];
            mint inv_val = mint(small_sz).inv();
            atcoder::internal::butterfly_inv(small_sz, arr_mint);
            for(int i = 0; i < num_rem+m-1; i++) arr[al+i] += (arr_mint[i] * inv_val).val();
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
