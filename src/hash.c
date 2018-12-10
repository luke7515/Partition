/****************************************************************************
*	Title : hash.c
*	Desc  : 
*	Author: Jeon Jinhwan
*	Date  : 1995.2.13
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "util.h"

static int _hashlen;

int default_hashfunc(char *data)
{
    int h = 0;
    while (*data)
	h = (64 * h + *data++) % _hashlen;

    return h;
}

int default_hashcmp(char *data1, char *data2)
{
    return strcmp(data1, data2);
}

void init_hash(HASH * hash, int (*func) (), int (*cmp) (), int len)
{
    if (!func)
	func = default_hashfunc;
    if (!cmp)
	cmp = default_hashcmp;
    hash->func = func;
    hash->cmp = cmp;
    hash->len = len;
    hash->hdr = (HASHTABLE *) mem_calloc(sizeof(HASHTABLE), len);
}

void close_hash(HASH * hash)
{
    HASHTABLE *h, *hn;
    int i;
    for (i = 0; i < hash->len; i++) {
	h = hash->hdr + i;
	h = h->next;
	while (h) {
	    hn = h->next;
	    free(h->data);
	    free(h);
	    h = hn;
	}
    }
    free(hash->hdr);
}

void *hash_find(HASH * hash, void *data)
{
    int i;
    HASHTABLE *hp;
    _hashlen = hash->len;
    i = (*hash->func) (data) % hash->len;

    hp = hash->hdr + i;
    while (hp && hp->data) {
	if (!(*hash->cmp) (hp->data, data))
	    return hp->data;
	hp = hp->next;
    }
    return 0;
}

void hash_insert(HASH * hash, void *data)
{
    int i;
    HASHTABLE *hp, *h;
    _hashlen = hash->len;
    i = (*hash->func) (data) % hash->len;

    hp = hash->hdr + i;
    if (hp->data != 0) {
	h = (HASHTABLE *) mem_calloc(sizeof(HASHTABLE), 1);
	h->data = data;
	h->next = hp->next;
	hp->next = h;
    } else {
	hp->data = data;
    }
}
