/*

 MikMod Sound System

  By Jake Stine of Divine Entertainment (1996-1999)

 Support:
  If you find problems with this code, send mail to:
    air@divent.simplenet.com

 Distribution / Code rights:
  Use this source code in any fashion you see fit.  Giving me credit where
  credit is due is optional, depending on your own levels of integrity and
  honesty.

 -----------------------------------------
 Header: uniform.h

  Uniformat structures and procedure prototypes.

*/
  
#ifndef _UNIFORM_H_
#define _UNIFORM_H_

#include "unisamp.h"

#ifdef __cplusplus
extern "C" {
#endif


// Module-only Playback Flags

// PSF flags (Player Sample Format): 
//
// PSF_OWNPAN    -  Modules can optional enable or disable a sample from using
//   its set default panning positon, in which case the current channel panning
//   position is used.
// PSF_UST_LOOP  -  UST modules treat the sample loopstart/loop begin differently
//   from normal loops.  The sample only plays the looped portion, ignoring the
//   start of the sample completely.

#define PSF_OWNPAN          16384ul
#define PSF_UST_LOOP        32768ul

// UNISAMPLE.

typedef struct
{   uint   flags;           // looping and player flags!

    uint   speed;           // default playback speed (almost anything is legal)
    int    volume;          // 0 to 128 volume range
    int    panning;         // PAN_LEFT to PAN_RIGHT range
    int    handle;          // handle to the sample loaded into the driver

    uint   length;          // Length of the sample (samples, not bytes!)
    uint   loopstart,       // Sample looping smackdown!
           loopend,
           susbegin,        // sustain loop begin (in samples) \  Not Supported
           susend;          // sustain loop end                /      Yet!

    CHAR  *samplename;      // name of the sample    

    // Sample Loading information - relay info between the loaders and
    // unimod.c - like wow dude!

    uint   format;          // diskformat flags, describe sample prior to loading!
    uint   compress;        // compression status of the sample.
    long   seekpos;         // seekpos within the module.

#ifdef __MM_WINAMP__
    BOOL   old16bits;        // For use by Winamp only.  Tracks the original
#endif                       // bit depth of samples prior to load/convert
} UNISAMPLE;

/**************************************************************************
****** Unitrack stuff: ****************************************************
**************************************************************************/

// Repeat / Length flags and masks
#define TFLG_GLOBLEN_MASK    127
#define TFLG_LENGTH_MASK      63
#define TFLG_NOTE             64
#define TFLG_EFFECT          128


//Effect Flags.
#define TFLG_EFFECT_MEMORY       1
#define TFLG_EFFECT_INFO         2
#define TFLG_PARAM_POSITIVE      4
#define TFLG_PARAM_NEGATIVE      8
#define TFLG_OVERRIDE_EFFECT    16
#define TFLG_OVERRIDE_FRAMEDLY  32


// Enumaerated UniMod Commands
//  Commands are divided into two segments:  Global commands and
//  channel commands.

enum      // Global UniMod commands.
{   UNI_GLOB_VOLUME = 1,
    UNI_GLOB_VOLSLIDE,
    UNI_GLOB_FINEVOLSLIDE,
    UNI_GLOB_TEMPO,
    UNI_GLOB_TEMPOSLIDE,
    UNI_GLOB_SPEED,
    UNI_GLOB_LOOPSET,
    UNI_GLOB_LOOP,
    UNI_GLOB_DELAY,
    UNI_GLOB_PATJUMP,
    UNI_GLOB_PATBREAK,
    UNI_GLOB_LAST
};

enum
{   UNI_ARPEGGIO = 1,
    UNI_VOLUME,
    UNI_CHANVOLUME,
    UNI_PANNING,
    UNI_VOLSLIDE,
    UNI_CHANVOLSLIDE,
    UNI_TREMOLO_SPEED,
    UNI_TREMOLO_DEPTH,
    UNI_TREMOR,

    UNI_PITCHSLIDE,
    UNI_VIBRATO_SPEED,
    UNI_VIBRATO_DEPTH,
    UNI_PORTAMENTO_LEGACY,
    UNI_PORTAMENTO,

    UNI_PANSLIDE,
    UNI_PANBRELLO_SPEED,
    UNI_PANBRELLO_DEPTH,

    UNI_VIBRATO_WAVEFORM,
    UNI_TREMOLO_WAVEFORM,
    UNI_PANBRELLO_WAVEFORM,

    UNI_NOTEKILL,
    UNI_NOTEDELAY,
    UNI_RETRIG,
    UNI_OFFSET_LEGACY,
    UNI_OFFSET,
    UNI_KEYOFF,
    UNI_KEYFADE,

    UNI_ENVELOPE_CONTROL,
    UNI_NNA_CONTROL,
    UNI_NNA_CHILDREN,

    UNI_LAST
};

enum
{   UNIMEM_NONE = 0,
    PTMEM_PITCHSLIDEUP,
    PTMEM_PITCHSLIDEDN,
    PTMEM_VIBRATO_SPEED,
    PTMEM_VIBRATO_DEPTH,
    PTMEM_TREMOLO_SPEED,
    PTMEM_TREMOLO_DEPTH,
    PTMEM_TEMPO,
    PTMEM_PORTAMENTO,
    PTMEM_OFFSET,
    PTMEM_LAST
};

#pragma pack (push,1)
// These structures have 1 byte packing else they eat LOTS of memory (since we
// will be using a possible several thousand of these babies per module).

typedef union _BYTE_MOB
{   UBYTE      u;
    SBYTE      s;
} BYTE_MOB;

typedef union _WORD_MOB
{   UWORD      u;
    SWORD      s;
} WORD_MOB;

typedef union _INT_MOB
{   ULONG      u;
    SLONG      s;

    struct _WORD_PAIR
    {   WORD_MOB  loword;
        WORD_MOB  hiword;
    };

    struct _BYTE_BUNCH
    {   UBYTE     byte_a, byte_b,
                  byte_c, byte_d;
    };
} INT_MOB;

// Unitrk Framedelay bitwise constants.
#define UFD_TICKMASK  127
#define UFD_RUNONCE   128

typedef struct _UNITRK_NOTE
{   UBYTE    note, inst;
} UNITRK_NOTE;

// This is mighty creul of me.  Two structures of almost the same name.
// The first one (UNITRK_EFFECT) is used by the player and the loaders.  It is a memory-less
// effect structure.  The second (UTRK_EFFECT) has memory info in it, and is for use
// by trackers and info viewers.

typedef struct _UNITRK_EFFECT
{   INT_MOB  param;             // Parameter
    UBYTE    effect;            // Effect 
    UBYTE    framedly;          // framedelay for effect.  If UFD_RUNONCE is set,
                                // command is run once on the given tick.
} UNITRK_EFFECT;

//--------------
// UE_EFFECT - Unitrk Extended Effect Structure.
//  This structure contains the memory information in addition to the
//  standard effect information.
//
// Flags:
//  UEF_GLOBAL  - Is a global effect, which means the chnslot var is valid.
//  UEF_MEMORY  - Set if it is a memory (ie, no effect data).  Effect data will
//                be invaild if this flag is set.

#define UEF_GLOBAL  1
#define UEF_MEMORY  2

typedef struct _UE_EFFECT
{   int           flags;
    UNITRK_EFFECT effect;
    int           memslot,      // Memory slot effect uses.
                  memchan;      // Only valid if UEF_GLOBAL flag is set.
} UE_EFFECT;

typedef struct _UNITRK_ROW
{   int    pos;
    UBYTE *row;
} UNITRK_ROW;


#pragma pack (pop)


// ------------------------------------
// ------- Replay Flags Stuff: --------
// ------------------------------------


// New Note Action Flags (used by samps and insts)

#define NNA_CUT         0
#define NNA_CONTINUE    1
#define NNA_OFF         2
#define NNA_FADE        3

// Envelope flags:
// Note on EF_VOLENV:  That specifies a 'volume envelope' stype of handling,
// where the fadeout is activated when the end of the envelope is reached.
// Used by ITs, but not by XMs.

#define EF_ON           1
#define EF_SUSTAIN      2
#define EF_LOOP         4
#define EF_VOLENV       8
#define EF_INCLUSIVE   16      // ITs are inclusive, XMs are exclusive.

// Duplicate Note Check Flags

#define DCT_OFF         0
#define DCT_NOTE        1                         
#define DCT_SAMPLE      2                         
#define DCT_INST        3           

#define DCA_CUT         0
#define DCA_OFF         1
#define DCA_FADE        2


// Miscellaneous flags

#define KEY_KICK        0
#define KEY_OFF         1
#define KEY_FADE        2
#define KEY_KILL        3

#define AV_IT           1   // IT vs. XM vibrato info


typedef struct ENVPT
{   UWORD pos;
    SWORD val;
} ENVPT;


// ---------------------------------------
// ------- Extended Sample stuff: --------
// ---------------------------------------

// Each extanded sample info block extends the information for each cor-
// responding core SAMPLE structure in the samples[] array in the UNIMOD
// struct (ie same index for extsamples and samples arrays - info for same
// sample).  

typedef struct EXTSAMPLE
{   UBYTE  globvol;          // global volume
    UBYTE  nnatype;

    SBYTE  pitpansep;        // pitch pan separation (-64 to 64)
    UBYTE  pitpancenter;     // pitch pan center (0 to 119)
    UBYTE  rvolvar;          // random volume varations (0 - 100%)
    UBYTE  rpanvar;          // random panning varations (0 - 100%)
    UWORD  volfade;

    UBYTE  volflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  volpts;
    UBYTE  volsusbeg;
    UBYTE  volsusend;
    UBYTE  volbeg;
    UBYTE  volend;
    ENVPT  *volenv;

    UBYTE  panflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  panpts;
    UBYTE  pansusbeg;
    UBYTE  pansusend;
    UBYTE  panbeg;
    UBYTE  panend;
    ENVPT  *panenv;

    UBYTE  pitflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  pitpts;
    UBYTE  pitsusbeg;
    UBYTE  pitsusend;
    UBYTE  pitbeg;
    UBYTE  pitend;
    ENVPT  *pitenv;

    UBYTE  vibflags;         // set = IT vibrato type, clear = XM type
    UBYTE  vibtype;
    UBYTE  vibsweep;
    UBYTE  vibdepth;
    UBYTE  vibrate;

    UWORD  avibpos;          // autovibrato pos [internal use only]
} EXTSAMPLE;


// ----------------------------------
// ------- Instrument stuff: --------
// ----------------------------------

// Instrument format flags

#define IF_OWNPAN       1

typedef struct INSTRUMENT
{   UBYTE  flags;

    UBYTE  samplenumber[120];
    UBYTE  samplenote[120];

    UBYTE  dct;              // duplicate check type
    UBYTE  dca;              // duplicate check action

    UBYTE  globvol;          // global volume
    SWORD  panning;          // instrument-based panning var
    UBYTE  nnatype;
    
    SBYTE  pitpansep;        // pitch pan separation (-64 to 64)
    UBYTE  pitpancenter;     // pitch pan center (0 to 119)
    UBYTE  rvolvar;          // random volume varations (0 - 100%)
    UBYTE  rpanvar;          // random panning varations (0 - 100%)

    UWORD  volfade;          // fadeout rate

    UBYTE  volflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  volpts;
    UBYTE  volsusbeg;
    UBYTE  volsusend;
    UBYTE  volbeg;
    UBYTE  volend;
    ENVPT  volenv[32];

    UBYTE  panflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  panpts;
    UBYTE  pansusbeg;
    UBYTE  pansusend;
    UBYTE  panbeg;
    UBYTE  panend;
    ENVPT  panenv[32];

    UBYTE  pitflg;           // bit 0: on 1: sustain 2: loop
    UBYTE  pitpts;
    UBYTE  pitsusbeg;
    UBYTE  pitsusend;
    UBYTE  pitbeg;
    UBYTE  pitend;
    ENVPT  pitenv[32];

    UBYTE  vibflags;
    UBYTE  vibtype;
    UBYTE  vibsweep;
    UBYTE  vibdepth;
    UBYTE  vibrate;

    CHAR   *insname;

    UWORD  avibpos;          // autovibrato pos [internal use only]
} INSTRUMENT;


/******************************************************
******** MikMod UniMod type: **************************
*******************************************************/

// -- UniMod flags --

#define UF_XMPERIODS        1     // XM periods / finetuning
#define UF_LINEAR           2     // LINEAR periods
#define UF_LINEAR_PITENV    4     // linear slides on envelopes?
#define UF_INST             8     // Instruments are used
#define UF_NNA             16     // New Note Actions used (set numvoices rather than numchn)

#define UF_NO_PANNING      32     // Disable 8xx panning effects
#define UF_NO_EXTSPEED     64     // Disable PT extended speed.


typedef struct UNIMOD
{
    // This section of elements are all file-storage related.
    // all of this information can be found in the UNIMOD disk format.
    // For further details about there variables, see the MikMod Docs.

    uint        flags;          // See UniMod Flags above
    uint        numchn;         // number of module channels
    uint        numvoices;      // voices allocated to NNA playback
    long        songlen;        // length of the song as predicted by PredictSongLength (1/1000th of a second).
    long        filesize;       // size of the module file, in bytes
    uint        memsize;        // number of per-channel effect memory entires (a->memory) to allocate.

    UWORD       numpos;         // number of positions in this song
    UWORD       numpat;         // number of patterns in this song
    UWORD       numtrk;         // number of tracks
    UWORD       numins;         // number of instruments
    UWORD       numsmp;         // number of samples
    UWORD       reppos;         // restart position
    UBYTE       initspeed;      // initial song speed
    UBYTE       inittempo;      // initial song tempo
    UBYTE       initvolume;     // initial global volume (0 - 128)
    SWORD       panning[64];    // 64 initial panning positions
    UBYTE       chanvol[64];    // 64 initial channel volumes
    CHAR       *songname;       // name of the song
    CHAR       *filename;       // name of the file this song came from (optional)
    CHAR       *composer;       // name of the composer
    CHAR       *comment;        // module comments
    CHAR       *modtype;        // string type of module loaded

    // Instruments and Samples!

    INSTRUMENT *instruments;    // all instruments
    UNISAMPLE  *samples;        // all samples
    EXTSAMPLE  *extsamples;     // all extended sample-info (corresponds to each sample)

    // UniTrack related -> Tracks and reading them!
        
    UBYTE     **tracks;         // array of numtrk pointers to tracks
    UBYTE     **globtracks;     // array of numpat pointers to global tracks
    UWORD      *patterns;       // array of Patterns [index to tracks for each channel].
    UWORD      *pattrows;       // array of number of rows for each pattern
    UWORD      *positions;      // all positions

    //void      (*localeffects)(int effect, INT_MOB dat);
    //void      (*globaleffects)(int effect, INT_MOB dat);

    MDRIVER    *md;             // the driver this module is bound to.
} UNIMOD;


// ====================
// Unitrack prototypes:
// ====================

extern UBYTE utrk_blanktrack[2];

extern void pt_write_effect(unsigned int eff, unsigned int dat);
extern void pt_write_exx(unsigned int eff, unsigned int dat);
extern void pt_global_consolidate(UNIMOD *of, UBYTE **globtrack);

extern UBYTE *utrk_global_copy(UBYTE *track, int chn);


// These should be called by the player only (special coded to work
// directly with the unimod structure in a simple and efficient manner).

extern void    utrk_local_seek(UNITRK_ROW *urow, UBYTE *track, int row);
extern void    utrk_global_seek(UNITRK_ROW *urow, UBYTE *track, int row);
extern void    utrk_local_nextrow(UNITRK_ROW *urow);
extern void    utrk_global_nextrow(UNITRK_ROW *urow);
extern BOOL    utrk_local_geteffect(UE_EFFECT *eff, UNITRK_ROW *urow);
extern BOOL    utrk_global_geteffect(UE_EFFECT *eff, UNITRK_ROW *urow);
extern void    utrk_getnote(UNITRK_NOTE *note, UNITRK_ROW *urow);

extern BOOL    utrk_init(int nc);
extern void    utrk_cleanup(void);
//extern void    utrk_global_memflag(int memslot, int flags);
extern void    utrk_local_memflag(int memslot, int eff, int fdly);

extern void    utrk_reset(void);
extern void    utrk_settrack(int trk);
extern void    utrk_memory_reset(void);

extern void    utrk_write_global(UNITRK_EFFECT *data, int memslot);
extern void    utrk_write_local(UNITRK_EFFECT *data, int memslot);
extern void    utrk_memory_global(UNITRK_EFFECT *data, int memslot);
extern void    utrk_memory_local(UNITRK_EFFECT *data, int memslot, int signs);

extern void    utrk_write_inst(unsigned int ins);
extern void    utrk_write_note(unsigned int note);
extern void    utrk_newline(void);
extern UBYTE   *utrk_dup_track(int chn);
extern UBYTE   *utrk_dup_global(void);
extern BOOL    utrk_dup_pattern(UNIMOD *mf);

extern UWORD   TrkLen(UBYTE *trk);
extern BOOL    MyCmp(UBYTE *a, UBYTE *b, UWORD l);


/***************************************************
****** Loader stuff: *******************************
****************************************************/

typedef void ML_HANDLE;

// loader structure:

typedef struct MLOADER
{   CHAR    *Type;
    CHAR    *Version;

    // Below is the 'private' stuff, to be used by the MDRIVER and the
    // driver modules themselves [commenting is my version of C++]

    struct MLOADER *next;
    BOOL       (*Test)(MMSTREAM *modfp);
    ML_HANDLE *(*Init)(void);                // creates an instance handle for loading songs.
    void       (*Cleanup)(ML_HANDLE *inst);  // clean up (free) instance

    BOOL       (*Load)(ML_HANDLE *handle, UNIMOD *of, MMSTREAM *mmfile);
#ifndef __MM_WINAMP__
    CHAR      *(*LoadTitle)(MMSTREAM *mmfile);
#endif
} MLOADER;

// public loader variables:

extern UWORD  finetune[16];
extern UWORD  npertab[60];          // used by the original MOD loaders

// main loader prototypes:

extern void     MikMod_RegisterAllLoaders(void);
extern MLOADER *MikMod_LoaderInfo(int loader);
extern int      MikMod_GetNumDrivers(void);

extern UNIMOD  *Unimod_LoadFP(MDRIVER *md, MMSTREAM *modfp, MMSTREAM *smpfp, int mode);
extern UNIMOD  *Unimod_Load(MDRIVER *md, const CHAR *filename);
extern UNIMOD  *Unimod_LoadInfo(const CHAR *filename);
extern void     Unimod_Free(UNIMOD *mf);

extern void     ML_RegisterLoader(MLOADER *ldr);  // use the macro MikMod_RegisterLoader instead


// other loader prototypes: (used by the loader modules)

//extern BOOL    InitTracks(void);
extern void    AddTrack(UBYTE *tr);
extern BOOL    ReadComment(UNIMOD *of, UWORD len);
extern BOOL    AllocPositions(UNIMOD *of,int total);
extern BOOL    AllocPatterns(UNIMOD *of);
extern BOOL    AllocTracks(UNIMOD *of);
extern BOOL    AllocInstruments(UNIMOD *of);
extern BOOL    AllocSamples(UNIMOD *of, BOOL ext);
extern CHAR    *DupStr(CHAR *s, UWORD len);


// Declare external loaders:

extern MLOADER  load_uni;        // Internal UniMod Loader (Current version of UniMod only)
extern MLOADER  load_mod;        // Standard 31-instrument Module loader (Protracker, StarTracker, FastTracker, etc)
extern MLOADER  load_m15;        // 15-instrument (SoundTracker and Ultimate SoundTracker)
extern MLOADER  load_mtm;        // Multi-Tracker Module (by Renaissance)
extern MLOADER  load_s3m;        // ScreamTracker 3 (by Future Crew)
extern MLOADER  load_stm;        // ScreamTracker 2 (by Future Crew)
extern MLOADER  load_ult;        // UltraTracker 
extern MLOADER  load_xm;         // FastTracker 2 (by Trition)
extern MLOADER  load_it;         // Impulse Tracker (by Jeffrey Lim)
extern MLOADER  load_669;        // 669 and Extended-669 (by Tran / Renaissance)
extern MLOADER  load_dsm;        // DSIK internal module format
extern MLOADER  load_med;        // MMD0 and MMD1 Amiga MED modules (by OctaMED)
extern MLOADER  load_far;        // Farandole Composer Module

#ifdef __cplusplus
}
#endif

#endif

