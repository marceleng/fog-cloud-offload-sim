#include <stdio.h>
#include <string.h>

#include "trace_reader.h"
#include "helpers/log.h"

#define ENTRY_BUFFER_SIZE 1000
#define LINE_BUFFER_SIZE 128

typedef struct trace_entry {
        double time;
        size_t content;
} trace_entry;

struct trace_reader {
        FILE *f;
        trace_entry entries[ENTRY_BUFFER_SIZE];
        size_t pos_in_entries;
        int done;
};

static int _tr_fill_buffer (trace_reader *tr) {
        int count = 0;
        char buffer[LINE_BUFFER_SIZE];
        memset(&buffer, 0, sizeof(buffer));
        while (count<ENTRY_BUFFER_SIZE && fgets(&buffer[0], LINE_BUFFER_SIZE, tr->f)) {
                trace_entry *te = &tr->entries[count];
                if (! sscanf(&buffer[0], "%lf %zu\n", &(te->time), &(te->content))) {
                        LOG(LOG_ERROR, "Error with line %s\n", &buffer[0]);
                        exit(1);
                }
                memset(&buffer, 0, sizeof(buffer));
                count++;
        }
        tr->done = ENTRY_BUFFER_SIZE - count; //>0 <=> We read less than count <=> EOF
        for ( ; count < ENTRY_BUFFER_SIZE; count ++) {
                tr->entries[count].time=-1;
        }

        tr->pos_in_entries = 0;
        return count;
}

trace_reader *tr_alloc(char *filename)
{
        trace_reader *tr = (trace_reader *) malloc(sizeof(trace_reader));
        memset(tr, 0, sizeof(trace_reader));
        tr->f = fopen(filename, "r");
        tr->done = 1;
        _tr_fill_buffer(tr);
        return tr;
}

void tr_free(trace_reader *tr)
{
        fclose(tr->f);
        free(tr);
}

double tr_read_next_arrival(trace_reader *tr, size_t *key)
{
        double ret = -1.;
        if ( (tr->pos_in_entries < ENTRY_BUFFER_SIZE && tr->entries[tr->pos_in_entries].time > 0)
                        || (!tr->done && _tr_fill_buffer(tr)) ) {
                *key = tr->entries[tr->pos_in_entries].content;
                ret = tr->entries[tr->pos_in_entries].time;
        }

        return ret;
}

double tr_pop_next_arrival(trace_reader *tr, size_t *key)
{
        tr->pos_in_entries++;
        return tr_read_next_arrival(tr, key);
}
