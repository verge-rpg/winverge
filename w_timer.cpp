// ****************************************************************
// * PCP                                                          *
// * Copyright (C)1999 Dark Nova Software (DNS)                   *
// * All Rights Reserved.                                         *
// *                                                              *
// * File: win_timer.cpp                                          *
// * Author: zeromus, vecna                                       *
// * Portability: Win32 only                                      *
// * Description: Win32 Timer code                                *
// ****************************************************************

#define TIMER_H
#include "pcp.h"

void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2);

// ***************************** Code *****************************

vTimer::vTimer(int hz, LPTIMECALLBACK TimeProc)
{
	if (currtimer) timeKillEvent(currtimer);
    currtimer = timeSetEvent(1000/hz,0,(LPTIMECALLBACK)TimeProc,0,TIME_PERIODIC);   
}

vTimer::~vTimer()
{
	timeKillEvent(currtimer);
    currtimer=0;
}

// ***************************** Data *****************************

int key_timer, key_repeater, repeatedkey;
int systemtime, timer, timer2, hooktimer=0;
vTimer *mainTimeEvent;
byte an=0,tickctr=0,sec=0,min=0,hr=0;

// ****************************************************************

void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	timer++;
	timer2++;
    systemtime++;
   	if (repeatedkey)
	if (key_timer + 20 < systemtime)
	{
		key_repeater++;
		if (key_repeater > 8)
		{
			last_pressed = repeatedkey;
			key[last_pressed] = 1;
			key_repeater = 0;
		}
	}

	UpdateSound();
	if (an) check_tileanimation();
	if (hooktimer) ExecuteHookedScript(hooktimer);

    tickctr++;
    if (tickctr == 100) { tickctr=0; sec++; }
    if (sec == 60)      { min++; sec=0; }
    if (min == 60)      { hr++; min=0; }
}

void ShutdownTimer(void)
{ 
	delete mainTimeEvent;
}

void InitTimer(int hz)
{
	systemtime = 0;
	timer = 0;
	mainTimeEvent = new vTimer(hz, TimeProc);
	atexit(ShutdownTimer); //, "Timer subsystem");
}
