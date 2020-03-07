/********************************************************** 
   g_vc.cpp
   winverge
   Copyright (C) 2000 Benjamin Eirich (vecna)
   All Rights Reserved
 **********************************************************/

#include "pcp.h"
#include "vc.h"

// ============================ data ============================

byte *mapvc;                           // MAP VC code buffer
byte *effectvc,*startupvc;             // Effect VC code buffer / Startup.vcs

byte *vcdatabuf;                       // The data buffer for VC use
byte *code, *basevc;                   // current code data ptr
quad scriptofstbl[1024];       // Map script offset table
quad effectofstbl[1024];       // Effect script offset table
quad startupofstbl[1024];      // Startup script offset table
int numscripts;                        // Number of scripts in current map VC
quad varl[10];                 // Chain/call pass variables
quad tvar[26];                 // Temporary/Throwaway variables
char killvc=0;                         // abort VC loop;

// ============================ code ============================

void InitVCMem()
{
	effectvc = new byte[25000];
	startupvc = new byte[25000];
	vcdatabuf = new byte[2000000];
//	vcdatabuf = valloc(vcbufm,"vcdatabuf");
}

void LoadVC(FILE *f)
{
	int b,e,mapvcs;

	fread(&numscripts, 1, 4, f);
	fread(&scriptofstbl, 4, numscripts, f);

	b=ftell(f);             // get beginning offset of map vc code
	fseek(f, 0, SEEK_END);  // seek to end of map vc code
	e=ftell(f);             // get end offset of map vc code
	fseek(f, b, SEEK_SET);  // reset file pos
	mapvcs = e - b + 1;     // calc total map vc code size (in bytes)
	if (mapvc) delete[] mapvc; // free mapvc mem (if necessary)
	mapvc = new byte[mapvcs]; // allocate necessary mapvc mem
	fread(mapvc, mapvcs, 1, f); // read in the map vc code from disk
}

byte GrabC()
{
	byte c = *code;
	code++;
	return c;
}

word GrabW()
{ 
	word c, *ptr;

    ptr = (word*) code;
	c = *ptr;
	code += 2;
	return c;
}

quad GrabD()
{ 
	quad c, *ptr;
  
	ptr = (quad *) code;
	c = *ptr;
	code += 4;
	return c;
}

void GrabString(char *str)
{
	int i;

	i = 0;
	while (*code)
	{
		str[i] = *code;
		code++;
		i++;
	}
	str[i]=0;
	code++;
}

void ProcessVar0Assign()
{
	int t,w;
	byte b;

	t = GrabC();
	b = GrabC();
	switch (b)
	{
		case SET: WriteVar0(t,ResolveOperand()); return;
		case INCSET: w=ReadVar0(t); w += ResolveOperand(); WriteVar0(t,w); return;
		case DECSET: w=ReadVar0(t); w -= ResolveOperand(); WriteVar0(t,w); return;
		case INCREMENT: w=ReadVar0(t); w++; WriteVar0(t,w); return;
		case DECREMENT: w=ReadVar0(t); w--; WriteVar0(t,w); return;
	}
}

void ProcessVar1Assign()
{
	int t,a,w;
	byte b;

	t = GrabC();
	a = ResolveOperand();
	b = GrabC();
	switch (b)
	{
		case SET: WriteVar1(t,a,ResolveOperand()); return;
		case INCSET: w=ReadVar1(t,a); w+=ResolveOperand(); WriteVar1(t,a,w); return;
		case DECSET: w=ReadVar1(t,a); w-=ResolveOperand(); WriteVar1(t,a,w); return;
		case INCREMENT: w=ReadVar1(t,a); w++; WriteVar1(t,a,w); return;
		case DECREMENT: w=ReadVar1(t,a); w--; WriteVar1(t,a,w); return;
	}
}

void ProcessVar2Assign()
{
	int t,a,c,w;
	byte b;

	t = GrabC();
	a = ResolveOperand();
	c = ResolveOperand();
	b = GrabC();
	switch (b)
	{
		case SET: WriteVar2(t,a,c,ResolveOperand()); return;
		case INCSET: w=ReadVar2(t,a,c); w+=ResolveOperand(); WriteVar2(t,a,c,w); return;
		case DECSET: w=ReadVar2(t,a,c); w-=ResolveOperand(); WriteVar2(t,a,c,w); return;
		case INCREMENT: w=ReadVar2(t,a,c); w++; WriteVar2(t,a,c,w); return;
		case DECREMENT: w=ReadVar2(t,a,c); w--; WriteVar2(t,a,c,w); return;
	}
}

quad ResolveOperand()
{
	quad cr;
	byte c;

	cr = ProcessOperand();              // Get base number
	while (1)
	{
		c = GrabC();
		switch (c)
		{
			case ADD: cr += ProcessOperand(); continue;
			case SUB: cr -= ProcessOperand(); continue;
			case DIV: cr = cr / ProcessOperand(); continue;
			case MULT: cr = cr * ProcessOperand(); continue;
			case MOD: cr = cr % ProcessOperand(); continue;
			case OP_END: break;
		}
		break;
	}
	return cr;
}

int ProcessOperand()
{
	byte op_desc;
	int a,b,c;

	op_desc = GrabC();
	switch (op_desc)
	{
		case OP_IMMEDIATE: return GrabD();
		case OP_VAR0: return ReadVar0(GrabC());
		case OP_VAR1: a=GrabC();
					  b=ResolveOperand();
                      return ReadVar1(a,b);
		case OP_VAR2: a=GrabC();
                      b=ResolveOperand();
                      c=ResolveOperand();
                      return ReadVar2(a,b,c);
		case OP_GROUP: return ResolveOperand();
	}
	return -1;
}

