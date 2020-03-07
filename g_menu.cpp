/********************************************************** 
   g_menu.cpp
   winverge
   Copyright (C) 2000 Benjamin Eirich (vecna)
   All Rights Reserved
 **********************************************************/

#include "pcp.h"

#define bgcolor 154
#define grey1 14
#define grey2 26
#define white 31

char menuactive=1;
char whoisptr=0, iuflag;
byte gsimg[2048];

void border(int x, int y, int x2, int y2)
{
	VLine(x+1,y+1,y2-1,grey1);
	VLine(x+2,y+2,y2-2,grey2);
	VLine(x+3,y+2,y2-2,grey2);
	VLine(x+4,y+3,y2-3,grey1);

	VLine(x2-1,y+1,y2-1,grey1);
	VLine(x2-2,y+2,y2-2,grey2);
	VLine(x2-3,y+2,y2-2,grey2);
	VLine(x2-4,y+3,y2-3,grey1);

	HLine(x+1,y+1,x2-1,grey1);
	HLine(x+2,y+2,x2-2,grey2);
	HLine(x+4,y+3,x2-4,grey1);

	HLine(x+1,y2-1,x2,grey1);
	HLine(x+2,y2-2,x2-1,grey2);
	HLine(x+4,y2-3,x2-3,grey1);
}

void menubox(int x, int y, int x2, int y2)
{
	Box(x,y,x2,y2,bgcolor);
	border(x,y,x2,y2);
}

void tmenubox(int x, int y, int x2, int y2)
{
	ColorField(x,y,x2+1,y2+1,menuxlatbl);
	border(x,y,x2,y2);
}

void DrawPartyStats()
{ 
	int i,b;
	char c;
	// Draws a box on the right side of the screen that shows the current party
	// statistics.

	tmenubox(210,20,330,32+(numchars*30));

	for (i=0; i<numchars; i++)
    { 
		gotoxy(217,27+(i*30));             // print character name and level
        c=partyidx[i]-1;
        printstring(pstats[c].name);
        gotoxy(290,27+(i*30));
        switch (pstats[c].status)
        {
			case 0: printstring("LV");
				    dec_to_asciiz(pstats[c].lv,strbuf);
	                b=strlen(strbuf)*8; gotoxy(322-b,27+(i*30));
					printstring(strbuf);
					break;
			case 1: printstring("DEAD");
					break;
        }

        gotoxy(230,37+(i*30));             // print character curhp/maxhp
        printstring("HP ");
        dec_to_asciiz(pstats[c].curhp,strbuf);
        gotoxy(278-(strlen(strbuf)*8),37+(i*30));
        printstring(strbuf);
        gotoxy(278,37+(i*30));
        printstring("/");
        dec_to_asciiz(pstats[c].maxhp,strbuf);
        gotoxy(310-(strlen(strbuf)*8),37+(i*30));
        printstring(strbuf);

        gotoxy(230,47+(i*30));             // print character curmp/maxmp
        printstring("MP ");
        dec_to_asciiz(pstats[c].curmp,strbuf);
        gotoxy(278-(strlen(strbuf)*8),47+(i*30));
        printstring(strbuf);
        gotoxy(278,47+(i*30));
        printstring("/");
        dec_to_asciiz(pstats[c].maxmp,strbuf);
        gotoxy(310-(strlen(strbuf)*8),47+(i*30));
        printstring(strbuf);
	}
}

void DrawMainMenu(int ptr)
{
	tmenubox(20,20,120,95);
	gotoxy(40,27);
	printstring("Item");
	gotoxy(40,37);
	printstring("Equip");
	gotoxy(40,47);
	printstring("Magic");
	gotoxy(40,57);
	printstring("Status");
	gotoxy(40,67);
	printstring("Order");
	gotoxy(40,77);
	printstring("Tactic");
	TCopySprite(20,25+(ptr*10),16,16,menuptr);

	tmenubox(210,185,330,205);
	gotoxy(308,191); printstring("GP");
	dec_to_asciiz(gp,strbuf);
	gotoxy(300-(strlen(strbuf)*8),191);
	printstring(strbuf);
}

int WhoIs(int i)
{ 
	int j;
	char c,first=1,ec=0;

	if (numchars==1) return 1;
	playeffect(1);

drawloop:
	drawmap();
	DrawMainMenu(i);
	DrawPartyStats();
	tmenubox(20,110,120,122+(numchars*10));
	for (j=0; j<numchars; j++)
	{ 
		c=partyidx[j]-1;
        gotoxy(40,117+(j*10));
        printstring(pstats[c].name);
    }
	TCopySprite(20,115+(whoisptr*10),16,16,menuptr);
	vgadump();
	ReadControls();

	if (first==2) if (b1 || b2 || b4) goto drawloop;
		else return ec;
	if (first && !b1 && !b4 && !down && !up) first=0;
		else if (first) goto drawloop;

	if (down) 
	{ 
		whoisptr++;
        if (whoisptr==numchars) whoisptr=0;
        playeffect(0);
        first=1;
	}
	if (up)   
	{ 
		if (!whoisptr) whoisptr=numchars-1;
			else whoisptr--;
        playeffect(0);
        first=1;
    }

	if (b1) { ec=whoisptr+1; first=1; }
	while (!b2 && !b1) goto drawloop;
	while (b2 || b1) { first=2; goto drawloop; }
	return 0;
}

void ReOrder(int i)
{ 
	char cnt1=0, cnt2=0,mp=0, t1=0,c,j;
	char flash=0,first=1;

	playeffect(1);
drawloop:
	drawmap();
	DrawMainMenu(i);
	DrawPartyStats();
	tmenubox(20,110,120,123+(numchars*10));
	for (j=0; j<numchars; j++)
    { 
		c=partyidx[j]-1;
        gotoxy(40,117+(j*10));
        printstring(pstats[c].name);
    }
	TCopySprite(20,115+(mp*10),16,16,menuptr);
	if (t1 && flash)                           // flashing lastsel pointer
    { TCopySprite(20,115+(cnt1*10),16,16,menuptr); flash=0; }
	else flash++;
	vgadump();
	ReadControls();

	if (first==2) if (b1 || b2 || b4) goto drawloop;
		else return;
	if (first && !b1 && !b2 && !b4 && !down && !up) first=0;
		else if (first) goto drawloop;

	if (up)
    { 
		if (mp) mp--;
		else mp=numchars-1;
		if ((t1==1) && (mp==cnt1))
        { if (mp) mp--;
          else mp=numchars-1; }
		playeffect(0);
		first=1;
    }

	if (down)
    { 
		if (mp<(numchars-1)) mp++;
		else mp=0;
		if ((t1==1) && (mp==cnt1)) mp++;
		if (mp==numchars) mp=0;
		playeffect(0);
		first=1;
    }

	if (b1)
	{  
		t1++;
		playeffect(1);
		if (t1==1)
		{  
			cnt1=mp;
	        if (mp<(numchars-1)) mp++;
		    else mp=0;
	        first=1;
		}

		if (t1==2)
		{  
			cnt2=mp;
			t1=partyidx[cnt1];     // Here we actually flip the order of partyidx
			mp=partyidx[cnt2];
			partyidx[cnt1]=mp;
			partyidx[cnt2]=t1;
			mp=cnt2;
			t1=numchars;

			numchars=0;            // Here we reload CHRs
			for (cnt1=0; cnt1<t1; cnt1++)
				addcharacter(partyidx[cnt1]);
			return;
		}
	}
	while (!b2) goto drawloop;
	if (t1==1)
    { t1=0; cnt1=0; first=1; goto drawloop; }
	while (b2 || b1) { first=2; goto drawloop; }
}

