#ifndef CPU_H
#define CPU_H

#include "pqueue.h"

typedef struct cpu cpu_t;

typedef struct cpu {
    int cpu_id;
    process_t *cur_process;
    Pqueue *queue;
}cpu_t;

unsigned int total_time_remain(cpu_t* cpu);

cpu_t *soonest_cpu(cpu_t *cpu, int num);
cpu_t *soonest_cpu_unused(cpu_t *cpu, int num, int *used_cpu_list);
void execute_process(cpu_t *cpu_current, int time);

#endif