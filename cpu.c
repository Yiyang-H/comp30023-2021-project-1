#include "cpu.h"
#include "pqueue.h"


typedef struct cpu {
    int cpu_id;
    int total_time_remain;
    Pqueue *queue;
};