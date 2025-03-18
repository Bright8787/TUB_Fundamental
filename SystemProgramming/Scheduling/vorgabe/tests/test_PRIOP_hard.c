#include "../lib/scheduler.h"
#include "../lib/process.h"
#include "helpers.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PROCESS_COUNT 4
#define RR_QUANTUM 2
#define STRATEGY PRIOP

int main()
{
    /*Defining the processes                Arrival Dur.    Prio    ID
                                            uint    uint    uint    char*/
    process processes[PROCESS_COUNT] = {{0, 5, 4, 'A'},
                                        {3, 3, 1, 'B'},
                                        {4, 8, 2, 'C'},
                                        {8, 4, 3, 'D'}};
    const char *expected_result = "AAAAACCCDDDDCCCCCBBB";
    print_schedule_info(PROCESS_COUNT, STRATEGY, 0, processes);

    char *resulting_schedule = scheduler(processes, PROCESS_COUNT, STRATEGY, RR_QUANTUM);

    int err = compare_result(resulting_schedule, expected_result);
    free(resulting_schedule);
    exit(err);
}
