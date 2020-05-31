//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdatediff_20200531"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2020 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil.h"

INT  main();
VOID print_version();
VOID print_help();

// [文字色] + ([背景色] * 16)
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

#define   DATE_FORMAT         "%g%y-%m-%d" // (注)%g付けないと全て正数表示

/*
	出力フォーマット
	-f=STR | -format=STR
*/
MBS  *_Format = DATE_FORMAT;
/*
	改行するとき TRUE
	-N
*/
BOOL _NL = TRUE;
/*
	実行関係
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
	$colorDefault = iConsole_getBgcolor(); // 現在の文字色／背景色

	// -help "-h"はhour
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

	INT  *iAryDtBgn = icalloc_INT(6); // y, m, d, h, n, s
	INT  *iAryDtEnd = icalloc_INT(6); // y, m, d, h, n, s

	// [0] . | now
	iAryDtBgn = (imb_cmppi($args[0], ".") || imb_cmpp($args[0], "now") ?
		idate_now_to_iAryYmdhns_localtime() :
		idate_MBS_to_iAryYmdhns($args[0])
	);

	// [1] . | now
	iAryDtEnd = (imb_cmppi($args[1], ".") || imb_cmpp($args[1], "now") ?
		idate_now_to_iAryYmdhns_localtime() :
		idate_MBS_to_iAryYmdhns($args[1])
	);

	// [2..]
	for(INT _i1 = 2; _i1 < $argsSize; _i1++)
	{
		MBS **_as1 = ija_split($args[_i1], "=", "\"\"\'\'", FALSE);
		MBS **_as2 = ija_split(_as1[1], ",", "\"\"\'\'", TRUE);

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

	// diff[8]
	INT *iAryDiff=idate_diff(
		iAryDtBgn[0], iAryDtBgn[1], iAryDtBgn[2], iAryDtBgn[3], iAryDtBgn[4], iAryDtBgn[5],
		iAryDtEnd[0], iAryDtEnd[1], iAryDtEnd[2], iAryDtEnd[3], iAryDtEnd[4], iAryDtEnd[5]
	);

	P(
		idate_format_diff(
			_Format,
			iAryDiff[0], iAryDiff[1], iAryDiff[2], iAryDiff[3], iAryDiff[4], iAryDiff[5], iAryDiff[6], iAryDiff[7]
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
		P (" %s [日付1] [日付2] [オプション] \n\n", $program);
	iConsole_setTextColor(ColorExp1);
		P2(" (使用例)");
	iConsole_setTextColor(ColorText1);
		P ("   %s . 2000/01/01 -f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n\n", $program);
	iConsole_setTextColor(ColorExp2);
		P2(" [日付1] [日付2]");
	iConsole_setTextColor(ColorText1);
		P2("   \".\" \"now\" (現在日時)");
		P2("   \"+2000/01/01\" \"+2000-01-01\"");
		P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"");
		NL();
	iConsole_setTextColor(ColorExp2);
		P2(" [オプション]");
	iConsole_setTextColor(ColorExp3);
		P2("   -format=STR | -f=STR");
	iConsole_setTextColor(ColorText1);
		P ("       ※STRが無指定のとき \"%s\"\n", DATE_FORMAT);
		P2("       %g:+/-表\示");
		P2("       %y:年  %m:月  %d:日  %h:時  %n:分  %s:秒");
		P2("       通算(満)  %M:月  %D:日  %H:時  %N:分  %S:秒  %W:週  %w:週余日");
		P2("       \\t:タブ  \\n:改行");
	iConsole_setTextColor(ColorExp3);
		P2("   -N");
	iConsole_setTextColor(ColorText1);
		P2("       改行しない");
		NL();
	iConsole_setTextColor(ColorExp1);
		P2(" (備考)");
	iConsole_setTextColor(ColorText1);
		P2("   ・ユリウス暦 （-4712/01/01～1582/10/04）");
		P2("   ・グレゴリオ暦（1582/10/15～9999/12/31）");
		P2("    (注1) 空白暦 1582/10/5～1582/10/14 は、\"1582/10/4\" として取扱う。");
		P2("    (注2) BC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。");
		P2("    (注3) プログラム上は、\"JD通日でなくCJD通日\" を使用。");
	iConsole_setTextColor(ColorHeaderFooter);
		LN();
	iConsole_setTextColor($colorDefault); // 元の文字色／背景色
}
