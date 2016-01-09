/*
This code was provided by [Prof. Dr.-Ing S. Kühn](http://www.htw-dresden.de/en/faculty-of-informaticsmathematics/personal/professuren/prof-dr-sabine-kuehn.html)
in December 2015
at HTW Dresden for the purpose of solving the exercise
without any notice of a license or copyright.
All rights reserved, German law applies.
*/
struct timeouts
{
	unsigned long seq_nr;
	unsigned long timer;	// number of TIMEOUT_INT for retransmitting
	struct timeouts* next;	// pointer to next timeout
};

// add in order
struct timeouts* add_timer(struct timeouts *list, int timer_val, unsigned long seq_nr);
struct timeouts* del_timer(struct timeouts *list, unsigned long seq_nr);
int decrement_timer(struct timeouts *list);