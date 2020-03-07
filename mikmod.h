/*

 Mikmod Sound System

  By Jake Stine of Divine Entertainment (1996-2000)

 Support:
  If you find problems with this code, send mail to:
    air@divent.org

 Distribution / Code rights:
  Use this source code in any fashion you see fit.  Giving me credit where
  credit is due is optional, depending on your own levels of integrity and
  honesty.

 -----------------------------------------
 Header: mikmod.h

  General stuffs needed to use or compile mikmod.  Of course.
  It is easier to list what things are NOT containted within this header
  file:

  a) module player (or any other player) stuffs.  See mplayer.h.
  b) unimod format/structure/object stuffs (unimod.h)
  c) mikmod streaming input/output and memory management (mmio.h,
     automatically included below).
  d) Mikmod typedefs, needed for compiling mikmod only (mmtypes.h)

*/

#ifndef MIKMOD_H
#define MIKMOD_H

#include "mmio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GLOBVOL_FULL  128
#define VOLUME_FULL   128

#define PAN_LEFT         (0-128)
#define PAN_RIGHT        128
#define PAN_FRONT        (0-128)
#define PAN_REAR         128
#define PAN_CENTER       0

#define PAN_SURROUND     512         // panning value for Dolby Surround


#define Mikmod_RegisterDriver(x) MD_RegisterDriver(&x)
#define Mikmod_RegisterLoader(x) ML_RegisterLoader(&x)
#define Mikmod_RegisterErrorHandler(x) _mmerr_sethandler(x)
#define Mikmod_PrecacheSamples() SL_LoadSamples()


// ==========================================================================
// Sample format [in-memory] flags:
// These match up to the bit format passed to the drivers.

// Notes:
//   SF Prefixes - Driver flags.  These describe the sample format to the driver
//     for both loading and playback.
//   SL Prefixes - Looping flags.

#define SL_LOOP             (1<<0)
#define SL_BIDI             (1<<1)
#define SL_REVERSE          (1<<2)
#define SL_SUSTAIN_LOOP     (1<<3)   // Sustain looping, as used by ImpulseTracker
#define SL_SUSTAIN_BIDI     (1<<4)   //   (I hope someone finds a use for this someday
#define SL_SUSTAIN_REVERSE  (1<<5)   //    considering how much work it was!)

#define SF_16BITS           (1<<0)
#define SF_STEREO           (1<<1)
#define SF_SIGNED           (1<<2)
#define SF_DELTA            (1<<3)
#define SF_BIG_ENDIAN       (1<<4)


// Used to specify special values for start in Voice_Play().
#define SF_START_CURRENT     -1


    // Struct : MD_STREAM
//  This is a streaming audio template structure, which can be attached to any
//  SAMPLE struct to turn that struct into an automatic streaming audio sample.

typedef struct MD_STREAM
{   void    (*callback)(struct MD_VOICESET *voiceset, uint voice, void *dest, uint len);
    void    *streaminfo;        // information set by and used by the streaming device.
    uint    blocksize;          // buffer size... (in samples)
    uint    numblocks;          // number of blocks (> 1 means block mode!)
} MD_STREAM;

// --> Struct : SAMPLOAD
//
// This structure contains all pertinent information to loading and properly
// converting a sample.

enum
{   DECOMPRESS_IT214 = 1,
    DECOMPRESS_MPEG3,
};

typedef struct SAMPLOAD
{   uint      length;         // length of the sample to load

    uint      infmt,          // format of the sample on disk (never changes)
              outfmt;         // format it will be in when loaded
    UBYTE     decompress;     // decompression flags.

    MMSTREAM    mmfp;           // file and seekpos of module
    long      seekpos;        // seek position within the file (-1 = current position)
    int       *handle;        // place to write the handle to

    int         scalefactor;
    struct SAMPLOAD *next;      // linked list hoopla.
} SAMPLOAD;

extern void SL_HalveSample(SAMPLOAD *s);
extern void SL_Sample8to16(SAMPLOAD *s);
extern void SL_Sample16to8(SAMPLOAD *s);
extern void SL_SampleSigned(SAMPLOAD *s);
extern void SL_SampleUnsigned(SAMPLOAD *s);
extern BOOL SL_LoadSamples(struct MDRIVER *md);
extern void SL_Load(void *buffer, SAMPLOAD *smp, int length);
extern BOOL SL_Init(SAMPLOAD *s);
extern void SL_Exit(SAMPLOAD *s);