void StatusScreen(char cz)
{ 
	char c, first=1;
	int i;

	cz--; c=partyidx[cz]-1;
	playeffect(1);
drawloop:
	drawmap();
	TCopySprite(20,20,96,96,chr2+(cz*9216));  // Status portrait
	tmenubox(120,20,220,40);                  // name box
	i=strlen(pstats[c].name)*4;
	gotoxy(170-i,26);
	printstring(pstats[c].name);
	tmenubox(230,20,330,40);
	gotoxy(256,26); printstring("STATUS");
	tmenubox(120,45,220,116);                 // HP/MP/Status box
	gotoxy(127,51); printstring("HP");        // Do HP right-shifted

	dec_to_asciiz(pstats[c].curhp,strbuf);
	gotoxy(183-(strlen(strbuf)*8),51);
	printstring(strbuf);
	gotoxy(183,51);
	printstring("/");
	dec_to_asciiz(pstats[c].maxhp,strbuf);
	gotoxy(215-(strlen(strbuf)*8),51);
	printstring(strbuf);

	gotoxy(127,61); printstring("MP");
	dec_to_asciiz(pstats[c].curmp,strbuf);
	gotoxy(183-(strlen(strbuf)*8),61);
	printstring(strbuf);
	gotoxy(183,61);
	printstring("/");
	dec_to_asciiz(pstats[c].maxmp,strbuf);
	gotoxy(215-(strlen(strbuf)*8),61);
	printstring(strbuf);
	
	gotoxy(127,100);
	switch (pstats[c].status)
	{ 
		case 0: printstring("GOOD"); break;
		case 1: printstring("DEAD"); break;
	}

	tmenubox(230,45,330,116);                 // Level/XP/Next box
	gotoxy(237,51); printstring("LV");
	dec_to_asciiz(pstats[c].lv,strbuf); i=strlen(strbuf)*8;
	gotoxy(320-i,51); printstring(strbuf);
	gotoxy(237,61); printstring("EXP");
	dec_to_asciiz(pstats[c].exp,strbuf); i=strlen(strbuf)*8;
	gotoxy(320-i,61); printstring(strbuf);
	gotoxy(237,71); printstring("NEXT");
	dec_to_asciiz((pstats[c].nxt-pstats[c].exp),strbuf);
	i=strlen(strbuf)*8; gotoxy(320-i,71); printstring(strbuf);

	tmenubox(180,120,330,200);               // Stats box
	gotoxy(187,129); printstring("STR"); dec_to_asciiz(pstats[c].str,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,129); printstring(strbuf);
	gotoxy(187,139); printstring("END"); dec_to_asciiz(pstats[c].end,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,139); printstring(strbuf);
	gotoxy(187,149); printstring("HIT"); dec_to_asciiz(pstats[c].hitc,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,149); printstring(strbuf);
	gotoxy(187,159); printstring("DOD"); dec_to_asciiz(pstats[c].dodc,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,159); printstring(strbuf);
	gotoxy(187,169); printstring("MAG"); dec_to_asciiz(pstats[c].magc,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,169); printstring(strbuf);
	gotoxy(187,179); printstring("MGR"); dec_to_asciiz(pstats[c].mgrc,strbuf); i=strlen(strbuf)*8; gotoxy(250-i,179); printstring(strbuf);
	
	gotoxy(260,129); printstring("REA"); dec_to_asciiz(pstats[c].reac,strbuf); i=strlen(strbuf)*8; gotoxy(320-i,129); printstring(strbuf);
	gotoxy(260,139); printstring("MBL"); dec_to_asciiz(pstats[c].mblc,strbuf); i=strlen(strbuf)*8; gotoxy(320-i,139); printstring(strbuf);
	gotoxy(260,149); printstring("FER"); dec_to_asciiz(pstats[c].ferc,strbuf); i=strlen(strbuf)*8; gotoxy(320-i,149); printstring(strbuf);
	gotoxy(260,169); printstring("ATK"); dec_to_asciiz(pstats[c].atk,strbuf); i=strlen(strbuf)*8; gotoxy(320-i,169); printstring(strbuf);
	gotoxy(260,179); printstring("DEF"); dec_to_asciiz(pstats[c].def,strbuf); i=strlen(strbuf)*8; gotoxy(320-i,179); printstring(strbuf);
	
	tmenubox(20,120,170,200);                // Equipment box
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b4 || b1) goto drawloop;
		else return;
	if (first && !right && !left && !b1) first=0;
	else if (first) goto drawloop;

	if (right)
    { 
		cz++;
		if (cz == numchars) cz=0;
		c=partyidx[cz]-1;
		playeffect(0);
		first=1;
    }

	if (left)
	{
		if (!cz) cz=numchars-1;
		else cz--;
		c=partyidx[cz]-1;
		playeffect(0);
		first=1;
    }

	while (!b4 && !b2 && !b1) goto drawloop;
	while (b4 || b2 || b1) { first=2; goto drawloop; }
}

void MainMenu()
{ 
	int first=1;
	int ptr=0;
	char c;

	if (!menuactive) return;
	playeffect(1);
	an=1;
	whoisptr=0;
drawloop:
	drawmap();
	DrawMainMenu(ptr);
	DrawPartyStats();
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b4) goto drawloop;
		else { an=0; timer=0; return; }
	if (first && !b1 && !b4 && !down && !up) first=0;
		else if (first) goto drawloop;

	if (down) 
	{ 
		ptr++;
        if (ptr==6) ptr=0;
        playeffect(0);
        first=1;
    }
	if (up)   
	{ 
		if (!ptr) ptr=5;
        else ptr--;
        playeffect(0);
        first=1;
    }
	if (b1)   
	{ 
		switch (ptr)
        {
			case 0: while (c=WhoIs(ptr))
                    { ItemMenu(c-1);
                      if (numchars==1) break; }
                    break;
            case 1: while (c=WhoIs(ptr))
                    { EquipMenu(c-1);
                      if (numchars==1) break; }
                    break;
            case 2: break;
            case 3: c=WhoIs(ptr);
                    if (!c) break;
                    StatusScreen(c);
                    break;
            case 4: if (numchars==1) { first=1;
                                       break; }
                    ReOrder(ptr); first=1; break;
            case 5: break;
        }
	}

	if (!b4 && !b2) goto drawloop;
	if (b4) { first=2; goto drawloop; }

	timer=0;
	an=0;
}

void Volume()
{ 
	int first=1;
	int mp_volume = s_getmusicvolume();

	playeffect(1);
	an=1;
drawloop:
	drawmap();
	tmenubox(96,96,256,136);
	Box(126,114,(126+mp_volume),118,32);
	vgadump();

	ReadControls();

	if (first==2) if (b2) goto drawloop;
		else { an=0; return; }
	if (first && !b1 && !b2 && !down && !up) first=0;
	else if (first) goto drawloop;

	if (right && mp_volume<101) { mp_volume++; s_setmusicvolume(mp_volume); }
	if (left && mp_volume) mp_volume--;

	if (!b2) goto drawloop;
	if (b2) { first=2; goto drawloop; } 
}

void SystemMenu()
{ 
	int first=1,ptr=0;

  	an=1;
	playeffect(1);
drawloop:
	if (qabort) return;
	drawmap();
	tmenubox(20,20,150,83);
	gotoxy(40,27);
	if (!saveflag) fontcolor(17);
	printstring("Save Game");
	if (!saveflag) fontcolor(31);
	gotoxy(40,37);
	printstring("Load Game");
	gotoxy(40,47);
	printstring("Erase Game");
	gotoxy(40,57);
	printstring("Sound volume");
	gotoxy(40,67);
	printstring("Exit");
	TCopySprite(20,25+(ptr*10),16,16,menuptr);
	vgadump();

	ReadControls();

	if (first==3) if (b2 || b3) goto drawloop;
		else { an=0; timer=0; return; }
	if (first==2) { fin(); first=1; }
	if (first && !b1 && !b3 && !down && !up) first=0;
	else if (first) goto drawloop;

	if (down) 
	{ 
		ptr++;
        if (ptr==5) ptr=0;
        playeffect(0);
        first=1;
    }
	if (up)   
	{ 
		if (!ptr) ptr=4;
			else ptr--;
        playeffect(0);
        first=1;
    }
	if (b1)   
	{ 
		an=0;
        switch (ptr)
        {
//			case 0: if (saveflag)
//                    { LoadSaveErase(1); first=2; break; }
//                    first=1; break;
//            case 1: LoadSaveErase(0); first=2; break;
//            case 2: LoadSaveErase(2); first=2; break;
            case 3: Volume(); break;
            case 4: qabort=1;
                    killvc=1;
                    while (b1) ReadControls();
                    timer=0;
                    return;
        }
        an=1;
        goto drawloop;
	}

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=3; goto drawloop; }
}

