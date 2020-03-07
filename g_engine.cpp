/********************************************************** 
   g_engine.cpp
   winverge
   Copyright (C) 2000 Benjamin Eirich (vecna)
   All Rights Reserved
 **********************************************************/

#include "pcp.h"

// ============================ data ============================

// Useflag values:   0 for nonusable.
//                   1 for usable once, then it goes away.
//                   2 for usable an infinite number of times.
//                   3 for usable ONLY camped, only once.
//                   4 for usable ONLY camped, infinitely
//                   5 for usable in combat only, once, then it goes away.
//                   6 for usable in combat only, indefinetly.

// Itemtype values:  0 for OS. (Own Self)
//                   1 for 1A. (One Ally)
//                   2 for AA. (All Allies)
//                   3 for 1E. (One Enemy)
//                   4 for AE. (All Enemy)

char lastmoves[6];                     // the party leader's last six moves
char partyidx[5];                      // current character-ref indexes
char numchars=0;                       // number of characters to draw
struct vspanim va0[100];               // tile animation data
struct r_entity party[101];            // party entities
struct zoneinfo zone[128];             // entity records
struct charstats pstats[30];           // party-stats record
struct itemdesc items[255];            // item definition array

struct equipstruc equip[255];          // equipment definition array
char zonedelay,lz;                     // zone delay counter
char tchars;                           // total characters in PARTY.DAT
int gp;                                // party's gold (or meseta, s, $, etc)

word vadelay[100];					   // delay counter

word *map0=NULL;					   // map data pointers
word *map1=NULL;        
byte *mapp=NULL;

byte *vsp0,*chrs;					   // graphic data pointers
byte *itemicons,*chr2;				   // more graphic ptrs
quad flags[8000];					   // misc flags

FILE *map,*vsp;                        // file handles
char mapname[13], musname[13];         // map/music filenames
char vsp0name[13], levelname[30];      // vsp filename / level name
char layerc,showname,saveflag;         // control flags

int xwin, ywin;                        // window coordinates
int xtc,ytc,xofs,yofs;                 // variables for drawmap()
word xsize,ysize;					   // x/y map dimensions
word startx,starty;					   // default start location
byte pmultx,pdivx;					   // parallax speed X
byte pmulty,pdivy;					   // parallax speed X
char warp, hide;                       // map warpable / hidable flags

char dontshowname=0,usenxy=0,qabort=0,autoent=0;
word nx=0,ny=0,numtiles;
char strbuf[1024];
byte menuptr[256],itmptr[576],charptr[960], *speech;

extern byte *msbuf, nummovescripts;
extern int msofstbl[100];

// ============================ code ============================

void allocbuffers()
{
	chrs = new byte[512000];
	chr2 = new byte[46080];
	itemicons = new byte[50688];
	speech = new byte[450000];
	msbuf = new byte[20000];
}

void addcharacter(int i)
{
	FILE *chrf, *p;
	int b;
        
	numchars++;
	partyidx[numchars-1]=i;
	p = fopen("PARTY.DAT","r");
	if (!p)
		err("Fatal error: PARTY.DAT not found");
	fscanf(p,"%s",strbuf);
	tchars = atoi(strbuf);
	if (i>atoi(strbuf))
		err("addcharacter(): index out of range");
	for (b=1;b<i;b++)
    {
		fscanf(p,"%s",strbuf);
        fscanf(p,"%s",strbuf);
        fscanf(p,"%s",strbuf);
	}
	fscanf(p,"%s",strbuf);

	party[0].chrindex=0;

	party[1].x=party[0].x;
	party[1].y=party[0].y;
	party[1].cx=party[0].cx;
	party[1].cy=party[0].cy;
	party[1].facing=0;
	party[1].moving=0;
	party[1].chrindex=1;

	party[2].x=party[0].x;
	party[2].y=party[0].y;
	party[2].cx=party[0].cx;
	party[2].cy=party[0].cy;
	party[2].facing=0;
	party[2].moving=0;
	party[2].chrindex=2;

	party[3].x=party[0].x;
	party[3].y=party[0].y;
	party[3].cx=party[0].cx;
	party[3].cy=party[0].cy;
	party[3].facing=0;
	party[3].moving=0;
	party[3].chrindex=3;

	party[4].x=party[0].x;
	party[4].y=party[0].y;
	party[4].cx=party[0].cx;
	party[4].cy=party[0].cy;
	party[4].facing=0;
	party[4].moving=0;
	party[4].chrindex=4;

	lastmoves[0]=0; lastmoves[1]=0; lastmoves[2]=0;
	lastmoves[3]=0; lastmoves[4]=0; lastmoves[5]=0;

	chrf=fopen(pstats[i-1].chrfile,"rb");
	if (!chrf) err("addcharacter(): CHR file not found");
	fread(chrs+((numchars-1)*15360), 1, 15360, chrf);
	fclose(chrf);

	fscanf(p,"%s",strbuf);
	fclose(p);
	chrf=fopen(strbuf,"rb");
	if (!chrf) err("addcharacter(): CR2 file not found");
	fread(chr2+((numchars-1)*9216), 1, 9216, chrf);
	fclose(chrf);
}

