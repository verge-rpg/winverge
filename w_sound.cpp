#include "a_cache.h"
#include "pcp.h"
#include "mikmod.h"
#include "mplayer.h"

#include "mwav.h"
#include "mdsfx.h"

#include <vector>

class SongCacheEntry:public CacheEntry
{
public:
    UNIMOD      *mf;

    SongCacheEntry(SongCacheEntry *list, const char *n)
    {   name       = _strdup(n);
        next       = list;
        prev       = NULL;
        if(list) list->prev = this;
    };

    SongCacheEntry *Next()
    {   return (SongCacheEntry *)next;
    };
};


class SampleCacheEntry:public CacheEntry
{
public:
    SAMPLE     *handle;

    SampleCacheEntry(SampleCacheEntry *list, const char *n)
    {   name       = _strdup(n);
        next       = list;
        prev       = NULL;
        if(list) list->prev = this;
    };

    SampleCacheEntry *Next()
    {    return (SampleCacheEntry *)next;
    }

    SAMPLE *Fetch(const char *res)
    {
        return NULL;
    }
};

typedef struct
{   SongCacheEntry   *song;
    SampleCacheEntry *sample;
} SNDCACHE;


/*****************************/
//    ---> Publics <---

char playingsng[80] = {0};
int  numsamples     = 0;
bool UseSound       = false;
bool music          = false;

/*****************************/
//    ---> Privates <---

static MDRIVER      *md;
static MPLAYER      *mp;
static UNIMOD       *mf;
static MD_VOICESET  *vs_global, *vs_music, *vs_sndfx, *vs_menu;
static SNDCACHE     CurCache, NewCache;

struct
{   MPLAYER     *mp[8];
    char        sng[8][80];
    int         pos;
} fifo;

static bool Session;

std::vector<MD_SAMPLE*> sfx;



// ==============================
//       Private Functions
// ==============================

static void ShutdownSound(void) { Mikmod_Exit(md); }
static void errorhandler(int merrnum, const CHAR *merrstr)
{
    // the addition of a dialog box on certain error types might be a
    // useful 'user-friendly' idea.  Something to think about later...
    
    UseSound = false;
    return;
}

static void killmodule(void)
// Stops playing and unloads the current module.
{
    music = false;
    
    if (mf)
    {    // see if our resource is cached -- if not, then unload the song.
        if(mp)
        {   Player_Stop(mp);
		    //Player_FreeSong(mp);
        Player_Free(mp);
            mp = NULL;
        }
        
        if(!CurCache.song->CheckFor(playingsng))
        {   Unimod_Free(mf);
		    mf = NULL;
        }
	}
}


static bool FetchSongCache(SongCacheEntry *list, const char *res)
// See if the resource name 'res exists in the given cache list.  If so, it
// sets the mf global to that resource, so that it may be played.
// Returns: 0 if not found.  1 if found.
{
    SongCacheEntry *cruise;

    cruise = list;
    while(cruise)
    {   if(!strcmp(cruise->name, res))
        {   mf = cruise->mf;
            return 1;
        }
        cruise = cruise->Next();
    }
    return 0;
}


// ================================
//     Initialization and Stuff
// ================================