// Shop code

char bcs=0;

void PutBuySellBox(char p)
{
	tmenubox(20,20,116,48);
	gotoxy(40,26); printstring("Buy");
	gotoxy(40,36); printstring("Sell");
	TCopySprite(22,24+(p*10),16,16,menuptr);
}

void PutGPBox()
{
	tmenubox(20,50,116,68);
	gotoxy(96,56); printstring("GP");
	dec_to_asciiz(gp,strbuf);
	gotoxy(90-(strlen(strbuf)*8),56);
	printstring(strbuf);
}

void PutCharBox(char a,char b, char c, char d, char e, char p)
{ 
	char baseaddr;

	switch (numchars)
	{
		case 1: baseaddr=61; break;
		case 2: baseaddr=53; break;
		case 3: baseaddr=45; break;
		case 4: baseaddr=37; break;
		case 5: baseaddr=29; break;
	}

	tmenubox(20,70,116,115);
	if (!a) { greyscale(16,32,chrs,gsimg);
              TCopySprite(baseaddr,77,16,32,gsimg); }
	else TCopySprite(baseaddr,77,16,32,chrs);
	if (numchars>1)
	{
		if (!b) { greyscale(16,32,chrs+15360,gsimg);
                  TCopySprite(baseaddr+16,77,16,32,gsimg); }
		else TCopySprite(baseaddr+16,77,16,32,chrs+15360);
	}
	if (numchars>2)
    {
		if (!c) { greyscale(16,32,chrs+30720,gsimg);
                  TCopySprite(baseaddr+32,77,16,32,gsimg); }
        else TCopySprite(baseaddr+32,77,16,32,chrs+30720);
    }
    if (numchars>3)
    {
		if (!d) { greyscale(16,32,chrs+46080,gsimg);
                  TCopySprite(baseaddr+48,77,16,32,gsimg); }
        else TCopySprite(baseaddr+48,77,16,32,chrs+46080);
    }
    if (numchars>4)
	{
		if (!e) { greyscale(16,32,chrs+61440,gsimg);
                  TCopySprite(baseaddr+64,77,16,32,gsimg); }
		else TCopySprite(baseaddr+64,77,16,32,chrs+61440);
    }
    if (p)
    {
		p--;
		TCopySprite(baseaddr+(p*16)-5,73,24,40,charptr);
    }
}

void PutMessageBox(const char *str)
{
	tmenubox(118,20,330,38);
	gotoxy(224-(strlen(str)*4),26);
	printstring(str);
}

void PutItemName(const char *str)
{
	tmenubox(118,40,330,58);
	gotoxy(224-(strlen(str)*4),46);
	printstring(str);
}

void PutItemDesc(const char *str)
{
	tmenubox(118,60,330,78);
	gotoxy(224-(strlen(str)*4),66);
	printstring(str);
}

void PutEquipBox(char c)
{ 
	byte *img, j,a;

	tmenubox(118,80,330,115);
	for (j=0; j<6; j++)
	{
		a=pstats[c].inv[j];
		img=itemicons+(items[a].icon*256);
		TCopySprite(136+(j*32),90,16,16,img);
	}
}

void PutItemBox(char l)
{ 
	byte k, j, a, *img;

	tmenubox(118,117,330,210);
	for (k=0; k<3; k++)
		for (j=0; j<6; j++)
		{
			a=pstats[l].inv[((k+1)*6)+j];
			img=itemicons+(items[a].icon*256);
			TCopySprite(136+(j*32),130+(k*24),16,16,img);
		}
}

