//program demonstrating different library linking methods
//static, shared and dynamic
#include <stdio.h>

#ifndef USE_DYNAMIC
    #include "collatz.h"
#else
    #include <dlfcn.h>
    int (*test_collatz_convergence)(int, int);
#endif

int main(){
    int tab[5] = {10, 20, 15, 156, 7};

    #ifdef USE_DYNAMIC
        void *handle = dlopen("libcollatz.so", RTLD_LAZY);
        if (!handle){
            fprintf(stderr, "error opening a library\n");
            return -1;
        }
        test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
        if (!test_collatz_convergence){
            fprintf(stderr, "error searching for function\n");
            return -1;
        }
    #endif
    for (int i = 0; i < 5; ++i){
        printf("Liczba: %d ", tab[i]);
        printf("liczba iteracji: %d\n", test_collatz_convergence(tab[i], 1000));
    }
    #ifdef USE_DYNAMIC
        dlclose(handle);
    #endif
    return 0;
}