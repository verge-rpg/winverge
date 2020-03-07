#define  WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500
#include "pcp.h"

/*
 fixed saveload window (which is a total piece-of-shit-code),
 fix ForceEquip and 
 and the sound system.

  test out games to ensure compatibility! 
*/

// ***************************** Data *****************************

char Foreground=0;
char WinCursorOn=1;
HWND hMainWnd;
HINSTANCE hMainInst;

POINT MyPt;
int mx, my, mb_Left, mb_Right, mb, mw=0;
HMENU hMenu,hSubMenu;
UINT umsg;

// ***************************** Code *****************************

void InitPStats()
{
	FILE *pdat,*cdat;
	char i;

	pdat = fopen("PARTY.DAT","r");
	if (!pdat) 
		err("Fatal error: PARTY.DAT not found");
	fscanf(pdat,"%s",strbuf);
	tchars = atoi(strbuf);
	for (i=0; i<tchars; i++)
    {
		fscanf(pdat,"%s",&pstats[i].chrfile);
        fscanf(pdat,"%s",strbuf);
        fscanf(pdat,"%s",strbuf);
        cdat=fopen(strbuf,"r");
        if (!cdat) 
			err("Could not open character DAT file.");
        fscanf(cdat,"%s",&pstats[i].name);
        fgets(strbuf,99,cdat);
        fgets(strbuf,99,cdat);
        fscanf(cdat,"%s",strbuf);
        pstats[i].exp=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].curhp=atoi(strbuf);
        pstats[i].maxhp=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].curmp=atoi(strbuf);
        pstats[i].maxmp=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].str=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].end=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].mag=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].mgr=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].hit=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].dod=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].mbl=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].fer=atoi(strbuf);
        fscanf(cdat,"%s",strbuf);
        pstats[i].rea=atoi(strbuf);
        pstats[i].lv=1;
        fscanf(cdat,"%s",strbuf);
        pstats[i].nxt=atoi(strbuf);
        pstats[i].status=0;
        pstats[i].invcnt=6;
        memset(&pstats[i].inv, 0, 512);
        fclose(cdat);
	}
	fclose(pdat);
}

void StartNewGame(const char *startp)
{
	numchars=0;
	InitPStats();
	addcharacter(1);
	usenxy=0; gp=0; party[0].speed=4;
	memset(flags,0,32000);
	hr=0; min=0; sec=0; tickctr=0;
	quake=0; hookretrace=0; foregroundlock=1;  hooktimer=0;
	screengradient=0; layervc=0; VCClear();
	layer0=1; layer1=1; drawparty=1; drawentities=1; cameratracking=1;
	UpdateEquipStats();
	startmap(startp);
}

void LoadGame(const char *fn)
{
	FILE *f;
	char i,b;

	quake=0; hookretrace=0; hooktimer=0; 
	screengradient=0; layervc=0; foregroundlock=1; VCClear();
	layer0=1; layer1=1; drawparty=1; drawentities=1; cameratracking=1;
	numchars=0;
	f=fopen(fn,"rb");
	fread(strbuf, 1, 51, f);
	fread(&gp, 1, 4, f);
	fread(&hr, 1, 1, f);
	fread(&min, 1, 1, f);
	fread(&sec, 1, 1, f);
	fread(&b, 1, 1, f);
	fread(&menuactive, 1, 1, f);
	fread(scr, 1, 2560, f);
	fread(&mapname, 1, 13, f);
	fread(&party, 1, sizeof party, f);
	fread(&partyidx, 1, 5, f);
	fread(&flags, 1, 32000, f);
	fread(&tchars, 1, 1, f);
	fread(&pstats, 1, sizeof pstats, f);
	fclose(f);
	for (i=0; i<b; i++)
		addcharacter(partyidx[i]);
	nx=party[0].x/16;
	ny=party[0].y/16;
	usenxy=1;
	startmap(mapname);
}