void SellMenu()
{ 
	int first=1,p=0;
	char carray[5];

	playeffect(1);
drawloop:
	carray[0]=0;carray[1]=0;carray[2]=0;carray[3]=0;carray[4]=0;carray[p]=1;
	drawmap();
	PutBuySellBox(1);
	PutGPBox();
	PutCharBox(carray[0],carray[1],carray[2],carray[3],carray[4],p+1);
	PutMessageBox("Sell whose item?");
	PutItemName("");
	PutItemDesc("");
	PutEquipBox(partyidx[p]-1);
	PutItemBox(partyidx[p]-1);
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else return;
	if (first && !b1 && !b3 && !right && !left) first=0;
	else if (first) goto drawloop;

	if (right)
	{
		p++;
		if (p == numchars) p=0;
		playeffect(0);
		first=1;
	}

	if (left)
	{
		if (!p) p=numchars-1;
		else p--;
		playeffect(0);
		first=1;
	}

	if (b1) SellCharItem(p);

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

void SellCharItem(char c)
{ 
	int first=1,ptr=0;
	char carray[5],mx=0,my=0,a,l;

	playeffect(1);
	l=partyidx[c]-1;
drawloop:
	memset(&carray, 0, 5);
	carray[c]=1;
	drawmap();
	PutBuySellBox(1);
	PutGPBox();
	PutCharBox(carray[0],carray[1],carray[2],carray[3],carray[4],c+1);
	if (items[pstats[l].inv[ptr]].price)
		{dec_to_asciiz(items[pstats[l].inv[ptr]].price/2,strbuf);a=strlen(strbuf);
	strbuf[a]=' ';strbuf[a+1]='G';strbuf[a+2]=0;
	PutMessageBox(strbuf);   }
	else PutMessageBox("");
	PutItemName((char*) items[pstats[l].inv[ptr]].name);
	PutItemDesc((char*) items[pstats[l].inv[ptr]].desc);
	PutEquipBox(partyidx[c]-1);
	PutItemBox(partyidx[c]-1);
	a=ptr/6;
	if (ptr<6) TCopySprite(132+(ptr*32),86,24,24,itmptr);
	else TCopySprite(132+((ptr-(a*6))*32),102+(a*24),24,24,itmptr);

	vgadump();
	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else return;
	if (first && !b1 && !b3 && !right && !left && !up && !down) first=0;
	else if (first) goto drawloop;

	if (down)  
	{ 
		if (my<3) my++;
		ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (up)     
	{ 
		if (my) my--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (right) 
	{ 
		if (mx<5) mx++;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
	}             
	if (left)  
	{ 
		if (mx) mx--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }

	if (b1)
	{
		if (items[pstats[l].inv[ptr]].price) ConfirmSell(c,ptr);
		else playeffect(12);
		first=1;
	}

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

void ConfirmSell(char c, char ptr)
{ 
	int first=1,p=0;
	char carray[5],a,l;

	playeffect(1);
	l=partyidx[c]-1;
drawloop:
	memset(&carray, 0, 5); carray[c]=1;
	drawmap();

	tmenubox(20,20,116,48);
	gotoxy(40,26); printstring("Sell");
	gotoxy(40,36); printstring("Cancel");
	TCopySprite(22,24+(p*10),16,16,menuptr);

	PutGPBox();
	PutCharBox(carray[0],carray[1],carray[2],carray[3],carray[4],c+1);
	if (items[pstats[l].inv[ptr]].price)
		PutMessageBox("Are you sure?");
	PutItemName((char*) items[pstats[l].inv[ptr]].name);
	PutItemDesc((char*) items[pstats[l].inv[ptr]].desc);
	PutEquipBox(partyidx[c]-1);
	PutItemBox(partyidx[c]-1);
	a=ptr/6;
	if (ptr<6) TCopySprite(132+(ptr*32),86,24,24,itmptr);
	else TCopySprite(132+((ptr-(a*6))*32),102+(a*24),24,24,itmptr);
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else return;
	if (first && !b1 && !b3 && !down && !up) first=0;
		else if (first) goto drawloop;

	if (down || up)
	{
		p=p^1;
		playeffect(0);
		first=1;
	}

	if (b1)
	{
		if (!p) 
		{	
			playeffect(13); gp+=items[pstats[l].inv[ptr]].price/2;
			if (ptr>5) RemoveItem(l,ptr);
				else pstats[l].inv[ptr]=0;
			UpdateEquipStats();
		}
		first=2;
	}

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

void PutStoreInv()
{ 
	byte k, j, *img;

	tmenubox(118,80,330,140);
	for (k=0; k<2; k++)
		for (j=0; j<6; j++)
		{
			img=itemicons+(items[storeinv[(k*6)+j]].icon*256);
			TCopySprite(136+(j*32),90+(k*24),16,16,img);
		}
}

void PutBuyCharBox(char ptr, char p)
{ 
	char carray[5],a;

	memset(&carray, 0, 5);
	a=storeinv[ptr];
	if (!a)
    { PutCharBox(0,0,0,0,0,0); return; }
	if (!items[a].equipflag)
    { PutCharBox(1,1,1,1,1,p); return; }

	// It's equipment, only colorize people that can equip the item.

	PutCharBox(equip[items[a].equipidx].equipable[partyidx[0]-1],
               equip[items[a].equipidx].equipable[partyidx[1]-1],
               equip[items[a].equipidx].equipable[partyidx[2]-1],
               equip[items[a].equipidx].equipable[partyidx[3]-1],
               equip[items[a].equipidx].equipable[partyidx[4]-1],p);
}

void BuyMenu(char p)
{ 
	int first=1,ptr=0;
	char mx=0,my=0,a;

	playeffect(1);
drawloop:
	drawmap();
	PutBuyCharBox(ptr,0);
	PutBuySellBox(0);
	PutGPBox();
	if (items[storeinv[ptr]].price)
	{ dec_to_asciiz(items[storeinv[ptr]].price,strbuf);a=strlen(strbuf);
      strbuf[a]=' ';strbuf[a+1]='G';strbuf[a+2]=0;
      PutMessageBox(strbuf);   }
	else PutMessageBox("");
	PutItemName((char*) items[storeinv[ptr]].name);
	PutItemDesc((char*) items[storeinv[ptr]].desc);
	PutStoreInv();
	a=ptr/6;
	TCopySprite(132+((ptr-(a*6))*32),86+(a*24),24,24,itmptr);
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else { bcs=0; return; }
	if (first && !b1 && !b3 && !right && !left && !up && !down) first=0;
	else if (first) goto drawloop;

	if (down) 
	{ 
		if (my<1) my++;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (up)
	{ 
		if (my) my--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (right) 
	{	if (mx<5) mx++;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (left)  
	{	
		if (mx) mx--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }

	if (b1)    
	{
		if (storeinv[ptr])
        {
			if (gp<items[storeinv[ptr]].price) playeffect(12);
				else BuyItem(ptr);
        }
        first=1;
    }

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

int atkp,defp,magp,mgrp,hitp,dodp,mblp,ferp,reap;

void CalcBuyEquipPreview(int a,int i)
{ 
	int c,d;

	d=items[i].equipflag-1;
	c=pstats[a].inv[d];
	pstats[a].inv[d]=0;
	UpdateEquipStats();
	i=items[i].equipidx;

	atkp=pstats[a].atk+equip[i].str;
	defp=pstats[a].def+equip[i].end;
	magp=pstats[a].magc+equip[i].mag;
	mgrp=pstats[a].mgrc+equip[i].mgr;
	hitp=pstats[a].hitc+equip[i].hit;
	dodp=pstats[a].dodc+equip[i].dod;
	mblp=pstats[a].mblc+equip[i].mbl;
	ferp=pstats[a].ferc+equip[i].fer;
	reap=pstats[a].reac+equip[i].rea;
	
	pstats[a].inv[d]=c;
	UpdateEquipStats();
}

void PutEquipPreview(char c, char ptr)
{ 
	char l;

	l=partyidx[c]-1;
	
	if (items[storeinv[ptr]].equipflag && equip[items[storeinv[ptr]].equipidx].equipable[l])
	{
		CalcBuyEquipPreview(l,storeinv[ptr]);
		tmenubox(20,117,116,210);
		gotoxy(26,124);
		printstring("ATK "); dec_to_asciiz(pstats[l].atk,strbuf); gotoxy(80-(strlen(strbuf)*8),124); printstring(strbuf); printstring(">");dec_to_asciiz(atkp,strbuf);
		if (pstats[l].atk<atkp) fontcolor(97); if (atkp<pstats[l].atk) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),124); printstring(strbuf); fontcolor(31); gotoxy(26,133);
		printstring("DEF "); dec_to_asciiz(pstats[l].def,strbuf); gotoxy(80-(strlen(strbuf)*8),133); printstring(strbuf); printstring(">");dec_to_asciiz(defp,strbuf);
		if (pstats[l].def<defp) fontcolor(97); if (defp<pstats[l].def) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),133); printstring(strbuf); fontcolor(31);gotoxy(26,142);
		printstring("HIT "); dec_to_asciiz(pstats[l].hitc,strbuf); gotoxy(80-(strlen(strbuf)*8),142); printstring(strbuf); printstring(">");dec_to_asciiz(hitp,strbuf);
		if (pstats[l].hitc<hitp) fontcolor(97); if (hitp<pstats[l].hitc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),142); printstring(strbuf); fontcolor(31);gotoxy(26,151);
		printstring("DOD "); dec_to_asciiz(pstats[l].dodc,strbuf); gotoxy(80-(strlen(strbuf)*8),151); printstring(strbuf); printstring(">");dec_to_asciiz(dodp,strbuf);
	     if (pstats[l].dodc<dodp) fontcolor(97); if (dodp<pstats[l].dodc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),151); printstring(strbuf); fontcolor(31);gotoxy(26,160);
		printstring("MAG "); dec_to_asciiz(pstats[l].magc,strbuf); gotoxy(80-(strlen(strbuf)*8),160); printstring(strbuf); printstring(">");dec_to_asciiz(magp,strbuf);
		if (pstats[l].magc<magp) fontcolor(97); if (magp<pstats[l].magc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),160); printstring(strbuf); fontcolor(31);gotoxy(26,169);
		printstring("MGR "); dec_to_asciiz(pstats[l].mgrc,strbuf); gotoxy(80-(strlen(strbuf)*8),169); printstring(strbuf); printstring(">");dec_to_asciiz(mgrp,strbuf);
		if (pstats[l].mgrc<mgrp) fontcolor(97); if (mgrp<pstats[l].mgrc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),169); printstring(strbuf); fontcolor(31);gotoxy(26,178);
		printstring("REA "); dec_to_asciiz(pstats[l].reac,strbuf); gotoxy(80-(strlen(strbuf)*8),178); printstring(strbuf); printstring(">"); dec_to_asciiz(reap,strbuf);
		if (pstats[l].reac<reap) fontcolor(97); if (reap<pstats[l].reac) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),178); printstring(strbuf); fontcolor(31);gotoxy(26,187);
		printstring("FER "); dec_to_asciiz(pstats[l].ferc,strbuf); gotoxy(80-(strlen(strbuf)*8),187); printstring(strbuf); printstring(">");dec_to_asciiz(ferp,strbuf);
		if (pstats[l].ferc<ferp) fontcolor(97); if (ferp<pstats[l].ferc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),187); printstring(strbuf); fontcolor(31);gotoxy(26,196);
		printstring("MBL "); dec_to_asciiz(pstats[l].mblc,strbuf); gotoxy(80-(strlen(strbuf)*8),196); printstring(strbuf); printstring(">");dec_to_asciiz(mblp,strbuf);
		if (pstats[l].mblc<mblp) fontcolor(97); if (mblp<pstats[l].mblc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),196); printstring(strbuf); fontcolor(31);
	}
}

