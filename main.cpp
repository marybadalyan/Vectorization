#include <iostream>
#include <vector>
#include <random>
#include <immintrin.h> 
#include "compiler_config.h"

void add(int *__restrict a, int *__restrict b, int *__restrict c, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = b[i] + c[i];
}

DISABLE_OPTIMIZATION

void add_vectorized(int *a, int *b, int *c, int n) {
    int i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256i vb = _mm256_loadu_si256((__m256i*)&b[i]);
        __m256i vc = _mm256_loadu_si256((__m256i*)&c[i]);
        __m256i va = _mm256_add_epi32(vb, vc);
        _mm256_storeu_si256((__m256i*)&a[i], va);
    }
    for (; i < n; ++i) {
        a[i] = b[i] + c[i];
    }
}

RESTORE_OPTIMIZATION

int main(){
    int n = 64;
   
    std::vector<int> a(n,0);
    std::vector<int> b(n);
    std::vector<int> c(n);


    std::random_device rd;                     
    std::mt19937 gen(rd());                   
    std::uniform_int_distribution<> dist(1000,100000);  

    for(int i = 0; i < n; i++){
        b[i] = dist(gen);
        c[i] = dist(gen);
    }

    add(a.data(),b.data(),c.data(),n);
    add_vectorized(a.data(),b.data(),c.data(),n);
    
}