#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int number_of_raindeers = 0;
int present_deliver_times = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

pthread_mutex_t ren_mutexes[9] = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER
};

pthread_t ren[9];
pthread_t mik;


void* fun_watka_mik(void* a) {
    while (present_deliver_times < 4) {
        pthread_cond_wait(&cond, &mutex);
        printf("Rozwozimy prezenety HO HO HO\n");
        present_deliver_times++;
        sleep(rand() % 3 + 2);
        printf("rozwiozlem juz %d razy prezenty, lece na wakacje\n", present_deliver_times);
        for (int i = 0; i < 9; i++) {
            pthread_mutex_unlock(&ren_mutexes[i]);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* fun_watka_ren(void* a) {
    int id = *(int *) a;
    printf("id = %d\n", id);
    pthread_mutex_lock(&ren_mutexes[id]);
    while (present_deliver_times < 4) {
        sleep(rand() % 6 + 5);
        pthread_mutex_lock(&mutex);
        number_of_raindeers++;
        printf("Renifer nr %d: czeka %d reniferow na mikolaja\n", id, number_of_raindeers);
        if (number_of_raindeers == 9) {
            printf("Wybudzam mikolaja\n");
            number_of_raindeers = 0;
            pthread_cond_broadcast(&cond);
        }
        pthread_mutex_unlock(&mutex);
        pthread_mutex_lock(&ren_mutexes[id]);
    }
    return NULL;
}

int main() {
    int ids[9];
    pthread_create(&mik, NULL, fun_watka_mik, NULL);
    for (int i = 0; i < 9; i++) {
        ids[i] = i;
        pthread_create(ren + i, NULL, fun_watka_ren, &ids[i]);
    }
    while(present_deliver_times < 4);
}