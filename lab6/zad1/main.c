//using pipes compute integral based on given integral, each process computes some part of the result

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

double function(double x) {
    return 4.0 / (x * x + 1);
}

double integral(double a, double b, double (*function)(double), int precision) {
    double out = 0;
    double width = (b - a) / precision;
    for (int i = 0; i < precision; i++) {
        out += function(a + (i + 0.5) * width) * width;
    }
    return out;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s usage: {n x} n - number of intervals, x - precision\n", argv[0]);
        return -1;
    }
    int n = atoi(argv[1]);
    int x = atoi(argv[2]);
    double a = 0.0, b = 1.0, result;
    pid_t pid;
    int fd[n][2];

    for (int i = 0; i < n; i++) {
        pipe(fd[i]);
        pid = fork();
        if (pid == 0) {
            close(fd[i][0]);
            a = 1.0 / n * i;
            b = 1.0 / n * (i + 1);
            result = integral(a, b, &function, x / n);
            write(fd[i][1], &result, sizeof(double));
            close(fd[i][1]);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            return -1;
        }
        close(fd[i][1]);
    }
    
    double temp, output = 0;
    int status;
    for (int j = 0; j < n; j++) {
        read(fd[j][0], &temp, sizeof(double));
        close(fd[j][0]);
        output += temp;
    }

    while (wait(&status) > 0);

    printf("estimated pi: %lf\n", output);
    return 0;
}
