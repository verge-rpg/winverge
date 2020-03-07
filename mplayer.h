#ifndef _MPLAYER_H_
#define _MPLAYER_H_

#include "uniform.h"

#define MUTE_EXCLUSIVE  32000
#define MUTE_INCLUSIVE  32001

// Cross-Platform Thread Synchronization Code

#if  defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <windows.h>
typedef CRITICAL_SECTION MM_MUTEX;
#elif UNIX
typedef pthread_mutex_t MM_MUTEX;
#else
typedef char MM_MUTEX;
#endif

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************
****** Player stuff: ******************************************************
**************************************************************************/

typedef struct MP_EFFMEM
{   // Effect memory structure.  Flag indicates whether this is a global or
    // local effect.  Sloppy, but works.
    UBYTE          flag;
    UNITRK_EFFECT  effect;
} MP_EFFMEM;

typedef struct
{   UBYTE pts;          // number of envelope points
    UBYTE susbeg;       // envelope sustain index begin
    UBYTE susend;       // envelope sustain index end
    UBYTE beg;          // envelope loop begin
    UBYTE end;          // envelope loop end
    ENVPT *env;         // envelope points

    // Dyamic data modified by the envelope handler
    
    UWORD p;            // current envelope counter
    int   a;            // envelope index a
    int   b;            // envelope index b
} ENVPR;


typedef struct _MP_SIZEOF
{   INSTRUMENT  *i;
    UNISAMPLE   *s;
    EXTSAMPLE   *es;
    UBYTE  sample;       // which instrument number
    SWORD  volume;       // output volume (vol + sampcol + instvol)
    SWORD  panning;      // panning position
    SBYTE  chanvol;      // channel's "global" volume
    UWORD  fadevol;      // fading volume rate
    UWORD  period;       // period to play the sample at
    UBYTE  volflg;       // volume envelope settings
    UBYTE  panflg;       // panning envelope settings
    UBYTE  pitflg;       // pitch envelope settings
    UBYTE  note;         // the audible note (as heard, direct rep of period)
    UBYTE  nna;          // New note action type + master/slave flags
    SWORD  handle;       // which sample-handle
    long   start;        // The start byte index in the sample
    UBYTE  kick;         // if true = sample has to be restarted
} MP_SIZEOF;

//  Struct MP_VOICE - Used by NNA only player (audio control.  AUDTMP is
//                  used for full effects control).
typedef struct _MP_VOICE
{   INSTRUMENT  *i;
    UNISAMPLE   *s;
    EXTSAMPLE   *es;
    UBYTE  sample;       // which instrument number

    SWORD  volume;       // output volume (vol + sampcol + instvol)
    SWORD  panning;      // panning position
    SBYTE  chanvol;      // channel's "global" volume
    UWORD  fadevol;      // fading volume rate
    UWORD  period;       // period to play the sample at

    UBYTE  volflg;       // volume envelope settings
    UBYTE  panflg;       // panning envelope settings
    UBYTE  pitflg;       // pitch envelope settings

    UBYTE  note;         // the audible note (as heard, direct rep of period)
    UBYTE  nna;          // New note action type + master/slave flags
    SWORD  handle;       // which sample-handle
    SLONG  start;        // The start byte index in the sample

    UBYTE  kick;         // if true = sample has to be restarted

    // ----------------------------------
    // Below here is info NOT in MP_CONTROL!!
    // ----------------------------------

    UBYTE  keyoff;        // if true = fade out and stuff

    ENVPR  venv;
    ENVPR  penv;
    ENVPR  cenv;

    UWORD  avibpos;      // autovibrato pos
    UWORD  aswppos;      // autovibrato sweep pos

    ULONG  totalvol;     // total volume of channel (before global mixings)

    BOOL   mflag;
    SWORD  masterchn;
    struct _MP_CONTROL *master;// index of "master" effects channel
} MP_VOICE;