extern SAMPLOAD *SL_RegisterSample(struct MDRIVER *md, int *handle, uint infmt, uint length, int decompress, MMSTREAM *fp, long seekpos);


/**************************************************************************
****** Driver stuff: ******************************************************
**************************************************************************/

#define  MM_STATIC    1
#define  MM_DYNAMIC   2

enum
{   MD_MONO = 1,
    MD_STEREO,
    MD_SURROUND,
    MD_QUADSOUND
};

// CPU mode options.  These will force the use of the selected mode, whether that
// cpu is available or not, so make sure you check first!  Speed enhancments are
// present in sample loading and software mixing.

enum
{   CPU_AUTODETECT = 0,
    CPU_MMX,
    CPU_3DNOW
};


// possible mixing mode bits:
// --------------------------
// These take effect only after Mikmod_Init or Mikmod_Reset.

#define DMODE_DEFAULT    (1ul<<16)    // use default/current settings.  Ignore all other flags

#define DMODE_16BITS          1         // enable 16 bit output
#define DMODE_NOCLICK         2         // enable declicker - uses volume micro ramps to remove clicks.
#define DMODE_REVERSE         4         // reverse stereo
#define DMODE_INTERP          8         // enable interpolation
#define DMODE_EXCLUSIVE      16         // enable exclusive (non-cooperative) mode.
#define DMODE_SAMPLE_8BIT    32         // force use of 8 bit samples only.
#define DMODE_SAMPLE_DYNAMIC 64         // force dynamic sample support

// driver structure:
// Each driver must have a structure of this type.

typedef struct MD_DEVICE
{   CHAR    *Name;
    CHAR    *Version;
    uint    HardVoiceLimit,       // Limit of hardware mixer voices for this driver
            SoftVoiceLimit;       // Limit of software mixer voices for this driver

    // Below is the 'private' stuff, to be used by the MDRIVER and the
    // driver modules themselves [commenting is my version of C++ private]

    struct MD_DEVICE  *next;

    // sample loading
    int     (*SampleAlloc)     (uint length, uint *flags);
    void   *(*SampleGetPtr)    (uint handle);
    int     (*SampleLoad)      (SAMPLOAD *s, int type);
    void    (*SampleUnLoad)    (uint handle);
    ULONG   (*FreeSampleSpace) (int type);
    ULONG   (*RealSampleLength)(int type, SAMPLOAD *s);

    // detection and initialization
    BOOL    (*IsPresent)       (void);
    BOOL    (*Init)            (void *optstr);  // init driver using curent mode settings.
    void    (*Exit)            (void);
    void    (*Update)          (struct MDRIVER *md); // update driver (for polling-based updates).
    void    (*Preempt)         (void);          // request update preemption and player resync

    BOOL    (*SetHardVoices)   (uint num);
    BOOL    (*SetSoftVoices)   (uint num);

    // GetMode and Setmode.  They don't use structures because Zero and Vecna
    // are sick of me making structs for everything.
    // Notes:
    //   SetMode can be used at any time to re-configure the driver settings.
    //   Some drivers will reconfigure, others may just ignore it.  Depends
    //   on the driver.

    BOOL    (*SetMode)         (uint mixspeed, uint mode, uint channels, uint cpumode);
    void    (*GetMode)         (uint *mixspeed, uint *mode, uint *channels, uint *cpumode);

    // Driver volume system (quadsound support!)
    //  (There is no direct panning function because all panning is done 
    //  through manipulation of the four volumes).

    void    (*SetVolume)       (const MMVOLUME *volume);
    void    (*GetVolume)       (MMVOLUME *volume);

    // Voice control and Voice information
    
    int     (*GetActiveVoices)    (void);

    void    (*VoiceSetVolume)     (uint voice, const MMVOLUME *volume);
    void    (*VoiceGetVolume)     (uint voice, MMVOLUME *volume);
    void    (*VoiceSetFrequency)  (uint voice, ulong frq);
    ulong   (*VoiceGetFrequency)  (uint voice);
    void    (*VoiceSetPosition)   (uint voice, ulong pos);
    ulong   (*VoiceGetPosition)   (uint voice);

    void    (*VoicePlay)          (uint voice, uint handle, uint start, uint length, int reppos, int repend, int suspos, int susend, uint flags);
    void    (*VoiceResume)        (uint voice);
    void    (*VoiceStop)          (uint voice);
    BOOL    (*VoiceStopped)       (uint voice);
    void    (*VoiceReleaseSustain)(uint voice);
    ulong   (*VoiceRealVolume)    (uint voice);

    void    (*WipeBuffers)        (void);
} MD_DEVICE;


