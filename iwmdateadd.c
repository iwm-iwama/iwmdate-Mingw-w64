//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdateadd_20200809"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2020 iwm-iwama"
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
#define   ColorHeaderFooter   ( 7 + ( 0 * 16))
#define   ColorBgText1        (15 + (12 * 16))
#define   ColorExp1           (13 + ( 0 * 16))
#define   ColorExp2           (14 + ( 0 * 16))
#define   ColorExp3           (11 + ( 0 * 16))
#define   ColorText1          (15 + ( 0 * 16))

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
/*
	é¿çsä÷åW
*/
MBS  *$program     = "";
MBS  **$args       = {0};
UINT $argsSize     = 0;
UINT $colorDefault = 0;

INT
main()
{
	$program      = iCmdline_getCmd();
	$args         = iCmdline_getArgs();
	$argsSize     = iary_size($args);
	$colorDefault = iConsole_getBgcolor(); // åªç›ÇÃï∂éöêFÅ^îwåiêF

	// -help "-h"ÇÕhour
	if($argsSize == 0 || imb_cmpp($args[0], "-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(imb_cmpp($args[0], "-v") || imb_cmpp($args[0], "-version"))
	{
		print_version();
		LN();
		imain_end();
	}

	INT  *iAryDt    = icalloc_INT(6); // y, m, d, h, n, s
	INT  *iAryDtAdd = icalloc_INT(6); // Å}y, Å}m, Å}d, Å}h, Å}n, Å}s

	// [0]
	/*
		"." "now" => åªç›éû
	*/
	if(imb_cmpp($args[0], ".") || imb_cmpp($args[0], "now"))
	{
		iAryDt = idate_now_to_iAryYmdhns_localtime();
	}
	else
	{
		iAryDt = idate_MBS_to_iAryYmdhns($args[0]);
	}




	// [1..]
	for(INT _i1 = 1; _i1 < $argsSize; _i1++)
	{
		MBS **_as1 = ija_split($args[_i1], "=", "\"\"\'\'", FALSE);
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
	P (" %s\n",
		IWM_COPYRIGHT
	);
	P ("   Ver.%s+%s\n",
		IWM_VERSION,
		LIB_IWMUTIL_VERSION
	);
}

VOID
print_help()
{
	iConsole_setTextColor(ColorHeaderFooter);
		print_version();
		LN();
	iConsole_setTextColor(ColorBgText1);
		P (" %s [ì˙ït] [ÉIÉvÉVÉáÉì] \n\n", $program);
	iConsole_setTextColor(ColorExp1);
		P2(" (égópó·)");
	iConsole_setTextColor(ColorText1);
		P ("   %s \"2000/1/1\" -y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n", $program);
	iConsole_setTextColor(ColorExp2);
		P2(" [ì˙ït]");
	iConsole_setTextColor(ColorText1);
		P2("   \"now\" \".\" (åªç›ì˙éû)");
		P2("   \"+2000/01/01\" \"+2000-01-01\"");
		P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"");
		NL();
	iConsole_setTextColor(ColorExp2);
		P2(" [ÉIÉvÉVÉáÉì]");
	iConsole_setTextColor(ColorExp3);
		P2("   -y=[Å}îN] -m=[Å}åé] -d=[Å}ì˙] -w=[Å}èT] -h=[Å}éû] -n=[Å}ï™] -s=[Å}ïb]");
		NL();
		P2("   -format=STR | -f=STR");
	iConsole_setTextColor(ColorText1);
		P ("       Å¶STRÇ™ñ≥éwíËÇÃÇ∆Ç´ \"%s\"\n", DATE_FORMAT);
		P2("       %g : +/-ï\\é¶");
		P2("       %y : îN(0000)  %m : åé(00)  %d : ì˙(00)");
		P2("       %h : éû(00)  %n : ï™(00)  %s : ïb(00)");
		P2("       %a : ójì˙  %A : ójì˙êî");
		P2("       %c : îNí éZì˙  %C : èCê≥ÉÜÉäÉEÉXí éZì˙  %J : ÉÜÉäÉEÉXí éZì˙");
		P2("       %e : îNí éZèT");
		P2("       \\t : É^Éu  \\n : â¸çs");
	iConsole_setTextColor(ColorExp3);
		P2("   -N");
	iConsole_setTextColor(ColorText1);
		P2("       â¸çsÇµÇ»Ç¢");
		NL();
	iConsole_setTextColor(ColorExp1);
		P2(" (îıçl)");
	iConsole_setTextColor(ColorText1);
		P2("   ÅEÉÜÉäÉEÉXóÔ Åi-4712/01/01Å`1582/10/04Åj");
		P2("   ÅEÉOÉåÉSÉäÉIóÔÅi1582/10/15Å`9999/12/31Åj");
		P2("    (íç1) ãÛîíóÔ 1582/10/5Å`1582/10/14 ÇÕÅA\"1582/10/4\" Ç∆ÇµÇƒéÊàµÇ§ÅB");
		P2("    (íç2) BCóÔÇÕÅA\"-1/1/1\" Ç \"0/1/1\" Ç∆ÇµÇƒéÊàµÇ§ÅB");
		P2("    (íç3) ÉvÉçÉOÉâÉÄè„ÇÕÅAèCê≥ÉÜÉäÉEÉXóÔÇégópÅB");
	iConsole_setTextColor(ColorHeaderFooter);
		LN();
	iConsole_setTextColor($colorDefault); // å≥ÇÃï∂éöêFÅ^îwåiêF
}
