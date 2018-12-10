/****************************************************************************
*   Title : util.c
*   Desc  : 
*   Author: Jinhwan Jeon
*   Date  : 1995.2.13, 1995.2.15
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdarg.h>

void errorMsg(const char *format, ...)
{
    va_list ap;
    char buf[1024];

    va_start(ap, format);
    vsprintf(buf, format, ap);
    fprintf(stderr, "%s\n", buf);
    va_end(ap);
    exit(-1);
}

FILE *file_open(const char *name, const char *mode)
{
    FILE *fp;
    fp = fopen(name, mode);
    if (!fp) {
	errorMsg("File open error %s", name);
    }
    return fp;
}

void *mem_alloc(int len)
{
    void *ptr;
    ptr = malloc(len);
    if (!ptr) {
	errorMsg("Memory allocation error(in malloc(%d))", len);
    }
    return ptr;
}

void *mem_calloc(int len, int num)
{
    void *ptr;
    ptr = calloc(len, num);
    if (!ptr) {
	errorMsg("Memory allocation error(in calloc(%d, %d))", len, num);
    }
    return ptr;
}

int randomize()
{
    struct timeval tv;
    struct timezone tz;
    int seed;

    gettimeofday(&tv, &tz);

    seed = tv.tv_usec;

    srand(seed);
    return seed;
}

double get_ptime(double *utime, double *stime)
{
    struct rusage rusage;
    double user, system;

    getrusage(RUSAGE_SELF, &rusage);
    user = rusage.ru_utime.tv_sec * 1.0e6 + rusage.ru_utime.tv_usec;
    system = rusage.ru_stime.tv_sec * 1.0e6 + rusage.ru_stime.tv_usec;

    if (utime)
	*utime = user;
    if (stime)
	*stime = system;

    return user + system;
}

double get_utime()
{
    double utime;
    double stime;

    get_ptime(&utime, &stime);

    return utime;
}

double get_stime()
{
    double utime;
    double stime;

    get_ptime(&utime, &stime);

    return stime;
}

int sign(int val)
{
    return val >= 0 ? 1 : -1;
}
