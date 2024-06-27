//analogy for zad1, but this time provider processes and calculates result based on interval provided by requester

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

double function(double x){
    return 4.0 / (x * x + 1);
}

double integral(double a, double b, double (*function)(double), int precision){
    double out = 0;
    double width = (b - a) / precision * 1.0;
    for (int i = 0; i < precision; i++){
        out += function(a + width / 2) * width;
        a += width;
        b += width;
    }
    return out;
}


int main() { 
    int n = 10000; //number of parts
    int precision = 100000; //number of points in each part
    mkfifo("pipe1", S_IRWXU);
    mkfifo("pipe2", S_IRWXU);
    int r = open("pipe1", O_RDONLY);
    if (r < 0) {
        printf("error opening pipe1\n");
        return -1;
    }
    double a, b;
    read(r, &a, sizeof(double));
    read(r, &b,  sizeof(double));
    close(r);
    printf("a: %lf, b: %lf\n", a, b);
    double result = 0;
    double width = (b - a) / n * 1.0;
    for (int i = 0; i < n; i++){
        result += integral(a, a + width, &function, precision);
        a += width;
    }
    int w = open("pipe2", O_WRONLY);
    if (w < 0) {
        printf("error opening pipe2");
        return -1;
    }
    printf("sending back: %lf\n", result);
    write(w, &result, sizeof(double));
    close(w);
    return 0;
}