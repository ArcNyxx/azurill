/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "die.h"

static void start(char *name, dev_t dev, ino_t ino);

struct { dev_t dev; ino_t ino; pid_t pid; } serv[256];
int servlen = 0;

static void
start(char *name, dev_t dev, ino_t ino)
{
	int i;
	for (i = 0; i < servlen; ++i)
		if (serv[i].dev == dev && serv[i].ino == ino)
			break;
	if (i == 256 || serv[i].pid != 0) {
		return;
	} else if (i == servlen) {
		serv[i].dev = dev;
		serv[i].ino = ino;
		++servlen;
	}

	pid_t pid;
	switch (pid = fork()) {
	case -1:
		return;
	default:
		serv[i].pid = pid;
		break;
	case 0:
		execvp("azuwatch", (char *[]){
				(char []){ "azuwatch" }, name, NULL });
		die("azurun: unable to exec: azuwatch %s: ", name);
	}
}

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("usage: azurun [dir]\n");
	if (chdir(argv[1]) == -1)
		die("azuwatch: unable to chdir: %s: ", argv[1]);

	for (;;) {
		DIR *dir;
		if ((dir = opendir(".")) == NULL)
			goto check;
		for (struct dirent *entry; (entry = readdir(dir)) != NULL; ) {
			if (entry->d_name[0] == '.')
				continue;

			struct stat info;
			if (stat(entry->d_name, &info) == -1)
				continue;
			if (!S_ISDIR(info.st_mode))
				continue;
			start(entry->d_name, info.st_dev, info.st_ino);
		}
		closedir(dir);

check:
		for (pid_t check; (check =
				waitpid(-1, NULL, WNOHANG)) != 0; ) {
			if (check == -1 && errno != EINTR)
				break;
			for (int i = 0; i < servlen; ++i)
				if (check == serv[i].pid)
					serv[i].pid = 0;
		}
		sleep(5);
	}
}
