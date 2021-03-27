#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "pqueue.h"
#include "cpu.h"

// Follow project description to allocate processes
void task(cpu_t*, int, Pqueue*);

// Modify the algorithm to achieve a lower makespan
void challenge(cpu_t* processors, int num_processors, Pqueue* main_queue, 
process_t* all_processes, int num_processes, int time);
int calc_remain_proc_exe_time(process_t* all_processes,int num_processes,int time);
bool any_processor_free(cpu_t* processors,int num_processors);


int count_line(char*);
void print_statistics(process_t*, int, int);
void print_queue(Pqueue*);
int total_processes_remaining(process_t*, int, int);
double my_round(double data);



int main(int argc, char** argv) {
    int num_processors,num_processes;
    bool is_challenge = false;
    FILE *fp;

    // Reads command line arguments and initialise
    for(int i = 1; i < argc; i++) {
        if(!strcmp(argv[i], "-f")) {
            num_processes = count_line(argv[i+1]);
            fp = fopen(argv[i+1],"r");
        }
        if(!strcmp(argv[i], "-p")) {
            num_processors = atoi(argv[i+1]);
        }
        if(!strcmp(argv[i], "-c")) {
            is_challenge = true;
        }
    }

    // Initialise all processors
    cpu_t processors[num_processors];
    for(int i = 0; i < num_processors; i++) {
        processors[i].cpu_id = i;
        processors[i].cur_process = NULL;
        processors[i].queue = new_queue();
    }

    // Initialise all processes and put them in an array
    unsigned int time_arr, pro_id, exe_time;
    char par;
    process_t all_processes[num_processes];
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


    int processes_arrived = 0;

    Pqueue *second_main_queue = new_queue();
    for(unsigned int time = 0; time < UINT_MAX; time++) {
        // Check process arrive at this time, push them onto the main queue
        Pqueue *main_queue = new_queue();
        for(int i = processes_arrived; i < num_processes; i++) {
            if(all_processes[i].time_arrived == time) {
                if(is_challenge) {
                    push(second_main_queue, all_processes + i);
                } else {
                    push(main_queue, all_processes + i);
                }
               
                processes_arrived++;
            }else {
                break;
            }
        }

        // Push all current process onto cpu's queue
        if(!is_challenge) {
            task(processors,num_processors,main_queue);
        }else {
            challenge(processors,num_processors,second_main_queue,all_processes,num_processes,time);
        }
        
        // Free the main queue
        free_queue(main_queue);

        // Check any current processes are done
        for(int i = 0; i < num_processors; i++) {
            process_t *current = processors[i].cur_process;
            if(current != NULL && current->time_remain == 0) {
                if(current->parent_process == NULL) {
                    printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",time,
                        current->process_id,total_processes_remaining(all_processes,num_processes,time));
                    current->time_finished = time;

                    processors[i].cur_process = NULL;
                }else {
                    // Reduce the num_subprocess in parent process
                    process_t* parent = current->parent_process;
                    parent->num_subprocess--;
                    if(parent->num_subprocess == 0) {
                        parent->time_remain = 0;
                        parent->time_finished = time;
                        printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",time,
                        parent->process_id,total_processes_remaining(all_processes,num_processes,time));                    }
                    
                    processors[i].cur_process = NULL;
                    free(current);
                }
            }
        }

        // Check if all processes are done
        if(processes_arrived == num_processes && 
        total_processes_remaining(all_processes, num_processes,time) == 0) {
            print_statistics(all_processes,num_processes,time);
            break;
        }

        // Execute any processes if possible
        for(int i = 0; i < num_processors; i++) {
            execute_process(processors+i,time);
        }
    }
    
    free_queue(second_main_queue);

    // Free all memeory allocated to queues
    for(int i = 0; i < num_processors; i++) {
        free_queue(processors[i].queue);
    }
    
    return 0; 
}


