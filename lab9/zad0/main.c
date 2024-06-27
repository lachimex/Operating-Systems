#include <stdio.h>
#include <time.h>
#include <pthread.h>

pthread_t watek1, watek2;
int i;

void* fun_watka(void* cos){
    int a = 10;
    while(1){
        a++;
        printf("%s %d %d\n", (char *)cos, a, i);
        fflush(stdout);
        sleep(1);
    }
}

int main(){
    pthread_create(&watek1, NULL, &fun_watka, "A");
    pthread_create(&watek2, NULL, &fun_watka, "B");
    for (i = 1; i < 10000; i++) sleep(3);
    return 0;
}