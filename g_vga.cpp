/********************************************************** 
   g_vga.cpp
   winverge
   Copyright (C) 2000 Benjamin Eirich (vecna)
   All Rights Reserved
 **********************************************************/

#include "ddraw.h"
#include "pcp.h"

/************************** data **************************/

byte *scr;
byte *scr_surf;
const int scr_size = 320*200*8;
const int scr_w	   = 320;
const int scr_h	   = 200;

byte pal[768], pal2[768];
PALETTEENTRY ddpal[256];

int x1=17, y1=17;

/*****************************************************************/

LPDIRECTDRAW dd;
LPDIRECTDRAWSURFACE ps, bs;
DDSURFACEDESC psd, bsd;
LPDIRECTDRAWPALETTE dp;

/************************** code **************************/

/* Base graphics library stuff */

void CloseVGA()
{
	dp -> Release();
	bs -> Release();
	ps -> Release();
	dd -> Release();
	ShowCursor(1);
}

void InitVGA()
{
	HRESULT hr;
	
	vmemset(ddpal,0,sizeof(PALETTEENTRY)*256);
	ShowCursor(0);
	hr = DirectDrawCreate(NULL,&dd,NULL);
	hr = dd -> SetCooperativeLevel(hMainWnd,DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
	dd -> SetDisplayMode(320, 200, 8);
	hr = dd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256,ddpal,&dp,NULL);
	dp->SetEntries(0,0,256,ddpal);	
	vmemset(&psd, 0, sizeof (DDSURFACEDESC));
	vmemset(&bsd, 0, sizeof (DDSURFACEDESC));
	psd.dwSize = sizeof (DDSURFACEDESC);
	psd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	psd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX | DDSCAPS_SYSTEMMEMORY;
	psd.dwBackBufferCount = 1;
	dd -> CreateSurface(&psd, &ps, 0);
	ps->SetPalette(dp);
	bsd.dwSize = sizeof (DDSURFACEDESC);
	bsd.dwFlags = DDSD_CAPS;
	bsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	ps -> GetAttachedSurface(&bsd.ddsCaps, &bs);
	scr = new byte[scr_size];
	memset(scr,0,scr_size);
	atexit(CloseVGA);
}

void vgadump()
{
	int i;

	//lock surface to get memory pointer
	bs->Lock(NULL,&bsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL);
	scr_surf=(byte*)bsd.lpSurface;
	
	//fill surface memory with system mem framebuffer
	for(i=0;i<scr_h;i++)
		memcpy(scr_surf+(bsd.lPitch*i),scr+(scr_w*i),scr_w);

	bs->Unlock(NULL);	
	ps->Flip(0,DDFLIP_NOVSYNC);
}

/* Palette-handling routines */

void ConvPal(byte *p)
{
	// FIXME: 63 * 4 is 252, not 255. Everything displayed is a bit
	// darker than it ought to be. Create a correct xlat table.
	for (int i=0; i<256; i++)
	{
		ddpal[i].peRed   = p[(i*3)+0] * 4;
		ddpal[i].peGreen = p[(i*3)+1] * 4;
		ddpal[i].peBlue  = p[(i*3)+2] * 4;
	}
}

void SetPalette(byte *p)
{ 
	ConvPal(p);
	dp->SetEntries(0,0,256,ddpal);
}

void GetPalette()
{
	for (int i=0; i<256; i++)
	{
		pal[(i*3)+0] = ddpal[i].peRed / 4;
		pal[(i*3)+1] = ddpal[i].peGreen / 4;
		pal[(i*3)+2] = ddpal[i].peBlue / 4;
	}	
}

void SetIntensity(int n)
{
	int i;
	if (n>63) n = 63;

	for (i=767; i>=0; --i)
      pal2[i] = (pal[i] * n) / 63;
    SetPalette(pal2);
}

void fin()
{
	int i;

  if (!fade) return;
  if (cancelfade) { cancelfade--; return; }

	timer = 0;
	while (true)
	{
		i = (timer * 64) / 30;
		SetIntensity(i);
		if (timer<30) continue;
		break;
	}
	SetIntensity(63);
}

void fout()
{
	int i;

	if (!fade) return;
	if (cancelfade) { cancelfade--; return; }

	timer = 0;
	while (true)
	{
		i = (timer * 64) / 30;
		i = 64 - i;
		SetIntensity(i);
		if (timer<30) continue;
		break;
	}
	SetIntensity(0);
}


/* Graphics primitives */

#define cx1 0
#define cx2 319
#define cy1 0
#define cy2 199

void SetPixel(int x, int y, byte c)
{
	if (!Foreground) return;
	x -= 16; y -= 16;
	if (x<cx1 || x>cx2 || y<cy1 || y>cy2)
		return;
	scr[(y * scr_w) + x] = c;
}

void VLine(int x, int y, int y2, byte c)
{
	for (int i=0; i<(y2-y); i++)
		SetPixel(x,(y+i),c);
}