void BuyItem(char ptr)
{ 
	int first=1,a,l;

	playeffect(1);
drawloop:
	drawmap();
	PutBuySellBox(0);
	PutGPBox();
	PutMessageBox("Buy for who?");
	PutItemName((const char*) items[storeinv[ptr]].name);
	PutItemDesc((const char*) items[storeinv[ptr]].desc);
	PutBuyCharBox(ptr,bcs+1);
	PutStoreInv();
	PutEquipPreview(bcs,ptr);
	a=ptr/6;
	TCopySprite(132+((ptr-(a*6))*32),86+(a*24),24,24,itmptr);
	vgadump();
	l=partyidx[bcs]-1;
	
	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else return;
	if (first && !b1 && !b3 && !right && !left) first=0;
		else if (first) goto drawloop;

	if (right)
	{
		bcs++;
		if (bcs == numchars) bcs=0;
		playeffect(0);
		first=1;
	}

	if (left)
	{
		if (!bcs) bcs=numchars-1;
		else bcs--;
		playeffect(0);
		first=1;
	}

	if (b1)
	{
		if (pstats[l].invcnt<24)
		{
			if (!items[storeinv[ptr]].equipflag ||
		        !equip[items[storeinv[ptr]].equipidx].equipable[l])
			{ PurchaseItem(l,storeinv[ptr]); first=2; }
			else { BuyOrBuyEquip(bcs,ptr); first=2; }
		} else
		{
			playeffect(12);
			first=1;
		}
	}

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

void PurchaseItem(char c, char i)
{
	playeffect(13);
	gp-=items[i].price;
	pstats[c].inv[pstats[c].invcnt]=i;
	pstats[c].invcnt++;
}

void BuyOrBuyEquip(char c, char ptr)
{ 
	int first=1,p=0;
	char a,l;

	playeffect(1);
	l=partyidx[c]-1;
drawloop:
	drawmap();

	tmenubox(20,20,116,48);
	gotoxy(40,26); printstring("Yes");
	gotoxy(40,36); printstring("No");
	TCopySprite(22,24+(p*10),16,16,menuptr);

	PutGPBox();
	PutMessageBox("Equip item now?");
	PutItemName((char*) items[storeinv[ptr]].name);
	PutItemDesc((char*) items[storeinv[ptr]].desc);
	PutBuyCharBox(ptr,c+1);
	PutStoreInv();
	PutEquipPreview(c,ptr);
	a=ptr/6;
	TCopySprite(132+((ptr-(a*6))*32),86+(a*24),24,24,itmptr);
	vgadump();

	ReadControls();

	if (first==2) if (b2 || b3) goto drawloop;
		else return;
	if (first && !b1 && !b3 && !down && !up) first=0;
		else if (first) goto drawloop;
	
	if (down || up)
	{
		p=p^1;
		playeffect(0);
		first=1;
	}

	if (b1)
	{
		if (p) { PurchaseItem(l,storeinv[ptr]); first=2; }
			else { playeffect(13); first=2;
	               gp-=items[storeinv[ptr]].price;
                   a=pstats[l].inv[items[storeinv[ptr]].equipflag-1];
                   pstats[l].inv[items[storeinv[ptr]].equipflag-1]=storeinv[ptr];
                   pstats[l].inv[pstats[l].invcnt]=a;
                   if (a) pstats[l].invcnt++;
                   UpdateEquipStats(); }
	}

	while (!b3 && !b2) goto drawloop;
	while (b3 || b2) { first=2; goto drawloop; }
}

/********************** menu2 **********************/

char sg1[]="SAVEDAT.000";
char sg2[]="SAVEDAT.001";
char sg3[]="SAVEDAT.002";
char sg4[]="SAVEDAT.003";
char *savename[]=
{ sg1, sg2, sg3, sg4 };

struct menu {
  unsigned short int posx,posy;
  char linktype;
};

struct menu menus[4];

void greyscale(int width, int height, unsigned char *src, unsigned char *dest)
{ 
	int i,j;
	byte r,g,b,c;

	for (j=0;j<height;j++)
		for (i=0;i<width;i++)
		{ 
			c=src[(j*width)+i];
			r=pal[(c*3)];
			g=pal[(c*3)+1];
			b=pal[(c*3)+2];
			c=(r+g+b)/6;
			dest[(j*width)+i]=c; 
		}
}

void grabregion(int x, int y, int x2, int y2, byte *d)
{

}

void LoadSaveErase(char mode)
// The Save Game / Load Game / Erase Game menu interface. Since the three
// functions are almost identical in their interface, I crammed them all
// into one function, using the Mode variable to specify the intent.
// MODE: 0=Load Game, 1=Save Game, 2=Erase Game
{ 
	FILE *f;
	int i,j,r=0;
	byte tbuf[2560],buf1[2560],buf2[2560],buf3[2560],buf4[2560];
	byte rbuf[256];
	byte *img=nullptr,mpos,b;
	char mnu1[]="LOADGAME.MNU";
	char mnu2[]="SAVEGAME.MNU";
	char mnu3[]="DELGAME.MNU";
	char *mnuname[]={ mnu1, mnu2, mnu3 };

	playeffect(1);
	fout();
redraw:
	if (!(f=fopen(mnuname[mode],"r")))
		err("Fatal Error: Could not open specified MNU file.");
parseloop:
	fscanf(f,"%s",strbuf);

	if (strcmp(strbuf,"background")==0) 
	{
		fscanf(f,"%s",strbuf);
		loadpcx(strbuf,scr);
		goto parseloop; 
	}

	if (strcmp(strbuf,"print")==0) 
	{
		fscanf(f,"%u",&i); i+=16;
		fscanf(f,"%u",&j); j+=16;
		gotoxy(i,j); fscanf(f,"%s",strbuf);
		printstring(strbuf);
		goto parseloop; 
	}

	if (strcmp(strbuf,"printb")==0) 
	{
		fscanf(f,"%u",&i); i+=16;
		fscanf(f,"%u",&j); j+=16;
		gotoxy(i,j); fscanf(f,"%s",strbuf);
		bigprintstring(strbuf);
		goto parseloop; 
	}

	if (strcmp(strbuf,":selectors")==0) 
	{
		for (i=0; i<4; i++)
		{ 
			fscanf(f,"%s",strbuf); menus[i].posx=atoi(strbuf)+16;
			fscanf(f,"%s",strbuf); menus[i].posy=atoi(strbuf)+16; 
		}
		goto parseloop; 
	}
	fclose(f);

	for (i=0; i<4; i++) 
	{
		if (!(f=fopen(savename[i],"rb"))) 
		{
			gotoxy(menus[i].posx+100,menus[i].posy+10);
			menus[i].linktype=0;
			printstring("- NOT USED -"); 
		}
		else 
		{
			menus[i].linktype=1;
            fread(strbuf,1,30,f);
            gotoxy(menus[i].posx+90,menus[i].posy+1);
            printstring(strbuf);
            fread(strbuf,1,9,f);
            gotoxy(menus[i].posx+90,menus[i].posy+11);
            printstring(strbuf);
            fread(&j,1,4,f);
            dec_to_asciiz(j,strbuf);
            gotoxy(menus[i].posx+162,menus[i].posy+11);
            printstring("LV ");printstring(strbuf);
            fread(&j,1,4,f);
            gotoxy(menus[i].posx+230,menus[i].posy+11);
            dec_to_asciiz(j,strbuf);
            printstring(strbuf);printstring("/");
            fread(&j,1,4,f);
            dec_to_asciiz(j,strbuf);
            printstring(strbuf);
            fread(&j,1,4,f);
            gotoxy(menus[i].posx+90,menus[i].posy+21);
            dec_to_asciiz(j,strbuf);
            printstring(strbuf);
            printstring(" G");
            fread(&b,1,1,f);
            gotoxy(menus[i].posx+162,menus[i].posy+21);
            dec_to_asciiz(b,strbuf);if (b<10) printstring("0");
            printstring(strbuf);printstring(":");
            fread(&b,1,1,f);dec_to_asciiz(b,strbuf);
            if (b<10) printstring("0"); printstring(strbuf);
            fread(&b,1,2,f);
            fread(&j,1,1,f);
            if (!i) img=buf1;
            if (i==1) img=buf2;
            if (i==2) img=buf3;
            if (i==3) img=buf4;
            fread(img,1,2560,f);
            greyscale(80,32,img,tbuf);

            TCopySprite(menus[i].posx,menus[i].posy,16,32,tbuf);
            TCopySprite(menus[i].posx+16,menus[i].posy,16,32,&tbuf[512]);
            TCopySprite(menus[i].posx+32,menus[i].posy,16,32,&tbuf[1024]);
            TCopySprite(menus[i].posx+48,menus[i].posy,16,32,&tbuf[1536]);
            TCopySprite(menus[i].posx+64,menus[i].posy,16,32,&tbuf[2048]); 
		}
		fclose(f); 
	}

	mpos=0;
	grabregion(menus[0].posx-16,menus[0].posy+10,16,16,rbuf);
	TCopySprite(menus[0].posx-16,menus[0].posy+10,16,16,menuptr);
	if (menus[0].linktype) 
	{
		TCopySprite(menus[0].posx,menus[0].posy,16,32,buf1);
		TCopySprite(menus[0].posx+16,menus[0].posy,16,32,&buf1[512]);
		TCopySprite(menus[0].posx+32,menus[0].posy,16,32,&buf1[1024]);
		TCopySprite(menus[0].posx+48,menus[0].posy,16,32,&buf1[1536]);
		TCopySprite(menus[0].posx+64,menus[0].posy,16,32,&buf1[2048]); 
	}
	vgadump();
	if (!r) fin();
	if (r) while (b1) ReadControls();
	
inputloop:
	ReadControls();

	if (down) 
	{
		CopySprite(menus[mpos].posx-16,menus[mpos].posy+10,16,16,rbuf);
		if (!mpos) img=buf1;
		if (mpos==1) img=buf2;
		if (mpos==2) img=buf3;
		if (mpos==3) img=buf4;
		if (menus[mpos].linktype) 
		{
			greyscale(80,32,img,tbuf);
			TCopySprite(menus[mpos].posx,menus[mpos].posy,16,32,tbuf);
			TCopySprite(menus[mpos].posx+16,menus[mpos].posy,16,32,&tbuf[512]);
			TCopySprite(menus[mpos].posx+32,menus[mpos].posy,16,32,&tbuf[1024]);
			TCopySprite(menus[mpos].posx+48,menus[mpos].posy,16,32,&tbuf[1536]);
			TCopySprite(menus[mpos].posx+64,menus[mpos].posy,16,32,&tbuf[2048]); 
		}
		mpos++; if (mpos==4) mpos=0;
		grabregion(menus[mpos].posx-16,menus[mpos].posy+10,16,16,rbuf);
		TCopySprite(menus[mpos].posx-16,menus[mpos].posy+10,16,16,menuptr);
		if (menus[mpos].linktype) 
		{
			if (!mpos) img=buf1;
			if (mpos==1) img=buf2;
			if (mpos==2) img=buf3;
			if (mpos==3) img=buf4;
			TCopySprite(menus[mpos].posx,menus[mpos].posy,16,32,img);
			TCopySprite(menus[mpos].posx+16,menus[mpos].posy,16,32,img+512);
			TCopySprite(menus[mpos].posx+32,menus[mpos].posy,16,32,img+1024);
			TCopySprite(menus[mpos].posx+48,menus[mpos].posy,16,32,img+1536);
			TCopySprite(menus[mpos].posx+64,menus[mpos].posy,16,32,img+2048); 
		}
		playeffect(0);
		vgadump();
		while (down) ReadControls(); 
	}

	if (up) 
	{
		CopySprite(menus[mpos].posx-16,menus[mpos].posy+10,16,16,rbuf);
		if (!mpos) img=buf1;
		if (mpos==1) img=buf2;
		if (mpos==2) img=buf3;
		if (mpos==3) img=buf4;
		if (menus[mpos].linktype) 
		{
	        greyscale(80,32,img,tbuf);
			TCopySprite(menus[mpos].posx,menus[mpos].posy,16,32,tbuf);
			TCopySprite(menus[mpos].posx+16,menus[mpos].posy,16,32,&tbuf[512]);
			TCopySprite(menus[mpos].posx+32,menus[mpos].posy,16,32,&tbuf[1024]);
			TCopySprite(menus[mpos].posx+48,menus[mpos].posy,16,32,&tbuf[1536]);
			TCopySprite(menus[mpos].posx+64,menus[mpos].posy,16,32,&tbuf[2048]); 
		}
		if (!mpos) mpos=3;
		else mpos--;
		grabregion(menus[mpos].posx-16,menus[mpos].posy+10,16,16,rbuf);
		TCopySprite(menus[mpos].posx-16,menus[mpos].posy+10,16,16,menuptr);
		if (menus[mpos].linktype) 
		{
			if (!mpos) img=buf1;
			if (mpos==1) img=buf2;
			if (mpos==2) img=buf3;
			if (mpos==3) img=buf4;
			TCopySprite(menus[mpos].posx,menus[mpos].posy,16,32,img);
			TCopySprite(menus[mpos].posx+16,menus[mpos].posy,16,32,img+512);
			TCopySprite(menus[mpos].posx+32,menus[mpos].posy,16,32,img+1024);
			TCopySprite(menus[mpos].posx+48,menus[mpos].posy,16,32,img+1536);
			TCopySprite(menus[mpos].posx+64,menus[mpos].posy,16,32,img+2048); 
		}
		playeffect(0);
		vgadump();
		while (up) ReadControls(); 
	}

	if ((b1) && (menus[mpos].linktype) && (!mode))
    { fout(); LoadGame(savename[mpos]);
      return; }

	if ((b1) && (mode==1))
    { SaveGame(savename[mpos]); r++; goto redraw; }

	if ((b1) && (menus[mpos].linktype) && (mode==2))
    { remove(savename[mpos]); r++; goto redraw; }

	if (!b2) goto inputloop;
	fout(); timer=0;
}

void RemoveItem(char c, char i)
{ 
	char j;

	for (j=i; j<pstats[c].invcnt; j++)
		pstats[c].inv[j]=pstats[c].inv[j+1];
	pstats[c].invcnt--;
}

void DrawItemMenu(char c,char ptr)
{ 
	byte l,i,a,*img,z;
	int j,k;

	l=partyidx[c]-1;
	drawmap();
	TCopySprite(20,20,96,96,chr2+(c*9216));   // Status portrait

	tmenubox(120,20,224,38);                  // name box
	i=strlen(pstats[l].name)*4;
	gotoxy(172-i,26);
	printstring(pstats[l].name);
	tmenubox(226,20,330,38);
	gotoxy(262,26); printstring("ITEM");

	tmenubox(120,40,330,59);                 // Item name
	a=pstats[l].inv[ptr];
	if (!items[a].useflag || items[a].useflag>=3) fontcolor(17);
	j=vstrlen((char *) items[a].name);
	gotoxy(225-(j*4),46);
	if (ptr<pstats[l].invcnt)
		printstring((char*) items[a].name);
	fontcolor(31);

	tmenubox(120,61,330,80);                 // Item desc
	j=vstrlen((char*) items[a].desc);
	gotoxy(225-(j*4),67);
	if (ptr<pstats[l].invcnt)
		printstring((char *) items[a].desc);

	if (items[a].equipflag && !iuflag)           // Show who it's equipable by
	{
		tmenubox(20,117,115,186);
		gotoxy(27,123);
		printstring("Equip:"); z=0;
		for (j=0; j<5; j++)
		{
			i=partyidx[j]-1;
			if (equip[items[a].equipidx].equipable[i] && j < numchars)
			{
				gotoxy(33,133+(z*10));
				printstring(pstats[i].name);
				z++;
			}
		}
	}

	tmenubox(120,82,330,115);
	
	for (j=0; j<6; j++)
	{
		a=pstats[l].inv[j];
		img=itemicons+(items[a].icon*256);
		if (a) TCopySprite(137+(j*32),91,16,16,img);
	}

	tmenubox(120,117,330,210);
	for (k=0; k<3; k++)
		for (j=0; j<6; j++)
		{
			a=pstats[l].inv[((k+1)*6)+j];
			img=itemicons+(items[a].icon*256);
			if (a) TCopySprite(137+(j*32),130+(k*24),16,16,img);
		}
	a=ptr/6;
	if (ptr<6) TCopySprite(133+(ptr*32),87,24,24,itmptr);
	else TCopySprite(133+((ptr-(a*6))*32),102+(a*24),24,24,itmptr);
}

void ItemGive(char c, char p)
{ 
	int first=1, ptr=0,j;
	byte l,t1;

	playeffect(1);
drawloop:
	DrawItemMenu(c,p);
	tmenubox(20,117,115,139+(numchars*10));
	gotoxy(30,123); printstring("Give to:");

	for (j=0; j<numchars; j++)
    { 
		l=partyidx[j]-1;
		gotoxy(45,133+(j*10));
        printstring(pstats[l].name);
    }

	TCopySprite(27,131+(ptr*10),16,16,menuptr);
	vgadump();

	ReadControls();
	if (first==2) if (b1 || b2 || b4) goto drawloop;
    else { UpdateEquipStats(); return; }
	if (first && !b1 && !b2 && !b4 && !down && !up) first=0;
	else if (first) goto drawloop;

	if (down) 
	{ 
		ptr++;
        if (ptr==numchars) ptr=0;
        playeffect(0);
        first=1;
    }
	if (up)   
	{ 
		if (!ptr) ptr=numchars-1;
        else ptr--;
        playeffect(0);
        first=1;
    }

	if (b1)   
	{ 
		l=partyidx[c]-1;
        t1=pstats[l].inv[p];
        if (p<6) pstats[l].inv[p]=0;
			else RemoveItem(l,p);

        l=partyidx[ptr]-1;
        j=pstats[l].invcnt;
        pstats[l].inv[j]=t1;
        pstats[l].invcnt++;
        playeffect(1);
        first=2;
    }

	while (!b4 && !b2) goto drawloop;
	while (b4 || b2) { first=2; goto drawloop; }
}

void ItemUse(char c, char p)
{ 
	int first=1, ptr=0,j;
	byte l,t1,a;

	playeffect(1);
	a=partyidx[c]-1;
	t1=pstats[a].inv[p];

	if (!items[t1].useflag || items[t1].useflag>4) return;
	if (numchars==1)
    { l=a; goto usesec; }
	if (!items[t1].itemtype || items[t1].itemtype==2)
    { l=a; goto usesec; }

drawloop:
	DrawItemMenu(c,p);
	tmenubox(20,117,115,139+(numchars*10));
	gotoxy(30,123); printstring("Use item:");

	for (j=0; j<numchars; j++)
	{ 
		l=partyidx[j]-1;
        gotoxy(45,133+(j*10));
        printstring(pstats[l].name);
    }

	TCopySprite(27,131+(ptr*10),16,16,menuptr);
	vgadump();

	ReadControls();
	if (first==2) if (b1 || b2 || b4) goto drawloop;
    else return;
	if (first && !b1 && !b2 && !b4 && !down && !up) first=0;
	else if (first) goto drawloop;

	if (down) 
	{
		ptr++;
        if (ptr==numchars) ptr=0;
        playeffect(0);
        first=1;
    }
	if (up)   
	{ 
		if (!ptr) ptr=numchars-1;
        else ptr--;
        playeffect(0);
        first=1;
    }

	if (b1)   
	{ 
		l=partyidx[ptr]-1;
        goto usesec; 
	}

	while (!b4 && !b2) goto drawloop;
	while (b4 || b2) { first=2; goto drawloop; }

usesec:
	varl[0]=l;
	ExecuteEffect(items[t1].useeffect);
	if (items[t1].useflag==2 || items[t1].useflag==4) return;
	RemoveItem(c,p);
}

void ItemActionSelect(char c,char p)
{ 
	int first=1, ptr=0;
	unsigned char l, a;

	playeffect(1); iuflag=1;
	l=partyidx[c]-1;
drawloop:
	DrawItemMenu(c,p);

	tmenubox(20,117,115,157);                      // Put Use|Give|Drop menu up
	gotoxy(45,123); printstring("Use");
	gotoxy(45,133); printstring("Give");
	gotoxy(45,143); printstring("Drop");
	TCopySprite(26,121+(ptr*10),16,16,menuptr);
	vgadump();
	
	ReadControls();
	if (first==2) if (b1 || b2 || b4) goto drawloop;
    else { iuflag=0; UpdateEquipStats(); return; }
	if (first && !b1 && !b2 && !b4 && !down && !up) first=0;
	else if (first) goto drawloop;

	if (down) 
	{ 
		ptr++;
        if (ptr==3) ptr=0;
        playeffect(0);
        first=1;
    }
	if (up)  
	{ 
		if (!ptr) ptr=2;
        else ptr--;
        playeffect(0);
        first=1;
    }

	if (b1)   
	{ 
		switch (ptr)
        { 
			case 0: ItemUse(c,p);
                    first=2; break;
            case 1: if (numchars==1) break;
                    ItemGive(c,p);
                    first=2; break;
            case 2: if (!items[pstats[l].inv[p]].price)
                    { playeffect(12); first=2; break; }
                    if (p>5) RemoveItem(l,p);
                    else
                    { 
						a=pstats[l].inv[p];
						if (equip[items[a].equipidx].ondeequip)
							ExecuteEffect(equip[items[a].equipidx].ondeequip-1);
						pstats[l].inv[p]=0;
                    }
                    UpdateEquipStats();
                    first=2; break;
		}
        playeffect(1);
    }

	while (!b4 && !b2) goto drawloop;
	while (b4 || b2) { first=2; goto drawloop; }
}

void ItemMenu(char c)
{ 
	int first=1;
	byte l,ptr=6,mx=0,my=1;

	playeffect(1);
	l=partyidx[c]-1;
drawloop:
	DrawItemMenu(c,ptr);
	vgadump();
	ReadControls();

	if (first==2) if (b1 || b2 || b4) goto drawloop;
    else return;
	if (first && !b1 && !b2 && !b4 && !down && !up && !right && !left) first=0;
	else if (first) goto drawloop;

	if (down)  
	{ 
		if (my<3) my++;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (up)    
	{ 
		if (my) my--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (right) 
	{ 
		if (mx<5) mx++;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }
	if (left)  
	{ 
		if (mx) mx--;
        ptr=(my*6)+mx;
        playeffect(0);
        first=1;
    }

	if (b1) { if (ptr<pstats[l].invcnt)
              ItemActionSelect(c,ptr);
              first=1; goto drawloop; }

	while (!b4 && !b2 && !b1) goto drawloop;
	while (b4 || b2) { first=2; goto drawloop; }
}

void CalcEquipPreview(int a,int i,int p)
{ 
	int c,d;

	d=items[pstats[a].inv[p]].equipflag-1;
	c=pstats[a].inv[d];
	pstats[a].inv[d]=0;
	UpdateEquipStats();
	
	if (p>5)
	{
		atkp=pstats[a].atk+equip[i].str;
		defp=pstats[a].def+equip[i].end;
		magp=pstats[a].magc+equip[i].mag;
		mgrp=pstats[a].mgrc+equip[i].mgr;
		hitp=pstats[a].hitc+equip[i].hit;
		dodp=pstats[a].dodc+equip[i].dod;
		mblp=pstats[a].mblc+equip[i].mbl;
		ferp=pstats[a].ferc+equip[i].fer;
		reap=pstats[a].reac+equip[i].rea;
	} else
	{
		atkp=pstats[a].atk;
		defp=pstats[a].def;
	    magp=pstats[a].magc;
	    mgrp=pstats[a].mgrc;
	    hitp=pstats[a].hitc;
		dodp=pstats[a].dodc;
		mblp=pstats[a].mblc;
		ferp=pstats[a].ferc;
		reap=pstats[a].reac;
	}

	pstats[a].inv[d]=c;
	UpdateEquipStats();
}

void DrawEquipMenu(char c,char ptr)
{ 
	byte l,i,a,*img;
	int j,k;

	l=partyidx[c]-1;
	drawmap();
	TCopySprite(20,20,96,96,chr2+(c*9216));   // Status portrait

	tmenubox(120,20,224,38);                  // name box
	i=strlen(pstats[l].name)*4;
	gotoxy(172-i,26);
	printstring(pstats[l].name);
	tmenubox(226,20,330,38);
	gotoxy(258,26); printstring("EQUIP");

	tmenubox(120,40,330,59);                 // Item name
	a=pstats[l].inv[ptr];
	if (!items[a].equipflag || !equip[items[a].equipidx].equipable[l])
		fontcolor(17);
	j=vstrlen((char*) items[a].name);
	gotoxy(225-(j*4),46);
	if (ptr<pstats[l].invcnt)
		printstring((char*) items[a].name);
	fontcolor(31);

	tmenubox(120,61,330,80);                 // Item desc
	j=vstrlen((char*) items[a].desc);
	gotoxy(225-(j*4),67);
	if (ptr<pstats[l].invcnt)
		printstring((char*) items[a].desc);
	
	// If equipment, do effect preview box

	if (items[a].equipflag && equip[items[a].equipidx].equipable[l])
	{
		tmenubox(20,117,115,210);
		gotoxy(26,124);
		printstring("ATK "); dec_to_asciiz(pstats[l].atk,strbuf); gotoxy(80-(strlen(strbuf)*8),124); printstring(strbuf); printstring(">");dec_to_asciiz(atkp,strbuf);
		if (pstats[l].atk<atkp) fontcolor(97); if (atkp<pstats[l].atk) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),124); printstring(strbuf); fontcolor(31); gotoxy(26,133);
		printstring("DEF "); dec_to_asciiz(pstats[l].def,strbuf); gotoxy(80-(strlen(strbuf)*8),133); printstring(strbuf); printstring(">");dec_to_asciiz(defp,strbuf);
		if (pstats[l].def<defp) fontcolor(97); if (defp<pstats[l].def) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),133); printstring(strbuf); fontcolor(31);gotoxy(26,142);
		printstring("HIT "); dec_to_asciiz(pstats[l].hitc,strbuf); gotoxy(80-(strlen(strbuf)*8),142); printstring(strbuf); printstring(">");dec_to_asciiz(hitp,strbuf);
		if (pstats[l].hitc<hitp) fontcolor(97); if (hitp<pstats[l].hitc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),142); printstring(strbuf); fontcolor(31);gotoxy(26,151);
		printstring("DOD "); dec_to_asciiz(pstats[l].dodc,strbuf); gotoxy(80-(strlen(strbuf)*8),151); printstring(strbuf); printstring(">");dec_to_asciiz(dodp,strbuf);
		if (pstats[l].dodc<dodp) fontcolor(97); if (dodp<pstats[l].dodc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),151); printstring(strbuf); fontcolor(31);gotoxy(26,160);
		printstring("MAG "); dec_to_asciiz(pstats[l].magc,strbuf); gotoxy(80-(strlen(strbuf)*8),160); printstring(strbuf); printstring(">");dec_to_asciiz(magp,strbuf);
		if (pstats[l].magc<magp) fontcolor(97); if (magp<pstats[l].magc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),160); printstring(strbuf); fontcolor(31);gotoxy(26,169);
		printstring("MGR "); dec_to_asciiz(pstats[l].mgrc,strbuf); gotoxy(80-(strlen(strbuf)*8),169); printstring(strbuf); printstring(">");dec_to_asciiz(mgrp,strbuf);
		if (pstats[l].mgrc<mgrp) fontcolor(97); if (mgrp<pstats[l].mgrc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),169); printstring(strbuf); fontcolor(31);gotoxy(26,178);
		printstring("REA "); dec_to_asciiz(pstats[l].reac,strbuf); gotoxy(80-(strlen(strbuf)*8),178); printstring(strbuf); printstring(">"); dec_to_asciiz(reap,strbuf);
		if (pstats[l].reac<reap) fontcolor(97); if (reap<pstats[l].reac) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),178); printstring(strbuf); fontcolor(31);gotoxy(26,187);
		printstring("FER "); dec_to_asciiz(pstats[l].ferc,strbuf); gotoxy(80-(strlen(strbuf)*8),187); printstring(strbuf); printstring(">");dec_to_asciiz(ferp,strbuf);
		if (pstats[l].ferc<ferp) fontcolor(97); if (ferp<pstats[l].ferc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),187); printstring(strbuf); fontcolor(31);gotoxy(26,196);
		printstring("MBL "); dec_to_asciiz(pstats[l].mblc,strbuf); gotoxy(80-(strlen(strbuf)*8),196); printstring(strbuf); printstring(">");dec_to_asciiz(mblp,strbuf);
		if (pstats[l].mblc<mblp) fontcolor(97); if (mblp<pstats[l].mblc) fontcolor(36); gotoxy(110-(strlen(strbuf)*8),196); printstring(strbuf); fontcolor(31);
	}

	tmenubox(120,82,330,115);
	
	for (j=0; j<6; j++)
	{
		a=pstats[l].inv[j];
		img=itemicons+(items[a].icon*256);
		if (a) TCopySprite(137+(j*32),91,16,16,img);
	}	

	tmenubox(120,117,330,210);
	for (k=0; k<3; k++)
		for (j=0; j<6; j++)
		{
			a=pstats[l].inv[((k+1)*6)+j];
			img=itemicons+(items[a].icon*256);
			if (!items[a].equipflag || !equip[items[a].equipidx].equipable[l])
            { greyscale(16,16,img,gsimg);
              img=gsimg; }
			if (a) TCopySprite(137+(j*32),130+(k*24),16,16,img);
		}
	a=ptr/6;
	if (ptr<6) TCopySprite(133+(ptr*32),87,24,24,itmptr);
	else TCopySprite(133+((ptr-(a*6))*32),102+(a*24),24,24,itmptr);
}

