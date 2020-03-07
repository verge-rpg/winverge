#include <string.h>
#include <malloc.h>
#include "a_cache.h"

// -- Generic Cache Entry System --
// I suspect that this could be used for pretty much any portion of the BlackStar
// engine.  With that in mind I have coded it in a pretty modular fashion.  You can
// overload this class and put your own specific per-resource information in it.
// Unfortunately I didn't have a full understanding of constructors, and more spec-
// ifically constructor interitence, so there isn't one.   Yet.

CacheEntry::~CacheEntry()
{
    if(prev) prev->next = next;
    if(next) next->prev = prev;

    if(name) free(name);
}


bool CacheEntry::CheckFor(const CacheEntry *targ)
// See if the cache entry 'targ' exists in the given list!
// Returns: 0 if not found.  1 if found.
{
    CacheEntry *cruise;

    cruise = this;
    while(cruise)
    {   if(!strcmp(cruise->name, targ->name)) return 1;
        cruise = cruise->next;
    }
    return 0;
}


bool CacheEntry::CheckFor(const char *res)
// See if the resource name 'res exists in the given cache list.
// Returns: 0 if not found.  1 if found.
{
    CacheEntry *cruise;

    cruise = this;
    while(cruise)
    {   if(!strcmp(cruise->name, res)) return 1;
        cruise = cruise->next;
    }
    return 0;
}


void CacheEntry::FreeList()
{
    CacheEntry  *cruise;

    cruise = this;
    while(cruise)
    {   CacheEntry *old = cruise->next;
        delete cruise;
        cruise = old;
    }
}