typedef struct _MP_CONTROL
{   INSTRUMENT  *i;
    UNISAMPLE   *s;
    EXTSAMPLE   *es;
    UBYTE  sample;       // which instrument number

    SWORD  outvolume;    // output volume (vol + sampcol + instvol)
    SWORD  panning;      // panning position
    SBYTE  chanvol;      // channel's "global" volume
    UWORD  fadevol;      // fading volume rate
    UWORD  period;       // period to play the sample at

    UBYTE  volflg;       // volume envelope settings
    UBYTE  panflg;       // panning envelope settings
    UBYTE  pitflg;       // pitch envelope settings

    UBYTE  note;         // the audible note (as heard, direct rep of period)
    UBYTE  nna;          // New note action type + master/slave flags
    SWORD  handle;       // which sample-handle
    SLONG  start;        // The start byte index in the sample

    UBYTE  kick;         // if true = sample has to be restarted

    // ----------------------------------
    // Below here is info NOT in MP_VOICE!!
    // ----------------------------------

    UBYTE  keyoff;        // if true = fade out and stuff

    UBYTE  muted;        // if set, channel not played
    SWORD  volume;       // output volume (vol + sampcol + instvol)
    UBYTE  notedelay;    // (used for note delay)

    MP_VOICE *slave;      // Audio Slave of current effects control channel
    UBYTE  slavechn;      // Audio Slave of current effects control channel
    UBYTE  anote;         // the note that indexes the audible (note seen in tracker)
    SWORD  ownper;
    SWORD  ownvol;
    UBYTE  dca;           // duplicate check action
    UBYTE  dct;           // duplicate check type
    SBYTE  retrig;        // retrig value (0 means don't retrig)
    ULONG  speed;         // what finetune to use

    int    tmpvolume;     // tmp volume
    UWORD  tmpperiod;     // tmp period
    UWORD  wantedperiod;  // period to slide to (with effect 3 or 5)

    UBYTE  glissando;     // glissando (0 means off)
    UBYTE  wavecontrol;   //

    SBYTE  vibpos;        // current vibrato position
    SBYTE  trmpos;        // current tremolo position
    UBYTE  tremor;

    ULONG  soffset;       // last used low order of sample-offset (effect 9)

    UBYTE  panbwave;      // current panbrello waveform
    SBYTE  panbpos;       // current panbrello position

    UWORD  newsamp;       // set to 1 upon a sample / inst change

    // Pattern loop (PT E6x Effect) is now channel-based per PT-std.

    UWORD  rep_patpos;     // patternloop position (row)
    SWORD  rep_sngpos;     // patternloop position (pattern order)
    UWORD  pat_repcnt;     // times to loop

    // UniTrack Stuff (current row / position index)
    UBYTE  *row;           // row currently playing on this channel
    int    pos;            // current position to read from in a->row
    MP_EFFMEM *memory;     // memory space used by effects (munitrk.c).

    UWORD  offset_lo, offset_hi;
} MP_CONTROL;

#ifdef __MM_WINAMP__

// Pattern looping structure used for smooth skipping / song length prediction.
//  Winamp Plugin Only!

typedef struct PATLOOP
{   UWORD       rep_patpos;     // patternloop position (row)
    SWORD       rep_sngpos;     // patternloop position (pattern order)
    UWORD       pat_repcnt;     // times to loop
} PATLOOP;

#endif


// ============================================================================
// Structre: MPLAYER
//
//

#define PF_FORBID    1
#define PF_LOOP      2
#define PF_NNA       4