// ==========================================================================
// VoiceSets!  The Magic of Modular Design!
//
// Each voiceset has set characteristics, including a couple flags (most
// specifically, the flag for hardware or software mixing, if available), and
// volume/voice descriptors.
//
// The voicesets must all be registered with the driver prior to initial-
// ization, and are then referenced via an integer handle.

#define MDVS_DYNAMIC      0      // software mixing enable
#define MDVS_STATIC       1      // preferred hardware mixing
#define MDVS_PLAYER       2      // enable the player (if (*player) is not NULL)
#define MDVS_FROZEN       4      // Freeze all voice activity on the voiceset.
#define MDVS_STREAM       8      // streaming voice set

#define MDVD_CRITICAL     1      // Voice is marked as critical
#define MDVD_PAUSED       2      // voice is suspended (paused) rather than off.


typedef struct _VS_STREAM
{   void    (*callback)(struct MD_VOICESET *voiceset, uint voice, void *dest, uint len);
    void    *streaminfo;        // information set by and used by the streaming device.

    uint    blocksize;          // buffer size...
    uint    numblocks;
    uint    handle;             // sample handle allocated for streaming
    ulong   oldpos;             // used for hard-headed streaming mode.
} _VS_STREAM;


typedef struct MD_VOICE_DESCRIPTOR
{   int     voice;             // reference/index to the 'real' voice
    int     volume;            // mdriver's pre-vs->volume modified volumes!
    int     pan_lr, pan_fb;    // panning values.  left/right and front/back.

    uint    flags;             // describes each voice

    // Streaming audio options:
    // Allows the user to put their own user-defineable streaming data into the
    // sample buffer, rather than mikmod filling it itself from precached
    // SAMPLEs.

    _VS_STREAM *stream;
} MD_VDESC;

typedef struct MD_VOICESET
{   
    // premix modification options:
    // Procedures are available for modifying the sample data before the
    // mixer adds it into the mixing buffer.  This is intended for streaming
    // audio and the option to add additional effects, such as flange or reverb.

    void    (*premix8)(struct MD_VOICESET *voiceset, uint voice, UBYTE *dest, uint len);
    void    (*premix16)(struct MD_VOICESET *voiceset, uint voice, SWORD *dest, uint len);

    // Streaming audio options:
    // Allows the user to put their own user-defineable streaming data into the
    // sample buffer, rather than mikmod filling it itself from precached
    // SAMPLEs.  This information is used for any voiceset that has the MDVS_STREAM
    // flag set.


    // Music options:
    // Mikmod includes support for playing music in an accurate and efficient
    // manner.  The mixer will ensure that all players are updated in appro-
    // priate fashion.

    uint    (*player)(struct MD_VOICESET *voiceset);
    void     *playerinfo;      // info block allocated for and used by the player.

    // MDRIVER Private stuff:
    //  Changing this stuff will either cause very bad, or simply unpredictable
    //  behaviour.  Please use the provided VoiceSet_class of API functions
    //  to modify this jazz.

    struct MD_VOICESET
             *next, *nextchild,
             *owner,
             *children;

    UBYTE    flags;            // voiceset flags
    int      volume;           // running volume of this voiceset.
    int      pan_lr, pan_fb;
    uint     voices;           // number of voices allocated to this voiceset.
    MD_VDESC *vdesc;           // voice descriptors + voice lookup table.

    int      countdown;        // milliseconds until the next call to this player.
    int      sfxpool;          // sound effects voice-search

    struct MDRIVER *md;        // the driver this voiceset is attached to.
} MD_VOICESET;

// MDRIVER
// The info in this structure, after it has been returned from Mikmod_Init, will be
// properly updated whenever the status of the driver changes (for whatever reason
// that may be).  Note that none of these should ever be changed manually.  Use
// the appropriate Mikmod_* functions to alter the driver state.

