/*
This code was provided by [Prof. Dr.-Ing S. Kühn](http://www.htw-dresden.de/en/faculty-of-informaticsmathematics/personal/professuren/prof-dr-sabine-kuehn.html)
in December 2015
at HTW Dresden for the purpose of solving the exercise
without any notice of a license or copyright.
All rights reserved, German law applies.
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <string.h> 


#include "timer.h"

#define DEBUG 

struct timeouts* add_timer(struct timeouts *list, int timer_val, unsigned long seq_nr)
{
	struct timeouts	*help, *new_elem;
	int sum = 0;

	new_elem = (struct timeouts*)malloc(sizeof(struct timeouts));
	// insert in order
	if (list == NULL)
	{
		list = new_elem;
		list->next = NULL;
	}
	else
	{
		if (timer_val < list->timer)			//must be the first element
		{
			help = new_elem;
			list->timer = list->timer - timer_val; //this is now the second element, must be relative to the previous one		N E U
			help->next = list;
			list = help;
		}
		else
		{
			help = list;
			timer_val = timer_val - help->timer;
			while (help->next != NULL)
			{
				if ((timer_val - (help->next)->timer) >= 0)
				{
					help = help->next;
					timer_val = timer_val - help->timer;
				}
				else
				{
					break;
				}
			}
			// new element will be inserted after help
			new_elem->next = help->next;
			help->next = new_elem;
		}
	}
	new_elem->seq_nr = seq_nr;
	new_elem->timer = timer_val;


#ifdef DEBUG 

	help = list;
	while (help != NULL)
	{
		//printf("add_timer:seq_nr: %d \t timer: %d \n\n", help->seq_nr,help->timer);
		help = help->next;
	}
#endif
	return list;
}

struct timeouts* del_timer(struct timeouts *list, unsigned long seq_nr)
{
	struct timeouts *help, *helper = list;

	if (list == NULL) return NULL;
	if (list->seq_nr == seq_nr)
	{
		help = list;
		list = list->next;

		// as the timer values are relative to each other the 
		// next element`s timer must be the sum of deleted element`s timer and its own timer
		if (list != NULL) list->timer += help->timer;
	}
	else
	{
		help = helper->next;
		while (help != NULL)
		{
			if (help->seq_nr == seq_nr)
			{
				helper->next = help->next;
				if (help->next != NULL) (help->next)->timer += help->timer;
				break;
			}
			else
			{
				helper = help;
				help = help->next;
			}
		}
	}

#ifdef DEBUG  
	if (help != NULL)
	{
	}//printf("del_timer:seq_nr %d \t timer%d \n", help->seq_nr,help->timer);
	else
		printf("del_timer ERROR: help is NULL!!\n");
#endif
	if (help != NULL)
		free(help);

#ifdef DEBUG  

	//printf("del_timer: list after delete: \n");
	help = list;
	while (help != NULL)
	{
		//printf("del_timer: seq_nr %d \t timer%d \n", help->seq_nr,help->timer);
		help = help->next;
	}
#endif

	return list;
}

int decrement_timer(struct timeouts *list)
{
	struct timeouts *help;
	if (list == NULL)
		return -1;
	list->timer--;

#ifdef DEBUG 
	help = list;
	if (list == NULL)
		printf("del_timer: LISTE empty \n");
	while (help != NULL)
	{
		//printf("decrement_timer:seq_nr %d \t timer%d \n", help->seq_nr,help->timer);
		help = help->next;
	}
#endif

	if (list->timer)
		return 1;
	else
		return 0;
}