void InitSound()
{
	fifo.pos   = 0;
    fifo.mp[0] = NULL;

    mp         = NULL;
    mf         = NULL;
    
    UseSound   = true;
    Session    = false;

    CurCache.song = NULL; CurCache.sample = NULL;

    //printlog = Log; 
    Mikmod_RegisterErrorHandler(errorhandler);

    Mikmod_RegisterLoader(load_it);
	Mikmod_RegisterLoader(load_xm);
	Mikmod_RegisterLoader(load_s3m);
	Mikmod_RegisterLoader(load_mod);
	Mikmod_RegisterAllDrivers();
	
    // AirChange:
    //  the new buffer size is set to 60 milliseconds, which is adequate for testing
    //  purposes.  We will want to toy with a 40 ms buffer later to see how stable it
    //  is on various hardware.

    // Notes:
    //  - We force dynamic sample support.  This is mostly a gesture of future expandability.

    md = Mikmod_Init(48000, 60, NULL, MD_STEREO, CPU_AUTODETECT, DMODE_16BITS | DMODE_INTERP | DMODE_NOCLICK | DMODE_SAMPLE_DYNAMIC);

    // Blackstar voiceset organization:
    // Notes:
    // - The global voiceset has no voices since everything will be assignd to one
    //   of its child voicesets.
    // - The music voiceset has no voices since those will be allocated by the player.
    // - the sound effects are differentiated from the menu system sounds so that we
    //   can preempt all in-game sound effects when the user enters a menu.

    vs_global = Voiceset_Create(md,NULL,64,0);
    vs_music  = Voiceset_Create(md,vs_global,64,0);
    vs_sndfx  = Voiceset_Create(md,vs_global,64,0);
    vs_menu   = Voiceset_Create(md,vs_global,64,0);
    
    // We should load and set configured volume levels for each class
    // of sound here... (or somewhere)
    
    AddShutdownProc(ShutdownSound,"Music subsystem");

    const char *fname = "MAIN.SFX";

    FILE* f;
    char i;

    int numfx = 0;
    if(!(f = fopen(fname, "r"))) err("Could not open sound effect index file.");
    fscanf(f, "%s", strbuf);
    numfx = atoi(strbuf);

    for(int i = 0; i < numfx; i++)
    {
      fscanf(f, "%s", strbuf);
      auto tmpsfx = mdsfx_loadwav(md, strbuf);
      if(!tmpsfx)
        err("WAV load error.");
      sfx.push_back(tmpsfx);
    }
    fclose(f);

}

void UpdateSound(void)
{
    if(music) Mikmod_Update(md);
}

void s_CacheStartSession(void)
{
    NewCache.song = NULL; NewCache.sample = NULL;
    Session = true;
}

void s_CacheStopSession(void)
{
    SongCacheEntry *cruise;

    // Compare the two linked lists -- currently cached and 'to be cached.'
    // and take appropriate action!

    // Step one, look for anything to unload.
    
    cruise = CurCache.song;
    while(cruise)
    {   SongCacheEntry *tmp = cruise->Next();
        if(!NewCache.song->CheckFor(cruise))
        {   // this resource isn't in the newcache list so lets unload it.
            Unimod_Free(cruise->mf);
        }
        cruise = tmp;
    }

    // Step Two, look for anything to load.
    
    cruise = NewCache.song;
    while(cruise)
    {   SongCacheEntry *tmp = cruise->Next();
        if(!CurCache.song->CheckFor(cruise))
        {   // this resource isn't in the curcache list so lets load it.
            Unimod_Load(md,cruise->name);
        }
        cruise = tmp;
    }

    // free up the crcache and move newcache over it.

    CurCache.song->FreeList();
    CurCache.song = NewCache.song;

    Session = false;
}

/* ==========================================================================
  *** Music Manipulation Interface

  *** Proposed functionality method for the music system:

  PlayMusic:
  A call to PlayMusic will check the in-memory song cache to see if the re-
  quested song is already loaded.  If not, it will attempt to load the song
  using the vfile packfile interface.  If that fails, it will attempt to
  load the song from the OS filesystem.  If the second param ('cache') is set
  to true, then the song will be loaded into the precache area and will not be
  unloaded when stopped.  This allows us to optionally distribute some of the
  loading process.

  CacheMusic:
  Caches the given song.  Loads it using the same search order as PlayMusic.
  Cached songs are only unloaded when the engine called FreeAllMusic();
  However, it is usually better to let the sound system play it smart. --> See
  'StartCacheSession'

  s_CacheStartSession:
  This applies to both music and sound effects.  When the precaching session
  is started, all calls to CacheMusic are tracked.  Afterward, the user calls
  s_CacheEndSession, which will compare the requested precache setup to the
  current one - unloading anything not in the new precache and loading anything
  that isn't already.  This allows us to utilize pre-loaded data with maximum
  efficiency.

  FreeAllMusic()
  Unloads all music in the cache, and whatever might be playing at the moment.
  Generally speaking, this shouldn't be used in favor of the much smarter music
  manamgement system offered via Cache Sessions.  But, you never know. :)


  ===========================================
*/
static bool loadmodule(const char *sng)
{
    // We don't know how long it could take to load the song,
    // so lets clear out the mixing buffer.
    Mikmod_WipeBuffers(md);
        
    // Load the song and add it to the list of loaded resources.
    mf = Unimod_Load(md,sng);
    if (!mf)
    {   err("Could not load module %s", sng);
        return 1;
    }
    return 0;
}