typedef struct MDRIVER
{
    MD_DEVICE device;      // device info / function pointers.
    
    // hardvoices and softvoices:
    //  Marked volatile because, well.. they are!  A player thread could change them at any time,
    //  for example.
    
    volatile
    int      hardvoices,    // total hardware voices allocated (-1 means hardware is not available!)
             softvoices;    // total software voices allocated (-1 means software is not available!)

    int      volume;        // Global sound volume (0-128)
    int      pan;           // panning position (PAN_LEFT to PAN_RIGHT)
    int      pansep;        // 0 == mono, 128 == 100% (full left/right)

    // Everything from here on down is generally static, unless the user
    // requests a driver mode chnage manually.

    CHAR    *name;          // name of this device
    CHAR    *optstr;        // options string, used by some drivers.

    uint     mode;          // Mode.  See DMODE_? flags above
    uint     mixspeed;      // mixing/sampling rate.
    uint     latency;       // set latency, in milliseconds

    // Voiceset Information

    MD_VOICESET  *voiceset;

    // Mdriver Voice Management:
    //  These indicate which voices of the two voice types have been assigned to
    //  voice sets, and which voice sets they are allocated to.  A value of -1 is
    //  unassigned.  Any other value is the index of the voiceset in use.

    MD_VOICESET **vmh,      // voice management, hardware (NULL if no hardware supported)
                **vms;      // voice management, software (NULL if no software supported)

    BOOL    isplaying;
} MDRIVER;

// main driver prototypes:

extern void       Mikmod_RegisterAllDrivers(void);
extern int        Mikmod_GetNumDrivers(void);
extern MD_DEVICE *Mikmod_DriverInfo(int driver);

extern void     Mikmod_SamplePrecacheFP(MDRIVER *md, int *handle, uint infmt, uint length, int decompress, FILE *fp, long seekpos);
extern void     Mikmod_SamplePrecacheMem(MDRIVER *md, int *handle, uint infmt, uint length, int decompress, void *data);

//extern int      Mikmod_PlaySound(MD_VOICESET *vs, SAMPLE *s, uint start, uint flags);

extern MDRIVER *Mikmod_Init(uint mixspeed, uint latency, void *optstr, uint chanmode, uint cpumode, uint drvmode);
extern void     Mikmod_Exit(MDRIVER *md);
extern BOOL     Mikmod_Reset(MDRIVER *md);
extern BOOL     Mikmod_Active(MDRIVER *md);
extern void     Mikmod_RegisterPlayer(MDRIVER *md, void (*plr)(void));
extern void     Mikmod_Update(MDRIVER *md);
extern int      Mikmod_GetActiveVoices(MDRIVER *md);
extern void     Mikmod_WipeBuffers(MDRIVER *md);

// Voiceset crapola -- from -- VOICESET.C --

extern MD_VOICESET *Voiceset_Create(MDRIVER *drv, MD_VOICESET *owner, uint voices, UBYTE flags);
extern void         Voiceset_SetPlayer(MD_VOICESET *vs, uint (*player)(struct MD_VOICESET *voiceset), void *playerinfo);
extern MD_VOICESET *Voiceset_CreatePlayer(MDRIVER *md, MD_VOICESET *owner, uint voices, uint (*player)(struct MD_VOICESET *voiceset), void *playerinfo, BOOL hardware);
extern void         Voiceset_Free(MD_VOICESET *vs);
extern int          Voiceset_SetNumVoices(MD_VOICESET *vs, uint voices);
extern void         Voiceset_SetVolume(MD_VOICESET *vs, int volume);

extern void     Voiceset_PlayStart(MD_VOICESET *vs);
extern void     Voiceset_PlayStop(MD_VOICESET *vs);
extern void     Voiceset_Reset(MD_VOICESET *vs);
extern void     Voiceset_EnableOutput(MD_VOICESET *vs);
extern void     Voiceset_DisableOutput(MD_VOICESET *vs);

// MD_Player - Voiceset Player Update Procedure
extern ulong    MD_Player(MDRIVER *drv, ulong timepass);

extern void     Voice_SetVolume(MD_VOICESET *vs, uint voice, int ivol);
extern int      Voice_GetVolume(MD_VOICESET *vs, uint voice);
extern void     Voice_SetPosition(MD_VOICESET *vs, uint voice, long pos);
extern long     Voice_GetPosition(MD_VOICESET *vs, uint voice);
extern void     Voice_SetFrequency(MD_VOICESET *vs, uint voice, long frq);
extern long     Voice_GetFrequency(MD_VOICESET *vs, uint voice);
extern void     Voice_SetPanning(MD_VOICESET *vs, uint voice, int pan_lr, int pan_fb);
extern void     Voice_GetPanning(MD_VOICESET *vs, uint voice, int *pan_lr, int *pan_fb);

