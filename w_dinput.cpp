// ****************************************************************
// * PCP                                                          *
// * Copyright (C)1999 Dark Nova Software (DNS)                   *
// * All Rights Reserved.                                         *
// *                                                              *
// * File: win_dinput.cpp                                         *
// * Author: zeromus, vecna                                       *
// * Portability: Win32 only                                      *
// * Description: Win32 DirectInput code                          *
// ****************************************************************

#define  WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0300
#include "pcp.h"
#include "dinput.h"

// ***************************** Data *****************************

byte b1,b2,b3,b4,up,down,left,right;

LPDIRECTINPUT dinput;
DIPROPDWORD dipdw;
LPDIRECTINPUTDEVICE  di_keyb;
LPDIRECTINPUTDEVICE di_joy;
char key[256];
byte last_pressed;

quad joy1_x_pos,joy1_y_pos,joy2_x_pos,joy2_y_pos;
quad joy_y_min[2],joy_y_max[2],joy_x_min[2],joy_x_max[2],joy_x_range[2],joy_y_range[2];
quad joy1_buttons[15], joy2_buttons[15];
byte joy1_up=0, joy1_down=0, joy1_left=0, joy1_right=0;
byte joy2_up=0, joy2_down=0, joy2_left=0, joy2_right=0;

void ShutdownKeyboard(void);

// ***************************** Code *****************************

quad sticks;
quad currstick;

void InitJoystick()
{
	JOYINFO joyinfo; 
	UINT wNumDevs;
	BOOL bDev1Attached, bDev2Attached; 
	JOYCAPS jc;

	sticks=0;
 
    if((wNumDevs = joyGetNumDevs()) == 0) 
        return; 
    bDev1Attached = joyGetPos(JOYSTICKID1,&joyinfo) != JOYERR_UNPLUGGED; 
    bDev2Attached = wNumDevs == 2 && joyGetPos(JOYSTICKID2,&joyinfo) != JOYERR_UNPLUGGED; 

	if(bDev1Attached)
	{
		sticks|=1;
		joyGetDevCaps(JOYSTICKID1,&jc,sizeof(JOYCAPS));
		joy_x_min[0]=jc.wXmin;
		joy_x_max[0]=jc.wXmax;
		joy_y_min[0]=jc.wYmin;
		joy_y_max[0]=jc.wYmax;
		joy_x_range[0]=joy_x_max[0]-joy_x_min[0];
		joy_y_range[0]=joy_y_max[0]-joy_y_min[0];
	}
	if(bDev2Attached)
	{
		sticks|=2;
		joyGetDevCaps(JOYSTICKID2,&jc,sizeof(JOYCAPS));
		joy_x_min[1]=jc.wXmin;
		joy_x_max[1]=jc.wXmax;
		joy_y_min[1]=jc.wYmin;
		joy_y_max[1]=jc.wYmax;
		joy_x_range[1]=joy_x_max[1]-joy_x_min[1];
		joy_y_range[1]=joy_y_max[1]-joy_y_min[1];
	}
	currstick=0;
}

void ReadJoystick()
{
	JOYINFO ji;

	if(joyGetPos(JOYSTICKID1,&ji)==JOYERR_NOERROR)
	{
		joy1_x_pos = ji.wXpos;
		joy1_y_pos = ji.wYpos;
		joy1_buttons[0]=ji.wButtons&JOY_BUTTON1;
		joy1_buttons[1]=ji.wButtons&JOY_BUTTON2;
		joy1_buttons[2]=ji.wButtons&JOY_BUTTON3;
		joy1_buttons[3]=ji.wButtons&JOY_BUTTON4;
		joy1_buttons[4]=ji.wButtons&JOY_BUTTON5;
		joy1_buttons[5]=ji.wButtons&JOY_BUTTON6;
		joy1_buttons[6]=ji.wButtons&JOY_BUTTON7;
		joy1_buttons[7]=ji.wButtons&JOY_BUTTON8;
		joy1_buttons[8]=ji.wButtons&JOY_BUTTON9;
		joy1_buttons[9]=ji.wButtons&JOY_BUTTON10;
		joy1_buttons[10]=ji.wButtons&JOY_BUTTON11;
		joy1_buttons[11]=ji.wButtons&JOY_BUTTON12;
	}

	if(joyGetPos(JOYSTICKID2,&ji)==JOYERR_NOERROR)
	{
		joy2_x_pos = ji.wXpos;
		joy2_y_pos = ji.wYpos;
		joy2_buttons[0]=ji.wButtons&JOY_BUTTON1;
		joy2_buttons[1]=ji.wButtons&JOY_BUTTON2;
		joy2_buttons[2]=ji.wButtons&JOY_BUTTON3;
		joy2_buttons[3]=ji.wButtons&JOY_BUTTON4;
		joy2_buttons[4]=ji.wButtons&JOY_BUTTON5;
		joy2_buttons[5]=ji.wButtons&JOY_BUTTON6;
		joy2_buttons[6]=ji.wButtons&JOY_BUTTON7;
		joy2_buttons[7]=ji.wButtons&JOY_BUTTON8;
		joy2_buttons[8]=ji.wButtons&JOY_BUTTON9;
		joy2_buttons[9]=ji.wButtons&JOY_BUTTON10;
		joy2_buttons[10]=ji.wButtons&JOY_BUTTON11;
		joy2_buttons[11]=ji.wButtons&JOY_BUTTON12;
	}
}

