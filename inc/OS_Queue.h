/*
 * OS_Queue.h
 *
 *  Created on: Aug 26, 2022
 *      Author: junior
 */

#ifndef MSE_ISO_MSE_OS_INC_OS_QUEUE_H_
#define MSE_ISO_MSE_OS_INC_OS_QUEUE_H_

#include "OS_Core.h"


#define MAX_QUEUE_ITEM 40

typedef struct _queueItem
{
	void*					item;
	struct _queueItem *	siguiente_item;
} queueItem_t;

typedef struct _queue
{
	queueItem_t * 	primer_item;
	queueItem_t * 	ultimo_item;
	uint32_t 		conteoQ;
	tarea* 		tarea_bloqueada;
} queue_t;

/**/
queueItem_t* QUEUE_ITEM_CREATE_OS(void* item);///////////////NO ESTA

/**/
queue_t* os_queue_create(void);///////////////////////////////NO ESTA

/**/
void QUEUE_ITEM_DELETE_OS(queue_t* queue);

/**/
void QUEUE_DELETE_OS(queue_t* queue);

/**/
void* os_queue_read( queue_t* queue);///////////////////////////////NO ESTA

/**/
void QUEUE_WRITE_OS( queue_t* queue, void* item , bool from_irq);

void init_queue(void);


#endif /* MSE_ISO_MSE_OS_INC_OS_QUEUE_H_ */