void HLine(int x, int y, int x2, byte c)
{
	byte *d = scr;
	
	if (!Foreground) return;
	x -= 16; y -= 16; x2 -= 16;
	if (x2<x) SWAP(x,x2);
	if (x>cx2 || y>cy2 || x2<cx1 || y<cy1)
		return;

	if (x2>cx2) x2=cx2;
	if (x<cx1)  x =cx1;

	d += (y * scr_w) + x;
	for (; x<=x2; x++)
		*d++ = c;
}

void Box(int x, int y, int x2, int y2, byte color)
{
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);

	for (int i=0; i<=(y2-y); i++)
		{ HLine(x,(y+i),x2+1,color); }
}

void CopyTile(int x, int y, byte *spr)
{
	byte *s=(byte *) spr,
		 *d=(byte *) scr;
	int spitch=16,
		dpitch=scr_w;
	int xlen = 16, ylen = 16;

	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+16<cx1 || y+16<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y) * spitch;
		ylen -= (cy1-y);
		y = cy1;
	}

	d += (y * dpitch) + x;
	for (; ylen--; s += spitch, d+= dpitch)
    	memcpy(d, s, xlen);
}

void TCopyTile(int x, int y, byte *spr)
{
	byte *s=(byte *) spr,
		 *d=(byte *) scr, c;
	int spitch=16,
		dpitch=scr_w;
	int xlen = 16, ylen = 16;

	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+16<cx1 || y+16<cy1)
		return;

	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y) * spitch;
		ylen -= (cy1-y);
		y = cy1;
	}
	d += (y * dpitch) + x;
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c = s[x];
			if (c) d[x] = c;
		}
		s += spitch;
		d += dpitch;
	}
}

void CopySprite(int x, int y, int w, int h, byte *src)
{
	byte *s=(byte *) src,
		 *d=(byte *) scr;
	int spitch=w,
		dpitch=scr_w;
	int xlen=w,
		ylen=h;
	
	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;
	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y)*spitch;
		ylen -= (cy1-y);
		y = cy1;
	}

	d += (y*dpitch)+x;
	for (; ylen--; s+=spitch,d+=dpitch)
    	memcpy(d, s, xlen);
}

void TCopySprite(int x, int y, int w, int h, byte *src)
{
	byte *s=(byte *) src,
		 *d=(byte *) scr, c;
	int spitch=w,
		dpitch=scr_w;
	int xlen=w,
		ylen=h;
	
	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;
	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y)*spitch;
		ylen -= (cy1-y);
		y = cy1;
	}

	d += (y * dpitch) + x;	
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			c = s[x];
			if (c) d[x] = c;
		}
		s += spitch;
		d += dpitch;
	}
}

/* Tranparency stuff */

byte vergepal[768];          // VERGE main palette
byte menuxlatbl[256];        // Menu transparencyfield (blue)
byte greyxlatbl[256];        // Grey transparencyfield
byte scrnxlatbl[256];        // screen transparencyfield
byte *transparencytbl;       // full transparency table (64k)

void ColorScale(byte *dest,int st,int fn,int inv)
{
	int i, intensity;

	for (i=0; i<256; i++)
	{
		intensity=vergepal[(i*3)];
		intensity+=vergepal[(i*3)+1];
		intensity+=vergepal[(i*3)+2];
		intensity=intensity*(fn-st)/192;
		if (inv) dest[i]=fn-intensity;
		else dest[i]=st+intensity;
	}
}

void PreCalc_TransparencyFields()
{
	FILE *f;

	// First read the VERGE palette from verge.pal

	f=fopen("VERGE.PAL","rb");
	if (!f) err("Egad! Where's your VERGE.PAL, pal?");
	fread(vergepal, 1, 768, f);
	fclose(f);
	SetPalette(vergepal);
	GetPalette();
	transparencytbl = new byte[65536];

	// Precompute some common translation tables.

	ColorScale(menuxlatbl, 141, 159, 1);
	ColorScale(greyxlatbl, 0, 31, 0);

	// Load in the 64k bitmap-on-bitmap transparency table (precomputed)

	f=fopen("TRANS.TBL","rb");
	fread(transparencytbl, 1, 65535, f);
	fclose(f);
}

void ColorFieldLine(int x, int y, int x2, byte *c)
{
	byte *d = scr;
	
	if (!Foreground) return;
	x -= 16; y -= 16; x2 -= 16;
	if (x2<x) SWAP(x,x2);
	if (x>cx2 || y>cy2 || x2<cx1 || y<cy1)
		return;

	if (x2>cx2) x2=cx2;
	if (x<cx1)  x =cx1;

	d += (y * scr_w) + x;
	for (; x<=x2; x++)
		*d++ = c[*d];
}

void ColorField(int x, int y, int x2, int y2, byte *colortbl)
{
	if (x2<x) SWAP(x,x2);
	if (y2<y) SWAP(y,y2);

	for (int i=0; i<=(y2-y); i++)
		ColorFieldLine(x,(y+i),x2+1,colortbl);
}

