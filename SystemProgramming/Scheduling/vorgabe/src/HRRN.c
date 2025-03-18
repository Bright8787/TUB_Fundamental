#include "../lib/HRRN.h"
#include <stdio.h>

static queue_object *HRRN_queue;
// You can add more global variables and structs here
static int current_time = -1; // inorder to start from 0

process *HRRN_tick(process *running_process)
{

	current_time++;

	// If there's no running process or the running process has finished
	if (running_process == NULL || running_process->time_left == 0)
	{

		// If there are processes in the queue
		if (HRRN_queue != NULL && HRRN_queue->next != NULL)
		{

			queue_object *copy_of_HRRN_queue = HRRN_queue;
			queue_object *object_highest_rr = HRRN_queue;
			queue_object *prev_highest_rr_obj = NULL;

			process *prev = NULL;

			process *process_highest_rr = copy_of_HRRN_queue->next->object;
			unsigned int waiting_time = current_time - process_highest_rr->start_time;
			float highest_rr = (float)(waiting_time + process_highest_rr->time_left) / process_highest_rr->time_left;
			object_highest_rr = copy_of_HRRN_queue->next;
			process *local_process;
			// printf("the highest rr is %f  (%c) ", highest_rr, process_highest_rr->id);
			// printf("Start time: (%d) and current time is (%d) time left is (%d) waiting time is (%d) \n", process_highest_rr->start_time, current_time, process_highest_rr->time_left, waiting_time);

			while (copy_of_HRRN_queue->next != NULL)
			{
				local_process = copy_of_HRRN_queue->next->object;
				unsigned int local_waiting_time = current_time - local_process->start_time;
				float local_rr = (float)(local_waiting_time + local_process->time_left) / local_process->time_left;

				if (local_rr >= highest_rr)
				{
					highest_rr = local_rr;
					prev_highest_rr_obj = copy_of_HRRN_queue;
					object_highest_rr = copy_of_HRRN_queue->next;
					process_highest_rr = local_process;
					// printf("im in condition and the the new rr ist %f (%c) ", local_rr, process_highest_rr->id);
					// printf("Start time: (%d) and current time is (%d) time left is (%d) local waiting time is (%d) \n", process_highest_rr->start_time, current_time, process_highest_rr->time_left, local_waiting_time);
				}
				copy_of_HRRN_queue = copy_of_HRRN_queue->next;
			}
			// printf("Result highest rr is %c \n", process_highest_rr->id);
			// Remove the process with the highest response ratio from the queue
			if (prev_highest_rr_obj != NULL)
			{
				prev_highest_rr_obj->next = object_highest_rr->next;
			}
			else
			{
				HRRN_queue->next = object_highest_rr->next;
			}

			object_highest_rr->next = NULL;
			running_process = process_highest_rr;
			free(object_highest_rr);
			// return process_highest_rr;
			running_process = process_highest_rr;
		}
		else
		{
			return NULL;
		}
	}
	if (running_process != NULL)
	{
		running_process->time_left--;
	}

	return running_process;
}

int HRRN_startup()
{
	// TODO
	HRRN_queue = new_queue();
	if (HRRN_queue == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

process *HRRN_new_arrival(process *arriving_process, process *running_process)
{
	// TODO

	if (arriving_process != NULL)
	{

		queue_add(arriving_process, HRRN_queue);
	}
	return running_process;
}

void HRRN_finish()
{
	// TODO
	free_queue(HRRN_queue);
}
