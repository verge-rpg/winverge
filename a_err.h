#ifndef ERR_H
#define ERR_H

void InitErrorSystem(void);
void AddShutdownProc(void (*FuncPtr) (void), const char *name);
void err_Shutdown(void);
void err(const char *s, ...);
void Log(const char *text, ...);
void Logp(const char *text, ...);
void LogDone(void);
char *va(const char* format, ...);

#endif
