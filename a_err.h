#ifndef ERR_H
#define ERR_H

void InitErrorSystem(void);
void AddShutdownProc(void (*FuncPtr) (void), char *name);
void err_Shutdown(void);
void err(char *s, ...);
void Log(char *text, ...);
void Logp(char *text, ...);
void LogDone(void);
char *va(char* format, ...);

#endif
