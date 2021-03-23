#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "pqueue.h"
#include "cpu.h"

// Each task takes a list of cpu and processes
void task_1(CPU*, int, Process*, int);
void task_2(CPU*, int, Process*, int);
void task_3(CPU*, int, Process*, int);
void task_4(CPU*, int, Process*, int);

int count_line(char*);
void print_statistics(Process*, int, int);
void print_queue(Pqueue*);


int main(int argc, char** argv) {
    int num_processors,num_processes;
    FILE *fp;

    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f")) {
            num_processes = count_line(argv[i+1]);
            fp = fopen(argv[i+1],"r");
        }
        if(!strcmp(argv[i], "-p")) {
            num_processors = atoi(argv[i+1]);
        }
    }

    CPU processors[num_processors];
    for(int i = 0; i < num_processors; i++) {
        processors[i].cpu_id = i;
        processors[i].cur_process = NULL;
        processors[i].queue = new_queue();
    }

    // Read all processes and put them in an array
    unsigned int time_arr, pro_id, exe_time;
    char par;
    Process all_processes[num_processes];
    for(int i = 0; fscanf(fp,"%u %u %u %c\n",&time_arr,&pro_id,&exe_time,&par) != EOF; i++) {
        all_processes[i].time_arrived = time_arr;
        all_processes[i].process_id = pro_id;
        all_processes[i].execution_time = exe_time;
        all_processes[i].parallelisable = (par == 'p');
        all_processes[i].parent_process = NULL;
        all_processes[i].subprocess_id = 0;
        all_processes[i].time_remain = exe_time;
        all_processes[i].time_finished = 0;
    }
    fclose(fp);


    task_1(processors,num_processors,all_processes,num_processes);
    
    
    // Free all memeory allocated to queues
    for(int i = 0; i < num_processors; i++) {
        free_queue(processors[i].queue);
    }
    
    return 0; 
}

void task_1(CPU* processors, int num_processors, Process* all_processes,
 int num_processes) {

    int processes_arrived = 0, total_processes_remaining = 0;
    for(unsigned int time = 0; time < 500; time++) {

     //   printf("Time:%d, Process_arrived:%d, Process_remaining:%d\n",time,processes_arrived,total_processes_remaining);

        // Check process arrive at this time, push them onto the main queue
        Pqueue *main = new_queue();
        for(int i = processes_arrived; i < num_processes; i++) {
            if(all_processes[i].time_arrived == time) {
                push(main, all_processes + i);
                processes_arrived++;
                total_processes_remaining++;
            }else {
                break;
            }
        }

        // Push all current process onto cpu's queue
        while(main->size > 0) {
            Process *temp = pop(main);
            if(!(temp->parallelisable)) {
                CPU *tem = soonest_cpu(processors, num_processors);
                push(tem->queue, temp);
            }
//             if(num_processors == 1) {
//                 push(processors[0].queue,temp);
//             }else if(num_processors == 2){
//                 if(total_time_remain(processors) <= total_time_remain(processors + 1)) {
//                     push(processors[0].queue,temp);
//                 }else {
//                     push(processors[1].queue,temp);
//                 }


// /*                 if(temp->parallelisable) {
//                     int k;
//                     unsigned int x = temp->execution_time;
//                     if(num_processors == 2) {
//                         k = 2;
//                     }else {
//                         k = num_processors > x ? x : num_processors;
//                     }
//                     unsigned int subprocess_execution = ceil(x / k) + 1;

//                     // Create k subprocesses
//                     for(int i = 0; i < k; i++) {
//                         Process *subprocess = malloc(sizeof(*subprocess));
//                         *subprocess = *temp;
//                         subprocess->parallelisable = false;
//                         subprocess->parent_process = temp;
//                         subprocess->subprocess_id = i;
//                         subprocess->execution_time = subprocess_execution;
//                         subprocess->time_remain = subprocess_execution;
//                         new_process(soonest_cpu())
//                     }
//                     // what happen when execution_time == 1?
//                     // Set subprocess_id of cpu to 0 and 1
//                     // Set time_remain
//                      */
//                // }

                

//             }
        }
        // Free the main queue
        free_queue(main);

        // Check any current processes are done
        for(int i = 0; i < num_processors; i++) {
            Process *current = processors[i].cur_process;
            if(current != NULL && current->time_remain == 0) {
             //   if(current->parent_process == NULL) {
                    total_processes_remaining--;
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",time,
                        current->process_id,total_processes_remaining);
                    current->time_finished = time;

                    processors[i].cur_process = NULL;
                // }else {
                //     printf("This should not happen now!");
                // }
            }
        }

        // Check if all processes are done
        if(processes_arrived == num_processes && total_processes_remaining == 0) {
            print_statistics(all_processes,num_processes,time);
            return;
        }


        // Start any processes if possible
        for(int i = 0; i < num_processors; i++) {
            CPU *cpu_current = processors + i;

            // Check if any process is waiting to be executed
            if(cpu_current->queue->size >= 1) {
                if(cpu_current->cur_process == NULL) {
                    
                    cpu_current->cur_process = pop(cpu_current->queue);
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",time,
                    cpu_current->cur_process->process_id,
                    cpu_current->cur_process->time_remain,cpu_current->cpu_id);

                }else if(compare_process(cpu_current->queue->start->process,cpu_current->cur_process)) {
                    push(cpu_current->queue, cpu_current->cur_process);
                    cpu_current->cur_process = pop(cpu_current->queue);
                    printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=%d\n",time,
                    cpu_current->cur_process->process_id,
                    cpu_current->cur_process->time_remain,cpu_current->cpu_id);
                }
            }
            if(cpu_current->cur_process != NULL) {
                cpu_current->cur_process->time_remain--;
            }
        }

/*         
        if(cpu_0.cur_process != NULL) {
            cpu_0.cur_process->time_remain--;
        }else if(cpu_0.queue->size == 0) {
            print_statistics(all_processes,num_processes,time);
            return;
        }

 */

    }
    

}

int count_line(char* filename) {
    FILE *fp;
    int line_num = 0;
    fp = fopen(filename,"r");
    char c;
    while(fscanf(fp,"%c",&c) != EOF) {
        if(c == '\n') {
            line_num++;
        }
    }
    
    fclose(fp);
    return line_num;
}


void print_statistics(Process* all_processes, int num_processes, int makespan) {
    double avg_turnaround = 0, max_overhead, avg_overhead = 0;
    for(int i = 0; i < num_processes; i++) {
        Process cur = all_processes[i];
        double turnaround = cur.time_finished - cur.time_arrived;
        double overhead = turnaround / cur.execution_time;
        avg_turnaround += turnaround;
        avg_overhead += overhead;
        max_overhead = overhead > max_overhead ? overhead : max_overhead;
    }

    printf("Turnaround time %.0lf\n",ceil(avg_turnaround / num_processes));
    printf("Time overhead %.2lf %.2lf\n",max_overhead,avg_overhead / num_processes);
    printf("Makespan %d\n",makespan);
}

void print_queue(Pqueue* queue) {
    if(queue->size == 0) {
        printf("This queue has no item");
        return;
    }
    Node *cur = queue->start;
    while(cur != NULL) {
        printf("%d",cur->process->process_id);
        cur = cur->next;
    }
    printf("\n");
}