void LoadCHRList()
{
	int i;
	FILE *f;

	for (i=0; i<20; i++)
		if (strlen(chrlist[i]))
		{
			f = fopen(chrlist[i],"rb");
			fread(chrs+((i+5)*15360),30,512,f);
			fclose(f);
		}
}

void load_map(const char *fname)
{
	byte b;
	int i;

	memcpy(mapname, fname, 13);

	map = fopen(fname,"rb");
	if (!map)
		err("Could not open specified MAP file %s.", fname);

	fread(&b, 1, 1, map);
	if (b != 4)
		err("*error* Incorrect MAP version.");

	fread(vsp0name, 1, 13, map);
	fread(musname, 1, 13, map);

	fread(&layerc, 1, 1, map);
	fread(&pmultx, 1, 1, map); pmulty=pmultx;
	fread(&pdivx, 1, 1, map); pdivy=pdivx;

	fread(levelname, 1, 30, map);

	fread(&showname, 1, 1, map);
	fread(&saveflag, 1, 1, map);

	fread(&startx, 1, 2, map);
	fread(&starty, 1, 2, map);

	fread(&hide, 1, 1, map);
	fread(&warp, 1, 1, map);

	fread(&xsize, 1, 2, map);
	fread(&ysize, 1, 2, map);

	fread(&b, 1, 1, map);
	if (b)
		err("*error* MAP compression not yet supported.");

	fread(scr, 1, 27, map); // skip buffer

	if (map0) { delete[] map0; map0 = 0; }
	if (map1) { delete[] map1; map1 = 0; }
	if (mapp) { delete[] mapp; mapp = 0; }
  
	// allocate exact amount of mem needed
	map0 = new word[xsize * ysize];
	map1 = new word[xsize * ysize];
	mapp = new byte[xsize * ysize];

	fread(map0, xsize, ysize*2, map); // read in background layer
	fread(map1, xsize, ysize*2, map); // read in foreground layer
	fread(mapp, xsize, ysize, map);   // read in zone info/obstruction layer

	fread(zone, 1, sizeof(zone), map);
	fread(chrlist, 13, 100, map);      // read in chr list

	LoadCHRList();

	fread(&entities, 1, 4, map);
	fread(&party[5], 88, entities, map);

	for (i=5; i<entities+5; i++)
	{
		party[i].cx = party[i].x;
		party[i].cy = party[i].y;
		party[i].x = party[i].x * 16;
		party[i].y = party[i].y * 16;
    }

	fread(&nummovescripts, 1, 1, map);
	fread(&i, 1, 4, map);
	fread(&msofstbl, 4, nummovescripts, map);
	fread(msbuf, 1, i, map);

	entities += 5;

	LoadVC(map);
	fclose(map);

	if (strlen(musname)) PlayMusic(musname, false);

	// load the .VSP file
	vsp = fopen(vsp0name,"rb");
	if (!vsp)
		err("Could not open specified VSP file.");
	fseek(vsp, 2, 0);
	fread(pal, 1, 768, vsp);
	fread(&numtiles, 1, 2, vsp);

	if (vsp0) { delete[] vsp0; vsp0 = 0; } 
	int vspm = numtiles << 8;
	vsp0 = new byte[vspm];

	fread(vsp0, 1, vspm, vsp);
	fread(&va0, 1, sizeof(va0), vsp);
	fclose(vsp);

	for (i=0; i<2048; i++)
		tileidx[i]=i;

	if (usenxy) { startx=nx; starty=ny; }

	party[0].x=startx*16;
	party[0].y=starty*16;
	party[0].moving=0;
	party[0].cx=startx;
	party[0].cy=starty;
	
	party[1].x=party[0].x;
	party[1].y=party[0].y;
	party[1].moving=0;
	party[1].cx=startx;
	party[1].cy=starty;
	
	party[2].x=party[0].x;
	party[2].y=party[0].y;
	party[2].moving=0;
	party[2].cx=startx;
	party[2].cy=starty;
	
	party[3].x=party[0].x;
	party[3].y=party[0].y;
	party[3].moving=0;
	party[3].cx=startx;
	party[3].cy=starty;

	party[4].x=party[0].x;
	party[4].y=party[0].y;
	party[4].moving=0;
	party[4].cx=startx;
	party[4].cy=starty;

	lastmoves[0]=0; lastmoves[1]=0; lastmoves[2]=0;
	lastmoves[3]=0; lastmoves[4]=0; lastmoves[5]=0;

	ExecuteScript(0);
}

