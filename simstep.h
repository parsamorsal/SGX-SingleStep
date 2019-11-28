#ifndef _SIMSTEP_H_
#define _SIMSTEP_H_

void trap_handler(int signum, siginfo_t *si, void *vuctx);
void register_trap_handler(void);
void single_step_init(int print_rip);
void start_single_stepping();
void stop_single_stepping();

#endif
