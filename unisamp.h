//  Mikmod's built in sample format.
//
//  This is tailored for use with basic sound effects for games and modules.

#ifndef _UNISAMP_H_
#define _UNISAMP_H_

#ifdef __cplusplus
extern "C" {
#endif


// ===========================================
//        Samples  and  Sample Loading
// ===========================================

typedef struct SAMPLE
{
//    MD_STREAM *stream;       // streaming information
    struct MDRIVER *md;
    int    handle;
    struct SAMPLE *owner;    // if not null, references to this sample
                             // will use owner->handle instead of handle.

    // These represent the default (overridable) values for playing the given sample.

    uint   speed;            // default playback speed (almost anything is legal)
    int    volume;           // default volume.  0 to 128 volume range
    int    panning;          // default panning. PAN_LEFT to PAN_RIGHT range

    // Samedata descriptor:
    // Modifying the length without reloading the sampledata properly can
    // result in pops or clicks when the sample ends.  All other values
    // can be modified with few or no adverse effects.

    int    length;           // length of the sample, in samples. (no modify)
    int    reppos,           // loopstart position (in samples)
           repend;           // loopend position (in samples)
    int    suspos,           // sustain loopstart (in samples)
           susend;           // sustain loopend (in samples)

    uint   flags;            // sample-looping and other flags.
} SAMPLE;


extern SAMPLE  *unisample_create(MDRIVER *md);
extern SAMPLE  *unisample_duplicate(SAMPLE *src);
extern void     unisample_free(SAMPLE *samp);


/**************************************************************************
****** Wavload stuff: *****************************************************
**************************************************************************/

SAMPLE *WAV_LoadFP(struct MDRIVER *md, MMSTREAM *mmfp);
SAMPLE *WAV_LoadFN(struct MDRIVER *md, const CHAR *filename);
void WAV_Free(SAMPLE *si);

#ifdef __cplusplus
}
#endif

#endif