void UpdateControls()
{
	HRESULT hr;
	int y=0;
	DIDEVICEOBJECTDATA rgod[64];
	DWORD cod=64, iod;
	
	HandleMessages();
	ReadMouse();
	hr = di_keyb -> GetDeviceData(sizeof(DIDEVICEOBJECTDATA),rgod, &cod, 0);	//retrive data
	if(hr!=DI_OK&&hr!=DI_BUFFEROVERFLOW)
	{
		hr = di_keyb -> Acquire();
		if(!SUCCEEDED(hr))
			if(hr==DIERR_OTHERAPPHASPRIO) goto nevermind;
	}
	else if(cod>0&&cod<=64)
	{
		for(iod=0;iod<cod;iod++)
		{
			if(rgod[iod].dwData==0x80)
			{
				key[rgod[iod].dwOfs]=(char) 0x80;
				last_pressed = (char) rgod[iod].dwOfs;
				if (last_pressed != repeatedkey)
				{
					key_timer = systemtime;
					key_repeater = 0;
					repeatedkey = last_pressed;
				}
			}
			if(rgod[iod].dwData==0)
			{				
				key[rgod[iod].dwOfs]=0;
				if (rgod[iod].dwOfs == (unsigned) repeatedkey)
				{
					key_timer = 0;
					key_repeater = 0;
					repeatedkey = 0;
				}
				switch(rgod[iod].dwOfs)
				{
				case 0xCB: key[SCAN_LEFT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xCD: key[SCAN_RIGHT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xD0: key[SCAN_DOWN]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 0xC8: key[SCAN_UP]=0; 
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 184: key[SCAN_ALT]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				case 156: key[SCAN_ENTER]=0;
					key_timer=0; key_repeater=0; repeatedkey=0;
					break;
				}
			}
		}
	}
	if(key[SCAN_ALT] && key[SCAN_X]) err("");
	if(key[SCAN_ALT] && key[SCAN_TAB])
	{
		key[SCAN_TAB]=0;
		key[SCAN_ALT]=0;
		repeatedkey=0;
		key[184]=0;
		if(last_pressed==SCAN_TAB) last_pressed=0;
	}
	if(key[0xCB]) { key[SCAN_LEFT]=(char)128; key[0xCB]=0; last_pressed=SCAN_LEFT; key_timer=systemtime; repeatedkey=SCAN_LEFT; key_repeater=0;}
	if(key[0xCD]) { key[SCAN_RIGHT]=(char)128; key[0xCD]=0; last_pressed=SCAN_RIGHT; key_timer=systemtime; repeatedkey=SCAN_RIGHT; key_repeater=0;}
	if(key[0xD0]) { key[SCAN_DOWN]=(char)128; key[0xD0]=0; last_pressed=SCAN_DOWN; key_timer=systemtime;repeatedkey=SCAN_DOWN; key_repeater=0;}
	if(key[0xC8]) { key[SCAN_UP]=(char)128; key[0xC8]=0; last_pressed=SCAN_UP; key_timer=systemtime; repeatedkey=SCAN_UP; key_repeater=0;}
	if(key[184]) { key[SCAN_ALT]=(char)128; key[184]=0; last_pressed=SCAN_ALT; key_timer=systemtime; repeatedkey=SCAN_ALT; key_repeater=0;}
	if(key[156]) { key[SCAN_ENTER]=(char)128; key[156]=0; last_pressed=SCAN_ENTER; key_timer=systemtime; repeatedkey=SCAN_ENTER; key_repeater=0;}

	if (last_pressed == SCAN_LEFT && key[SCAN_RIGHT]) key[SCAN_RIGHT]=0;
	if (last_pressed == SCAN_RIGHT && key[SCAN_LEFT]) key[SCAN_LEFT]=0;
	if (last_pressed == SCAN_UP && key[SCAN_DOWN]) key[SCAN_DOWN]=0;
	if (last_pressed == SCAN_DOWN && key[SCAN_UP]) key[SCAN_UP]=0;

nevermind:
	ReadJoystick();
	if(joy1_x_pos<(joy_x_range[0]/4+joy_x_min[0])) joy1_left=1; else joy1_left=0;
	if(joy1_x_pos>(joy_x_range[0]/4+joy_x_range[0]/2+joy_x_min[0])) joy1_right=1; else joy1_right=0;
	if(joy1_y_pos<(joy_y_range[0]/4+joy_y_min[0])) joy1_up=1; else joy1_up=0;
	if(joy1_y_pos>(joy_y_range[0]/4+joy_y_range[0]/2+joy_y_min[0])) joy1_down=1; else joy1_down=0;

	if(joy2_x_pos<(joy_x_range[0]/4+joy_x_min[0])) joy2_left=1; else joy2_left=0;
	if(joy2_x_pos>(joy_x_range[0]/4+joy_x_range[0]/2+joy_x_min[0])) joy2_right=1; else joy2_right=0;
	if(joy2_y_pos<(joy_y_range[0]/4+joy_y_min[0])) joy2_up=1; else joy2_up=0;
	if(joy2_y_pos>(joy_y_range[0]/4+joy_y_range[0]/2+joy_y_min[0])) joy2_down=1; else joy2_down=0;
}

extern HWND hMainWnd;
extern HINSTANCE hMainInst;

int InitKeyboard()
{
	HRESULT hr;
	static bool first = true;
   
	InitJoystick();
	hr = DirectInputCreate(hMainInst, DIRECTINPUT_VERSION, &dinput, NULL);
	if (FAILED(hr))
	{
		err("DirectInputCreate");
		return 0;
	}

	hr = dinput -> CreateDevice(GUID_SysKeyboard, &di_keyb, NULL);   
	if (FAILED(hr))
	{
        err("dinput CreateDevice");
		return 0;
	}

	hr = di_keyb -> SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
        err("dinput SetDataFormat");
		return 0;
	}

	hr = di_keyb -> SetCooperativeLevel(hMainWnd,DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr))
	{
        err("sinput SetCooperativeLevel");
		return 0;
	}

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = 64;

	hr = di_keyb -> SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	if (FAILED(hr))
	{
        err("Set buffer size");
		return 0;
	}
	return 1;
}

