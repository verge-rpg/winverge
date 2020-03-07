extern byte pal[768], *scr;
extern byte menuxlatbl[256];        // Menu transparencyfield (blue)
extern byte greyxlatbl[256];        // Grey transparencyfield
extern byte scrnxlatbl[256];        // screen transparencyfield
extern byte *transparencytbl;       // full transparency table (64k)

void CloseVGA();
void InitVGA();
void vgadump();

void ConvPal(byte *p);
void SetPalette(byte *p);
void GetPalette();
void fin();
void fout();

void SetIntensity(int n);
void SetPixel(int x, int y, byte c);
void VLine(int x, int y, int y2, byte c);
void HLine(int x, int y, int x2, byte c);
void Box(int x, int y, int x2, int y2, byte color);
void CopyTile(int x, int y, byte *spr);
void TCopyTile(int x, int y, byte *spr);
void CopySprite(int x, int y, int w, int h, byte *src);
void TCopySprite(int x, int y, int w, int h, byte *src);

void ColorScale(byte *dest,int st,int fn,int inv);
void PreCalc_TransparencyFields();
void ColorField(int x, int y, int x2, int y2, unsigned char *tbl);
void Tcopysprite(int x1, int y1, int width, int height, unsigned char *src);
void _Tcopysprite(int x1, int y1, int width, int height, unsigned char *src);

void LoadFont();
void pchar(int x, int y, char c);
void VCpchar(int x, int y, char c);
void bigpchar(int x, int y, char c);
void gotoxy(int x, int y);
void printstring(char *str);
void bigprintstring(char *str);
void VCprintstring(int xx, int yy, char *str);
void putbox();
void dec_to_asciiz(int num, char *buf);
void textwindow(char portrait, char *str1, char *str2, char *str3);
void fontcolor(byte c);