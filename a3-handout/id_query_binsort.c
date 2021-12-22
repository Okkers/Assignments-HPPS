#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <math.h>

#include "record.h"
#include "id_query.h"

struct binsort_data {
    struct record *rs;
    int n;
};


int compare_func(const void *p, const void *q){
    // Type casting
    struct record *x = (struct record *)p;
    struct record *y = (struct record *)q;
    
    if (x->osm_id < y->osm_id){
        return -1;
    }
    if (x->osm_id > y->osm_id){
        return 1;
    }
    return 0;
}

struct binsort_data* mk_binsort(struct record *rs, int n){
    assert(rs != NULL);
    
    // Sort the array based on osm_id
    qsort(rs, n, sizeof(struct record), compare_func);
    
    struct binsort_data *data = malloc(sizeof(struct binsort_data));
    data->rs = rs;
    data->n = n;

    return data;
}

void free_binsort(struct binsort_data * data ){
    assert(data != NULL);
    free(data);
}

const struct record *binary_search(struct binsort_data *data, int64_t low, int64_t high, int64_t needle){
    const struct record *rs;
    if (low > high){
        return NULL;
    }
    int64_t mid = floor((low+high)/2);
    int64_t haystray = ((data->rs)[mid].osm_id);
    if (haystray == needle){
        rs = &(data->rs[mid]);
        return rs;
    }
    else if (haystray < needle){
        return binary_search(data, mid+1, high, needle);
    }
    else{
        return binary_search(data, low, mid-1, needle);
    }
}

const struct record *lookup_binsort(struct binsort_data *data, int64_t needle){
    assert(data != NULL);
    const struct record *rs;

    rs = binary_search(data, 0, (data->n)-1, needle);

    return rs;
}


int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_binsort,
                    (free_index_fn)free_binsort,
                    (lookup_fn)lookup_binsort);
}