void process_stepzone()
{
	byte cz, t1;

	cz = mapp[(((party[0].y/16)*xsize)+(party[0].x/16))] >> 1;
	if (lz!=cz) { zonedelay=0; lz=cz; }
	if (!zone[cz].percent) return;
	if (zonedelay<zone[cz].delay)
	{ zonedelay++; return; }

	t1=(rand()*255);
	if (t1<=zone[cz].percent)
	{
		ExecuteScript(zone[cz].callevent);
		zonedelay=0;
	}
}

void lastmove(char n)
{ 
	lastmoves[5]=lastmoves[4];
	lastmoves[4]=lastmoves[3];
	lastmoves[3]=lastmoves[2];
	lastmoves[2]=lastmoves[1];
	lastmoves[1]=lastmoves[0];
	lastmoves[0]=n;
}

void startfollow()
{
	char i;
  
	for (i=1; i<numchars; i++)
    {
		if (lastmoves[i]==1)
           { party[i].y++; party[i].facing=0; party[i].cy++;
             party[i].moving=1; party[i].movcnt=15; }
        if (lastmoves[i]==2)
           { party[i].y--; party[i].facing=1; party[i].cy--;
             party[i].moving=2; party[i].movcnt=15; }
        if (lastmoves[i]==3)
           { party[i].x++; party[i].facing=2; party[i].cx++;
             party[i].moving=3; party[i].movcnt=15; }
        if (lastmoves[i]==4)
           { party[i].x--; party[i].facing=3; party[i].cx--;
             party[i].moving=4; party[i].movcnt=15; }
	}
}

int InvFace()
{
	switch(party[0].facing)
	{
		case 0: return 1;
		case 1: return 0;
		case 2: return 3;
		case 3: return 2;
	}
	return -1;
}

void Activate()
{
	int tx,ty;
	byte cz,t;

	// First, determine what tile we're looking at.
	tx = party[0].x/16;
	ty = party[0].y/16;
	switch (party[0].facing)
	{
		case 0: { ty++; break; }
		case 1: { ty--; break; }
		case 2: { tx++; break; }
		case 3: { tx--; break; }
	}

	// Here is where we first check for entities in our way.

	t = EntityAt(tx,ty);
	if (t)
		if (!party[t].activmode && party[t].actscript)
		{
			if (party[t].face) party[t].facing=InvFace();
				ExecuteScript(party[t].actscript);
			return;
		}

	// Check adjacent zones.
	cz = mapp[((ty*xsize)+tx)] >> 1;
	if (zone[cz].aaa)
	{	
		ExecuteScript(zone[cz].callevent);
		return;
	}
}

