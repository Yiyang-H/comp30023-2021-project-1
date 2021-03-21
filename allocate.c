#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "pqueue.h"
#include "cpu.h"

// Each stage takes a list of cpu and processes
void stage_1(CPU*, int, Process*, int);
void stage_2(CPU*, int, Process*, int);
void stage_3(CPU*, int, Process*, int);
void stage_4(CPU*, int, Process*, int);

int count_line(char*);
void print_statistics(Process*, int, int);


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
        processors[i].total_time_remain = 0;
        processors[i].cur_process = NULL;
        processors[i].queue = new_queue();
    }

    // Read all processes and put them in an array
    unsigned int time_arr, pro_id, exe_time;
    char par;
    Process all_processes[num_processes];
    for(int i = 0; fscanf(fp,"%u %u %u %c\n",&time_arr,&pro_id,&exe_time,&par) != EOF; i++) {
        Process tem = {time_arr,pro_id,exe_time,par=='p',0,exe_time,0};
        all_processes[i] = tem;
    }
    fclose(fp);

    stage_1(processors,num_processors,all_processes,num_processes);
    
    // Free all memeory allocated to queues
    for(int i = 0; i < num_processors; i++) {
        free_queue(processors[i].queue);
    }
    
    return 0; 
}

void stage_1(CPU* processors, int num_processors, Process* all_processes,
 int num_processes) {

    CPU cpu_0 = processors[0];
    int processes_executing = 0, total_processes_remaining = 0;
    for(unsigned int time = 0; time < 1000; time++) {
        // Check process arrive at this time, push them onto the queue
        for(int i = processes_executing; i < num_processes; i++) {
            if(all_processes[i].time_arrived == time) {

              //  Process test = all_processes[i];
              
              //  printf("%d %d %d %d\n",time,test.time_arrived,test.process_id,test.time_remain);

                push(cpu_0.queue, all_processes + i);
                

                processes_executing++;
                total_processes_remaining++;
            }
            break;
        }

                // Check any process completed at cur_process time
        /* Null checking for cur_process process!!                              */
        if(cpu_0.cur_process != NULL) {
            if(cpu_0.cur_process->time_remain == 0) {

                

                total_processes_remaining--;
                printf("%d,FINISHED,pid=%d,proc_remaining=%d\n",time,
                cpu_0.cur_process->process_id,total_processes_remaining);
                cpu_0.cur_process->time_finished = time;

                cpu_0.cur_process = NULL;
            }
        // Decrease the time remaining for cur_process process
        }


        // Check if any process is waiting to be executed
        if(cpu_0.queue->size >= 1) {
            if(cpu_0.cur_process == NULL) {
                 
                cpu_0.cur_process = pop(cpu_0.queue);

                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=0\n",time,
                cpu_0.cur_process->process_id,cpu_0.cur_process->time_remain);

            }else if(compare_process(cpu_0.queue->start->process,cpu_0.cur_process)) {
                push(cpu_0.queue, cpu_0.cur_process);
                cpu_0.cur_process = pop(cpu_0.queue);
                printf("%d,RUNNING,pid=%d,remaining_time=%d,cpu=0\n",time,
                cpu_0.cur_process->process_id,cpu_0.cur_process->time_remain);
            }
        }



        if(cpu_0.cur_process != NULL) {
            cpu_0.cur_process->time_remain--;
        }else if(cpu_0.queue->size == 0) {
            print_statistics(all_processes,num_processes,time);
            return;
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


void print_statistics(Process* all_processes, int num_processes, int makespan) {
    double avg_turnaround = 0, max_overhead, avg_overhead = 0;
    for(int i = 0; i < num_processes; i++) {
        Process cur = all_processes[i];
        printf("%u %u %u %d %u %u %u\n", cur.time_arrived, cur.process_id, cur.execution_time,
        cur.parallelisable,cur.subprocess_id,cur.time_remain,cur.time_finished);
        double turnaround = (int) (cur.time_finished - cur.time_arrived);
        double overhead = turnaround / cur.execution_time;
        avg_turnaround += turnaround;
        avg_overhead += overhead;
        max_overhead = overhead > max_overhead ? overhead : max_overhead;
    }

    printf("Turnaround time %lf\n",avg_turnaround / num_processes);
    printf("Time overhead %lf %lf\n",max_overhead,avg_overhead / num_processes);
    printf("Makespan %d\n",makespan);
}

