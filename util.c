#include "util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>


// Compute the Euclidean distance between two d-dimensional points 'x'
// and 'y'.  Usual formula:
//
// 
// √( ∑ (x[i]-y[i])² )
double distance(int d, const double *x, const double *y) {
    int i;
    double sum = 0.0;
    for (i = 0; i < d; i++) {
        sum += pow((x[i]-y[i]), 2);
    }
    double result = sqrt(sum);
    return result;
}

// Return the index of the point in closest with the greatest distance to the query point
int argmax(int k, int d, const double *points, int *closest, const double *query) {
    double* distlist = malloc(k * sizeof(double));
    int i, argmax;
    int p;
    int b = 0;
    int j = 0;
    for (i=0; i < k; i++) {
        double* point = malloc(d * sizeof(double));
        for (p=0; p<d; p++) {
            point[p] = points[closest[i] + p];
        }
        distlist[i] = distance(d, point, query); 
    }
    if (distlist[b] < distlist[j]) {
        if (j<k) {
            b = j;
            j += 1;
        }
        else {
            argmax = b;
        }
    }
    else {
        if (j < k) {
            j +=1;
        }
        else {
            argmax = b;
        }
    }
    return argmax;
}

// Maintain a sorted sequence of indexes to the 'k' closest point seen
// so far.
// 'd' is the number of dimensions in the space.
// 'points' is the array of all reference points.
// 'closest' is an array of length 'k' that contains valid indexes
// into 'points', or -1 to indicate the absence of an element.
// 'query' is the query point from which distances are computed.
// 'candidate' is the index of a point in 'points'.
// Updates 'closest' to contain 'candidate' if 'candidate' is closer
// to 'query' than any point in 'closest'.
// Returns 1 if 'closest' was updated, and otherwise '0'.
int insert_if_closer(int k, int d,
                     const double *points, int *closest, const double *query,
                     int candidate) {
    int p;

    double* candpoint = malloc(d * sizeof(double));
        for (p=0; p<d; p++) {
            candpoint[p] = points[closest[candidate] + p];
        }

    double canddist = distance(d, candpoint, query);
    
    int maxclosest = argmax(k, d, points, closest, query);

    double* closestpoint = malloc(d * sizeof(double));
        for (p=0; p<d; p++) {
            closestpoint[p] = points[closest[maxclosest] + p];
        }

    double closedist = distance(d, closestpoint, query);

    if (canddist <  closedist) {
        closest[maxclosest] = candidate;
        return 1;
    }
    else {
        return 0;
    }
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