int ObstructionAt(int tx,int ty)
{
  if ((mapp[((ty)*xsize)+tx] & 1)==1) return 1;
  return 0;
}

void process_entities()
{
	int i;

	for (i=5; i<entities; i++)
		ProcessSpeedAdjEntity(i);

	if (autoent)
		for (i=95; i<95+numchars; i++)
			ProcessSpeedAdjEntity(i);
}

void ProcessControls()
{
	if (party[0].speed<4)
	{
		switch (party[0].speed)
		{
			case 1: if (party[0].speedct<3) { party[0].speedct++; return; }
			case 2: if (party[0].speedct<2) { party[0].speedct++; return; }
			case 3: if (party[0].speedct<1) { party[0].speedct++; return; }
		}
	}
	if (party[0].speed<5) process_controls();
	switch (party[0].speed)
	{
		case 5: process_controls(); process_controls(); return;
		case 6: process_controls(); process_controls(); process_controls(); return;
		case 7: process_controls(); process_controls();
				process_controls(); process_controls(); return;
	}
}

void process_controls()
{
	byte i;
	char l,r,t,b;

	party[0].speedct = 0;
	if (!party[0].moving)
	{
		xtc = party[0].x/16; ytc=party[0].y/16;
		if ((mapp[((ytc+1)*xsize)+xtc] & 1)==1) b=0; else b=1;
		if ((mapp[((ytc-1)*xsize)+xtc] & 1)==1) t=0; else t=1;
		if ((mapp[(ytc*xsize)+((party[0].x+17)/16)] & 1)==1) r=0; else r=1;
		if ((mapp[(ytc*xsize)+((party[0].x-15)/16)] & 1)==1) l=0; else l=1;

		if (xtc==0) l = 0;
		if (ytc==0) t = 0;
		if (xtc==xsize-1) r = 0;
		if (ytc==ysize-1) b = 0;

		ReadControls();
		if (b1) Activate();
		if (b3) SystemMenu();
		if (b4) MainMenu();

//		for(i=0;i<128;i++)
//		{
//			if ((key_map[i].pressed) && (key_map[i].boundscript))
//				ExecuteStartUpScript(key_map[i].boundscript);
//	    }

		xtc = party[0].x/16; ytc=party[0].y/16;

		if (right) party[0].facing=2;
		if (down) party[0].facing=0;
		if (left) party[0].facing=3;
		if (up) party[0].facing=1;

		if ((right) && (r) && !EntityAt(xtc+1,ytc))
		{ party[0].x++; party[0].facing=2; party[0].moving=3; party[0].cx++;
		  party[0].movcnt=15; lastmove(3); startfollow(); return; }
		if ((down) && (b) && !EntityAt(xtc,ytc+1))
		{ party[0].y++; party[0].facing=0; party[0].moving=1; party[0].cy++;
             party[0].movcnt=15; lastmove(1); startfollow(); return; }
		if ((left) && (l) && !EntityAt(xtc-1,ytc))
		{ party[0].x--; party[0].facing=3; party[0].moving=4; party[0].cx--;
		  party[0].movcnt=15; lastmove(4); startfollow(); return; }
		if ((up) && (t) && !EntityAt(xtc,ytc-1))
		{ party[0].y--; party[0].facing=1; party[0].moving=2; party[0].cy--;
		  party[0].movcnt=15; lastmove(2); startfollow(); return; }

		party[0].framectr=0; party[1].framectr=0; party[2].framectr=0;
		party[3].framectr=0; party[4].framectr=0;
	}

	if (party[0].moving)
	{
		for (i=0; i<numchars; i++)
		{
			if (party[i].moving==1)
			{ party[i].y++; party[i].movcnt--; party[i].framectr++; }
			if (party[i].moving==2)
			{ party[i].y--; party[i].movcnt--; party[i].framectr++; }
			if (party[i].moving==3)
			{ party[i].x++; party[i].movcnt--; party[i].framectr++; }
			if (party[i].moving==4)
			{ party[i].x--; party[i].movcnt--; party[i].framectr++; }
			if (party[i].framectr==80) party[i].framectr=0;
		}
	if (!party[0].movcnt)
    { party[0].moving=0; process_stepzone(); }
	}
}

