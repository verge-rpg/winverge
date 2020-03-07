#ifndef STRINGS_H
#define STRINGS_H

char vtolower(char c);
char vtoupper(char c);
void vstrup(char *c);
void vstrlwr(char *c);
int  vstrcmp(char *s1, char *s2);
int  vstrcasecmp(char *s1, char *s2);
int  vstricmp(char *s1, char *s2);
void vmemset (void *dest, int fill, int count);
void vmemcpy (void *dest, void *src, int count);
int  vmemcmp (void *m1, void *m2, int count);
void vstrcpy(char *dest, char *src);
void vstrncpy(char *dest, char *src, int n);
int  vstrlen(char *s);
void vkillCRLF(char *s);
int  rnd(int lo, int hi);
#endif
