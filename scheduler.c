#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROCESSES 100
#define MAX_GANTT 10000
#define MAX_ALGOS 10

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
    int current_priority;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
    bool started;
    bool completed;
} Process;

typedef struct {
    int pid;
    double start_time;
    double end_time;
} GanttEntry;

typedef struct {
    char name[60];
    double avg_wt;
    double avg_tat;
    double avg_rt;
    double cpu_util;
    double throughput;
} AlgoResult;

Process proc_list[MAX_PROCESSES];
Process proc_copy[MAX_PROCESSES];
GanttEntry gantt[MAX_GANTT];
int gantt_count = 0;
int num_processes = 0;

AlgoResult results[MAX_ALGOS];
int result_count = 0;

void reset_processes() {
    gantt_count = 0;
    for (int i = 0; i < num_processes; i++) {
        proc_copy[i] = proc_list[i];
        proc_copy[i].remaining_time = proc_list[i].burst_time;
        proc_copy[i].current_priority = proc_list[i].priority;
        proc_copy[i].started = false;
        proc_copy[i].completed = false;
        proc_copy[i].start_time = -1;
    }
}

void print_output(const char* algo_name, double cs_overhead) {
    printf("\n========================================================\n");
    printf("Algorithm: %s (Context Switch Overhead: %.2f ms)\n", algo_name, cs_overhead);
    printf("========================================================\n\n");

    printf("Gantt Chart:\n|");
    for (int i = 0; i < gantt_count; i++) {
        if (gantt[i].pid == -1)
            printf(" IDLE |");
        else
            printf(" P%d |", gantt[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < gantt_count; i++) {
        printf("    %.1f", gantt[i].end_time);
    }
    printf("\n\n");

    printf("PID\tArrival\tBurst\tPriority\tFinish\tWT\tTAT\tRT\n");
    printf("-------------------------------------------------------------------\n");
    
    double total_wt = 0, total_tat = 0, total_rt = 0, total_burst = 0;
    for (int i = 0; i < num_processes; i++) {
        total_wt += proc_copy[i].waiting_time;
        total_tat += proc_copy[i].turnaround_time;
        total_rt += proc_copy[i].response_time;
        total_burst += proc_copy[i].burst_time;

        printf("%d\t%d\t%d\t%d\t\t%d\t%d\t%d\t%d\n",
               proc_copy[i].pid, proc_copy[i].arrival_time, proc_copy[i].burst_time,
               proc_copy[i].priority, proc_copy[i].completion_time,
               proc_copy[i].waiting_time, proc_copy[i].turnaround_time,
               proc_copy[i].response_time);
    }

    double total_time = (gantt_count > 0) ? gantt[gantt_count - 1].end_time : 0;
    int num_cs = (gantt_count > 0) ? (gantt_count - 1) : 0;
    double total_cs_time = num_cs * cs_overhead;
    double cpu_utilization = (total_time > 0) ? (total_burst / (total_burst + total_cs_time)) * 100.0 : 0;
    double throughput = (total_time > 0) ? (num_processes / total_time) : 0;

    double avg_wt = total_wt / num_processes;
    double avg_tat = total_tat / num_processes;
    double avg_rt = total_rt / num_processes;

    printf("-------------------------------------------------------------------\n");
    printf("Average Waiting Time   : %.2f ms\n", avg_wt);
    printf("Average Turnaround Time: %.2f ms\n", avg_tat);
    printf("Average Response Time  : %.2f ms\n", avg_rt);
    printf("Throughput             : %.4f processes/ms\n", throughput);
    printf("CPU Utilization        : %.2f%%\n", cpu_utilization);
    printf("========================================================\n");

    /* Save to summary results array */
    snprintf(results[result_count].name, sizeof(results[result_count].name), "%s", algo_name);
    results[result_count].avg_wt = avg_wt;
    results[result_count].avg_tat = avg_tat;
    results[result_count].avg_rt = avg_rt;
    results[result_count].cpu_util = cpu_utilization;
    results[result_count].throughput = throughput;
    result_count++;
}

/* Algorithms */
void run_fcfs(double cs_overhead) {
    reset_processes();
    int current_time = 0, completed = 0;
    while (completed < num_processes) {
        int idx = -1, min_arrival = 1e9;
        for (int i = 0; i < num_processes; i++) {
            if (!proc_copy[i].completed && proc_copy[i].arrival_time <= current_time) {
                if (proc_copy[i].arrival_time < min_arrival) {
                    min_arrival = proc_copy[i].arrival_time;
                    idx = i;
                }
            }
        }
        if (idx == -1) { current_time++; continue; }

        proc_copy[idx].start_time = current_time;
        proc_copy[idx].response_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        proc_copy[idx].waiting_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        current_time += proc_copy[idx].burst_time;
        proc_copy[idx].completion_time = current_time;
        proc_copy[idx].turnaround_time = proc_copy[idx].completion_time - proc_copy[idx].arrival_time;
        proc_copy[idx].completed = true;

        gantt[gantt_count].pid = proc_copy[idx].pid;
        gantt[gantt_count].start_time = proc_copy[idx].start_time;
        gantt[gantt_count].end_time = current_time;
        gantt_count++;
        completed++;
    }
    print_output("FCFS", cs_overhead);
}

void run_sjf_non_preemptive(double cs_overhead) {
    reset_processes();
    int current_time = 0, completed = 0;
    while (completed < num_processes) {
        int idx = -1, min_burst = 1e9;
        for (int i = 0; i < num_processes; i++) {
            if (!proc_copy[i].completed && proc_copy[i].arrival_time <= current_time) {
                if (proc_copy[i].burst_time < min_burst) {
                    min_burst = proc_copy[i].burst_time;
                    idx = i;
                }
            }
        }
        if (idx == -1) { current_time++; continue; }

        proc_copy[idx].start_time = current_time;
        proc_copy[idx].response_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        proc_copy[idx].waiting_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        current_time += proc_copy[idx].burst_time;
        proc_copy[idx].completion_time = current_time;
        proc_copy[idx].turnaround_time = proc_copy[idx].completion_time - proc_copy[idx].arrival_time;
        proc_copy[idx].completed = true;

        gantt[gantt_count].pid = proc_copy[idx].pid;
        gantt[gantt_count].start_time = proc_copy[idx].start_time;
        gantt[gantt_count].end_time = current_time;
        gantt_count++;
        completed++;
    }
    print_output("SJF (Non-Preemptive)", cs_overhead);
}

void run_srtf(double cs_overhead) {
    reset_processes();
    int current_time = 0, completed = 0, prev_pid = -1;
    while (completed < num_processes) {
        int idx = -1, min_remaining = 1e9;
        for (int i = 0; i < num_processes; i++) {
            if (!proc_copy[i].completed && proc_copy[i].arrival_time <= current_time) {
                if (proc_copy[i].remaining_time < min_remaining) {
                    min_remaining = proc_copy[i].remaining_time;
                    idx = i;
                }
            }
        }
        if (idx == -1) { current_time++; continue; }

        if (!proc_copy[idx].started) {
            proc_copy[idx].start_time = current_time;
            proc_copy[idx].response_time = current_time - proc_copy[idx].arrival_time;
            proc_copy[idx].started = true;
        }

        if (prev_pid != proc_copy[idx].pid) {
            gantt[gantt_count].pid = proc_copy[idx].pid;
            gantt[gantt_count].start_time = current_time;
            gantt_count++;
            prev_pid = proc_copy[idx].pid;
        }

        proc_copy[idx].remaining_time--;
        current_time++;
        gantt[gantt_count - 1].end_time = current_time;

        if (proc_copy[idx].remaining_time == 0) {
            proc_copy[idx].completion_time = current_time;
            proc_copy[idx].turnaround_time = proc_copy[idx].completion_time - proc_copy[idx].arrival_time;
            proc_copy[idx].waiting_time = proc_copy[idx].turnaround_time - proc_copy[idx].burst_time;
            proc_copy[idx].completed = true;
            completed++;
        }
    }
    print_output("SRTF (Preemptive SJF)", cs_overhead);
}

void run_priority_non_preemptive(bool enable_aging, double cs_overhead) {
    reset_processes();
    int current_time = 0, completed = 0;
    while (completed < num_processes) {
        int idx = -1, highest_priority = 1e9;
        for (int i = 0; i < num_processes; i++) {
            if (!proc_copy[i].completed && proc_copy[i].arrival_time <= current_time) {
                if (proc_copy[i].current_priority < highest_priority) {
                    highest_priority = proc_copy[i].current_priority;
                    idx = i;
                }
            }
        }
        if (idx == -1) { current_time++; continue; }

        proc_copy[idx].start_time = current_time;
        proc_copy[idx].response_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        proc_copy[idx].waiting_time = proc_copy[idx].start_time - proc_copy[idx].arrival_time;
        current_time += proc_copy[idx].burst_time;
        proc_copy[idx].completion_time = current_time;
        proc_copy[idx].turnaround_time = proc_copy[idx].completion_time - proc_copy[idx].arrival_time;
        proc_copy[idx].completed = true;

        gantt[gantt_count].pid = proc_copy[idx].pid;
        gantt[gantt_count].start_time = proc_copy[idx].start_time;
        gantt[gantt_count].end_time = current_time;
        gantt_count++;

        if (enable_aging) {
            for (int i = 0; i < num_processes; i++) {
                if (!proc_copy[i].completed && proc_copy[i].arrival_time <= current_time && i != idx) {
                    if (proc_copy[i].current_priority > 0)
                        proc_copy[i].current_priority--;
                }
            }
        }
        completed++;
    }

    char title[60];
    snprintf(title, sizeof(title), "Priority Non-Preemptive%s", enable_aging ? " (With Aging)" : "");
    print_output(title, cs_overhead);
}

void run_round_robin(int quantum, double cs_overhead) {
    reset_processes();
    int current_time = 0, completed = 0;
    int queue[MAX_PROCESSES * 20], front = 0, rear = 0;
    bool in_queue[MAX_PROCESSES] = {false};

    for (int i = 0; i < num_processes; i++) {
        if (proc_copy[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    while (completed < num_processes) {
        if (front == rear) {
            current_time++;
            for (int i = 0; i < num_processes; i++) {
                if (!proc_copy[i].completed && !in_queue[i] && proc_copy[i].arrival_time <= current_time) {
                    queue[rear++] = i;
                    in_queue[i] = true;
                }
            }
            continue;
        }

        int idx = queue[front++];
        if (!proc_copy[idx].started) {
            proc_copy[idx].start_time = current_time;
            proc_copy[idx].response_time = current_time - proc_copy[idx].arrival_time;
            proc_copy[idx].started = true;
        }

        int exec_time = (proc_copy[idx].remaining_time > quantum) ? quantum : proc_copy[idx].remaining_time;
        
        gantt[gantt_count].pid = proc_copy[idx].pid;
        gantt[gantt_count].start_time = current_time;
        current_time += exec_time;
        gantt[gantt_count].end_time = current_time;
        gantt_count++;

        proc_copy[idx].remaining_time -= exec_time;

        for (int i = 0; i < num_processes; i++) {
            if (i != idx && !proc_copy[i].completed && !in_queue[i] && proc_copy[i].arrival_time <= current_time) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        if (proc_copy[idx].remaining_time > 0) {
            queue[rear++] = idx;
        } else {
            proc_copy[idx].completion_time = current_time;
            proc_copy[idx].turnaround_time = proc_copy[idx].completion_time - proc_copy[idx].arrival_time;
            proc_copy[idx].waiting_time = proc_copy[idx].turnaround_time - proc_copy[idx].burst_time;
            proc_copy[idx].completed = true;
            completed++;
        }
    }

    char title[50];
    snprintf(title, sizeof(title), "Round Robin (q=%d)", quantum);
    print_output(title, cs_overhead);
}

/* Evaluation logic and CSV export */
void print_evaluation_and_export_csv(const char* csv_filename) {
    printf("\n========================================================\n");
    printf("            COMPARATIVE EVALUATION SUMMARY             \n");
    printf("========================================================\n\n");

    printf("%-30s | %-8s | %-8s | %-8s | %-8s\n", "Algorithm", "Avg WT", "Avg TAT", "Avg RT", "CPU Util");
    printf("--------------------------------------------------------------------------------\n");

    int best_wt_idx = 0, best_tat_idx = 0, best_rt_idx = 0;
    FILE* csv_file = fopen(csv_filename, "w");
    if (csv_file) {
        fprintf(csv_file, "Algorithm,Avg_WT,Avg_TAT,Avg_RT,CPU_Util,Throughput\n");
    }

    for (int i = 0; i < result_count; i++) {
        printf("%-30s | %-8.2f | %-8.2f | %-8.2f | %-7.2f%%\n",
               results[i].name, results[i].avg_wt, results[i].avg_tat, results[i].avg_rt, results[i].cpu_util);

        if (csv_file) {
            fprintf(csv_file, "%s,%.2f,%.2f,%.2f,%.2f,%.4f\n",
                    results[i].name, results[i].avg_wt, results[i].avg_tat, results[i].avg_rt, results[i].cpu_util, results[i].throughput);
        }

        if (results[i].avg_wt < results[best_wt_idx].avg_wt) best_wt_idx = i;
        if (results[i].avg_tat < results[best_tat_idx].avg_tat) best_tat_idx = i;
        if (results[i].avg_rt < results[best_rt_idx].avg_rt) best_rt_idx = i;
    }

    if (csv_file) {
        fclose(csv_file);
        printf("\n[+] Raw results saved to CSV file: %s\n", csv_filename);
    }

    printf("\n--------------------------------------------------------------------------------\n");
    printf("VERDICT & RECOMMENDATIONS:\n");
    printf("1. Minimum Waiting Time : %s (%.2f ms)\n", results[best_wt_idx].name, results[best_wt_idx].avg_wt);
    printf("2. Minimum Turnaround   : %s (%.2f ms)\n", results[best_tat_idx].name, results[best_tat_idx].avg_tat);
    printf("3. Fastest Response     : %s (%.2f ms)\n", results[best_rt_idx].name, results[best_rt_idx].avg_rt);
    printf("\nRECOMMENDED USAGE:\n");
    printf("• Best for Batch Server     : %s (minimizes total wait and turnaround time)\n", results[best_wt_idx].name);
    printf("• Best for Interactive System: %s (delivers fastest initial user response)\n", results[best_rt_idx].name);
    printf("========================================================\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file> [cs_overhead_ms] [csv_output_file]\n", argv[0]);
        return 1;
    }

    double cs_overhead = (argc >= 3) ? atof(argv[2]) : 0.0;
    const char* csv_filename = (argc >= 4) ? argv[3] : "results.csv";

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening input file");
        return 1;
    }

    num_processes = 0;
    while (fscanf(file, "%d %d %d %d",
                  &proc_list[num_processes].pid,
                  &proc_list[num_processes].arrival_time,
                  &proc_list[num_processes].burst_time,
                  &proc_list[num_processes].priority) == 4) {
        num_processes++;
    }
    fclose(file);

    run_fcfs(cs_overhead);
    run_sjf_non_preemptive(cs_overhead);
    run_srtf(cs_overhead);
    run_priority_non_preemptive(false, cs_overhead);
    run_priority_non_preemptive(true, cs_overhead);
    run_round_robin(4, cs_overhead);

    print_evaluation_and_export_csv(csv_filename);

    return 0;
}