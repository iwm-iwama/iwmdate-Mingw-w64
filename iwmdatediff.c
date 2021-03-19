//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdatediff_20210319"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2021 iwm-iwama"
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

// タイトル
#define   COLOR01             (15 + ( 9 * 16))
// 入力例／注
#define   COLOR11             (15 + (12 * 16))
#define   COLOR12             (13 + ( 0 * 16))
#define   COLOR13             (12 + ( 0 * 16))
// 引数
#define   COLOR21             (14 + ( 0 * 16))
#define   COLOR22             (11 + ( 0 * 16))
// 説明
#define   COLOR91             (15 + ( 0 * 16))
#define   COLOR92             ( 7 + ( 0 * 16))

#define   DATE_FORMAT         "%g%y-%m-%d" // (注)%g付けないと全て正数表示

#define   CJD                 "-4712-01-01 00:00:00"
#define   JD                  "-4712-01-01 12:00:00"

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

INT
main()
{
	// lib_iwmutil 初期化
	iCLI_getCmd();       //=> $IWM_Cmd
	iCLI_getCmdOpt();    //=> $IWM_CmdOption, $IWM_CmdOptionSize
	iConsole_getColor(); //=> $IWM_ColorDefault, $IWM_StdoutHandle
	iExecSec_init();     //=> $IWM_ExecSecBgn

	// -help "-h"はhour
	if(! $IWM_CmdOptionSize || imb_cmpp($IWM_CmdOption[0], "-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(imb_cmpp($IWM_CmdOption[0], "-v") || imb_cmpp($IWM_CmdOption[0], "-version"))
	{
		print_version();
		imain_end();
	}

	INT *iAryDtBgn = icalloc_INT(6); // y, m, d, h, n, s
	INT *iAryDtEnd = icalloc_INT(6); // y, m, d, h, n, s

	// [0], [1]
	/*
		"." "now" => 現在時
		"cjd"     => 修正ユリウス開始時
		"jd"      => ユリウス開始時
	*/
	if(imb_cmpp($IWM_CmdOption[0], ".") || imb_cmpp($IWM_CmdOption[0], "now"))
	{
		iAryDtBgn = idate_now_to_iAryYmdhns_localtime();
	}
	else if(imb_cmpp($IWM_CmdOption[0], "cjd"))
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns(CJD);
	}
	else if(imb_cmpp($IWM_CmdOption[0], "jd"))
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns(JD);
	}
	else
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns($IWM_CmdOption[0]);
	}

	if(imb_cmpp($IWM_CmdOption[1], ".") || imb_cmpp($IWM_CmdOption[1], "now"))
	{
		iAryDtEnd = idate_now_to_iAryYmdhns_localtime();
	}
	else if(imb_cmpp($IWM_CmdOption[1], "cjd"))
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns(CJD);
	}
	else if(imb_cmpp($IWM_CmdOption[1], "jd"))
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns(JD);
	}
	else
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns($IWM_CmdOption[1]);
	}

	// [2..]
	for(INT _i1 = 2; _i1 < $IWM_CmdOptionSize; _i1++)
	{
		MBS **_as1 = ija_split($IWM_CmdOption[_i1], "=", "\"\"\'\'", FALSE);
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
	INT *iAryDiff = idate_diff(
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
	P (" %s\n", IWM_COPYRIGHT);
	P ("   Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
}

VOID
print_help()
{
	PZ(COLOR92, NULL);
		print_version();
	PZ(COLOR01, " 日時差を計算 \n\n");
	PZ(COLOR11, " %s [日付1] [日付2] [オプション] \n\n", $IWM_Cmd);
	PZ(COLOR12, " (使用例)\n");
	PZ(COLOR91, "   %s \"now\" \"2000/01/01\" -f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n\n", $IWM_Cmd);
	PZ(COLOR21, " [日付1] [日付2]\n");
	PZ(COLOR91, NULL);
		P2("   \"now\" \".\" (現在日時)");
		P2("   \"cjd\"     (修正ユリウス開始日 -4712/01/01 00:00:00)");
		P2("   \"jd\"      (ユリウス開始日     -4712/01/01 12:00:00)");
		P2("   \"+2000/01/01\" \"+2000-01-01\"");
		P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"\n");
	PZ(COLOR21, " [オプション]\n");
	PZ(COLOR22, "   -format=STR | -f=STR\n");
	PZ(COLOR91, NULL);
		P ("       ※STRが無指定のとき \"%s\"\n", DATE_FORMAT);
		P2("       %g : +/-表\示");
		P2("       %y : 年  %m : 月  %d : 日  %h : 時  %n : 分  %s : 秒");
		P2("       通算  %Y : 年  %M : 月  %D : 日");
		P2("             %H : 時  %N : 分  %S : 秒");
		P2("             %W : 週  %w : 週余日");
		P2("       \\t : タブ  \\n : 改行");
	PZ(COLOR22, "   -N\n");
	PZ(COLOR91, "       改行しない\n\n");
	PZ(COLOR12, " (備考)\n");
	PZ(COLOR91, NULL);
		P2("   ・ユリウス暦 （-4712/01/01～1582/10/04）");
		P2("   ・グレゴリオ暦（1582/10/15～9999/12/31）");
	PZ(COLOR13, "    (注１) ");
	PZ(COLOR91, "空白暦 1582/10/5～1582/10/14 は、\"1582/10/4\" として取扱う。\n");
	PZ(COLOR13, "    (注２) ");
	PZ(COLOR91, "BC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。\n");
	PZ(COLOR13, "    (注３) ");
	PZ(COLOR91, "プログラム上は、修正ユリウス暦を使用。\n\n");
	PZ(COLOR92, NULL);
		LN();
	PZ(-1, NULL);
}
