#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

int main(){
    int arg[6] = {8, 7, 6, 4, 3, 2};
    int* left = arg;
    int* right = arg+3;
    for (int i = 0; i < 5; i++){
        printf("%d", left[i]);
        // printf("%d", right[i]);
        // printf("%d", )
    }
    
}
