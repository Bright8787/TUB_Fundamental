#include "../lib/PRIOP.h"
#include <stdio.h>

static queue_object *PRIOP_queue;
// You can add more global variables here

process *PRIOP_tick(process *running_process)
{
	// TODO
	if (running_process == NULL || running_process->time_left == 0)
	{
		queue_object *highest_prio_obj = PRIOP_queue;
		queue_object *copy_prio_queue = PRIOP_queue;
		queue_object *prev_prio_obj = NULL;

		if (copy_prio_queue == NULL || copy_prio_queue->next == NULL)
		{
			return NULL;
		}

		process *highest_prio = copy_prio_queue->next->object;
		highest_prio_obj = copy_prio_queue->next;

		while (copy_prio_queue->next != NULL)
		{

			queue_object *next_obj = copy_prio_queue->next;
			process *current = next_obj->object;

			if (current->priority >= highest_prio->priority)
			{
				highest_prio = current;
				prev_prio_obj = copy_prio_queue;
				highest_prio_obj = next_obj;
			}

			copy_prio_queue = next_obj;
		}

		if (prev_prio_obj != NULL)
		{
			prev_prio_obj->next = highest_prio_obj->next;
		}
		else
		{
			PRIOP_queue->next = highest_prio_obj->next;
		}

		highest_prio_obj->next = NULL;
		free(highest_prio_obj);
		running_process = highest_prio;
	}

	if (running_process != NULL)
	{
		running_process->time_left--;
	}

	return running_process;
}
int PRIOP_startup()
{
	// TODO
	PRIOP_queue = new_queue();
	if (PRIOP_queue == NULL)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

process *PRIOP_new_arrival(process *arriving_process, process *running_process)
{
	// TODO
	if (arriving_process != NULL)
	{
		if (running_process == NULL)
		{
			return arriving_process;
		}
		else if (arriving_process->priority > running_process->priority)
		{
			queue_add(running_process, PRIOP_queue);
			return arriving_process;
		}
		else
		{
			queue_add(arriving_process, PRIOP_queue);
		}
	}
	return running_process;
}

void PRIOP_finish()
{
	// TODO
	free(PRIOP_queue);
}
