/* azurill - init system
 * Copyright (C) 2022 ArcNyxx
 * see LICENCE file for licensing information */

typedef struct status {
	time_t start;
	pid_t parent, child;
	bool down, pause;
} status_t;
