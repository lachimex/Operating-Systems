#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>


#define MAX_THREADS 16

typedef struct {
    char* foreground;
    char* background;
} for_back;

typedef struct {
    for_back* map;
    int starting_col;
    int ending_col;
    pthread_barrier_t* barrier;
} thread_data;

void* fun_watka(void* a) {
    thread_data* watek = (thread_data*)a;
    while (1) {
        update_grid(watek->map->foreground, watek->map->background, watek->starting_col, watek->ending_col);
        pthread_barrier_wait(watek->barrier);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2){
        printf("usage -> ./life <number of threads>\n");
        return 0;
    }
    int threads_num = atoi(argv[1]);
    if (threads_num > MAX_THREADS){
        threads_num = MAX_THREADS;
    }
    pthread_t threads[MAX_THREADS];
    pthread_barrier_t barrier;
    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode

    char *tmp;
    for_back* map = malloc(sizeof(for_back));
    map->foreground = create_grid();
    map->background = create_grid();
    pthread_barrier_init(&barrier, NULL, threads_num);
	int width = GRID_WIDTH;
    thread_data* tab[threads_num];
    for (int i = 0; i < threads_num; i++){
        thread_data* w = malloc(sizeof(thread_data));
        tab[i] = w;
        w->map = map;
        w->starting_col = i * width / threads_num;
        w->ending_col = (i + 1) * width / threads_num - 1;
        w->barrier = &barrier;
        pthread_create(&threads[i], NULL, &fun_watka, w);
    }

    init_grid(map->foreground);

    while (true)
    {
        draw_grid(map->foreground);
        usleep(500 * 1000);
        tmp = map->foreground;
        map->foreground = map->background;
        map->background = tmp;
    }

    endwin(); // End curses mode
    for (int i = 0; i < threads_num; i++) {
        free(tab[i]);
    }
    destroy_grid(map->foreground);
    destroy_grid(map->background);
    free(map);

    return 0;
}
