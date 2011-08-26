#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <err.h>

#include "shm_config.h"
#include "shmqueue.h"

void usage(void);

struct shmqueue *shmqueue;

void
usage()
{
	printf("usage: shmdump -t\n");
}

int
main(int argc, char *argv[])
{
	int ch, id;
	int opt_t = 0;
	int opt_s = 0;

	while ((ch = getopt(argc, argv, "st")) != -1) {
		switch (ch) {
		case 's':
			opt_s = 1;
			break;
		case 't':
			opt_t = 1;
			break;
		default:
			usage();
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	id = shmget(MEMID, MEMSIZE, 0);
	if (id == -1)
		err(1, "shmget");

	shmqueue = shmqueue_new(MEMID, 0, 1, 0);

	if (opt_s) {
		shmqueue_dump_statistics(shmqueue);
		return 0;
	}


	if (opt_t) {
		shmqueue_dump_tsv(shmqueue);
	} else {
		shmqueue_dumpall(shmqueue);
	}

	return 0;
}
