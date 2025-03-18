#include "../lib/MLF.h"
#include <stdio.h>
static queue_object **MLF_queues;
// You can add more global variables here
#define NUMS_LEVEL 4
static int current_level = 0;
static int current_time = 0;
static int counter = 0;

static int *time_quanta; // Array of time quanta for each level
// 1 2 4 8

process *remove_highest_priority_process(queue_object *queue)
{

	if (queue != NULL)
	{

		queue_object *highest_prio_obj = queue;
		queue_object *copy_prio_queue = queue;
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
			queue->next = highest_prio_obj->next;
		}

		process *highest_prio_process = highest_prio_obj->object;
		free(highest_prio_obj);		 // Free the queue object
		return highest_prio_process; // Return the highest priority process
	}
	return NULL;
}

process *MLF_tick(process *running_process)
{
	// TODO

	// No running time lefts find new Process
	if (running_process == NULL || running_process->time_left == 0)
	{
		// reset level
		current_level = 0;
		// reset counter
		counter = 0;
		for (int i = 0; i < NUMS_LEVEL; i++)
		{
			// check through each level
			if (MLF_queues[i] != NULL || MLF_queues[i]->next != NULL)
			{
				running_process = remove_highest_priority_process(MLF_queues[i]);
				if (running_process != NULL)
					break;
			}
			current_level++;
		}
	}
	// if the time slice exceed the level
	else if (counter >= time_quanta[current_level] && running_process->time_left > 0)
	{
		// move it to the next level
		if (current_level < 3)
		{
			counter = 0;
			queue_add(running_process, MLF_queues[current_level + 1]);
			current_level = 0;
			for (int i = 0; i < NUMS_LEVEL; i++)
			{
				// check through h level
				if (MLF_queues[i] != NULL || MLF_queues[i]->next != NULL)
				{
					running_process = remove_highest_priority_process(MLF_queues[i]);
					if (running_process != NULL)
						break;
				}
				current_level++;
			}
		}

		else
		{
			// running_process->time_left--;
			counter++;
			// add running process at the back of the level 3 queue
			queue_object *MLF_Level_3 = MLF_queues[3];
			queue_object *new_node = (queue_object *)malloc(sizeof(queue_object));
			new_node->next = NULL;
			new_node->object = running_process;
			while (MLF_Level_3->next)
			{
				MLF_Level_3 = MLF_Level_3->next;
			}
			MLF_Level_3->next = new_node;

			running_process = remove_highest_priority_process(MLF_queues[3]);
		}
	}

	if (running_process != NULL)
	{
		running_process->time_left--;
		counter++;
	}
	current_time++;
	return running_process;
}

int MLF_startup()
{

	// TODO
	MLF_queues = (queue_object **)malloc(NUMS_LEVEL * sizeof(queue_object *));
	time_quanta = (int *)malloc(NUMS_LEVEL * sizeof(int));

	if (MLF_queues == NULL || time_quanta == NULL)
	{
		return 1;
	}
	else
	{
		// reserve memory
		for (int i = 0; i < NUMS_LEVEL; i++)
		{
			// time_quanta[i] = 2 * time_quanta[i - 1];
			time_quanta[i] = 1 << i; // shift by 2
			MLF_queues[i] = new_queue();
		}
		return 0;
	}
}

process *MLF_new_arrival(process *arriving_process, process *running_process)
{
	// TODO
	if (arriving_process != NULL)
	{

		queue_add(arriving_process, MLF_queues[0]);
	}
	return running_process;
}
void MLF_finish()
{
	// TODO
	for (int i = 0; i < NUMS_LEVEL; i++)
	{
		free_queue(MLF_queues[i]);
	}
	free(MLF_queues);
	free(time_quanta);
}
