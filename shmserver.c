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
int reap_items(void);

struct shmqueue *shmqueue;

void
usage()
{
	printf("usage: shmserver [-afi]\n");
}


int
reap_items()
{
	int i, nexpire;
	struct keyvalue kv;

	shmqueue_dump(shmqueue);

	nexpire = 0;
	for (i = 0; i < 5000; i++) {
#if 1
		if (!shmqueue_watermark(shmqueue))
			break;
#else
		if (shmqueue_inuse(shmqueue) < 5000)
			break;
#endif

		/* XXX: TODO */

		/* fetch from expire queue, and store to db */
		if (shmqueue_getoldest(shmqueue, &kv) != 0)
			break;

		if ((i & 0xff) == 0) {
			fprintf(stderr, "EXPIRE: key=<%s>, storage_size=%u, storage=<%s>\n",
			    KEYVALUE_KEY(&kv),
			    kv.kv_storagesize,
			    KEYVALUE_STORAGE(&kv));
			fflush(stdout);
		}

		nexpire++;
	}

	fflush(stdout);

	return nexpire;
}


int
main(int argc, char *argv[])
{
	int ch;
	int opt_a = 0, opt_f = 0, opt_i = 0;

	while ((ch = getopt(argc, argv, "afi")) != -1) {
		switch (ch) {
		case 'a':
			opt_a = 1;
			break;
		case 'f':
			opt_f = 1;
			break;
		case 'i':
			opt_i = 1;
			break;
		default:
			usage();
			return 1;
		}
	}
	argc -= optind;
	argv += optind;

	shmqueue = shmqueue_new(MEMID, MEMSIZE, !opt_i, 1);
	if (shmqueue == NULL) {
		shmqueue = shmqueue_new(MEMID, 0, !opt_i, 1);
		if (shmqueue == NULL)
			err(1, "shmqueue_new");
	}

	while (1) {
		if (reap_items() == 0) {
//			sleep(1);
		}
	}

	return 0;
}
