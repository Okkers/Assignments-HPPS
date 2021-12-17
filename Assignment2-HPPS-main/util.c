#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

double distance(int d, const double *x, const double *y) {
    double sum = 0.0;
    for (int i = 0; i < d; i++) {
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
            if (i==0){
                return 1;
            }
            for (int j=i; j > 0; j--){
                // printf("First loop: %d\n", j);
                double canddist = distance(d, &points[closest[j]*d], query);
                // printf("  canddist: %f\n", canddist);
                double prevdist = distance(d, &points[(closest[j]-1)*d], query);
                // printf("  prevdist: %f\n", prevdist);
                if (canddist < prevdist){
                    double save = closest[j];
                    closest[j] = closest[(j-1)];
                    closest[(j-1)] = save;
                    // printf("  Closestlist: %d, %d, %d\n", closest[0], closest[1], closest[2]);
                }
            }
            // printf("  Closestlist: %d, %d, %d\n", closest[0], closest[1], closest[2]);
            return 1;
        }
    }
    // printf("No Loop\n");
    double canddist = distance(d, &points[candidate*d], query);
    // printf("  Canddist: %f\n", canddist);

    // printf("For closemax\n");
    double closemax = distance(d, &points[closest[k-1]*d], query); 
    double closemin = distance(d, &points[closest[0]*d], query); 
    // printf("  closemax: %f\n", closemax);
    // printf("  closemin: %f\n", closemin);
    if (canddist < closemin){
        for (int i = k-1; i > 0; i--){
            closest[i] = closest[i - 1];
        }
        closest[0] = candidate;
        // printf("  Closestlist: %d, %d, %d\n", closest[0], closest[1], closest[2]);
        return 1;
    }
    // printf("Closemax: %f\n", closemax);
    if (canddist < closemax){
        for (int j = k-2; j>=0; j--){
            // printf("LOOP: %d\n", j);
            double closedist = distance(d, &points[closest[j]*d], query);
            // printf("  closedist %f\n", closedist);
            if (canddist > closedist){
                for (int i = k-1; i > j+1; i--)
                    closest[i] = closest[i - 1];
                closest[j+1] = candidate;

                // printf("  Closestlist: %d, %d, %d\n", closest[0], closest[1], closest[2]);
                return 1;
            }
        }
    }
    return 0; 
}





