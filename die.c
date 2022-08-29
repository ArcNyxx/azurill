/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "die.h"

void
die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[strlen(fmt) - 1] != '\n')
		perror(NULL);
	exit(1);
}
