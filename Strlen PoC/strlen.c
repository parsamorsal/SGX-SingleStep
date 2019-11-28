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
	printf("%d instructions single-stepped.\n",rip_count);
}

void gen_random(char *s, int l) {
	
    for (int c; c=rand()%62, *s++ = (c+"07="[(c+16)/26])*(l-->0););
}

int call_strlen(char *s) {
  return __builtin_strlen(s);
}

int main(){
	
	srand( time(NULL) );
	for(int i=0;i<10;i++){

	int len = rand() % (29) + 2;
	char s[len];
	gen_random(s,len);

	printf("%s\n",s);
	

	single_step_init(0);

	start_single_stepping();
	
	call_strlen(s);

	
	stop_single_stepping();
	printf("victim string length is: %d\n",rip_count-27);
	}

	
}

