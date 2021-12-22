#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "coord_query.h"

struct naive_data {
  struct record *rs;
  int n;
};

double distance(double lon1, double lat1, double lon2, double lat2) {
  // Type casting
  // struct record *a = (struct record *)x;
  // struct record *b = (struct record *)y;

  double sum = pow((lon1 - lon2), 2) + pow((lat1 - lat2), 2);
  double dist = sqrt(sum);

  return dist;
} 

struct naive_data* mk_naive(struct record* rs, int n) {
  assert(rs != NULL);
  struct naive_data *data = malloc(sizeof(struct naive_data));
  data->rs = rs;
  data->n = n;
  return data;
}

void free_naive(struct naive_data* data) {
  assert(data != NULL);
  free(data);
}

const struct record* lookup_naive(struct naive_data *data, double lon, double lat) {
  assert(data != NULL);
  struct record* rs = NULL;
  double prevdist = INFINITY;
  double currentdist;

  for (int i = 0; i < data->n; i++){
    double current_lon = ((data->rs)[i].lon);
    double current_lat = ((data->rs)[i].lat);
    currentdist = distance(lon, lat, current_lon, current_lat);

    if (currentdist < prevdist){
      prevdist = currentdist;
      rs = &(data->rs[i]);
    }
  }
  return rs;
}

int main(int argc, char** argv) {
  return coord_query_loop(argc, argv,
                          (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive,
                          (lookup_fn)lookup_naive);
}
