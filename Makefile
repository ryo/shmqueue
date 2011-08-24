


CFLAGS+=	-Wall -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith
CFLAGS+=	-Werror
CFLAGS+=	-fPIC -DPIC
CFLAGS+=	-g


PREFIX	=	/usr/local



ALL: shmserver shmclient shmdump libshmqueue.a

shmqueue_x86.o: shmqueue_x86.S
	cc ${CFLAGS} -c shmqueue_x86.S

shmqueue.o: shmqueue.c shmqueue.h rtailq.h
	cc ${CFLAGS} -c shmqueue.c

libshmqueue.a: shmqueue.o shmqueue_x86.o
	ar crs libshmqueue.a shmqueue.o shmqueue_x86.o

shmserver: shmserver.c shm_config.h libshmqueue.a
	cc ${CFLAGS} -o shmserver shmserver.c libshmqueue.a

shmclient: shmclient.c shm_config.h libshmqueue.a
	cc ${CFLAGS} -o shmclient shmclient.c libshmqueue.a

shmdump: shmdump.c shm_config.h libshmqueue.a
	cc ${CFLAGS} -o shmdump shmdump.c libshmqueue.a

clean:
	rm -f *.o shmserver shmclient shmdump

install:
	install -o bin -g bin -m 644 shmqueue.h ${PREFIX}/include
	install -o bin -g bin -m 644 rtailq.h ${PREFIX}/include
	install -o bin -g bin -m 644 libshmqueue.a ${PREFIX}/lib
