//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdateadd_20210319"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2021 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil.h"

INT  main();
VOID print_version();
VOID print_help();

// [ï∂éöêF] + ([îwåiêF] * 16)
//  0 = Black    1 = Navy     2 = Green    3 = Teal
//  4 = Maroon   5 = Purple   6 = Olive    7 = Silver
//  8 = Gray     9 = Blue    10 = Lime    11 = Aqua
// 12 = Red     13 = Fuchsia 14 = Yellow  15 = White

// É^ÉCÉgÉã
#define   COLOR01             (15 + ( 9 * 16))
// ì¸óÕó·Å^íç
#define   COLOR11             (15 + (12 * 16))
#define   COLOR12             (13 + ( 0 * 16))
#define   COLOR13             (12 + ( 0 * 16))
// à¯êî
#define   COLOR21             (14 + ( 0 * 16))
#define   COLOR22             (11 + ( 0 * 16))
// ê‡ñæ
#define   COLOR91             (15 + ( 0 * 16))
#define   COLOR92             ( 7 + ( 0 * 16))

#define   DATE_FORMAT         "%g%y-%m-%d" // (íç)%gïtÇØÇ»Ç¢Ç∆ëSÇƒê≥êîï\é¶

/*
	èoóÕÉtÉHÅ[É}ÉbÉg
	-f=STR | -format=STR
*/
MBS  *_Format = DATE_FORMAT;
/*
	â¸çsÇ∑ÇÈÇ∆Ç´ TRUE
	-N
*/
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil èâä˙âª
	iCLI_getCMD();       //=> $IWM_CMD
	iCLI_getARGS();      //=> $IWM_ARGV, $IWM_ARGC
	iConsole_getColor(); //=> $IWM_ColorDefault, $IWM_StdoutHandle
	iExecSec_init();     //=> $IWM_ExecSecBgn

	// -help "-h"ÇÕhour
	if(! $IWM_ARGC || imb_cmpp($IWM_ARGV[0], "-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(imb_cmpp($IWM_ARGV[0], "-v") || imb_cmpp($IWM_ARGV[0], "-version"))
	{
		print_version();
		imain_end();
	}

	INT *iAryDt    = icalloc_INT(6); // y, m, d, h, n, s
	INT *iAryDtAdd = icalloc_INT(6); // Å}y, Å}m, Å}d, Å}h, Å}n, Å}s

	// [0]
	/*
		"." "now" => åªç›éû
	*/
	if(imb_cmpp($IWM_ARGV[0], ".") || imb_cmpp($IWM_ARGV[0], "now"))
	{
		iAryDt = idate_now_to_iAryYmdhns_localtime();
	}
	else
	{
		iAryDt = idate_MBS_to_iAryYmdhns($IWM_ARGV[0]);
	}

	// [1..]
	for(INT _i1 = 1; _i1 < $IWM_ARGC; _i1++)
	{
		MBS **_as1 = ija_split($IWM_ARGV[_i1], "=", "\"\"\'\'", FALSE);
		MBS **_as2 = ija_split(_as1[1], ",", "\"\"\'\'", TRUE);

		// -y
		if(imb_cmpp(_as1[0], "-y"))
		{
			iAryDtAdd[0] += inum_atoi(_as2[0]);
		}

		// -m
		if(imb_cmpp(_as1[0], "-m"))
		{
			iAryDtAdd[1] += inum_atoi(_as2[0]);
		}

		// -d
		if(imb_cmpp(_as1[0], "-d"))
		{
			iAryDtAdd[2] += inum_atoi(_as2[0]);
		}

		// -w
		if(imb_cmpp(_as1[0], "-w"))
		{
			iAryDtAdd[2] += inum_atoi(_as2[0]) * 7;
		}

		// -h
		if(imb_cmpp(_as1[0], "-h"))
		{
			iAryDtAdd[3] += inum_atoi(_as2[0]);
		}

		// -n
		if(imb_cmpp(_as1[0], "-n"))
		{
			iAryDtAdd[4] += inum_atoi(_as2[0]);
		}

		// -s
		if(imb_cmpp(_as1[0], "-s"))
		{
			iAryDtAdd[5] += inum_atoi(_as2[0]);
		}

		// -f | -format
		if(imb_cmpp(_as1[0], "-f") || imb_cmpp(_as1[0], "-format"))
		{
			_Format = ims_clone(_as2[0]);
		}

		// -N
		if(imb_cmpp(_as1[0], "-N"))
		{
			_NL = FALSE;
		}

		ifree(_as2);
		ifree(_as1);
	}

	iAryDt = idate_add(
		iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5],
		iAryDtAdd[0], iAryDtAdd[1], iAryDtAdd[2], iAryDtAdd[3], iAryDtAdd[4], iAryDtAdd[5]
	);

	P(
		idate_format_ymdhns(
			_Format,
			iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5]
		)
	);

	if(_NL)
	{
		NL();
	}

	// Debug
	/// icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

	imain_end();
}

