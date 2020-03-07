/**********************************
 * v_dx8bpp.cpp                   *
 * DirectX 16bpp fullscreen       *
 * Author: zero!!                 *
 **********************************/

#include "ddraw.h"
#include "pcp.h"

// ***************************** Data *****************************

byte pal[768];
PALETTEENTRY ddpal[256];
byte *scr, tcol;
byte tmask;

/*****************************************************************/

LPDIRECTDRAW dd;
LPDIRECTDRAWSURFACE ps, bs;
DDSURFACEDESC psd, bsd;
LPDIRECTDRAWPALETTE dp;

// ***************************** Code *****************************

void dx8_CloseDriver(void)
{
	dp -> Release();
	bs -> Release();
	ps -> Release();
	dd -> Release();
	ShowCursor(1);
}

int dx8_abs(int a)
{
	if(a<0) return -a; else return a;
}

int dx8_MakeColor(int r, int g, int b)
{
	int i;

	for(i=0;i<256;i++)
	{
		if(ddpal[i].peRed==r&&ddpal[i].peGreen==g&&ddpal[i].peBlue==b) return i;
	}
	Log("didnt match this color: r:%d g:%d b:%d",r,g,b);
	return 0;
}

image *dx8_ImageFrom8bpp(unsigned char *src, int width, int height, unsigned char *pal)
{
	return 0;
}

void dx8_PutPixel(int x, int y, int color, image *dest)
{
	byte *ptr=(byte *)dest->data;

	if(!Foreground) return;

	if (x<dest->cx1 || x>dest->cx2 || y<dest->cy1 || y>dest->cy2)
		return;

	ptr[(y * dest->pitch) + x]=(byte)color;
}

void dx8_BlitTile(int x, int y, char *src, image *dest)
{
	byte *s=(byte *) src,
		 *d=(byte *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if(!Foreground) return;

	get_clip(&cx1, &cy1, &cx2, &cy2, dest);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;
	for (; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}


void dx8_TBlitTile(int x, int y, char *src, image *dest)
{
	byte *s=(byte *) src,
		 *d=(byte *) dest->data;
	int spitch=16,
		dpitch=dest->pitch;
	int xlen=16,
		ylen=16;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;
	byte c;

	if(!Foreground) return;

	get_clip(&cx1, &cy1, &cx2, &cy2, dest);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y=cy1;
	}
	d+=y*dpitch+x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (!c) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dx8_Blit(int x, int y, image *src, image *dest)
{
	byte *s=(byte *) src->data,
		 *d=(byte *) dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if(!Foreground) return;

	get_clip(&cx1, &cy1, &cx2, &cy2, dest);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}

	d+=(y*dest->pitch)+x;
	for (; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}

void dx8_TBlit(int x, int y, image *src, image *dest)
{
	byte *s=(byte *) src->data,c,
		 *d=(byte *) dest->data;
	int spitch=src->pitch,
		dpitch=dest->pitch;
	int xlen=src->width,
		ylen=src->height;
	int cx1=0, cy1=0,
		cx2=0, cy2=0;

	if(!Foreground) return;

	get_clip(&cx1, &cy1, &cx2, &cy2, dest);
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;

	if (x<cx1) {
		s +=(cx1-x);
		xlen-=(cx1-x);
		x  =cx1;
	}
	if (y<cy1) {
		s +=(cy1-y)*spitch;
		ylen-=(cy1-y);
		y  =cy1;
	}
	d+=y*dpitch+x;	
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c=s[x];
			if (!c) d[x]=c;
		}
		s+=spitch;
		d+=dpitch;
	}
}

void dx8_Clear(int color, image *dest)
{
	byte *d=(byte *)dest->data;
	int bytes=dest->pitch*dest->height;

	if(!Foreground) return;

	while (bytes--)
		*d++ = color;
}

// adapted from David Brackeen's useful '256-Color VGA Programming in C' website
// at http://www.brackeen.com/home/vga/. something i can nearly understand. :-)
// - aen

void dx8_Line(int x, int y, int xe, int ye, int color, image *dest)
{
	int dx=xe-x,	dy=ye-y,
		xg=sgn(dx),	yg=sgn(dy),
		i=0;
	float slope=0;

	if(!Foreground) return;

	// x major
	if (abs(dx)>=abs(dy))
	{
		slope=(float)dy/(float)dx;
		for (i=0; i!=dx; i+=xg)
			PutPixel(x+i, y+(int)(slope*i), color, dest);
	}
	// y major
	else
	{
		slope=(float)dx/(float)dy;
		for (i=0; i!=dy; i+=yg)
			PutPixel(x+(int)(slope*i), y+i, color, dest);
	}
	// endpoint (algo omits)
	dx8_PutPixel(xe, ye, color, dest);
}

