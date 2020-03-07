#ifndef STRINGS_H
#define STRINGS_H

char vtolower(char c);
char vtoupper(char c);
void vstrup(char *c);
void vstrlwr(char *c);
int  vstrcmp(const char *s1, const char *s2);
int  vstrcasecmp(const char *s1, const char *s2);
int  vstricmp(const char *s1, const char *s2);
void vmemset (void *dest, int fill, int count);
void vmemcpy (void *dest, const void *src, int count);
int  vmemcmp (const void *m1, const void *m2, int count);
void vstrcpy(char *dest, const char *src);
void vstrncpy(char *dest, const char *src, int n);
int  vstrlen(char *s);
void vkillCRLF(char *s);
int  rnd(int lo, int hi);
#endif