void Equip(char c,char ptr)
{ 
	byte a,l,b;

	l=partyidx[c]-1;
	a=pstats[l].inv[ptr];
	if (!items[a].equipflag || !equip[items[a].equipidx].equipable[l])
	{
		playeffect(12);
		return;
	}

	b=items[a].equipflag;
	if (!pstats[l].inv[b-1])
	{
		RemoveItem(l,ptr);
	}
	else pstats[l].inv[ptr]=pstats[l].inv[b-1];
	pstats[l].inv[b-1]=a;
	UpdateEquipStats();
	playeffect(15);
	if (equip[items[a].equipidx].onequip)
		ExecuteEffect(equip[items[a].equipidx].onequip-1);
}

void DeEquip(char c,char ptr)
{ 
	byte a,l;

	l=partyidx[c]-1;
	a=pstats[l].inv[ptr];

	if (pstats[l].invcnt==23 || !items[a].equipflag)
	{
		playeffect(12);
		return;
	}

	pstats[l].inv[ptr]=0;
	pstats[l].inv[pstats[l].invcnt]=a;
	pstats[l].invcnt++;
	UpdateEquipStats();
	playeffect(14);
	if (equip[items[a].equipidx].ondeequip)
		ExecuteEffect(equip[items[a].equipidx].ondeequip-1);
}

