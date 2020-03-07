// Mikmod Sound System
// Probably Version 3.5 or 4 or something near there.
// Dated sometime in the early 21st century.
//
// Code by Jake Stine of Divine Entertainment.
//
// Description:
//   virtch.c externals defined for use by the mikmod drivers. I see little
//   reason for any non-driver applications to call these functions directly,
//   but feel free to anyway.  Otherwise, don't bother including this thing,
//   you probably don't need it!

#ifndef __VIRTCH_C__
#define __VIRTCH_C__

#include "mmtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
****** Virtual channel stuff: *********************************************
**************************************************************************/

#define VC_MAXVOICES 0xfffffful

extern BOOL    VC_Init(void);
extern void    VC_Exit(void);
extern void    VC_Preempt(void);
extern BOOL    VC_SetSoftVoices(uint num);
extern ULONG   VC_SampleSpace(int type);
extern ULONG   VC_SampleLength(int type, SAMPLOAD *s);

extern int     VC_SampleAlloc(uint length, uint *flags);
extern void   *VC_SampleGetPtr(uint handle);
extern int     VC_SampleLoad(SAMPLOAD *sload, int type);
extern void    VC_SampleUnload(uint handle);

extern int     VC_GetActiveVoices(void);

extern BOOL    VC_SetMode(uint mixspeed, uint mode, uint channels, uint cpumode);
extern void    VC_GetMode(uint *mixspeed, uint *mode, uint *channels, uint *cpumode);
extern void    VC_SetVolume(const MMVOLUME *volume);
extern void    VC_GetVolume(MMVOLUME *volume);

extern void    VC_VoiceSetVolume(uint voice, const MMVOLUME *volume);
extern void    VC_VoiceGetVolume(uint voice, MMVOLUME *volume);
extern void    VC_VoiceSetFrequency(uint voice, ulong frq);
extern ulong   VC_VoiceGetFrequency(uint voice);
extern void    VC_VoiceSetPosition(uint voice, ulong pos);
extern ulong   VC_VoiceGetPosition(uint voice);

extern void    VC_VoicePlay(uint voice, uint handle, uint start, uint length, int reppos, int repend, int suspos, int susend, uint flags);
extern void    VC_VoiceResume(uint voice);

extern void    VC_VoiceStop(uint voice);
extern BOOL    VC_VoiceStopped(uint voice);
extern void    VC_VoiceReleaseSustain(uint voice);
extern ULONG   VC_VoiceRealVolume(uint voice);

// These are functions the drivers use to update the mixing buffers.

extern void    VC_WriteSamples(MDRIVER *md, SBYTE *buf, long todo);
extern ULONG   VC_WriteBytes(MDRIVER *md, SBYTE *buf, long todo);
extern void    VC_SilenceBytes(SBYTE *buf, long todo);

#ifdef __cplusplus
}
#endif

#endif
