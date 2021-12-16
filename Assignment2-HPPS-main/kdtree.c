#include "kdtree.h"
#include "sort.h"
#include "util.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct node {
  int point_index;
  int axis;
  struct node *left;
  struct node *right;
};

struct kdtree {
  int d;
  const double *points;
  struct node* root;
};

struct compinfo {
  int d;
  int axis;
  const double* points;
};

int comparison (const int *a, const int *b, struct compinfo *arg) {
  const double* apoint = &arg[0].points[*a*arg[0].d+ arg[0].axis];
  const double* bpoint = &arg[0].points[*b*arg[0].d+ arg[0].axis];

  if (apoint > bpoint) {
    return 1;
  }
  else if (apoint < bpoint) {
    return -1;
  }
  else {
    return 0;
  }
}

struct node* kdtree_create_node(int d, const double *points,
                                int depth, int n, int *indexes) {

  struct node *newnode = malloc(sizeof(struct node));

  struct compinfo *info = malloc(sizeof(struct compinfo));
  info->d = d;
  info->axis= depth % d;
  info->points = points;

  hpps_quicksort(points, n, d, comparison, info);

  newnode->axis = depth % d;
  newnode->point_index = n/2;

  int pointsleft = floor(n/2);
  int pointsright = floor(n/2) + 1;

  if ((n % 2) == 0) {
    pointsright = pointsright - 1;
  }

  int *leftpoints = malloc(pointsleft*sizeof(int)*d);
  int *leftindexes = malloc(sizeof(int) * pointsleft);
  int *rightindexes = malloc(sizeof(int) * pointsright);

  int i;
  for (i = 0; i < pointsleft; i++) {
      leftpoints[i] = i*d;
      leftindexes[i] = i;
      rightindexes[i] = i + pointsleft;
  }

  int *rightpoints = malloc(pointsright*sizeof(int)*d);
  int j;
  for (j = pointsleft ; j > pointsright; j++) {
    rightpoints[j] = j*d;

  }

  if (rightpoints > 0) {
    newnode->right = kdtree_create_node(d, rightpoints, depth+1, pointsright, rightindexes);
    if (pointsleft > 0) {
       newnode->left = kdtree_create_node(d, leftpoints, depth+1, pointsleft, leftindexes);
    }
  }
  else {
    return newnode;
  }
}

struct kdtree *kdtree_create(int d, int n, const double *points) {
  struct kdtree *tree = malloc(sizeof(struct kdtree));
  tree->d = d;
  tree->points = points;

  int *indexes = malloc(sizeof(int) * n);

  for (int i = 0; i < n; i++) {
    indexes[i] = i;
  }

  tree->root = kdtree_create_node(d, points, 0, n, indexes);

  free(indexes);

  return tree;
}

void kdtree_free_node(struct node *node) {
  free(node);
  if (node[0].point_index > 0) {
    kdtree_free_node(node[0].right);
    if (node[0].point_index > 1) {
      kdtree_free_node(node[0].left);
    }
  }
}

void kdtree_free(struct kdtree *tree) {
  kdtree_free_node(tree->root);
  free(tree);
}

void kdtree_knn_node(const struct kdtree *tree, int k, const double* query,
                     int *closest, double *radius,
                     const struct node *node) {
  bool notfound = true;
  if (node == NULL) {
    return;
  }
  if (closest[k-1] == -1) {
    while (notfound) {
      for (int i = 0; i < k; i++) {
        if (closest[i] == -1) {
          notfound = false;
          closest[i] = node[0].point_index;
        }
      }
    }
  }
  else {
    insert_if_closer(k, tree[0].d,
                     tree[0].points, closest, query,
                     node[0].point_index);
  }

  double diff = tree[0].points[node[0].point_index*tree[0].d + node[0].axis] - query[node[0].axis];

  int maxclosest = argmax(k, tree[0].d, tree[0].points, closest, query);

  double* closestpoint = malloc(tree[0].d * sizeof(double));
  for (int p=0; p<tree[0].d; p++) {
    closestpoint[p] = tree[0].points[closest[maxclosest] + p];
  }

  double radius2 = distance(tree[0].d, closestpoint, query);

  double absdiff = diff;

  if (diff > 0) {
    absdiff = -1*diff;
  }

  if ((diff >= 0) || (radius2 > absdiff)) {
    kdtree_knn_node(tree, k, query, closest, &radius2, node[0].left);
  }
  if ((diff <= 0) || (radius2 > absdiff)) {
    kdtree_knn_node(tree, k, query, closest, &radius2, node[0].right);
  }
}

int* kdtree_knn(const struct kdtree *tree, int k, const double* query) {
  int* closest = malloc(k * sizeof(int));
  double radius = INFINITY;

  for (int i = 0; i < k; i++) {
    closest[i] = -1;
  }

  kdtree_knn_node(tree, k, query, closest, &radius, tree->root);

  return closest;
}

void kdtree_svg_node(double scale, FILE *f, const struct kdtree *tree,
                     double x1, double y1, double x2, double y2,
                     const struct node *node) {
  if (node == NULL) {
    return;
  }

  double coord = tree->points[node->point_index*2+node->axis];
  if (node->axis == 0) {
    // Split the X axis, so vertical line.
    fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
            coord*scale, y1*scale, coord*scale, y2*scale);
    kdtree_svg_node(scale, f, tree,
                    x1, y1, coord, y2,
                    node->left);
    kdtree_svg_node(scale, f, tree,
                    coord, y1, x2, y2,
                    node->right);
  } else {
    // Split the Y axis, so horizontal line.
    fprintf(f, "<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"1\" stroke=\"black\" />\n",
            x1*scale, coord*scale, x2*scale, coord*scale);
    kdtree_svg_node(scale, f, tree,
                    x1, y1, x2, coord,
                    node->left);
    kdtree_svg_node(scale, f, tree,
                    x1, coord, x2, y2,
                    node->right);
  }
}

void kdtree_svg(double scale, FILE* f, const struct kdtree *tree) {
  assert(tree->d == 2);
  kdtree_svg_node(scale, f, tree, 0, 0, 1, 1, tree->root);
}