void check_tileanimation()
{
	byte i;

	for (i=0; i<100; i++)
    {
		if ((va0[i].delay) && (va0[i].delay<vadelay[i]))
			animate(i);
        vadelay[i]++;
	}
}

void UpdateEquipStats()
{
	int i,j,a;

	// This function takes the base stats of all characters and re-calculates
	// current-stats by re-applying equipment modifiers.

	for (i=0; i<30; i++)
	{
		pstats[i].atk = pstats[i].str;
		pstats[i].def = pstats[i].end;
		pstats[i].magc = pstats[i].mag;
		pstats[i].mgrc = pstats[i].mgr;
		pstats[i].hitc = pstats[i].hit;
		pstats[i].dodc = pstats[i].dod;
		pstats[i].mblc = pstats[i].mbl;
		pstats[i].ferc = pstats[i].fer;
		pstats[i].reac = pstats[i].rea;

		for (j=0; j<6; j++)
		{
			a = items[pstats[i].inv[j]].equipidx;
			pstats[i].atk += equip[a].str;
			pstats[i].def += equip[a].end;
			pstats[i].magc += equip[a].mag;
			pstats[i].mgrc += equip[a].mgr;
			pstats[i].hitc += equip[a].hit;
			pstats[i].dodc += equip[a].dod;
			pstats[i].mblc += equip[a].mbl;
			pstats[i].ferc += equip[a].fer;
			pstats[i].reac += equip[a].rea;
		}
	}
}

void game_ai()
{
	ProcessControls();
	process_entities();
//	if (key[SCAN_CTRL] && speed) ProcessControls();
	if (key[SCAN_CTRL]) ProcessControls();
	check_tileanimation();
}

void CreateSaveImage(byte *buf)
{
	memcpy(buf,chrs,512);
	if (numchars>1) memcpy(buf+512,chrs+15360,512);
		else memset(buf+512,0,512);
	if (numchars>2) memcpy(buf+1024,chrs+30720,512);
        else memset(buf+1024,0,512);
	if (numchars>3) memcpy(buf+1536,chrs+46080,512);
        else memset(buf+1536,0,512);
	if (numchars>4) memcpy(buf+2048,chrs+61440,512);
        else memset(buf+2048,0,512);
}

void SaveGame(const char *fn)
{
	FILE *f;
	byte cz, temp[2560];

	f = fopen(fn,"wb");
	cz = mapp[(((party[0].y/16)*xsize)+(party[0].x/16))] >> 1;
	fwrite(&zone[cz].savedesc, 1, 30, f);
	cz = partyidx[0]-1;
	fwrite(&pstats[cz].name, 1, 9, f);
	fwrite(&pstats[cz].lv, 1, 4, f);
	fwrite(&pstats[cz].curhp, 1, 8, f);
	fwrite(&gp, 1, 4, f);
	fwrite(&hr, 1, 1, f);
	fwrite(&min, 1, 1, f);
	fwrite(&sec, 1, 1, f);
	fwrite(&numchars, 1, 1, f);
	fwrite(&menuactive, 1, 1, f);
	CreateSaveImage(temp);
	fwrite(&temp, 1, 2560, f);
	fwrite(&mapname, 1, 13, f);
	fwrite(&party, 1, sizeof party, f);
	fwrite(&partyidx, 1, 5, f);
	fwrite(&flags, 1, 32000, f);
	fwrite(&tchars, 1, 1, f);
	fwrite(&pstats, 1, sizeof pstats, f);
	fclose(f);
}

