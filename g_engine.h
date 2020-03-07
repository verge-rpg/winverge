
struct r_entity {                      // on-screen entities (chars)
  unsigned short x;                    // xwc position
  unsigned short y;                    // ywc position
  unsigned char facing;                // direction entity is facing
  unsigned char moving;                // direction entity is moving
  unsigned char movcnt;                // how far left to move in this tile
  unsigned char framectr;              // frame sequence counter
  unsigned char specframe;             // special-frame set thingo
  unsigned char chrindex, movecode;    // CHR index / movement pattern code
  unsigned char activmode, obsmode;    // activation mode, obstruction mode
  unsigned int actscript, movescript;  // script references
  unsigned char speed, speedct;        // entity speed, speedcount :)
  unsigned short step, delay,          // Misc data entries
                 data1, data2,         // More misc data
                 data3, data4,         // yet more crappy misc data.
                 delayct,adjactv;      // yet more internal crap
  unsigned short x1,y1,x2,y2;          // bounding box coordinates
  unsigned char curcmd, cmdarg;        // Script commands/arguments
  unsigned char *scriptofs;            // offset in script parsing
  unsigned char face,chasing,          // face player when activated | chasing
                chasespeed, chasedist; // chasing variables
  unsigned short cx,cy;                // current-tile pos (moving adjusted)
  int expand1;                         // always room for improvement
  char entitydesc[20];                 // Editing description
};

struct vspanim {
  unsigned short int start;            // start tile index
  unsigned short int finish;           // end tile index
  unsigned short int delay;            // delay between cycles
  unsigned short int mode; };          // animation mode

struct zoneinfo {                      // zone data
  char zonename[15];                   // zone description
  unsigned short int callevent;        // event number to call
  unsigned char percent;               // chance (in 255) of event occurance
  unsigned char delay;                 // step-delay before last occurance
  unsigned char aaa;                   // accept adjacent activation
  char savedesc[30];                   // savegame description
};

struct charstats {                     // Stat record for single character
  char name[9];                        // 8-character name + null
  char chrfile[13];                    // CHR file for this character
  int exp, rea;                        // experience points, REAction
  int curhp, maxhp;                    // current/max hit points
  int curmp, maxmp;                    // current/max magic points
  int atk, def;                        // ATtacK and DEFense stats
  int str, end;                        // STRength and ENDurance
  int mag, mgr;                        // MAGic and MaGic Resistance stats
  int hit, dod;                        // HIT% and DODge%
  int mbl, fer;                        // MoBiLity and FERocity (not related)
  int magc, mgrc;                      // MAGic and MaGic Resistance stats
  int hitc, dodc;                      // HIT% and DODge%
  int mblc, ferc;                      // MoBiLity and FERocity (not related)
  int reac;                            // reaction (all ***c stats are current)
  int nxt, lv;                         // exp to next level, current level
  char status;                         // 0=fine 1=dead 2=...
  unsigned char invcnt;                // number of items in inventory
  unsigned short int inv[24];          // inventory
};

struct itemdesc {
  unsigned char name[20];              // item name
  unsigned short int icon;             // icon index
  unsigned char desc[40];              // item description
  unsigned char useflag;               // Useable flag <see below>
  unsigned short int useeffect;        // effect list index
  unsigned char itemtype;              // item type index
  unsigned char equipflag;             // 0=nonequipable 1=equipable
  unsigned char equipidx;              // equip.dat index value
  unsigned char itmprv;                // item preview code
  unsigned int price;                  // Cost of the item in a store
};

struct equipstruc {
  int str,end,mag,mgr,hit,dod;
  int mbl,fer,rea;
  char equipable[30];
  char onequip, ondeequip;
};

/*****************************/

extern struct r_entity party[101];
extern struct vspanim va0[100];
extern struct equipstruc equip[255];
extern struct itemdesc items[255];
extern struct charstats pstats[30];

extern unsigned short int nx,ny;
extern quad flags[8000];
extern word numtiles;

extern char partyidx[5], layerc, saveflag, autoent, numchars, usenxy;
extern unsigned char *itemicons,*chrs,*chr2;
extern int gp,xwin,ywin,xtc,ytc,xofs,yofs;
extern unsigned short int *map0, *map1,xsize,ysize,vadelay[100];
extern unsigned char *mapp,pmultx,pdivx,pmulty,pdivy,*vsp0;
extern char strbuf[], tchars, mapname[13], lastmoves[6], qabort;
extern byte menuptr[256],itmptr[576],charptr[960],*speech;
/*****************************/

void allocbuffers();
void addcharacter(int i);
void LoadCHRList();
void load_map(const char *fname);
void process_stepzone();
void lastmove(char n);
void startfollow();
int InvFace();
void Activate();
int ObstructionAt(int tx,int ty);
void process_entities();
void ProcessControls();
void process_controls();
void check_tileanimation();
void UpdateEquipStats();
void game_ai();
void CreateSaveImage(byte *buf);
void SaveGame(const char *fn);
void startmap(const char *fname);

/************** Entities **************/

extern char chrlist[100][13];                // CHR list
extern int entities;                         // number of active entities
extern unsigned char nummovescripts;         // number of movement scripts
extern int msofstbl[100];                    // movement script offset table
extern byte *msbuf;                          // movement script data buffer
extern char movesuccess;                     // if call to MoveXX() was successful

int EntityAt(int ex, int ey);
int AnyEntityAt(int ex,int ey);
void MoveRight(int i);
void MoveLeft(int i);
void MoveUp(int i);
void MoveDown(int i);
int Zone(int cx, int cy);
void ProcessSpeedAdjEntity(int i);
void Wander1(int i);
void Wander2(int i);
void Wander3(int i);
void Whitespace(int i);
void GetArg(int i);
void GetNextCommand(int i);
void MoveScript(int i);
void TestActive(int i);
void Chase(int i);
void CheckChasing(int i);
void ProcessEntity(int i);