extern void     Voice_Play(MD_VOICESET *vs, uint voice, int handle, int start, int length, int reppos, int repend, int suspos, int susend, uint flags);
extern void     Voice_Pause(MD_VOICESET *vs, uint voice);
extern void     Voice_Resume(MD_VOICESET *vs, uint voice);
//extern void     Voice_PlaySample(MD_VOICESET *vs, uint voice, SAMPLE *s, int start);
//extern void     Voice_PlaySampleEx(MD_VOICESET *vs, uint voice, SAMPLE *s, int start, uint speed, int volume, int pan);
extern void     Voice_Stop(MD_VOICESET *vs, uint voice);
extern void     Voice_ReleaseSustain(MD_VOICESET *vs, uint voice);
extern BOOL     Voice_Stopped(MD_VOICESET *vs, uint voice);
extern BOOL     Voice_Paused(MD_VOICESET *vs, uint voice);

extern ulong    Voice_RealVolume(MD_VOICESET *vs, uint voice);

// Lower level 'stuff'
// -------------------

// use macro Mikmod_RegisterDriver instead
extern void     MD_RegisterDriver(MD_DEVICE *drv);

// loads a sample based on the give SAMPLOAD structure into the driver
extern int      MD_SampleLoad(MDRIVER *md, int type, SAMPLOAD *s);

// allocates a chunk of memory without loading anything into it.
extern int      MD_SampleAlloc(MDRIVER *md, int type);
extern void     MD_SampleUnload(MDRIVER *md, int handle);

// returns the actual amount of memory a sample will use.  Necessary because
// some drivers may only support 8 or 16 bit data, or may not support stereo.
extern ulong    MD_SampleLength(MDRIVER *md, int type, SAMPLOAD *s);
extern ulong    MD_SampleSpace(MDRIVER *md, int type);  // free sample memory in driver.


// ================================================================
// Declare external drivers
// (these all come with various mikmod packages):
// ================================================================

// Multi-platform jazz...
extern MD_DEVICE drv_nos;      // nosound driver, REQUIRED!
extern MD_DEVICE drv_raw;      // raw file output driver [music.raw]
extern MD_DEVICE drv_wav;      // RIFF WAVE file output driver [music.wav]

// Windows95 drivers:
extern MD_DEVICE drv_win;      // windows media (waveout) driver
extern MD_DEVICE drv_ds;       // Directsound driver
extern MD_DEVICE drv_dsaccel;  // accelerated directsound driver (sucks!)

// MS_DOS Drivers:
extern MD_DEVICE drv_awe;      // experimental SB-AWE driver
extern MD_DEVICE drv_gus;      // gravis ultrasound driver [hardware / software mixing]
extern MD_DEVICE drv_gus2;     // gravis ultrasound driver [hardware mixing only]
extern MD_DEVICE drv_sb;       // soundblaster 1.5 / 2.0 DSP driver
extern MD_DEVICE drv_sbpro;    // soundblaster Pro DSP driver
extern MD_DEVICE drv_sb16;     // soundblaster 16 DSP driver
extern MD_DEVICE drv_ss;       // ensoniq soundscape driver
extern MD_DEVICE drv_pas;      // PAS16 driver
extern MD_DEVICE drv_wss;      // Windows Sound System driver

// Various UNIX/Linux drivers:
extern MD_DEVICE drv_vox;      // linux voxware driver
extern MD_DEVICE drv_AF;       // Dec Alpha AudioFile driver
extern MD_DEVICE drv_sun;      // Sun driver
extern MD_DEVICE drv_os2;      // Os2 driver
extern MD_DEVICE drv_hp;       // HP-UX /dev/audio driver
extern MD_DEVICE drv_aix;      // AIX audio-device driver
extern MD_DEVICE drv_sgi;      // SGI audio-device driver
extern MD_DEVICE drv_tim;      // timing driver
extern MD_DEVICE drv_ultra;    // ultra driver for linux


extern void VC_EnableInterpolation(int handle);
extern void VC_DisableInterpolation(int handle);

#ifdef __cplusplus
}
#endif

#endif