void startmap(const char *fname)
{
	if (qabort) return;

	SetIntensity(0);
	load_map(fname);
	drawmap();
	vgadump();
	fin();
	
	timer=0; lz=0;
	tickctr=0; zonedelay=0;

main_loop:
	while (timer)
    {
		timer--;
        game_ai();
	}

	drawmap();
	vgadump();

	while (!timer);
	if (!qabort) goto main_loop;
}

/************** Entity code stuff **************/

// ============================ Data ============================

char chrlist[100][13];                // CHR list
int entities;                         // number of active entities
byte nummovescripts=0;       // number of movement scripts
int msofstbl[100];                    // movement script offset table
byte *msbuf;                          // movement script data buffer
char movesuccess;                     // if call to MoveXX() was successful

// ============================ Code ============================

int EntityAt(int ex, int ey)
{
	int i;

	for (i=5; i<entities; i++)
		if (ex==party[i].cx && ey==party[i].cy) 
			return i;

	return 0;
}

int AnyEntityAt(int ex,int ey)
{
	int i, st;

	if (autoent) st=5; else st=0;
	for (i=st; i<entities; i++)
		if (ex==party[i].cx && ey==party[i].cy) return i+1;

	return 0;
}

void MoveRight(int i)
{
	int tx,ty;

	tx=party[i].cx+1; ty=party[i].cy;
	if (!party[i].obsmode && (ObstructionAt(tx,ty) || (i>4 && AnyEntityAt(tx,ty))))
    { movesuccess=0; return; }
	party[i].x++; party[i].facing=2; party[i].moving=3; party[i].movcnt=15;
	party[i].cx++; movesuccess=1;
}

void MoveLeft(int i)
{
	int tx, ty;

	tx=party[i].cx-1; ty=party[i].cy;
	if (!party[i].obsmode && (ObstructionAt(tx,ty) || (i>4 && AnyEntityAt(tx,ty))))
    { movesuccess=0; return; }
	party[i].x--; party[i].facing=3; party[i].moving=4; party[i].movcnt=15;
	party[i].cx--; movesuccess=1;
}

void MoveUp(int i)
{ 
	int tx, ty;

	tx=party[i].cx; ty=party[i].cy-1;
	if (!party[i].obsmode && (ObstructionAt(tx,ty) || (i>4 && AnyEntityAt(tx,ty))))
    { movesuccess=0; return; }
	party[i].y--; party[i].facing=1; party[i].moving=2; party[i].movcnt=15;
	party[i].cy--; movesuccess=1;
}

void MoveDown(int i)
{
	int tx, ty;

	tx=party[i].cx; ty=party[i].cy+1;
	if (!party[i].obsmode && (ObstructionAt(tx,ty) || (i>4 && AnyEntityAt(tx,ty))))
    { movesuccess=0; return; }
	party[i].y++; party[i].facing=0; party[i].moving=1; party[i].movcnt=15;
	party[i].cy++; movesuccess=1;
}

int Zone(int cx, int cy)
{
	return mapp[((cy*xsize)+cx)] >> 1;
}

void ProcessSpeedAdjEntity(int i)
{
	if (party[i].speed<4)
	{
		switch (party[i].speed)
		{
			case 1: if (party[i].speedct<3) { party[i].speedct++; return; }
			case 2: if (party[i].speedct<2) { party[i].speedct++; return; }
			case 3: if (party[i].speedct<1) { party[i].speedct++; return; }
		}
	}
	if (party[i].speed<5) ProcessEntity(i);
	switch (party[i].speed)
	{
		case 5: ProcessEntity(i); ProcessEntity(i); return;
		case 6: ProcessEntity(i); ProcessEntity(i); ProcessEntity(i); return;
		case 7: ProcessEntity(i); ProcessEntity(i);
				ProcessEntity(i); ProcessEntity(i); return;
	}
}

