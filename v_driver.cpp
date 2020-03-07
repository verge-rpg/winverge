/**********************************
 * v_driver.cpp                   *
 * Video Subsystem                *
 * Manages function pointers and  *
 * client video drivers.          *
 * Author: vecna                  *
 **********************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ddraw.h"
#include "pcp.h"

#define VID_NONE      0
#define VID_8BPP_DX   1

int cycaption,cxfixedframe,cyfixedframe,cxscreen,cyscreen;

/* video driver functions */

int sgn(int a)
{
	if (a<0)
		return -1;
	else if (a>0)
		return +1;
	
	return 0;
}

void get_clip(int *x1, int *y1, int *x2, int *y2, image *im)
{
	if (!im)
	{
		*x1=*y1=*x2=*y2=0;
		return;
	}

	*x1=im->cx1;
	*y1=im->cy1;
	*x2=im->cx2;
	*y2=im->cy2;
}

int flag_PrimitiveFill=0;
int windowed=0;

void SetFill(int flag) { flag_PrimitiveFill=flag; }

int    (*InitDriver) (int xres, int yres);
void   (*ShutdownDriver) (void);
int    (*MakeColor) (int r, int g, int b);
void   (*ShowPage) (void);
void   (*Blit) (int x, int y, image *src, image *dest);
void   (*TBlit) (int x, int y, image *src, image *dest);
void   (*Clear) (int color, image *dest);
void   (*PutPixel) (int x, int y, int color, image *dest);
void   (*Line) (int x, int y, int xe, int ye, int color, image *dest);
void   (*HLine) (int x, int y, int xe, int color, image *dest);
void   (*Box) (int x, int y, int xe, int ye, int color, image *dest);
void   (*SetLucent) (int percent);

image *screen=0;

/* v_driver data */

char *DriverName;
char *DriverDesc;
char *DriverAuthor;
char *DriverDate;

char current_driver=0, bpp=0, bppMultiplier=0;
int alpha=0, ialpha=100;
int numtiles;

LPDIRECTDRAW dd;
LPDIRECTDRAWSURFACE ps, bs;
DDSURFACEDESC psd, bsd;
LPDIRECTDRAWPALETTE dp;

/* v_driver code */

extern int dx8_InitDriver(int xres, int yres);

void vShutdownDriver()
{
	ShutdownDriver();
}

bool has_dx4 = false;

int InitVideo(int driver, int xres, int yres)
{
	int c;
	static int first=1;
	
	screen = (image *) malloc(sizeof (image));
	switch (driver)
	{
		case VID_NONE: err("vid_none"); 
		case VID_8BPP_DX: c = dx8_InitDriver(xres, yres); windowed=0; break;
		default: return 0;
	}
	if (first) atexit(vShutdownDriver); //, "Video subsystem");
	first=0;
	return c;
}

void FreeImage(image *f)
{
	delete [] f -> data;
	delete f;
}

image *NewImage(int width, int height)
{
	image *b;

	b = new image;
	b -> width = b -> pitch = width;
	b -> height = height;
	b -> cx1 = 0;
	b -> cy1 = 0;
	b -> cx2 = width - 1;
	b -> cy2 = height - 1;
	switch (bpp)
	{
		case 15:
		case 16: b -> data = new word[width * height];
				 break;
		case 32: b -> data = new quad[width * height];
				 break;
	}
	return b;
}

void SetClip(int x1, int y1, int x2, int y2, image *f)
{
	f -> cx1 = x1 >= 0 ? x1 : 0;
	f -> cy1 = y1 >= 0 ? y1 : 0;
	f -> cx2 = x2 < f->width ? x2 : f->width-1;
	f -> cy2 = y2 < f->height ? y2 : f->height-1;
}