VOID
print_version()
{
	LN();
	P (" %s\n", IWM_COPYRIGHT);
	P ("   Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
}

VOID
print_help()
{
	PZ(COLOR92, NULL);
		print_version();
	PZ(COLOR01, " ì˙éûÇÃëOå„ÇåvéZ \n\n");
	PZ(COLOR11, " %s [ì˙ït] [ÉIÉvÉVÉáÉì] \n\n", $IWM_CMD);
	PZ(COLOR12, " (égópó·)\n");
	PZ(COLOR91, "   %s \"2000/1/1\" -y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n", $IWM_CMD);
	PZ(COLOR21, " [ì˙ït]\n");
	PZ(COLOR91, NULL);
		P2("   \"now\" \".\" (åªç›ì˙éû)");
		P2("   \"+2000/01/01\" \"+2000-01-01\"");
		P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"\n");
	PZ(COLOR21, " [ÉIÉvÉVÉáÉì]\n");
	PZ(COLOR22, NULL);
		P2("   -y=[Å}îN] -m=[Å}åé] -d=[Å}ì˙] -w=[Å}èT] -h=[Å}éû] -n=[Å}ï™] -s=[Å}ïb]\n");
		P2("   -format=STR | -f=STR");
	PZ(COLOR91, NULL);
		P ("       Å¶STRÇ™ñ≥éwíËÇÃÇ∆Ç´ \"%s\"\n", DATE_FORMAT);
		P2("       %g : +/-ï\\é¶");
		P2("       %y : îN(0000)  %m : åé(00)  %d : ì˙(00)");
		P2("       %h : éû(00)  %n : ï™(00)  %s : ïb(00)");
		P2("       %a : ójì˙  %A : ójì˙êî");
		P2("       %c : îNí éZì˙  %C : èCê≥ÉÜÉäÉEÉXí éZì˙  %J : ÉÜÉäÉEÉXí éZì˙");
		P2("       %e : îNí éZèT");
		P2("       \\t : É^Éu  \\n : â¸çs");
	PZ(COLOR22, "   -N\n");
	PZ(COLOR91, "       â¸çsÇµÇ»Ç¢\n\n");
	PZ(COLOR12, " (îıçl)\n");
	PZ(COLOR91, NULL);
		P2("   ÅEÉÜÉäÉEÉXóÔ Åi-4712/01/01Å`1582/10/04Åj");
		P2("   ÅEÉOÉåÉSÉäÉIóÔÅi1582/10/15Å`9999/12/31Åj");
	PZ(COLOR13, "    (íçÇP) ");
	PZ(COLOR91, "ãÛîíóÔ 1582/10/5Å`1582/10/14 ÇÕÅA\"1582/10/4\" Ç∆ÇµÇƒéÊàµÇ§ÅB\n");
	PZ(COLOR13, "    (íçÇQ) ");
	PZ(COLOR91, "BCóÔÇÕÅA\"-1/1/1\" Ç \"0/1/1\" Ç∆ÇµÇƒéÊàµÇ§ÅB\n");
	PZ(COLOR13, "    (íçÇR) ");
	PZ(COLOR91, "ÉvÉçÉOÉâÉÄè„ÇÕÅAèCê≥ÉÜÉäÉEÉXóÔÇégópÅB\n\n");
	PZ(COLOR92, NULL);
		LN();
	PZ(-1, NULL);
}