void Wander1(int i)
{
	if (!party[i].data1)
	{
		party[i].data2=random(0,3);
		party[i].data1=party[i].step+1;
	}
	if (party[i].data1==1)
	{
	    party[i].delayct++;
		if (party[i].delayct>=party[i].delay)
			party[i].data1=0;
		return;
	}
	if (party[i].data1>1)
	{
		switch(party[i].data2)
		{
			case 0: MoveUp(i); break;
			case 1: MoveDown(i); break;
			case 2: MoveLeft(i); break;
			case 3: MoveRight(i); break;
		}
		party[i].data1--;
		if (party[i].data1==1)
			party[i].delayct=0;
	}
}

void Wander2(int i)
{
	if (!party[i].data1)
	{
		party[i].data2=random(0,3);
		party[i].data1=party[i].step+1;
	}
	if (party[i].data1==1)
	{
		party[i].delayct++;
		if (party[i].delayct>=party[i].delay)
			party[i].data1=0;
		return;
	}
	if (party[i].data1>1)
	{
		switch(party[i].data2)
		{
			case 0: if (party[i].cy>party[i].y1) MoveUp(i); break;
			case 1: if (party[i].cy<party[i].y2) MoveDown(i); break;
			case 2: if (party[i].cx>party[i].x1) MoveLeft(i); break;
			case 3: if (party[i].cx<party[i].x2) MoveRight(i); break;
		}
		party[i].data1--;
		if (party[i].data1==1) 
			party[i].delayct=0;
	}
}

void Wander3(int i)
{
	if (!party[i].data1)
	{
		party[i].data2=random(0,3);
		party[i].data1=party[i].step+1;
	}
	if (party[i].data1==1)
	{
		party[i].delayct++;
		if (party[i].delayct>=party[i].delay)
			party[i].data1=0;
		return;
	}
	if (party[i].data1>1)
	{
		switch(party[i].data2)
		{
			case 0: if (Zone(party[i].cx,party[i].cy-1)==party[i].data3) MoveUp(i); break;
			case 1: if (Zone(party[i].cx,party[i].cy+1)==party[i].data3) MoveDown(i); break;
			case 2: if (Zone(party[i].cx-1,party[i].cy)==party[i].data3) MoveLeft(i); break;
			case 3: if (Zone(party[i].cx+1,party[i].cy)==party[i].data3) MoveRight(i); break;
		}
		party[i].data1--;
		if (party[i].data1==1) 
			party[i].delayct=0;
	}
}

void Whitespace(int i)
{
	while (*party[i].scriptofs==' ')
		party[i].scriptofs++;
}

void GetArg(int i)
{
	char token[10];

	int j=0;
	Whitespace(i);
	while (*party[i].scriptofs>=48 && *party[i].scriptofs<=57)
	{
		token[j] = *party[i].scriptofs;
		party[i].scriptofs++;
		j++;
	}
	token[j] = 0;
	party[i].cmdarg = atoi(token);
}

void GetNextCommand(int i)
{
	byte s;

	Whitespace(i);
	s = *party[i].scriptofs;
	party[i].scriptofs++;
	switch (s)
	{
		case 'U': party[i].curcmd=1; GetArg(i); break;
		case 'D': party[i].curcmd=2; GetArg(i); break;
		case 'L': party[i].curcmd=3; GetArg(i); break;
		case 'R': party[i].curcmd=4; GetArg(i); break;
		case 'S': party[i].curcmd=5; GetArg(i); break;
		case 'W': party[i].curcmd=6; GetArg(i); break;
		case 0:   party[i].movecode=0; party[i].curcmd=7;
			      party[i].cmdarg=0; party[i].scriptofs=0; break;
		case 'C': party[i].curcmd=8; GetArg(i); break;
		case 'B': party[i].curcmd=9; break;
		case 'X': party[i].curcmd=10; GetArg(i); break;
		case 'Y': party[i].curcmd=11; GetArg(i); break;
		case 'F': party[i].curcmd=12; GetArg(i); break;
		case 'Z': party[i].curcmd=13; GetArg(i); break;
		default: err("Invalid entity movement script.");
	}
}

