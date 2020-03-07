#define VID_NONE      0
#define VID_8BPP_DX   1

struct image
{
	int width, height, pitch;
	int cx1, cy1, cx2, cy2;
	void *data;
}; 

extern int sgn(int a);
extern void get_clip(int *x1, int *y1, int *x2, int *y2, image *im);
extern int flag_PrimitiveFill, numtiles;
extern void SetFill(int flag);
extern int cycaption,cxfixedframe,cyfixedframe,cxscreen,cyscreen;
extern bool has_dx4;
extern image *screen;

extern int InitVideo(int driver, int xres, int yres);

extern int    (*InitDriver) (int xres, int yres);
extern void   (*ShutdownDriver) (void);
extern int    (*MakeColor) (int r, int g, int b);
extern void   (*ShowPage) (void);
extern void   (*Blit) (int x, int y, image *src, image *dest);
extern void   (*TBlit) (int x, int y, image *src, image *dest);
extern void   (*Clear) (int color, image *dest);
extern void   (*PutPixel) (int x, int y, int color, image *dest);
extern void   (*Line) (int x, int y, int xe, int ye, int color, image *dest);
extern void   (*HLine) (int x, int y, int xe, int color, image *dest);
extern void   (*Box) (int x, int y, int xe, int ye, int color, image *dest);
extern void   (*SetLucent) (int percent);

#ifdef __DDRAW_INCLUDED__
extern LPDIRECTDRAW dd;
extern LPDIRECTDRAWSURFACE ps, bs;
extern DDSURFACEDESC psd, bsd;
extern LPDIRECTDRAWPALETTE dp;
#endif

extern char *DriverName;
extern char *DriverDesc;
extern char *DriverAuthor;
extern char *DriverDate;

void FreeImage(image *f);
image *NewImage(int width, int height);
void SetClip(int x1, int y1, int x2, int y2, image *f);

extern char current_driver, bpp, bppMultiplier;
extern int alpha, ialpha, windowed;