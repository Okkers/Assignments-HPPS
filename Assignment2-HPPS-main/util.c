#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

double distance(int d, const double *x, const double *y) {
    int i;
    double sum = 0.0;
    for (i = 0; i < d; i++) {
        // printf("x: %f, y: %f\n", x[i],y[i]);
        // printf("x-y: %f\n", (x[i]-y[i]));
        // printf("pow(x-y): %f\n", (pow((x[i]-y[i]), 2)));
        sum += pow((x[i]-y[i]), 2);
    }
    double result = sqrt(sum);
    // printf("result: %f\n", result);
    return result;
}

int insert_if_closer(int k, int d,
                     const double *points, int *closest, const double *query,
                     int candidate) {
    for (int i = 0; i < k; i++){
        if (closest[i] == -1){
            closest[i] = candidate;
            for (int j=i-1; j+1 > 0; j--){
                double canddist = distance(d, &points[i*d], query);
                double prevdist = distance(d, &points[j*d], query);
                if (canddist < prevdist){
                    double save = closest[i];
                    closest[i] = closest[j];
                    closest[j] = save;
                }
                else{
                    return 1;
                }
            }
            
            return 1;
        }
    }
    // printf("For canddist\n");
    double canddist = distance(d, &points[candidate*d], query);
    // printf("Canddist: %f\n", canddist);

    // printf("For closemax\n");
    double closemax = distance(d, &points[closest[k-1]*d], query); 
    double closemin = distance(d, &points[closest[0]], query); 
    if (canddist < closemin){
        for (int i = k-1; i > 0; i--){
            closest[i] = closest[i - 1];
        }
        closest[0] = candidate;
        return 1;
    }
    // printf("Closemax: %f\n", closemax);
    if (canddist < closemax){
        for (int j = k-2; j>=0; j--){
            // printf("%d", j);
            double closedist = distance(d, &points[closest[j]*d], query);
            if (canddist > closedist){
                // printf("%f", canddist);
                for (int i = k-1; i < j; i--)
                    closest[i] = closest[i - 1];
                closest[j+1] = candidate;
                return 1;
            }
        }
        
    }

    return 0;
    
}

// int main() {
//     double eudist;
//     int d = 2;
//     double* x = malloc(d * sizeof(double));
//     x[0] = 2.0;
//     x[1] = 2.0;
//     double* y = malloc(d * sizeof(double));
//     y[0] = 1.0;
//     y[1] = 1.0;
//     eudist = distance(d, x, y);
//     printf("%f", eudist);
// }

// int main(int argc, char** argv){
//     FILE * points_f = fopen(argv[1], "r");
//     assert(points_f != NULL);
//     FILE * queries_f = fopen(argv[2], "r");
//     assert(queries_f != NULL);

//     int n_points = -1;
//     int d;
//     double* points = read_points(points_f, &n_points, &d);
//     if (points == NULL) {
//         fprintf(stderr, "Failed reading data from %s\n", argv[1]);
//         exit(1);
//     }
//     fclose(points_f);

//     int i;
//     int *closest = malloc(3*sizeof(double));
//     for (i=0; i<3; i++) {
//         closest[i] = i;
//     }


//     int n_queries = -1;
//     int d_queries;
//     double* queries = read_points(queries_f, &n_queries, &d_queries);
//     if (queries == NULL) {
//         fprintf(stderr, "Failed reading data from %s\n", argv[2]);
//         exit(1);
//     }
//     fclose(queries_f);

//     int result;

//     result = insert_if_closer(3, 2, points, closest, queries, 1);
// }