void ProcessEquipDat()
{
	FILE *f;
	int a,i;

	// This function parses EQUIP.DAT, which sets all the stats for equipment.
	// It's pretty long as it has to have a processing section for each possible
	// stat, plus some other stuff. :P

	f = fopen("EQUIP.DAT","r");
	if (!f) 
		err("Could not open EQUIP.DAT.");
	fscanf(f,"%d",&a);
	for (i=1; i<=a; i++)
	{
pl1:
		fscanf(f,"%s",strbuf); _strupr(strbuf);
		if (!strcmp(strbuf,"//")) { fgets(strbuf,99,f); goto pl1; }
		if (!strcmp(strbuf,"ATK")) { fscanf(f,"%s",strbuf);
                                   equip[i].str=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"DEF")) { fscanf(f,"%s",strbuf);
                                   equip[i].end=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"MAG")) { fscanf(f,"%s",strbuf);
                                   equip[i].mag=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"MGR")) { fscanf(f,"%s",strbuf);
                                   equip[i].mgr=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"HIT")) { fscanf(f,"%s",strbuf);
                                   equip[i].hit=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"DOD")) { fscanf(f,"%s",strbuf);
                                   equip[i].dod=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"MBL")) { fscanf(f,"%s",strbuf);
                                   equip[i].mbl=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"FER")) { fscanf(f,"%s",strbuf);
                                   equip[i].fer=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"REA")) { fscanf(f,"%s",strbuf);
                                   equip[i].rea=atoi(strbuf); goto pl1; }
		if (!strcmp(strbuf,"ONEQUIP")) { fscanf(f,"%s",strbuf);
                                       equip[i].onequip=atoi(strbuf)+1; goto pl1; }
		if (!strcmp(strbuf,"ONDEEQUIP")) { fscanf(f,"%s",strbuf);
                                         equip[i].ondeequip=atoi(strbuf)+1; goto pl1; }
		if (!strcmp(strbuf,"EQABLE"))
		{
eqloop:
			fscanf(f,"%s",strbuf);
			if (!strcmp(strbuf,"-")) continue;
			equip[i].equipable[atoi(strbuf)-1]=1;
			goto eqloop;
		}
	}
}

void InitItems()
{
	FILE *f;
	byte b,i;
	int j;

	f = fopen("ITEMICON.DAT","rb");
	if (!f) err("Could not open ITEMICON.DAT.");
	fread(&b, 1, 1, f);

	fread(itemicons+256, 256, b, f);
	fclose(f);

	f = fopen("ITEMS.DAT","r");
	if (!f) err("Could not open ITEMS.DAT.");
	fscanf(f,"%s",strbuf);
	b=atoi(strbuf);
	for (i=1; i<b+1; i++)
    {
		fscanf(f,"%s",items[i].name);
        fscanf(f,"%s",strbuf);
        items[i].icon=atoi(strbuf)+1;
        fscanf(f,"%s",items[i].desc);
        fscanf(f,"%s",strbuf);
        items[i].useflag=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].useeffect=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].itemtype=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].equipflag=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].equipidx=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].itmprv=atoi(strbuf);
        fscanf(f,"%s",strbuf);
        items[i].price=atoi(strbuf); 
	}
	fclose(f);

	ProcessEquipDat();

	f=fopen("MISCICON.DAT","rb");
	if (!f) err("Could not open MISCICON.DAT.");
	fread(&b,1,1,f);
	fread(&menuptr, 1, 256, f);
	fread(&itmptr, 1, 576, f);
	fread(&charptr, 1, 960, f);
	fclose(f);

	f=fopen("SPEECH.SPC","rb");
	if (!f) err("Could not open SPEECH.SPC");
	fread(&b,1,1,f);
	fread(speech, b, 1024, f);
	fclose(f);

	f=fopen("EFFECTS.VCS","rb");
	if (!f) err("Could not open EFFECTS.VCS");
	fread(&j, 1, 4, f);
	fread(&effectofstbl, 4, j, f);
	fread(effectvc, 1, 50000, f);
	fclose(f);

	f=fopen("STARTUP.VCS","rb");
	if (!f) err("Could not open STARTUP.VCS");
	fread(&j, 1, 4, f);
	fread(&startupofstbl, 4, j, f);
	fread(startupvc, 1, 50000, f);
	fclose(f);
}