typedef struct MPLAYER
{
    UBYTE    flags;
    UNIMOD   *mf;               // the module associated with this information!
    int      numvoices;         // number of voices this song is allocated to use!

    // All following variables can be modified at any time.

    UBYTE       bpm;            // current beats-per-minute speed
    UWORD       sngspd;         // current song speed
    int         channel;        // current working channel in the module.
    int         volume;         // global volume (0-128)

    // The following variables are considered useful for reading, and should
    // not be directly modified by the end user.

    SWORD       panning[64];    // current channel panning positions
    UBYTE       chanvol[64];    // current channel volumes
    MP_CONTROL *control;        // Effects Channel information (pf->numchn alloc'ed)
    MP_VOICE   *voice;          // Voice information
    long        curtime;        // current time in the song in seconds
    UWORD       numrow;         // number of rows on current pattern
    UWORD       vbtick;         // tick counter (counts from 0 to sngspd)
    UWORD       patpos;         // current row number
    SWORD       sngpos;         // current song position.  This should not
                                // be modified directly.  Use MikMod_NextPosition,
                                // MikMod_PrevPosition, and MikMod_SetPosition.

    // The following variables should not be modified, and have information
    // that is pretty useless outside the internal player, so just ignore :)

    UWORD       patbrk;         // position where to start a new pattern
    UBYTE       patdly;         // patterndelay counter (command memory)
    UBYTE       patdly2;        // patterndelay counter (real one)
    UBYTE       framedly;       // frame-based patterndelay
    SWORD       posjmp;         // flag to indicate a position jump is needed...
                                //  changed since 1.00: now also indicates the
                                //  direction the position has to jump to:
                                //  0: Don't do anything
                                //  1: Jump back 1 position
                                //  2: Restart on current position
                                //  3: Jump forward 1 position

    UBYTE      *pos_played;     // Indicates if the position has been played.

    int         globtrk_pos;    // Global track current row
    UBYTE      *globtrk_row;    // global track!

#ifdef __MM_WINAMP__
    // Special WinAMP additions:

    // Advanced look-up table for skipping around in a song.  The state of the player is
    // recorded for each pattern by the pmstate structure.
    struct MPLAYER *state;
    int         pms_alloc;      // current max allocation (for reallocation stuffs)
    int         loopcount;      // special limited loopcount feature!
    
    PATLOOP    *patloop;        // Pattern loop effect memory (used by snglen.c)
#endif

    MD_VOICESET    *vs;         // voiceset this player instance is attached to.
    struct MPLAYER *next;

    MM_MUTEX       mutex;       // multithread mutex jazz..
} MPLAYER;


// MikMod Player Prototypes:
// ===========================================================

extern MPLAYER *Player_Create(UNIMOD *mf, UBYTE flags);
extern MPLAYER *Player_InitSong(UNIMOD *mf, MD_VOICESET *owner, UBYTE flags, uint maxvoices);
extern void     Player_FreeSong(MPLAYER *ps);
extern void     Player_Register(MD_VOICESET *vs, MPLAYER *ps, uint maxchan);
extern void     Player_Free(MPLAYER *ps);

extern BOOL     Player_Active(MPLAYER *ps);
extern void     Player_Deactivate(void);

extern void     Player_Start(MPLAYER *mp);
extern void     Player_Stop(MPLAYER *mp);
extern void     Player_Pause(MPLAYER *ps, BOOL nosilence);
extern void     Player_TogglePause(MPLAYER *mp, BOOL noslience);

extern void     Player_SetVolume(MPLAYER *mp, int volume);
extern BOOL     Player_Playing(MPLAYER *mp);
extern void     Player_NextPosition(MPLAYER *mp);
extern void     Player_PrevPosition(MPLAYER *mp);
extern void     Player_SetPosition(MPLAYER *mp, int pos, int row);

extern void     Player_Setpos_Time(MPLAYER *mp, long time);
extern void     Player_Mute(MPLAYER *mp, uint arg1, ...);
extern void     Player_Unmute(MPLAYER *mp, uint arg1, ...);
extern void     Player_ToggleMute(MPLAYER *mp, uint arg1, ...);
extern BOOL     Player_Muted(MPLAYER *mp, uint chan);
extern int      Player_GetChanVoice(MPLAYER *mp, uint chan);

extern UNIMOD  *Player_GetUnimod(void);

extern uint     Player_HandleTick(MD_VOICESET *vs);

//extern int    player_resolution;         // maximum resolution of the player's smooth skip
//extern BOOL   player_smoothskip;         // enable / disable smoothskip

extern void MP_Cleaner(MPLAYER *ps);
extern BOOL MP_BuildLookups(MPLAYER *ps);

// Defined in SNGLEN.C

extern void PredictSongLength(UNIMOD *mf);

#ifdef __cplusplus
}
#endif

#endif
