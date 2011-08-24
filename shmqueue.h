#ifndef _SHMQUEUE_H_
#define _SHMQUEUE_H_

#include <stddef.h>
#include "rtailq.h"

#define DEBUGTRACE	printf("%s:%d\n", __func__, __LINE__)

#define SHMQUEUE_ITEMSIZE		2048
#define SHMQUEUE_HASHNUM		199999	/* must be prime number! */

typedef volatile unsigned char	__cpu_simple_lock_t;
void __cpu_simple_lock_init(__cpu_simple_lock_t *);
void __cpu_simple_lock(__cpu_simple_lock_t *);
void __cpu_simple_unlock(__cpu_simple_lock_t *);
void __cpu_simple_lock_try(__cpu_simple_lock_t *);

struct keyvalue {
	uint32_t kv_keysize;	/* including '\0' */
	uint32_t kv_storagesize;
	uint8_t kv_data[2000];	/* XXX: "<key>\0<storage>" */
#define KEYVALUE_KEY(keyvalue)		((keyvalue)->kv_data)
#define KEYVALUE_STORAGE(keyvalue)	((keyvalue)->kv_data + (keyvalue)->kv_keysize)
};

#define SHMQUEUE_KEYVALUE_TOTALSIZE	(sizeof(struct shmqueue_item) - offsetof(struct shmqueue_item, shi_keyvalue))
#define SHMQUEUE_KEYVALUE_DATASIZE	(SHMQUEUE_KEYVALUE_TOTALSIZE - offsetof(struct keyvalue, kv_data))



#undef SHMQUEUE_BUCKET_HACK	/* XXX: notyet */

struct shmqueue_item {
	union {
		struct {
#ifdef SHMQUEUE_BUCKET_HACK
			int shi_bucketidx;
#endif
			RTAILQ_ENTRY(shmqueue_item) shi_list;		/* in sha_lrulist or sha_freelist */
			RTAILQ_ENTRY(shmqueue_item) shi_hashlist;	/* in sha_hashlist[myhash] */
			uint32_t shi_hashidx;
			/* userarea */
			struct keyvalue shi_keyvalue;
		};
		char stub[SHMQUEUE_ITEMSIZE];
	};
};



#ifdef SHMQUEUE_BUCKET_HACK
#define SHMQUEUE_NBUCKET	4
#define SHMQUEUE_BUCKET0_SIZE	128
#define SHMQUEUE_BUCKET1_SIZE	512
#define SHMQUEUE_BUCKET2_SIZE	1024
#define SHMQUEUE_BUCKET3_SIZE	2048
#endif

struct shmqueue_header {
#ifdef SHMQUEUE_BUCKET_HACK
	RTAILQ_HEAD(sha_freelists) sha_freelists[SHMQUEUE_NBUCKET];
#else
	RTAILQ_HEAD(sha_freelist) sha_freelist;
#endif
	RTAILQ_HEAD(sha_lrulist) sha_lrulist;
	size_t sha_memsize;
	unsigned int sha_itemsize;
	unsigned int sha_itemnum;
	unsigned int sha_item_inuse;
	unsigned int sha_hashnum;
	struct shmqueue_item *sha_pool;
	__cpu_simple_lock_t shmqueue_cpulock;
	RTAILQ_HEAD(sha_hashlist) sha_hashlist[];
};

struct shmqueue {
	struct shmqueue_header *header;
	uint32_t (*hash_callback)(uint8_t *);
};


#define SHMQUEUE_ALIGNSIZE	4096

#define SHMQUEUE_ALIGN(ptr, align)	\
	(((uintptr_t)(ptr) + ((uintptr_t)(align)) - 1) & -(((uintptr_t)(align))))

#define SHMQUEUE_STRCMP(a, b)	strcmp((char *)(a), (char *)(b))
#define SHMQUEUE_STRCPY(a, b)	strcpy((char *)(a), (char *)(b))

/* constructor */
struct shmqueue *shmqueue_new(int, size_t, int, int);

/* initialize */
int shmqueue_create(struct shmqueue *, void *, size_t);
int shmqueue_attach(struct shmqueue *, void *);
void shmqueue_setcallback(struct shmqueue *, uint32_t (*)(uint8_t *));

uint32_t shmqueue_hash_string(uint8_t *);

/* item handling */
struct shmqueue_item *shmqueue_remove_from_freelist(struct shmqueue *);
int shmqueue_add_to_lru(struct shmqueue *, struct shmqueue_item *, uint32_t);
struct shmqueue_item *shmqueue_remove_from_lru(struct shmqueue *);
void shmqueue_add_to_freelist(struct shmqueue *, struct shmqueue_item *);

/* for expirering */
int shmqueue_inuse(struct shmqueue *);
int shmqueue_watermark(struct shmqueue *);
int shmqueue_getoldest(struct shmqueue *, struct keyvalue *);

/* keyvalue store */
int shmqueue_keyvalue_store(struct shmqueue *, uint8_t *, uint8_t *, uint32_t);
const struct keyvalue *shmqueue_keyvalue_peek(struct shmqueue *, uint8_t *);
struct keyvalue *shmqueue_keyvalue_fetch(struct shmqueue *, uint8_t *, struct keyvalue *);
int shmqueue_keyvalue_delete(struct shmqueue *, uint8_t *);


/* for debug */
void shmqueue_dumpitem(struct shmqueue_item *);
void shmqueue_dump(struct shmqueue *);
void shmqueue_dumpall(struct shmqueue *);


#endif /* _SHMQUEUE_H_ */
