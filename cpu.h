#ifndef CPU_H
#define CPU_H

#include "pqueue.h"

typedef struct cpu CPU;

typedef struct cpu {
    int cpu_id;
    Process *cur_process;
    Pqueue *queue;
}CPU;

unsigned int total_time_remain(CPU* cpu);

CPU *soonest_cpu(CPU *cpu, int num);
CPU *soonest_cpu_unused(CPU *cpu, int num, int *used_cpu_list);

#endif