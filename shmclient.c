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
	printf("usage: shmclient -dt\n");
}

int
main(int argc, char *argv[])
{
	struct keyvalue kv;
	int ch, i, rc, nloop;
	int opt_d = 0, opt_t = 0;

	while ((ch = getopt(argc, argv, "dt")) != -1) {
		switch (ch) {
		case 'd':
			opt_d = 1;
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

	shmqueue = shmqueue_new(MEMID, 0, 1, 1);

	for (nloop = i = 0; i >= 0; i++, nloop++) {
#if 1
		char key[1024];
		char data[1024];

		sprintf(key, "%d", i);
		sprintf(data, "data of %d", i);

		rc = shmqueue_keyvalue_store(shmqueue, (uint8_t *)key, (uint8_t *)data, strlen(data));
		if (rc != 0) {
			if ((nloop & 0xffff) == 0)
				printf("失敗した(%d)\n", i);
			i--;
			continue;
		}

		if ((i & 0xff) == 0)
			printf("成功した(%d)\n", i);

		fflush(stdout);
#endif
	}

	(void)&rc;
	(void)&kv;
#if 0
	for (i = 0; i < 10000; i++) {
		rc = shmqueue_getoldest(shmqueue, &kv);
		printf("rc = %d\n", rc);
		fflush(stdout);

		if (rc == 0) {
			fprintf(stderr, "EXPIRE: key=<%s>, storage_size=%u, storage=<%s>\n",
			    KEYVALUE_KEY(&kv),
			    kv.kv_storagesize,
			    KEYVALUE_STORAGE(&kv));
		}
	}
#endif


	return 0;
}
