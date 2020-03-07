/*
    FILE : TIMER.H
    
    DOS-based Timer Interrupt functions
    Includes defines for TIMER.C and VTIMER.C
*/

#ifndef TIMER_H
#define TIMER_H

#include "mmtypes.h"

#define VT_VRSYNC    1
#define VT_CALLOLD   2

#ifdef __cplusplus
extern "C" {
#endif

// Defined in TIMER.C

void Timer_Exit(void);
void Timer_Init(int rate, void (*handler)(void));
void Timer_SetSpeed(UWORD rate);
void Timer_Start(void);
void Timer_Stop(void);


// Defined in VTIMER.C
void VT_Exit(void);
void VT_Init(void);
SWORD VT_Alloc(void);
void VT_Free(SWORD handle);
void VT_SetHandler(SWORD handle,void (*handler)(void));
void VT_SetSpeed(SWORD handle,SLONG speed);
void VT_SetBPM(SWORD handle,UBYTE bpm);
void VT_Start(SWORD handle);
void VT_Stop(SWORD handle);
void VT_Mode(UBYTE flags);
void VT_V0Handler(void (*handler)(void));
void VT_V1Handler(void (*handler)(void));

#ifdef __cplusplus
}
#endif


extern volatile int framecount;
void framecounter(void);

#endif