void Tcopysprite(int x, int y, int w, int h, byte *src)
{
	byte *s=(byte *) src,
		 *d=(byte *) scr, cs, cd;
	int spitch=w,
		dpitch=scr_w;
	int xlen=w,
		ylen=h;
	
	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;
	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y)*spitch;
		ylen -= (cy1-y);
		y = cy1;
	}

	d += (y * dpitch) + x;	
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			cs = s[x]; cd = d[x];
			if (cs) d[x] = transparencytbl[(cs*256)+cd];
		}
		s += spitch;
		d += dpitch;
	}
}

void _Tcopysprite(int x, int y, int w, int h, byte *src)
{
	byte *s=(byte *) src,
		 *d=(byte *) scr, cs, cd;
	int spitch=w,
		dpitch=scr_w;
	int xlen=w,
		ylen=h;
	
	if(!Foreground) return;
	x -= 16; y -= 16;
	if (x>cx2 || y>cy2 || x+xlen<cx1 || y+ylen<cy1)
		return;
	if (x+xlen > cx2) xlen=cx2-x+1;
	if (y+ylen > cy2) ylen=cy2-y+1;
	if (x<cx1)
	{
		s += (cx1-x);
		xlen -= (cx1-x);
		x = cx1;
	}
	if (y<cy1)
	{
		s += (cy1-y)*spitch;
		ylen -= (cy1-y);
		y = cy1;
	}

	d += (y * dpitch) + x;	
	for (; ylen; ylen--)
	{
		for (x=0; x<xlen; x++)
		{
			cs = s[x]; cd = d[x];
			if (cs) d[x] = transparencytbl[(cd*256)+cs];
		}
		s += spitch;
		d += dpitch;
	}
}

/* Font routines */

byte TextColor=31;
byte *fnt, *fnt2, *tbox;

void LoadFont()
{
	FILE *f;

	fnt = new byte[6000];
	fnt2 = new byte[14000];
	tbox = new byte[30000];

	/* Load small font */
	if (!(f = fopen("SMALL.FNT","rb")))
		err("FATAL ERROR: Could not open SMALL.FNT");
	fread(fnt, 63, 95, f);
	fclose(f);

	/* Load large text font */
	if (!(f = fopen("MAIN.FNT","rb")))
		err("FATAL ERROR: Could not open MAIN.FNT");
	fread(fnt2, 144, 95, f);
	fclose(f);

	/* Load text box */
	if (!(f = fopen("BOX.RAW","rb")))
		err("FATAL ERROR: Could not open BOX.RAW");
	fread(tbox, 320, 66, f);
	fclose(f);
}

void pchar(int x, int y, char c)
{
	byte *img;

	if (c<32 || c>126) return;
	img = fnt + ((c - 32) * 63);
	if (c==103 || c==106 || c==112 || c==113 || c==121)
		TCopySprite(x, y+2, 7, 9, img);
	else
		TCopySprite(x, y, 7, 9, img);
}

void VCpchar(int x, int y, char c)
{ 
	byte *img;

	if ((c<32) || (c>126)) return;
	img=fnt+((c-32)*63);
	if ((c==103) || (c==106) || (c==112) || (c==113) || (c==121))
		VCtcopysprite(x,y+2,7,9,img);
	else
		VCtcopysprite(x,y,7,9,img);
}

void bigpchar(int x, int y, char c)
{
	byte *img;

	if ((c<32) || (c>126)) return;
	img = fnt2 + ((c - 32) * 144);
	if ((c==103) || (c==106) || (c==112) || (c==113) || (c==121))
		TCopySprite(x, y+2, 9, 16, img);
	else
		TCopySprite(x, y, 9, 16, img);
}

void gotoxy(int x, int y)
{
	x1=x;
	y1=y;
}

void printstring(char *str)
{
	int i = 0;
	char c;

	if (!str[0]) return;
	while (1)
	{
		c = str[i];
		pchar(x1, y1, c);
		x1 += 8;
		i++;
		if (!str[i]) return;
	}
}

void VCprintstring(int xx, int yy, char *str)
{
	int i;
	char c;

	i=0;
	if (!str[0]) return;
mainloop:
	c=str[i];
	VCpchar(xx,yy,c);
	xx += 8;
	i++;
	if (str[i]!=0) goto mainloop;
}


void bigprintstring(char *str)
{
	int i=0;
	char c;

	if (!str[0]) return;
	while (1)
	{
		c = str[i];
		bigpchar(x1,y1,c);
		x1 += 10;
		i++;
		if (!str[i]) return;
	}
}

void putbox()
{
  ColorField(18, 151, 334, 213, menuxlatbl);
  TCopySprite(16, 149, 320, 66, tbox);
}

void dec_to_asciiz(int num, char *buf)
{
	sprintf(buf, "%d", num);
}

void textwindow(char portrait, char *str1, char *str2, char *str3)
{
	TCopySprite(20,114,32,32,speech+(portrait*1024));
	putbox();
	gotoxy(25,155);
	bigprintstring(str1);
	gotoxy(25,174);
	bigprintstring(str2);
	gotoxy(25,193);
	bigprintstring(str3);
}

void fontcolor(byte c)
{
	byte *ptr = fnt;
	for (int i=0; i<5985; i++)
    {
		if (*ptr == TextColor)
			*ptr = c;
        ptr++;
	}
  TextColor = c;
}
