#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ENABLE_TF \
	__asm__ volatile ("pushf\norl $0x100, (%rsp)\npopf\n")

#define DISABLE_TF \
	__asm__ volatile ("pushf\nandl $0xfffffeff, (%rsp)\npopf\n")

int rip_count=0;
int print=0;
void trap_handler(int signum, siginfo_t *si, void *vuctx)
{
	switch (signum)
	{
		case SIGTRAP:
			rip_count++;
			if(print)
				printf("Caught single-step trap (RIP=%p)\n", si->si_addr);
			break;		
	}
}


void register_trap_handler(void)
{
	struct sigaction act;
	memset(&act, sizeof(act), 0);
	act.sa_sigaction = trap_handler;
	act.sa_flags = SA_RESTART | SA_SIGINFO;
	sigfillset(&act.sa_mask);
	sigaction(SIGTRAP, &act, NULL);
}

void single_step_init(int print_rip){
	
	rip_count=0;
	print=print_rip;
	register_trap_handler();	
	signal(SIGTRAP, trap_handler);
}

void start_single_stepping(){
	ENABLE_TF;
}

void stop_single_stepping(){
	DISABLE_TF;
	printf("\n%d instructions single-stepped.\n",rip_count);
}