void EquipMenu(char c)
{ 
	int first=1,a;
	byte l,ptr=6,mx=0,my=1;

	playeffect(1);
	l=partyidx[c]-1;
	a=pstats[l].inv[ptr];
	CalcEquipPreview(l,items[a].equipidx,ptr);

drawloop:
	DrawEquipMenu(c,ptr);
	vgadump();
	ReadControls();

	if (first==2) if (b1 || b2 || b4) goto drawloop;
    else return;
	if (first && !b1 && !b2 && !b4 && !down && !up && !right && !left) first=0;
	else if (first) goto drawloop;

	if (down)  
	{ 
		if (my<3) my++;
        ptr=(my*6)+mx;
        a=pstats[l].inv[ptr];
        CalcEquipPreview(l,items[a].equipidx,ptr);
        playeffect(0);
        first=1;
    }
	if (up)    
	{ 
		if (my) my--;
        ptr=(my*6)+mx;
        a=pstats[l].inv[ptr];
        CalcEquipPreview(l,items[a].equipidx,ptr);
        playeffect(0);
        first=1;
	}
	if (right) 
	{ 
		if (mx<5) mx++;
        ptr=(my*6)+mx;
        a=pstats[l].inv[ptr];
        CalcEquipPreview(l,items[a].equipidx,ptr);
        playeffect(0);
        first=1;
    }
	if (left)  
	{ 
		if (mx) mx--;
        ptr=(my*6)+mx;
        a=pstats[l].inv[ptr];
        CalcEquipPreview(l,items[a].equipidx,ptr);
        playeffect(0);
        first=1;
    }

	if (b1) 
	{
		if (ptr>=pstats[l].invcnt) { first=1; goto drawloop; }
        if (ptr<6) DeEquip(c,ptr);
        if (ptr>=6) Equip(c,ptr);
        a=pstats[l].inv[ptr];
        CalcEquipPreview(l,items[a].equipidx,ptr);
        first=1; goto drawloop; 
	}
	
	while (!b4 && !b2 && !b1) goto drawloop;
	while (b4 || b2) { first=2; goto drawloop; }
}