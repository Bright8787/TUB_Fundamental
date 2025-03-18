#include "../lib/SRTN.h"

static queue_object *SRTN_queue;
// You can add more global variables here

process *SRTN_tick(process *running_process)
{

	// TODO
	if (running_process == NULL || running_process->time_left == 0)
	{
		queue_object *shortest_time_obj = SRTN_queue;
		queue_object *copy_SRTN_queue = SRTN_queue;
		queue_object *prev_shortest_time_obj = NULL;

		if (copy_SRTN_queue == NULL || copy_SRTN_queue->next == NULL)
		{

			return NULL;
		}

		process *shortest_time = copy_SRTN_queue->next->object;
		shortest_time_obj = copy_SRTN_queue->next;
		process *current;

		while (copy_SRTN_queue->next != NULL)
		{

			current = copy_SRTN_queue->next->object;

			if (current->time_left <= shortest_time->time_left)
			{
				shortest_time = current;
				prev_shortest_time_obj = copy_SRTN_queue;
				shortest_time_obj = copy_SRTN_queue->next;
			}
			copy_SRTN_queue = copy_SRTN_queue->next;
		}

		if (prev_shortest_time_obj != NULL)
		{
			prev_shortest_time_obj->next = shortest_time_obj->next;
		}
		else
		{
			SRTN_queue->next = shortest_time_obj->next;
		}
		shortest_time_obj->next = NULL;
		running_process = shortest_time;
		free(shortest_time_obj);
	}
	if (running_process != NULL)
	{
		running_process->time_left--;
	}

	return running_process;
}

int SRTN_startup()
{
	// TODO
	SRTN_queue = new_queue();
	if (SRTN_queue == NULL)
	{

		return 1;
	}
	else
	{
		return 0;
	}
}

process *SRTN_new_arrival(process *arriving_process, process *running_process)
{
	// TODO

	if (arriving_process != NULL)
	{
		if (running_process == NULL)
		{
			return arriving_process;
		}
		else if (arriving_process->time_left < running_process->time_left)
		{
			queue_add(running_process, SRTN_queue);
			return arriving_process;
		}
		else
		{
			queue_add(arriving_process, SRTN_queue);
		}
	}
	return running_process;
}

void SRTN_finish()
{
	// TODO

	free_queue(SRTN_queue);
}