void ShutdownKeyboard(void)
{
   if (di_keyb)
   {
      di_keyb -> Unacquire();
      di_keyb -> Release();
      di_keyb = NULL;
   }
   if (dinput)
   {
      dinput -> Release();
      dinput = NULL;
   }
}

void ReadControls()
{ 
	UpdateControls();

	b1=0; b2=0; b3=0; b4=0;
    up=0; down=0; left=0; right=0;

	if (joy1_up) up=1;
	if (joy1_down) down=1;
	if (joy1_left) left=1;
	if (joy1_right) right=1;
	if (joy1_buttons[0]) b1=1;
	if (joy1_buttons[1]) b2=1;
	if (joy1_buttons[2]) b3=1;
	if (joy1_buttons[3]) b4=1;

	if (key[SCAN_UP]) up=1;
	if (key[SCAN_DOWN]) down=1;
	if (key[SCAN_LEFT]) left=1;
	if (key[SCAN_RIGHT]) right=1;
/*	if (key[kb1]) b1=1;
	if (key[kb2]) b2=1;
	if (key[kb3]) b3=1;
	if (key[kb4]) b4=1;*/

	if (key[SCAN_ENTER]) b1=1;
	if (key[SCAN_ALT]) b2=1;
	if (key[SCAN_ESC]) b3=1;
	if (key[SCAN_SPACE]) b4=1;

//	for (i=0;i<128;i++)
//	{                          /* -- ric: 03/May/98 -- */
//		key_map[i].pressed=0;
//		if (keyboard_map[i])
//			key_map[i].pressed=1;   // no keys are bound yet
//	}

	if (key[SCAN_F10])
    {
		key[SCAN_F10]=0;
//		ScreenShot();
	}
}