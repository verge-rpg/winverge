extern word width,depth;
extern word bytes, i;
extern byte c, run, ss;
extern quad vidoffset, n;
extern FILE *pcxf;

void ReadPCXLine(byte *dest);
void LoadPCXHeader(char *fname);
void LoadPCXHeaderNP(char *fname);
void loadpcx(char *fname, byte *dest);
void WritePCXLine(unsigned char *p);
void WritePalette();
void ScreenShot();
