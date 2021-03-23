#include <stdlib.h>

#include "cpu.h"
#include "pqueue.h"

/* void new_process(CPU* cpu, Process* process) {
    cpu->total_processes_remaining += process->time_remain[cpu->cur_subprocess_id];
    push(cpu->queue, process);
}
*/

/* 
Process* execute_process(CPU* cpu) {

}
 */


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