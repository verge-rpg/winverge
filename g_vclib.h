extern byte storeinv[12];
extern char fade,cancelfade,stringbuffer[100],keepaz;
void VCtcopysprite(int, int ,int ,int, byte*);
void ExecLibFunc(byte func);
int ReadVar0(int var);
void WriteVar0(int var,int value);
int ReadVar1(int var, int arg1);
void WriteVar1(int var, int arg1, int value);
int ReadVar2(int var, int arg1, int arg2);
void WriteVar2(int var, int arg1, int arg2, int value);
void VCClear();