void StartupMenu()
{ 
	char cursel=0;
	int i,s;

drawloop:
	menubox(100,90,252,142);
	gotoxy(130,103);
	printstring("New Game");
	gotoxy(130,113);
	printstring("Load Game");
	gotoxy(130,123);
	printstring("Exit");
	
	if (!cursel) TCopySprite(110,102,16,16,menuptr);
	if (cursel==1) TCopySprite(110,112,16,16,menuptr);
	if (cursel==2) TCopySprite(110,122,16,16,menuptr);

	vgadump();
 	while ((down) || (up)) ReadControls();

inputloop:
	ReadControls();
	if (down) { cursel++;
                if (cursel==3) cursel=0;
//                playeffect(0);
//                playeffect(0);
                goto drawloop;
	}
	if (up) { if (!cursel) cursel=2;
		      else cursel--;
//              playeffect(0);
//              playeffect(0);
              goto drawloop; }

	if (!b1) goto inputloop;
	
	
	if (!cursel) StartNewGame("test.map");
	if (cursel==1) LoadSaveErase(0);
	if (cursel==2) { timer=0;
                     s=91;
fadeloop:            i=(timer*64)/s; i=64-i;
                     SetIntensity(i);
                     if (timer<s) goto fadeloop; SetIntensity(0);
                     err(""); }
}

LRESULT APIENTRY WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE zwhocares, LPSTR isuredont, int nCmdShow)
{
	WNDCLASS WndClass;

	vmemset(&WndClass,0,sizeof(WNDCLASS));
	vmemset(&hMainWnd,0,sizeof(HWND));
	hMainInst=hCurrentInst;
	vmemset(&WndClass,0,sizeof(WNDCLASS));
	WndClass.hCursor=LoadCursor(0, IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	WndClass.lpszClassName="verge main window type";
	WndClass.hInstance=hMainInst;
	WndClass.lpfnWndProc=WndProc;
	RegisterClass(&WndClass);
	RECT r = {0,0,320,200};
	AdjustWindowRect(&r,WS_OVERLAPPEDWINDOW,FALSE);
	char tmp[200];
	sprintf(tmp, "winverge 2020 msvc %d", _MSC_VER);
	hMainWnd=CreateWindowEx((int) NULL,"verge main window type",tmp,WS_OVERLAPPEDWINDOW | WS_VISIBLE,0,0,r.right-r.left,r.bottom-r.top,NULL,NULL,hMainInst,NULL);
	ShowWindow(hMainWnd,SW_NORMAL);
	UpdateWindow(hMainWnd);
	hSubMenu=GetSubMenu(hMenu,0);

	InitErrorSystem();	
	InitTimer(100);
	InitVGA();
	InitKeyboard();
	InitSound();

	allocbuffers();
	InitVCMem();
	LoadFont();
	PreCalc_TransparencyFields();
	InitRenderSystem();
	InitItems();

//	StartNewGame("island.map");
	//StartupMenu();

	while (1)
	{
		qabort=0;
		/* -- ric: 01/Jun/98 --
		* These variables set to allow the vc layer functions to work
		* by preventing the engine from trying to draw a non-existant
		* map
		*/
		cameratracking=0;
		layer0=0;
		layer1=0;
		drawparty=0;
		drawentities=0;
		StartupScript();
	}

	err("");
	return 0;
}

void HandleMessages(void)
{
	MSG msg;
 
	while (PeekMessage(&msg, hMainWnd, (int) NULL, (int) NULL, PM_REMOVE))
	{
		switch (msg.message)
		{
			case WM_LBUTTONDOWN: mb_Left=1; mb=1; break; 
			case WM_LBUTTONUP:   mb_Left=0; mb=1; break;
			case WM_RBUTTONDOWN: mb_Right=1; mb=1; break;
			case WM_RBUTTONUP:   mb_Right=0; mb=1; break;
			case WM_MOUSEWHEEL:
				mw+=(short int)HIWORD(msg.wParam);
 				break;
			default:
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
		}
	}
}
extern byte *scr_last_hicolor;
LRESULT APIENTRY WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_ACTIVATE:
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				Foreground = 0;
			}
			else Foreground = 1;
			break;
		case WM_ACTIVATEAPP:
			if((BOOL)wParam == 0)
			{
				Foreground = 0;
			}
			else Foreground = 1;
			break;

		case WM_TIMER:
			{
				RECT r;
				GetClientRect(hMainWnd, &r);
				InvalidateRect(hMainWnd, &r, FALSE);
				UpdateWindow(hMainWnd);
				break;
			}

		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hMainWnd, &ps);

				BITMAPINFO bmi;
				memset(&bmi, 0, sizeof(bmi));

				bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bmi.bmiHeader.biWidth = 320;
				bmi.bmiHeader.biHeight = -200;
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 32;
				bmi.bmiHeader.biCompression = BI_RGB;

				
				LPBYTE pBits;
				HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS,(void **)&pBits, NULL, 0);

				if(scr_last_hicolor)
					memcpy(pBits,scr_last_hicolor,320*200*4);
				
				HDC hdcMem = CreateCompatibleDC(hdc);
				HGDIOBJ hbmOld = SelectObject(hdcMem, (HGDIOBJ)hbm);
				BitBlt(hdc,0,0,320,200,hdcMem,0,0,SRCCOPY);
				DeleteObject(hbmOld);
				DeleteObject(hdcMem);
				DeleteObject(hbm);

				EndPaint(hMainWnd, &ps);
				break;
			}

		case WM_CLOSE:
			err("");
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void vExit(const char *msg)
{
	DestroyWindow(hMainWnd);
	if (vstrcmp(msg,""))
	{
		MessageBox(NULL, msg, "verge", MB_OK | MB_TASKMODAL);
		Log("Exiting: %s", msg);
	}
	PostQuitMessage(0);
	exit(-1);
}

