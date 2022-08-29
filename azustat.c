/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "azustat.h"
#include "die.h"

int
main(int argc, char **argv)
{
	if (argc != 2)
		die("usage: azustat [dir]\n");
	if (chdir(argv[1]) == -1)
		die("azustat: unable to chdir: %s: ", argv[1]);

	int fd;
	status_t status;
	if ((fd = open("stat", O_RDONLY)) == -1)
		die("azustat: unable to open: %s/stat: ", argv[1]);
	if (read(fd, &status, sizeof(status_t)) != sizeof(status_t))
		die("azustat: unable to read: %s/stat: ", argv[1]);
	close(fd);

	if (kill(status.parent, 0) != -1) {
		printf("azustat: parent running with pid %d\n", status.parent);
	} else {
		printf("azustat: parent not running\n");
		return 0;
	}
	if (status.child != 0) {
		printf("azustat: child running with pid %d\n", status.child);
		printf("azustat: child running since %ld\n", status.start);
	} else {
		printf("azustat: child not running\n");
	}
	if (status.down)
		printf("azustat: child wanted down\n");
	else
		printf("azustat: child wanted up\n");
	if (status.pause)
		printf("azustat: child paused\n");
}
