/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "die.h"

int
main(int argc, char **argv)
{
	if (argc < 3)
		die("usage: azuctl [flags] [dir...]\n");

	int fddir;
	if ((fddir = open(".", 0)) == -1)
		die("azuctl: unable to open: .: ");
	size_t len = strlen(argv[1]);

	for (int i = 0; argv[i + 2] != NULL; ++i) {
		if (chdir(argv[i + 2]) == -1)
			die("azuctl: unable to chdir: %s: ", argv[i + 2]);

		int fd;
		if ((fd = open("ctl", O_WRONLY | O_NONBLOCK)) == -1)
			die("azuctl: unable to open: %s/ctl: ", argv[i + 2]);
		if (write(fd, argv[1], len) != (ssize_t)len)
			die("azuctl: unable to write: %s/ctl: ", argv[i + 2]);
		close(fd);

		if (fchdir(fd) == -1)
			die("azuctl: unable to chdir: ");
	}
}
