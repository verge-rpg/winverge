// ****************************************************************
// * PCP                                                          *
// * Copyright (C)1999 Dark Nova Software (DNS)                   *
// * All Rights Reserved.                                         *
// *                                                              *
// * File: a_strings.cpp                                          *
// * Author: vecna                                                *
// * Portability: All systems                                     *
// * Description: String manipulation functions.                  *
// ****************************************************************

#include "pcp.h"

// ***************************** Code *****************************

char vtolower(char c)
{
   if (c >= 'A' && c <= 'Z')
   { 
      c -= 'A';
      c += 'a';
   }
   return c;
}

char vtoupper(char c)
{
   if (c >= 'a' && c <= 'z')
   { 
      c -= 'a';
      c += 'A';
   }
   return c;
}

void vstrup(char *c)
{
   while (*c)
   {
      if ((char) *c >= 'a' && (char) *c <= 'z')
      {
         *c = *c - 'a';
         *c = *c + 'A';
      }
      c++;
   }
}

void vstrlwr(char *c)
{
   while (*c)
   {
      if ((char) *c >= 'A' && (char) *c <= 'Z')
      {
         *c = *c - 'A';
         *c = *c + 'a';
      }
      c++;
   }
}

int vstrcmp(const char *s1, const char *s2)
{
   while (1)
   {
      if (*s1 != *s2)
         return -1;              // strings not equal
      if (!*s1)
         return 0;               // strings are equal
      s1++;
      s2++;
   }
   return -1;
}
 
int vstrcasecmp(const char *s1, const char *s2)
{
   while (1)
   {
      if (vtolower(*s1) != vtolower(*s2))
         return -1;
      if (!*s1)
         return 0;
      s1++;
      s2++;
   }
   return -1;
}

int vstricmp(const char *s1, const char *s2)
{
	while (1)
	{
		if (vtolower(*s1) != vtolower(*s2))
			return (vtolower(*s1)) - (vtolower(*s2));
		if (!*s1)
			return 0;
		s1++;
		s2++;
	}
	return -1;
}

void vmemset(void *dest, int fill, int count)
{
   int i;

   if ((((long) dest | count) & 3) == 0)
   {
      count >>= 2;
      fill = fill | (fill<<8) | (fill<<16) | (fill<<24);
      for (i=0; i<count; i++)
         ((int *) dest)[i] = fill;
   }
   else
      for (i=0; i<count; i++)
         ((char *) dest)[i] = fill;
}

void vmemcpy(void *dest, const void *src, int count)
{
   int i;

   if ((((long) dest | (long) src | count) & 3) == 0)
   {
      count>>=2;
      for (i=0; i<count; i++)
         ((int *) dest)[i] = ((int *) src)[i];
   }
   else
      for (i=0; i<count; i++)
         ((char *) dest)[i] = ((char *) src)[i];
}

int vmemcmp(const void *m1, const void *m2, int count)
{
   while (count)
   {
      count--;
      if (((char *) m1)[count] != ((char *) m2)[count])
         return -1;
   }
   return 0;
}

void vstrcpy(char *dest, const char *src)
{
   while (*src)
      *dest++ = *src++;
   *dest++ = 0;
}

void vstrncpy(char *dest, const char *src, int n)
{
   n--;
   while (*src && n)
   {
      *dest++ = *src++;
      n--;
   }
   *dest++ = 0;
}

int vstrlen(char *s)
{
   int count=0;

   while (s[count])
      count++;
   return count;
}

void vkillCRLF(char *s)
{
	while (*s)
	{
		if (*s == 10 || *s == 13)
		{ *s = 0; return; }
		s++;
	}
}

int rnd(int lo, int hi)
{
  int range=hi-lo+1;
  int i=rand() % range;
  return i+lo;
}