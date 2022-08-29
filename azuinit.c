/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

int
main(void)
{
	if (getpid() != 1) return 1;
	chdir("/");

	sigset_t set;
	sigfillset(&set);
	sigprocmask(SIG_BLOCK, &set, NULL);

	if (fork() != 0) for (;;) {
		int signal;
		sigwait(&set, &signal);

		if (signal == SIGCHLD) {
			while (waitpid(-1, NULL, WNOHANG) > 0);
		} else if (signal == SIGTERM || signal == SIGINT) {
			sigprocmask(SIG_UNBLOCK, &set, NULL);
			execve("/etc/rc.shutdown", signal != SIGTERM ?
					(char *[]){ (char []){
							"reboot" }, NULL } :
					(char *[]){ (char []){
							"poweroff" }, NULL },
					(char *[]){ NULL });
			return 1;
		}
	}

	/* child must execute init scripts */
	sigprocmask(SIG_UNBLOCK, &set, NULL);
	setsid(); setpgid(0, 0);
	execve("/etc/rc.init", (char *[]){ NULL }, (char *[]){ NULL });
	return 1;
}
