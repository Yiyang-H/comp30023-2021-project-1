#include <stdlib.h>
#include <stdio.h>

#include "cpu.h"
#include "pqueue.h"

unsigned int total_time_remain(cpu_t* cpu) {
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

cpu_t *soonest_cpu(cpu_t *cpu, int num) {
    cpu_t *min = cpu;
    for(int i = 1; i < num; i++) {
        if(total_time_remain(cpu + i) < total_time_remain(min)) {
            min = cpu + i;
        }
    }
    return min;
}

cpu_t *soonest_cpu_unused(cpu_t *cpu, int num, int *used_cpu_list) {
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

void execute_process(cpu_t *cpu_current, int time) {
    // Check if any process is waiting to be executed
    if(cpu_current->queue->size >= 1) {
        if(cpu_current->cur_process == NULL) {
            
            cpu_current->cur_process = pop(cpu_current->queue);
            process_t* proc_cuurent = cpu_current->cur_process;
            if(proc_cuurent->parent_process == NULL) {
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",time,
                proc_cuurent->process_id,
                proc_cuurent->time_remain,cpu_current->cpu_id);
            }else {
                printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n",time,
                proc_cuurent->process_id,proc_cuurent->subprocess_id,
                proc_cuurent->time_remain,cpu_current->cpu_id);
            }


        }else if(compare_process(cpu_current->queue->start->process,cpu_current->cur_process)) {
            push(cpu_current->queue, cpu_current->cur_process);
            cpu_current->cur_process = pop(cpu_current->queue);
            process_t* proc_cuurent = cpu_current->cur_process;
            if(proc_cuurent->parent_process == NULL) {
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",time,
                proc_cuurent->process_id,
                proc_cuurent->time_remain,cpu_current->cpu_id);
            }else {
                printf("%d,RUNNING,pid=%d.%d,remaining_time=%d,cpu=%d\n",time,
                proc_cuurent->process_id,proc_cuurent->subprocess_id,
                proc_cuurent->time_remain,cpu_current->cpu_id);
            }
        }
    }
    if(cpu_current->cur_process != NULL) {
        cpu_current->cur_process->time_remain--;
    }
}