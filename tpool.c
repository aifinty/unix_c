#include "tpool.h"
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void *task_call(void*arg)
{
	tpool_t *tp  = (tpool_t*)arg;
	task_t *tk;
	while(1)
	{
		pthread_mutex_lock(&tp->mutex);
		while(NULL==(tk = dequeue(tp->tq)))
		{
			pthread_cond_wait(&tp->cond,&tp->mutex);
		}
		pthread_mutex_unlock(&tp->mutex);
		(*(tk->task_f))(tk->arg);
		sleep(1);
		free(tk);
	}
}


tpool_t *creat_tpool(int num)
{
	tpool_t *tp = (tpool_t*)malloc(sizeof(tpool_t));
	tp->ptid = (pthread_t*)malloc(sizeof(pthread_t)*num);
	tp->tq = create_queue();
	pthread_mutex_init(&tp->mutex,NULL);
	pthread_cond_init(&tp->cond,NULL);
	while(num--)
	{
		pthread_create(&tp->ptid[num],NULL,\
				task_call,(void*)tp);
		/* 此处需要进行错误处理 */
	}
	return tp;
}
int insert_task(void(*tsk)(void*arg),\
		void *arg,tpool_t*tp)
{
	task_t *tk = (task_t*)malloc(sizeof(task_t));
	tk->next = NULL;
	tk->task_f = tsk;
	tk->arg = arg;
	enqueue(tk,tp->tq);
	pthread_cond_broadcast(&tp->cond);
}
int tp_destroy(tpool_t *tp)
{
	//等等
}


