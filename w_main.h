extern char Foreground;
#ifdef _INC_WINDOWS
extern HWND hMainWnd;
extern HINSTANCE hMainInst;
#endif

void StartupMenu();
void StartNewGame(char*);
void LoadGame(char *fn);

extern void ReadMouse(void);
extern void HandleMessages(void);
extern void vExit(char *str);


