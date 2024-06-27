int collatz_conjecture(int input){
    if (input % 2 == 0){
        return input / 2;
    } else{
        return 3 * input + 1;
    }
}

int test_collatz_convergence(int input, int max_iter){
    int index = 0;
    while (input != 1){
        if (index >= max_iter){
            return -1;
        }
        input = collatz_conjecture(input);
        index++;
    }
    return index;
}