void MoveScript(int i)
{
	if (!party[i].scriptofs) 
		party[i].scriptofs=msbuf+msofstbl[party[i].movescript];
	if (!party[i].curcmd) 
		GetNextCommand(i);

	switch(party[i].curcmd)
	{
		case 1: MoveUp(i); if (movesuccess) party[i].cmdarg--; break;
		case 2: MoveDown(i); if (movesuccess) party[i].cmdarg--; break;
		case 3: MoveLeft(i); if (movesuccess) party[i].cmdarg--; break;
		case 4: MoveRight(i); if (movesuccess) party[i].cmdarg--; break;
		case 5: party[i].speed=party[i].cmdarg; party[i].cmdarg=0; break;
		case 6: party[i].cmdarg--; break;
		case 7: return;
		case 8: ExecuteScript(party[i].cmdarg); party[i].cmdarg=0; break;
		case 9: party[i].scriptofs=msbuf+msofstbl[party[i].movescript];
				party[i].cmdarg=0; break;
		case 10: if (party[i].cx<party[i].cmdarg) MoveRight(i);
				 if (party[i].cx>party[i].cmdarg) MoveLeft(i);
				 if (party[i].cx==party[i].cmdarg) party[i].cmdarg=0; break;
				 break;
	    case 11: if (party[i].cy<party[i].cmdarg) MoveDown(i);
				 if (party[i].cy>party[i].cmdarg) MoveUp(i);
				 if (party[i].cy==party[i].cmdarg) party[i].cmdarg=0; break;
				 break;
		case 12: party[i].facing=party[i].cmdarg;
				 party[i].cmdarg=0; break;
		case 13: party[i].specframe=party[i].cmdarg;
				 party[i].cmdarg=0; break;
	}
	if (!party[i].cmdarg)
		party[i].curcmd=0;
}

void TestActive(int i)
{
	int dx, dy;

	dx = abs(party[i].x-party[0].x);
	dy = abs(party[i].y-party[0].y);
	if ((dx<=16 && dy<=3) || (dx<=3 && dy<=16))
	{
		if (!party[i].adjactv)
			ExecuteHookedScript(party[i].actscript);
		party[i].adjactv=1;
	}
	else party[i].adjactv=0;
}

void Chase(int i)
{
	int dx, dy, d;

	dx = party[0].cx-party[i].cx;
	dy = party[0].cy-party[i].cy;

	if (abs(dx) < abs(dy)) d=0; else d=1;

	if (d && dx<0) MoveLeft(i);
	if (d && dx>0) MoveRight(i);
	if (!d && dy<0) MoveUp(i);
	if (!d && dy>0) MoveDown(i);
}

void CheckChasing(int i)
{
	if (abs(party[0].cx-party[i].cx) <= party[i].chasedist &&
		abs(party[0].cy-party[i].cy) <= party[i].chasedist)
		{ party[i].chasing=2; party[i].movecode=5;
		  party[i].speed=party[i].chasespeed; }
}

void ProcessEntity(int i)
{
	party[i].speedct=0;
	if (party[i].activmode) TestActive(i);
	if (party[i].chasing==1) CheckChasing(i);

	if (!party[i].moving)
	{
		switch(party[i].movecode)
		{
			case 0: return;
			case 1: Wander1(i); break;
			case 2: Wander2(i); break;
			case 3: Wander3(i); break;
			case 4: MoveScript(i); break;
			case 5: Chase(i); break;
			default: err("*error* unknown entity movement pattern");
		}
	}

	if (party[i].moving)
	{
		if (party[i].moving==1)
		{ party[i].y++; party[i].movcnt--; party[i].framectr++; }
		if (party[i].moving==2)
        { party[i].y--; party[i].movcnt--; party[i].framectr++; }
		if (party[i].moving==3)
        { party[i].x++; party[i].movcnt--; party[i].framectr++; }
		if (party[i].moving==4)
        { party[i].x--; party[i].movcnt--; party[i].framectr++; }
		if (party[i].framectr==80)
			party[i].framectr=0;
		if (!party[i].movcnt) 
			party[i].moving=0;
	}
}