#ifndef PCP_H
#define PCP_H

// system includes

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <windows.h>
#include <mmsystem.h>

// Macros

#define SWAP(a,b) { a=a-b; b=a+b; a=b-a; }
#define Public public:
#define Private private:
#define Protected protected:

// Primary PCP data types

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   quad;

// Includes

#include "a_err.h"
#include "a_string.h"
#include "w_main.h"
#include "w_sound.h"
#include "scancode.h"

#include "g_vga.h"
#include "g_engine.h"
#include "g_render.h"
#include "g_menu.h"
#include "g_vc.h"
#include "g_vclib.h"
#include "g_menu.h"
#include "g_pcx.h"

extern int mx, my, mb_Left, mb_Right, mb, mw;
/*************************/
extern void InitTimer(int hz);
extern void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);
class vTimer
{
	Private MMRESULT currtimer;
	Public vTimer(int hz, LPTIMECALLBACK TimeProc);
	Public ~vTimer();
};
extern vTimer *mainTimeEvent;
extern int timer, timer2, systemtime, hooktimer;
extern int key_timer, key_repeater, repeatedkey;
extern byte an,tickctr,sec,min,hr;

extern int  InitKeyboard();
extern void ShutdownKeyboard();
extern void UpdateControls();
extern void ReadControls();
extern unsigned char last_pressed;
extern char key[256];
extern quad joy1_buttons[15];
extern byte joy1_up, joy1_down, joy1_left, joy1_right;
extern quad joy2_buttons[15];
extern byte joy2_up, joy2_down, joy2_left, joy2_right;
extern byte b1,b2,b3,b4,up,down,left,right;
#endif