void dx8_HLine(int x, int y, int xe, int color, image *dest)
{
	byte *d = (byte *)dest->data;
	int cx1=0, cy1=0, cx2=0, cy2=0;

	if(!Foreground) return;

	if (xe<x) SWAP(x,xe);

	get_clip(&cx1, &cy1, &cx2, &cy2, dest);
	if (x>cx2 || y>cy2 || xe<cx1 || y<cy1)
		return;

	if (xe>cx2) xe=cx2;
	if (x<cx1)  x =cx1;

	d+=(y*dest->pitch)+x;
	for (; x<=xe; x++)
		*d++ = color;
}

void dx8_Box(int x, int y, int x2, int y2, int color, image *dest)
{
        if (y2<y) SWAP(y,y2);

		if(!Foreground) return;

		if (flag_PrimitiveFill)
			for (; y<=y2; y++)
				HLine(x, y, x2, color, dest);
		else
		{
			HLine(x, y, x2, color, dest);
			HLine(x, y2, x2, color, dest);
			Line(x, y, x, y2, color, dest);
			Line(x2, y, x2, y2, color,dest);
		}
}

void dx8_ShowPage(void)
{
	HRESULT hr;
	/* If pitch can change each flip, I'm in trouble. */
	hr=ps->Flip(0,DDFLIP_NOVSYNC);
	if(hr!=DD_OK)
	{
		if(hr==DDERR_SURFACELOST)
		{
			hr=ps->Restore();
			if(hr!=DD_OK)
			{
				Foreground=0;
				return;
			}
		}
	}
	if(hr!=DD_OK)
	{
		Foreground=0;
		return;
	}
	bs->Lock(0, &bsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	if(hr!=DD_OK)
	{
		if(hr==DDERR_SURFACELOST)
		{
			hr=bs->Restore();
			if(hr!=DD_OK)
			{
				Foreground=0;
				return;
			}
		}
	}
	if(hr!=DD_OK)
	{
		Foreground=0;
		return;
	}
	Foreground=1;
	screen->data = (void *) bsd.lpSurface;
	scr = (byte *) screen->data;
	bs->Unlock(0);
}

void dx8_SetLucent(int percent)
{
	return;
}

int dx8_InitDriver(int xres, int yres)
{
	HRESULT hr;
	//DDPIXELFORMAT ddpf;
	int i=0;

	cycaption=0;
	cyfixedframe=0;
	cxfixedframe=0;
	cxscreen=0;
	cyscreen=0;
	
	memset(ddpal,0,sizeof(PALETTEENTRY)*256);

	ShowCursor(0);
	hr = DirectDrawCreate(NULL,&dd,NULL);
	hr = dd -> SetCooperativeLevel(hMainWnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	dd -> SetDisplayMode(xres, yres, 8);
	hr = dd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256,ddpal,&dp,NULL);
	dp->SetEntries(0,0,256,ddpal);
	if (!dd) return 0;

	vmemset(&psd, 0, sizeof (DDSURFACEDESC));
	vmemset(&bsd, 0, sizeof (DDSURFACEDESC));
	psd.dwSize = sizeof (DDSURFACEDESC);
	psd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	psd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
	psd.dwBackBufferCount = 1;
	dd -> CreateSurface(&psd, &ps, 0);
	ps->SetPalette(dp);
	bsd.dwSize = sizeof (DDSURFACEDESC);
	bsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	ps -> GetAttachedSurface(&bsd.ddsCaps, &bs);
	bs -> Lock(0, &bsd, DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR, 0);
	scr = (byte *) bsd.lpSurface;
	
	screen->width = xres;
	screen->height = yres;
	screen->cx1 = 0;
	screen->cy1 = 0;
	screen->cx2 = xres - 1;
	screen->cy2 = yres - 1;
	screen->data = scr;

	screen->pitch = bsd.lPitch;
	bs -> Unlock(0);

	InitDriver		= dx8_InitDriver;
	ShutdownDriver	= dx8_CloseDriver;
	MakeColor		= dx8_MakeColor;
	ShowPage		= dx8_ShowPage;
	Blit			= dx8_Blit;
	TBlit           = dx8_TBlit;
//	BlitTile        = dx8_BlitTile;
//	TBlitTile       = dx8_TBlitTile;
	Clear			= dx8_Clear;
	PutPixel		= dx8_PutPixel;
	Line			= dx8_Line;
	HLine           = dx8_HLine;
	Box				= dx8_Box;
	SetLucent       = dx8_SetLucent;
	tmask           = 0;
	bpp             = 8;

	return 1;
}
