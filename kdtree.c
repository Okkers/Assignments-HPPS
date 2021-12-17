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
  const double* apoint = &arg[0].points[*a*arg[0].d + arg[0].axis];
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
  if (n == 0) {
    return NULL;
  }
  
  struct node *newnode = malloc(sizeof(struct node));

  struct compinfo *info = malloc(sizeof(struct compinfo));
  info->d = d;
  info->axis= depth % d;
  info->points = points;

  hpps_quicksort(indexes, n, sizeof(int), (int (*)(const void *, const void *, void *))comparison, &info);

  int median = floor(n/2);

  newnode->axis = depth % d;
  newnode->point_index = median;

  const double *leftpoints = points;
  const double *rightpoints = points+median;
  int *leftindexes = indexes;
  int *rightindexes = indexes+median;

  if (median != 0) {
    newnode->left = kdtree_create_node(d, leftpoints, depth+1, median, leftindexes);
    newnode->right = kdtree_create_node(d, rightpoints, depth+1, ceil(n/2), rightindexes);
  }
  
  return newnode;
  
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
    while(notfound) {
      for (int i = 0; i < k; i++) {
        if (closest[i] == -1) {
          closest[i] = node->point_index;
          notfound = false;
        }
      }
    }
  }
  else {
    insert_if_closer(k, tree->d,
                     tree->points, closest, query,
                     node->point_index);
  }

  double diff = tree->points[node->point_index*tree->d + node->axis] - query[node->axis];

  //double maxdist = distance(d, &points[closest[j]*d], query);

  const double* maxclosest = &(tree->points[closest[k-1]*tree->d]);

  *radius = distance(tree->d, maxclosest, query);

  double absdiff = diff;

  if (diff < 0) {
    absdiff = -1*diff;
  }

  if ((diff >= 0) || (*radius > absdiff)) {
    kdtree_knn_node(tree, k, query, closest, radius, node->left);
  }
  if ((diff <= 0) || (*radius > absdiff)) {
    kdtree_knn_node(tree, k, query, closest, radius, node->right);
  }

  return;
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