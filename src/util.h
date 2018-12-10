/****************************************************************************
*   Title : 
*   Desc  : 
*   Author: 
*   Date  : 
*****************************************************************************/

#ifndef __UTIL_H__
#define	__UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

    void errorMsg(const char *s, ...);
    FILE *file_open(const char *fname, const char *mode);
    int randomize();
    void *mem_alloc(int size);
    void *mem_calloc(int size, int num);
    double get_ptime(double *user, double *sys);
    double get_utime();
//    double get_ptime();
    int sign(int val);

#ifdef __cplusplus
}
#endif
#endif