void CacheMusic(const char *sng)
{
    if(Session)
    {   // Put this song onto the 'NewCache' songlist to be loaded later.
        NewCache.song = new SongCacheEntry(NewCache.song, sng);
    } else
    {   loadmodule(sng);
        CurCache.song     = new SongCacheEntry(CurCache.song, sng);
        CurCache.song->mf = mf;
    }
}


void PlayMusic(char *sng, bool cache)
{
	if (!UseSound) return;
	if (!vstrlen(sng)) return;
	if (!vstrcmp(sng, playingsng)) return;
	
    killmodule();

    vstrcpy(playingsng, sng);

    if(!FetchSongCache(CurCache.song,sng))
    {   // song is not cached, so load it up ...
        if(cache) CacheMusic(sng); else loadmodule(sng);
    }
    
    if(mf)
    {   mp = Player_InitSong(mf, vs_music, PF_LOOP, 24);
        Player_Start(mp);
        music = true;
    }
}

void StopMusic(void)
{
	if (!UseSound) return;

    killmodule();
    playingsng[0]=0;
}

// --->  Music Suspension  <---

void SuspendMusic()
// Nested high-level music suspension stuff.  This sucker suspends the currently
// active song and puts it on the suspended-songs FIFO.  A call to ResumeMusic
// wipes it.
{
    if(mf && mp)
    {   Player_Pause(mp,0);    // pause with complete silence
        fifo.mp[fifo.pos] = mp;
        vstrcpy(fifo.sng[fifo.pos],playingsng);
        
        fifo.pos++;
        mp = NULL;
        mf = NULL;
        playingsng[0] = 0;
    }
}

void ResumeMusic()
// Pick the first voiceset off the FIFO and resume it.  If the fifo is empty or
// the pointer is null, it'll be ok.  We're not stupid! ;)
// If a song is currently playing, we stop and unload it.
{
    killmodule();

    if(fifo.pos)
    {   fifo.pos--;
        if(fifo.mp[fifo.pos]) Player_Start(mp=fifo.mp[fifo.pos]);
        mf = mp->mf;
        fifo.mp[fifo.pos] = NULL;

        vstrcpy(playingsng, fifo.sng[fifo.pos]);
        music = true;
    }
}

void FreeAllMusic()
{
    SongCacheEntry *cruise;
  	
    if (!UseSound) return;

    cruise = CurCache.song;
    while(cruise)
    {   SongCacheEntry  *tmp = cruise->Next();
        Unimod_Free(cruise->mf);
        delete cruise;
        cruise = tmp;
    }

}

int PlaySample(int i, int v, int p)
// returns the voice the sample has been played in.
{
//	int voice;

	if (!UseSound) return 0;

    return 0;
}

void playeffect(int efc)
{ 
  if (!UseSound) return;


  mdsfx_playeffect(sfx[efc], vs_sndfx, 0, 0);

//chanl=md_numchn-curch;
//if (curch==1) curch=2; else curch=1;
//
//MD_VoiceSetVolume(chanl,64);
//MD_VoiceSetPanning(chanl,128);
//MD_VoiceSetFrequency(chanl,sfx[efc]->loopend);
//MD_VoicePlay(chanl,sfx[efc]->handle,0,sfx[efc]->length,0,0,sfx[efc]->flags);

}


// ===========================================================================
// Volume set/retrive!!
//
// I assume you want all volumes range from 0 to 100, since you were returning
// 100 as the default music volume.  I made a macro anyway, so we can change
// thevolume scale with ease!

#define VOLSCALE  100

#define bs_setvol(x,a) Voiceset_SetVolume(x, (a * 128) / VOLSCALE)
#define bs_getvol(x)   ((x->volume * VOLSCALE) / 128)

int s_getglobalvolume()
{
    return bs_getvol(vs_global);
}

void s_setglobalvolume(int v)
{
    bs_setvol(vs_global, v);
}

int s_getmusicvolume()
{
    return bs_getvol(vs_music);
}

void s_setmusicvolume(int v)
{
    bs_setvol(vs_music, v);
}


int s_getsndfxvolume()
{
    return bs_getvol(vs_sndfx);
}

void s_setsndfxvolume(int v)
{
    bs_setvol(vs_sndfx, v);
}