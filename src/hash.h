/****************************************************************************
*	Title : 
*	Desc  : 
*	Author: Jeon Jinhwan
*	Date  : 1995.2.13
*****************************************************************************/


typedef struct hashtable {
    void *data;
    struct hashtable *next;
} HASHTABLE;

typedef struct hashtype {
    int (*func) ();
    int (*cmp) ();
    HASHTABLE *hdr;
    int len;
} HASH;

void init_hash( /*HASH *hash, int (*func)(), int(*cmp)(), int len */ );
void close_hash( /*HASH *hash */ );
void *hash_find( /*HASH *hash, void *data */ );
void hash_insert( /*HASH *hash, void *data */ );
int default_hashfunc();
int default_hashcmp();
