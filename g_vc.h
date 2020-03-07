extern byte *mapvc;
extern byte *effectvc,*startupvc;

extern byte *vcdatabuf;
extern byte *code, *basevc;
extern quad scriptofstbl[1024];
extern quad effectofstbl[1024];
extern quad startupofstbl[1024];
extern int numscripts;
extern quad varl[10];
extern quad tvar[26];
extern char killvc;

// ============================ code ============================

void InitVCMem();
void LoadVC(FILE *f);
byte GrabC();
word GrabW();
quad GrabD();
void GrabString(char *str);
void ProcessVar0Assign();
void ProcessVar1Assign();
void ProcessVar2Assign();
quad ResolveOperand();
int ProcessOperand();
void ProcessIf();
void ProcessFor0();
void ProcessFor1();
void ProcessSwitch();
void ExecuteScript(word s);
void ExecuteHookedScript(word s);
void ExecuteEffect(word s);
void ExecuteBlock();
void StartupScript();