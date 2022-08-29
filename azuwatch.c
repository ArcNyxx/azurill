/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "azustat.h"
#include "die.h"

static pid_t start(const char *dir);
static void status(pid_t pid);

time_t newtime;
bool down, once, ppause = false, pexit = false;

static pid_t
start(const char *dir)
{
	pid_t pid;
	switch (pid = fork()) {
	case -1:
		sleep(60);
		return 0;
	default:
		newtime = time(NULL);
		status(pid);
		sleep(1);
		return pid;
	case 0:
		execvp("./run", (char *[]){ (char []){ "./run" }, NULL });
		die("azuwatch: unable to exec: %s/run: ", dir);
	}
	/* NOTREACHED */
	return 0;
}

static void
status(pid_t pid)
{
	int fd;
	if ((fd = open("stat.new", O_WRONLY | O_CREAT | O_TRUNC)) == -1)
		return;
	if (write(fd, &(status_t){ 
		newtime, getpid(), pid, down, once, ppause
	}, sizeof(status_t)) != sizeof(status_t)) {
		close(fd);
		return;
	}
	close(fd);
	rename("stat.new", "stat"); /* ignore return value */
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("usage: azuwatch [dir]\n");
	if (chdir(argv[1]) == -1)
		die("azuwatch: unable to chdir: %s: ", argv[1]);

	struct stat info;
	down = stat("down", &info) != -1;
	if (!down && errno != ENOENT)
		die("azuwatch: unable to stat: %s/down: ", argv[1]);
	once = stat("once", &info) != -1;
	if (!once && errno != ENOENT)
		die("azuwatch: unable to stat: %s/once: ", argv[1]);

	int rctl, wctl;
	if (unlink("ctl") == -1 && errno != ENOENT)
		die("azuwatch: unable to unlink: %s/ctl: ", argv[1]);
	if (mkfifo("ctl", 0600) == -1)
		die("azuwatch: unable to mkfifo: %s/ctl: ", argv[1]);
	if ((rctl = open("ctl", O_RDONLY | O_NONBLOCK | O_CLOEXEC)) == -1)
		die("azuwatch: unable to open for reading: %s/ctl: ", argv[1]);
	if ((wctl = open("ctl", O_WRONLY | O_CLOEXEC)) == -1)
		die("azuwatch: unable to open for writing: %s/ctl: ", argv[1]);

	pid_t pid = 0;
	if (!down)
		pid = start(argv[1]);
	status(pid);

	while (!pexit || pid != 0) {
		for (pid_t check; (check =
				waitpid(-1, NULL, WNOHANG)) != 0; ) {
			if (check == -1 && errno != EINTR)
				break;
			if (check == pid) {
				if (pexit)
					return 0;
				if (once)
					down = once = false;
				pid = down ? 0 : start(argv[1]);
				status(pid);
				break;
			}
		}

		char ch;
		if (read(rctl, &ch, 1) != 1)
			continue;
		switch (ch) {
		case 'd':
			down = true, ppause = false;
			if (pid != 0) {
				kill(pid, SIGTERM);
				kill(pid, SIGCONT);
			}
			status(pid);
			break;
		case 'u':
			down = ppause = false;
			if (pid == 0)
				pid = start(argv[1]);
			status(pid);
			break;
		case 'p':
			ppause = true;
			if (pid != 0)
				kill(pid, SIGSTOP);
			status(pid);
			break;
		case 'c':
			ppause = false;
			if (pid != 0)
				kill(pid, SIGCONT);
			status(pid);
			break;
		case 'a':
			if (pid != 0)
				kill(pid, SIGALRM);
			break;
		case 'h':
			if (pid != 0)
				kill(pid, SIGHUP);
			break;
		case 'k':
			if (pid != 0)
				kill(pid, SIGKILL);
			break;
		case 't':
			if (pid != 0)
				kill(pid, SIGTERM);
			break;
		case 'i':
			if (pid != 0)
				kill(pid, SIGINT);
			break;
		case 'o':
			once = true;
			status(pid);
			break;
		case 'x':
			pexit = true;
			break;
		default:
			break;
		}
	}
}
