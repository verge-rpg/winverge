typedef struct SMS_SND_PLAYER
{
	MD_VOICESET *vs;
	WORD sin[65536], noise[65536];

	int incs[4];
	int vols[4];
	
} SMS_SND_PLAYER;

SMS_SND_PLAYER *SMS_SND_Player_Init();
void SMS_SND_Player_SetChannel(struct SMS_SND_PLAYER *ps, int chan, double freq, int vol);
