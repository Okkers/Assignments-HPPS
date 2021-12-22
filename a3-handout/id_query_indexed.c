#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>

#include "record.h"
#include "id_query.h"

struct index_record {
    int64_t osm_id;
    const struct record *rec;
};

struct indexed_data {
    struct index_record *irs;
    int n;
};

struct indexed_data* mk_indexed(struct record *rs, int n){
    assert(rs != NULL);
    struct index_record *irs = malloc(n*sizeof(struct index_record));
    irs->rec = rs;
    for (int i = 0; i < n; i++){
        irs[i].osm_id = rs[i].osm_id;
    }

    struct indexed_data *data = malloc(sizeof(struct indexed_data));
    data->irs = irs;
    data->n = n;
    return data;
}

void free_indexed(struct indexed_data * data ){
    assert(data != NULL);
    free(data->irs);
    free(data);
}

const struct record *lookup_indexed(struct indexed_data *data, int64_t needle){
    assert(data != NULL);
    const struct record *rs;

    for (int i = 0; i < data->n; i++){
        int64_t haystray = ((data->irs)[i].osm_id);
        if (haystray == needle){
            rs = &(data->irs->rec[i]);
            return rs;
        }
    }
    return 0;
}






int main(int argc, char** argv) {
  return id_query_loop(argc, argv,
                    (mk_index_fn)mk_indexed,
                    (free_index_fn)free_indexed,
                    (lookup_fn)lookup_indexed);
}