void task(cpu_t* processors, int num_processors, Pqueue* main_queue) {
    while(main_queue->size > 0) {
        process_t *temp = pop(main_queue);
        if(!(temp->parallelisable)) {
            cpu_t *tem = soonest_cpu(processors, num_processors);
            push(tem->queue, temp);
        }else {
            if(num_processors == 1) {
                // Push onto the only processor
                push(processors->queue, temp);
            }else if(num_processors == 2) {
                int sub_exe_time = ceil((double)temp->execution_time / 2) + 1;
                temp->num_subprocess = 2;
                process_t *sub_0 = malloc(sizeof(*sub_0));
                process_t *sub_1 = malloc(sizeof(*sub_1));
                *sub_0 = *temp;
                sub_0->parent_process = temp;
                sub_0->subprocess_id = 0;
                sub_0->time_remain = sub_exe_time;

                *sub_1 = *temp;
                sub_1->parent_process = temp;
                sub_1->subprocess_id = 1;
                sub_1->time_remain = sub_exe_time;

                push(processors[0].queue, sub_0);
                push(processors[1].queue, sub_1);
            } else {
                int x = temp->execution_time;
                int k = num_processors > x ? x : num_processors;
                int sub_exe_time = ceil((double)temp->execution_time / k) + 1;
                temp->num_subprocess = k;
                int used_cpu_list[num_processors];
                for(int i = 0; i < num_processors; i++) {
                    used_cpu_list[i] = 0;
                }
                

                for(int i = 0; i < k; i++) {
                    process_t *subprocess = malloc(sizeof(*subprocess));
                    *subprocess = *temp;
                    subprocess->parent_process = temp;
                    subprocess->subprocess_id = i;
                    subprocess->time_remain = sub_exe_time;
                    // Find a cpu which is hasn't been used
                    cpu_t* tem = soonest_cpu_unused(processors,num_processors,used_cpu_list);
                    //printf("\n cpu_id: %d \n", tem->cpu_id);
                    push(tem->queue,subprocess);
                }
            }
        }
    }
}

int calc_remain_proc_exe_time(process_t* all_processes,int num_processes,int time) {
    int total = 0;
    for(int i = 0; i < num_processes; i++) {
        if((all_processes+i)->time_arrived > time) {
            total += (all_processes+i)->execution_time;
        }
    }
    return total;
}

bool any_processor_free(cpu_t* processors,int num_processors) {
    for(int i = 0; i < num_processors; i++) {
        if(total_time_remain(processors+i) == 0) {
            return true;
        }
    }
    return false;
}

void challenge(cpu_t* processors, int num_processors, Pqueue* main_queue, 
process_t* all_processes, int num_processes, int time) {

    /*
    1. Simply push longest can pass 5 test, 2,3,7,8,9
    2. If can solve for non-parallisible ?
    
    
    */
    
    // unsigned int remain_proc_exe_time = calc_remain_proc_exe_time(all_processes,num_processes,time);
    // int max = all_processes->execution_time;
    // for(int i = 1; i < num_processes; i++) {
    //     max = max > (all_processes+i)->execution_time ? max : (all_processes+i)->execution_time;
    // }
    // unsigned int threshold_time = num_processors * 20;

    while(main_queue->size > 0 && any_processor_free(processors,num_processors)) {
        /*
        1. Check the total remaining time of all processes
            a) If taking a long time, not parallelising process
            b) If remaining time less than some value, parallel processes
        2. None of the cpu should be at rest
        3. 

        Another approch:
        Set a goal

        */

        

        process_t *temp = pop_longest(main_queue);
        
        if(true) {
            cpu_t *tem = soonest_cpu(processors, num_processors);
            push(tem->queue, temp);
        }else {
            int x = temp->execution_time;
            int k = num_processors > x ? x : num_processors;
            int sub_exe_time = ceil((double)temp->execution_time / k) + 1;
            temp->num_subprocess = k;
            int used_cpu_list[num_processors];
            for(int i = 0; i < num_processors; i++) {
                used_cpu_list[i] = 0;
            }
            

            for(int i = 0; i < k; i++) {
                process_t *subprocess = malloc(sizeof(*subprocess));
                *subprocess = *temp;
                subprocess->parent_process = temp;
                subprocess->subprocess_id = i;
                subprocess->time_remain = sub_exe_time;
                // Find a cpu which is hasn't been used
                cpu_t* tem = soonest_cpu_unused(processors,num_processors,used_cpu_list);
                push(tem->queue,subprocess);
            }
            
        }
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

double my_round(double data) {
    return round(data * 100) / 100;
}

void print_statistics(process_t* all_processes, int num_processes, int makespan) {
    double avg_turnaround = 0, max_overhead = 0, avg_overhead = 0;
    for(int i = 0; i < num_processes; i++) {
        process_t *cur = all_processes + i;
        double turnaround = cur->time_finished - cur->time_arrived;
        double overhead = turnaround / cur->execution_time;
        avg_turnaround += turnaround;
        avg_overhead += overhead;
        max_overhead = overhead > max_overhead ? overhead : max_overhead;
    }

    printf("Turnaround time %.0lf\n",ceil(avg_turnaround / num_processes));
    printf("Time overhead %g %g\n",my_round(max_overhead),my_round(avg_overhead / num_processes));
    printf("Makespan %d\n",makespan);
}

void print_queue(Pqueue* queue) {
    if(queue == NULL) {
        printf("This queue does not exist\n");
    }
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

int total_processes_remaining(process_t* all_processes, int num_processes, int time) {
    int num = 0;
    for(int i = 0; i < num_processes; i++) {
        if(all_processes[i].time_arrived <= time && all_processes[i].time_remain > 0) {
            num++;
        }
    }
    return num;
}