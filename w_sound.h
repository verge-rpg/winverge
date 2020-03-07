
void SuspendMusic();
void ResumeMusic();


void InitSound();
void PlayMusic(char *sng, bool cache);
void StopMusic(void);
int  CacheSample(char *si);
void FreeAllSamples(void);
int  PlaySample(int i, int v, int p);

int  s_getglobalvolume();
int  s_getmusicvolume();
int  s_getsndfcvolume();
void s_setglobalvolume(int);
void s_setmusicvolume(int);
void s_setsndfxvolume(int);

void UpdateSound(void);

//extern bool music;
//extern char playingsng[80];