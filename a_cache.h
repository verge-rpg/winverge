
class CacheEntry
{   
public:
    class CacheEntry *next, *prev;   // oh my a linked list!!
    char             *name;          // resource name/identifier

    ~CacheEntry();

    bool  CheckFor(const CacheEntry *targ);
    bool  CheckFor(const char *res);
    void  FreeList();
};

