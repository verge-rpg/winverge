// ****************************************************************
// * PCP                                                          *
// * Copyright (C)1999 Dark Nova Software (DNS)                   *
// * All Rights Reserved.                                         *
// *                                                              *
// * File: a_err.c                                                *
// * Author: vecna                                                *
// * Portability: All systems                                     *
// * Description: Error reporting / Logfile routines              *
// *                                                              *
// * Note: The memory for each node in the Shutdown linked list   *
// *       is allocated with qvalloc. It will not show up on      *
// *       memory allocation lists. If there seems to be bugs     *
// *       related to the shutdown routines, it might be a good   *
// *       idea to use valloc() with PARANOID and check for chunk *
// *       corruption.                                            *
// ****************************************************************

#include "pcp.h"
#define LOGFILE "verge.log"

// ***************************** Data *****************************

typedef struct shutdownType
{
   char name[50];
   void (*Funcptr) (void);
   struct shutdownType *next;
} ShutdownType;

struct shutdownType *shutdown_top;
FILE *logf;

// ***************************** Code *****************************

void InitErrorSystem(void)
{
   remove(LOGFILE);
   shutdown_top = 0;
}

void AddShutdownProc(void (*FuncPtr) (void), const char *name)
{
   ShutdownType *n;

   n = (ShutdownType *) malloc(sizeof(ShutdownType));
   vstrncpy(n -> name, name, 50);
   n -> Funcptr = FuncPtr;
   n -> next = shutdown_top;
   shutdown_top = n;
}

void err_Shutdown()
{
   ShutdownType *n;

   n = shutdown_top;
   while (n)
   {
      n -> Funcptr();
      n = n -> next;
   }
}

void err(const char *s, ...)
{
  va_list argptr;
  char msg[256];

  va_start(argptr,s);
  vsprintf(msg,s,argptr);
  va_end(argptr);

  err_Shutdown();
  vExit(msg);
}

void Log(const char *text, ...)
{
   va_list argptr;
   char msg[256];

   va_start(argptr, text);
   vsprintf(msg, text, argptr);
   va_end(argptr);

   logf=fopen(LOGFILE,"aw");
   fprintf(logf,"%s\n",msg);
   fflush(logf);
   fclose(logf);
}

void Logp(char *text, ...)
{
   va_list argptr;
   char msg[256];

   va_start(argptr, text);
   vsprintf(msg, text, argptr);
   va_end(argptr);

   logf=fopen(LOGFILE,"aw");
   fprintf(logf,"%s",msg);
   fflush(logf);
   fclose(logf);
}

void LogDone(void)
{
   logf=fopen(LOGFILE,"aw");
   fprintf(logf," ... OK \n");
   fflush(logf);
   fclose(logf);
}

char *va(char* format, ...)
{
    va_list argptr;
    static char string[1024];

    va_start(argptr, format);
    vsprintf(string, format, argptr);
    va_end(argptr);

	return string;
}