void ProcessIf()
{
	quad elseofs, arg1, arg2;
	byte numargs,i;
	char exec=1, controlbyte;

	numargs = GrabC();                     // Get number of arguements in this IF.
	elseofs = GrabD();                     // Get jumplocation if IF is false.

	for (i=0; i<numargs; i++)
    {
		arg1 = ResolveOperand();
        controlbyte = GrabC();

        if (controlbyte==ZERO || controlbyte==NONZERO)
        {
			switch (controlbyte)
            {
				case ZERO: if (arg1) exec=0; break;
                case NONZERO: if (!arg1) exec=0; break;
			}
        } else
        {
			arg2 = ResolveOperand();
            switch (controlbyte)
            {
				case EQUALTO: if (arg1 != arg2) exec=0; break;
				case NOTEQUAL: if (arg1 == arg2) exec=0; break;
				case GREATERTHAN: if (arg1 <= arg2) exec=0; break;
				case GREATERTHANOREQUAL: if (arg1 < arg2) exec=0; break;
				case LESSTHAN: if (arg1 >= arg2) exec=0; break;
				case LESSTHANOREQUAL: if (arg1 > arg2) exec=0; break;
            }
        }

        if (!exec)
        {
			code = basevc + elseofs;
            return;
		}
	}
}

void ProcessFor0()
{
	quad vidx=0, min, max, incv, curval, t, bptr;
	byte incs;

	vidx = GrabC();

	min = ResolveOperand();
	max = ResolveOperand();
	incs = GrabC();
	incv = ResolveOperand();
	bptr = (quad) code;

	WriteVar0(vidx, min);
	curval = min;

execloop:
	code = (byte *) bptr;

	ExecuteBlock();

	t = ReadVar0(vidx);
	if (incs){ t+=incv; curval+=incv; }
    else { t-=incv; curval-=incv; }
	WriteVar0(vidx, t);

	// Now determine if we've passed the min/max

	if (!incs && curval > min) return;
	if (incs && curval <= max) goto execloop;
	if (!incs && curval >= max) goto execloop;
}

void ProcessFor1()
{
	quad vidx=0, min, max, incv, curval, t, bptr, varg;
	byte incs;

	vidx = GrabC();
	varg = ResolveOperand();

	min = ResolveOperand();
	max = ResolveOperand();
	incs = GrabC();
	incv = ResolveOperand();
	bptr = (quad) code;

	WriteVar1(vidx, varg, min);
	curval = min;

execloop:
	code = (byte *) bptr;

	ExecuteBlock();

	t = ReadVar1(vidx, varg);
	if (incs) { t+=incv; curval+=incv; }
    else { t-=incv; curval-=incv; }
	WriteVar1(vidx, varg, t);

	// Now determine if we've passed the min/max

	if (!incs && curval > min) return;
	if (incs && curval <= max) goto execloop;
	if (!incs && curval >= max) goto execloop;
}

void ProcessSwitch()
{ 
	int realvalue,compvalue;
	byte c, *next;

	realvalue = ResolveOperand();
	c = GrabC();
	while (c != ENDSCRIPT)
	{
		compvalue = ResolveOperand();
		next = (byte *) ((quad) GrabD());
		if (compvalue != realvalue)
		{
			code = (byte *) ((int) basevc + (int) next);
			c = GrabC();
			continue;
		}
		ExecuteBlock();
		c = GrabC();
	}
}

void ExecuteScript(word s)
{
	byte *buf,*basebuf; /* -- ric: 09/Jun/98 --         */
	basebuf = basevc;   /* save the old code buffers    */
	buf = code;

	basevc = mapvc;
	code = mapvc + scriptofstbl[s];

	ExecuteBlock();
	basevc = basebuf;     /* -- ric: 09/Jun/98 --         */
	code = buf;           /* restore the old code buffers */
}

void ExecuteHookedScript(word s)
{
	byte *codeb,abortvcb,*basevcb;

	codeb = code;
	abortvcb = killvc;
	basevcb = basevc;
	ExecuteScript(s);
	killvc = abortvcb;
	code = codeb;
	basevc = basevcb;
}

void ExecuteEffect(word s)
{
	byte *buf,*basebuf; /* -- ric: 09/Jun/98 --         */
	basebuf = basevc;     /* save the old code buffers    */
	buf = code;

	basevc = effectvc;
	code = effectvc + effectofstbl[s];

	ExecuteBlock();
	basevc = basebuf;     /* -- ric: 09/Jun/98 --         */
	code = buf;           /* restore the old code buffers */
}

void ExecuteBlock()
{
	byte c;

	while (1)
	{
		if (killvc) { killvc=0; break; }
        c = GrabC();
        if (c == EXEC) { ExecLibFunc(GrabC()); continue; }
		if (c == VAR0_ASSIGN) { ProcessVar0Assign(); continue; }
        if (c == VAR1_ASSIGN) { ProcessVar1Assign(); continue; }
        if (c == VAR2_ASSIGN) { ProcessVar2Assign(); continue; }
        if (c == GENERAL_IF) { ProcessIf(); continue; }
        if (c == FOR_LOOP0) { ProcessFor0(); continue; }
        if (c == FOR_LOOP1) { ProcessFor1(); continue; }
        if (c == GOTO) { code=basevc+GrabD(); continue; }
        if (c == SWITCH) { ProcessSwitch(); continue; }
        if (c == ENDSCRIPT) break;
  }
}

void StartupScript()
{
  byte *buf,*basebuf; /* -- ric: 09/Jun/98 --         */
  basebuf = basevc;   /* save the old code buffers    */
  buf = code;

  basevc = startupvc;
  code = startupvc;

  ExecuteBlock();
  basevc = basebuf;     /* -- ric: 09/Jun/98 --         */
  code = buf;           /* restore the old code buffers */
}