#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ENABLE_AC \
	__asm__ volatile ("pushf\norl $0x40000,(%rsp)\npopf")
#define DISABLE_AC \
	__asm__ volatile ("pushf\nandl $~(0x40000),(%rsp)\npopf")

#define ENABLE_TF \
	__asm__ volatile ("pushf\norl $0x100, (%rsp)\npopf\n")

#define DISABLE_TF \
	__asm__ volatile ("pushf\nandl $0xfffffeff, (%rsp)\npopf\n")

void trap_handler(int signum, siginfo_t *si, void *vuctx)
{
    /* we disable AC checking here, so we can do printf and stuff :) */
	DISABLE_AC;

	switch (signum)
	{
		case SIGTRAP:
			printf("Caught single-step trap (RIP=%p)\n", si->si_addr);
			break;		
	}


    /* here we want to get a signal if the victim accesses unaligned */
	ENABLE_AC;
}

void bus_handler(int signum, siginfo_t *si, void *vuctx)
{
    /* we disable AC checking here, so we can do printf and stuff :) */
	DISABLE_AC;

	switch (signum)
	{
		case SIGBUS:
            printf("we got an #AC at %p!\n", si->si_addr);
            exit(1);
			break;
	}

    /* we don't re-enable AC so the victim is allowed to finish :) */
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

void register_bus_handler(void)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = bus_handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset(&sa.sa_mask);
	sigaction(SIGBUS, &sa, NULL);
}

void my_inc(int input)
{
	static char *cptr = NULL;
	int *iptr;

	if (!cptr)
		cptr = malloc(sizeof(int));

	iptr = (int *)++cptr;

	if (input)
		*cptr = 1;
	else
		*iptr = 1;
}

int main(int argc, char **argv)
{
    if (argc !=2)
    {
        printf("Usage: %s int\n", argv[0]);
        exit(1);
    }
    int arg = atoi(argv[1]);

	register_bus_handler();
	signal(SIGBUS, bus_handler);

	register_trap_handler();
	signal(SIGTRAP, trap_handler);

    /* we first do a "warm-up" call so the static cptr gets malloc-ed and we
     * don't have to single-step through libc :) */
    my_inc(0);

	ENABLE_TF;
	ENABLE_AC;
	my_inc(arg);
	DISABLE_AC;
	DISABLE_TF;
}
