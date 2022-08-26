/*
 * OS_Queue.c
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#include "OS_Queue.h"
#include "stdlib.h"

queue_t* displayQueue;
queue_t* controlTecQueue;
/**/
queueItem_t* QUEUE_ITEM_CREATE_OS(void* item)
{
	queueItem_t* new_item;

	new_item = (queueItem_t*) malloc(sizeof(queueItem_t));

	if(new_item == NULL)
	{
		os_set_error(NULL, MEMORY_ERROR);
		return NULL;
	}

	new_item->item =  item;
	new_item->siguiente_item = NULL;

	return new_item;
}

/**/
queue_t* os_queue_create(void)
{
	queue_t* new_queue;

	new_queue = (queue_t*) malloc(sizeof(queue_t));

	if(new_queue == NULL)
	{
		os_set_error(NULL, MEMORY_ERROR);
		return NULL;
	}

	new_queue->ultimo_item  =  NULL;
	new_queue->primer_item = NULL;
	new_queue->conteoQ = 0;
	new_queue->tarea_bloqueada = NULL;

	return new_queue;
}

/**/
void QUEUE_ITEM_DELETE_OS(queue_t* queue)
{
	queueItem_t* aux_item;

	if(queue->primer_item != NULL)
	{
		aux_item = queue->primer_item;
		queue->primer_item = queue->primer_item->siguiente_item;

		queue->conteoQ--;

		free(aux_item);

		if(queue->primer_item == NULL)
			queue->ultimo_item = NULL;
	}
}

/**/
void QUEUE_DELETE_OS(queue_t* queue)
{
	while(queue->primer_item != NULL)
	{
		QUEUE_ITEM_DELETE_OS(queue);
	}
	free(queue);
}

/**/
void* os_queue_read( queue_t* queue)
{
	void* item;

	if(queue->primer_item == NULL)
	{
		item =  NULL;
		queue->tarea_bloqueada = GET_CURRENT_TASK_OS();
		BLOCK_CURRENT_TASK_OS();
	}

	if(queue->primer_item != NULL)
	{
		item = queue->primer_item->item;
		QUEUE_ITEM_DELETE_OS(queue);
		if(queue->tarea_bloqueada != NULL)
		{
			UNBLOCK_CURRENT_TASK_OS(queue->tarea_bloqueada);
			queue->tarea_bloqueada = NULL;
		}
	}
	return item;
}

/**/
void QUEUE_WRITE_OS( queue_t* queue, void* item, bool from_irq )
{
	queueItem_t* new_item = QUEUE_ITEM_CREATE_OS(item);

	TASK_ENTER_CRITICAL_OS();
	if(new_item)
	{
		new_item->item = item;

		if(queue->primer_item == NULL)
		{
			queue->primer_item = new_item;
			queue->ultimo_item = new_item;
			queue->conteoQ++;
			if(queue->tarea_bloqueada != NULL)
			{
				UNBLOCK_CURRENT_TASK_OS(queue->tarea_bloqueada);
				queue->tarea_bloqueada = NULL;
			}
		}
		else
		{
			if(queue->conteoQ == MAX_QUEUE_ITEM)
			{
				if(from_irq == FALSE)
				{
					queue->tarea_bloqueada = GET_CURRENT_TASK_OS();
					BLOCK_CURRENT_TASK_OS();
				}
			}
			else
			{
				queue->ultimo_item->siguiente_item = new_item;
				queue->ultimo_item = new_item;
				queue->conteoQ++;
				if(queue->tarea_bloqueada != NULL)
				{
					UNBLOCK_CURRENT_TASK_OS(queue->tarea_bloqueada);
					queue->tarea_bloqueada = NULL;
				}
			}
		}
	}
	TASK_EXIT_CRITICAL_OS();
}


void QUEUE_INIT_OS(void)
{
	displayQueue = os_queue_create();
	controlTecQueue = os_queue_create();
}
