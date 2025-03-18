#include "../lib/queue.h"
#include <stdlib.h>
#include <stdio.h>

int queue_add(void *new_object, queue_object *queue)
{
	// TODO
	queue_object *new_node = (queue_object *)malloc(sizeof(queue_object));
	new_node->next = NULL;
	new_node->object = new_object;
	// new_node->next = NULL;

	if (queue == NULL)
	{
		queue = new_node;
	}
	else
	{
		// save the head of the queue
		queue_object *next_head = queue->next;
		// add new Node before the next element of the head
		queue->next = new_node;
		// assign the head to the new node<
		new_node->next = next_head;

		return 0;
	}
}

void *queue_poll(queue_object *queue)
{
	// TODO
	queue_object *temp = queue;
	if (queue == NULL || queue->next == NULL)
	{
		return NULL;
	}
	else
	{

		while (temp->next->next != NULL)
		{
			temp = temp->next;
		}
		// save object before the last element
		queue_object *before_end = temp->next;
		// save the value of the object
		void *obj = before_end->object;
		temp->next = NULL;
		free(before_end);
		// free(temp);
		return obj;
	}
}

queue_object *new_queue()
{
	// TODO
	queue_object *new_queue = (queue_object *)malloc(sizeof(queue_object));
	new_queue->next = NULL;
	new_queue->object = NULL;
	return new_queue;
}

void free_queue(queue_object *queue)
{
	// TODO
	while (queue)
	{
		queue_object *temp_queue = queue;
		queue = queue->next;
		free(temp_queue);
	}
}

void *queue_peek(queue_object *queue)
{
	// TODO
	queue_object *temp = queue;
	if (temp == NULL)
	{
		return NULL;
	}
	else
	{
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
	}
	return temp->object;
}
