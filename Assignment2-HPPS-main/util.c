#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

double distance(int d, const double *x, const double *y) {
    double sum = 0.0;
    for (int i = 0; i < d; i++) {
        sum += pow((x[i]-y[i]), 2);
    }
    double result = sqrt(sum);
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
                double canddist = distance(d, &points[closest[j]*d], query);
                double prevdist = distance(d, &points[(closest[j]-1)*d], query);
                if (canddist < prevdist){
                    double save = closest[j];
                    closest[j] = closest[(j-1)];
                    closest[(j-1)] = save;
                }
            }
            return 1;
        }
    }
    double canddist = distance(d, &points[candidate*d], query);

    double closemax = distance(d, &points[closest[k-1]*d], query); 
    double closemin = distance(d, &points[closest[0]*d], query); 
    if (canddist < closemin){
        for (int i = k-1; i > 0; i--){
            closest[i] = closest[i - 1];
        }
        closest[0] = candidate;
        return 1;
    }
    if (canddist < closemax){
        for (int j = k-2; j>=0; j--){
            double closedist = distance(d, &points[closest[j]*d], query);
            if (canddist > closedist){
                for (int i = k-1; i > j+1; i--)
                    closest[i] = closest[i - 1];
                closest[j+1] = candidate;

                return 1;
            }
        }
    }
    return 0; 
}





