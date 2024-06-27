//analogy for zad1, but this time requester requests computing integral with custom interval and then it waits for the
// calculated result

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    double a, b;
    printf("start of an interval: \n");
    scanf("%lf", &a);
    printf("end of an interval: \n");
    scanf("%lf", &b);
    int w = open("pipe1", O_WRONLY);
    write(w, &a, sizeof(double));
    write(w, &b, sizeof(double));
    close(w);
    int r = open("pipe2", O_RDONLY);
    if (r < 0){
        printf("error opening a pipe2\n");
        return -1;
    }
    double output;
    read(r, &output, sizeof(double));
    printf("received: %lf\n", output);
    close(r);
    return 0;
}