void ReadMouse()
{
	RECT dr;
	POINT pt;
	char flag=0;
	
	if(!GetCursorPos(&MyPt)) return;
	GetClientRect(hMainWnd,&dr);
	pt.x=dr.left; pt.y=dr.top;
	ClientToScreen(hMainWnd,&pt);
	dr.left=pt.x; dr.top=pt.y;
	pt.x=dr.right; pt.y=dr.bottom;
	ClientToScreen(hMainWnd,&pt);
	dr.right=pt.x; dr.bottom=pt.y;

	mx=(int) ((double)(MyPt.x-dr.left)/((double)(dr.right-dr.left)/(double)320));
	my=(int) ((double)(MyPt.y-dr.top)/((double)(dr.bottom-dr.top)/(double)200));


   if(mx<0)
   {
	   mx=0;
	   if(!WinCursorOn)
	   {
		   ShowCursor(1);
		   WinCursorOn=1;
	   }
	   flag=1;
   }
   if(my<0)
   {
	   my=0;
	   if(!WinCursorOn)
	   {
		   ShowCursor(1);
		   WinCursorOn=1;
	   }
	   flag=1;
   }
   if(mx>=320)
   {
	   mx=320-1;
	   if(!WinCursorOn)
	   {
		   ShowCursor(1);
		   WinCursorOn=1;
	   }
	   flag=1;
   }
   if(my>=200)
   {
	   my=200-1;
	   if(!WinCursorOn)
	   {
		   ShowCursor(1);
		   WinCursorOn=1;
	   }
	   flag=1;
   }
   if(flag) return;
   if(WinCursorOn)
   {
	   ShowCursor(0);
	   WinCursorOn=0;
   }

}