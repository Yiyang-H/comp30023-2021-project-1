#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "pqueue.h"

unsigned int total_time_remain(CPU* cpu) {
    int total_time = 0;
    if(cpu->cur_process != NULL) {
        total_time += cpu->cur_process->time_remain;
    }
    Node *current_node = cpu->queue->start;
    while(current_node != NULL) {
        total_time += current_node->process->time_remain;
        current_node = current_node->next;
    }

    return total_time;
}

CPU *soonest_cpu(CPU *cpu, int num) {
    CPU *min = cpu;
    for(int i = 1; i < num; i++) {
        if(total_time_remain(cpu + i) < total_time_remain(min)) {
            min = cpu + i;
        }
    }
    return min;
}

CPU *soonest_cpu_unused(CPU *cpu, int num, int *used_cpu_list) {
    int min_index = -1;
    for(int i = 0; i < num; i++) {
        if(!used_cpu_list[i]) {
            if(min_index == -1) {
                min_index = i;
                continue;
            }
            if(total_time_remain(cpu + i) < total_time_remain(cpu + min_index)) {
                min_index = i;
            }
        }
    }
    if(min_index == -1) {
        printf("Something wrong!\n");
    }
    return cpu + min_index;
}