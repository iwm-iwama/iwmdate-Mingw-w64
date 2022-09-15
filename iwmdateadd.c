//------------------------------------------------------------------------------
#define  IWM_VERSION         "iwmdateadd_20220912"
#define  IWM_COPYRIGHT       "Copyright (C)2008-2022 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT  main();
VOID print_version();
VOID print_help();

// リセット
#define  PRGB00()            P0("\033[0m")
// ラベル
#define  PRGB01()            P0("\033[38;2;255;255;0m")    // 黄
#define  PRGB02()            P0("\033[38;2;255;255;255m")  // 白
// 入力例／注
#define  PRGB11()            P0("\033[38;2;255;255;100m")  // 黄
#define  PRGB12()            P0("\033[38;2;255;220;150m")  // 橙
#define  PRGB13()            P0("\033[38;2;100;100;255m")  // 青
// オプション
#define  PRGB21()            P0("\033[38;2;80;255;255m")   // 水
#define  PRGB22()            P0("\033[38;2;255;100;255m")  // 紅紫
// 本文
#define  PRGB91()            P0("\033[38;2;255;255;255m")  // 白
#define  PRGB92()            P0("\033[38;2;200;200;200m")  // 銀

#define  DATE_FORMAT         L"%g%y-%m-%d" // (注)%g付けないと全て正数表示

/*
	出力フォーマット
	-f=STR | -format=STR
*/
WCS *_Format = DATE_FORMAT;
/*
	改行するとき TRUE
	-N
*/
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil 初期化
	iExecSec_init();       //=> $ExecSecBgn
	iCLI_getCommandLine(); //=> $CMD, $ARGC, $ARGV, $ARGS
	iConsole_EscOn();

	// -h | -help
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(iCLI_getOptMatch(0, L"-v", L"-version"))
	{
		print_version();
		imain_end();
	}

	WCS *wp1 = 0;
	U8N *up1 = 0;

	INT *iAryDt = { 0 };

	// [0]
	/*
		"." "now" => 現在時
	*/
	if(iCLI_getOptMatch(0, L".", L"now"))
	{
		iAryDt = idate_now_to_iAryYmdhns_localtime();
	}
	else
	{
		iAryDt = idate_WCS_to_iAryYmdhns($ARGS[0]);
	}

	INT *iAryDtAdd = icalloc_INT(6); // ±y, ±m, ±d, ±h, ±n, ±s

	// [1..]
	for(INT _i1 = 1; _i1 < $ARGC; _i1++)
	{
		// -y
		if((wp1 = iCLI_getOptValue(_i1, L"-y=", NULL)))
		{
			iAryDtAdd[0] += inum_wtoi(wp1);
		}

		// -m
		if((wp1 = iCLI_getOptValue(_i1, L"-m=", NULL)))
		{
			iAryDtAdd[1] += inum_wtoi(wp1);
		}

		// -d
		if((wp1 = iCLI_getOptValue(_i1, L"-d=", NULL)))
		{
			iAryDtAdd[2] += inum_wtoi(wp1);
		}

		// -w
		if((wp1 = iCLI_getOptValue(_i1, L"-w=", NULL)))
		{
			iAryDtAdd[2] += inum_wtoi(wp1) * 7;
		}

		// -h
		if((wp1 = iCLI_getOptValue(_i1, L"-h=", NULL)))
		{
			iAryDtAdd[3] += inum_wtoi(wp1);
		}

		// -n
		if((wp1 = iCLI_getOptValue(_i1, L"-n=", NULL)))
		{
			iAryDtAdd[4] += inum_wtoi(wp1);
		}

		// -s
		if((wp1 = iCLI_getOptValue(_i1, L"-s=", NULL)))
		{
			iAryDtAdd[5] += inum_wtoi(wp1);
		}

		// -f | -format
		if((wp1 = iCLI_getOptValue(_i1, L"-f=", L"-format=")))
		{
			_Format = iws_clone(wp1);
		}

		// -N
		if(iCLI_getOptMatch(_i1, L"-N", NULL))
		{
			_NL = FALSE;
		}
	}

	INT *iAryDt2 = idate_add(
		iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5],
		iAryDtAdd[0], iAryDtAdd[1], iAryDtAdd[2], iAryDtAdd[3], iAryDtAdd[4], iAryDtAdd[5]
	);

	wp1 = idate_format_ymdhns(_Format, iAryDt2[0], iAryDt2[1], iAryDt2[2], iAryDt2[3], iAryDt2[4], iAryDt2[5]);
	up1 = W2U(wp1);
		P0(up1);
	ifree(up1);
	ifree(wp1);

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
	PRGB92();
	LN();
	P (" %s\n", IWM_COPYRIGHT);
	P ("   Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
	PRGB00();
}

VOID
print_help()
{
	U8N *_cmd = W2U($CMD);
	U8N *_format = W2U(DATE_FORMAT);

	print_version();
	PRGB01();
	P2("\033[48;2;50;50;200m 日時の前後を計算 \033[0m");
	NL();
	PRGB02();
	P ("\033[48;2;200;50;50m %s [Date] [Option] \033[0m\n\n", _cmd);
	PRGB11();
	P2(" (使用例)");
	PRGB91();
	P ("   %s \033[38;2;255;150;150m\"2000/1/1\" \033[38;2;150;150;255m-y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n", _cmd);
	PRGB02();
	P2("\033[48;2;200;50;50m [Date] \033[0m");
	PRGB91();
	P2("   now  .  (現在日時)");
	P2("   \"+2000/01/01\"  \"+2000-01-01\"");
	P2("   \"+2000/01/01 00:00:00\"  \"+2000-01-01 00:00:00\"");
	NL();
	PRGB02();
	P2("\033[48;2;200;50;50m [Option] \033[0m");
	PRGB21();
	P2("   -y=[±年]  -m=[±月]  -d=[±日]  -w=[±週]");
	PRGB21();
	P2("   -h=[±時]  -n=[±分]  -s=[±秒]");
	NL();
	PRGB21();
	P2("   -format=STR | -f=STR");
	PRGB91();
	P ("       ※STRが無指定のとき \"%s\"\n", _format);
	P2("       %g：+/-表示");
	P2("       %y：年(0000)  %m：月(00)  %d：日(00)");
	P2("       %h：時(00)  %n：分(00)  %s：秒(00)");
	P2("       %a：曜日  %A：曜日数");
	P2("       %c：年通算日  %C：修正ユリウス通算日  %J：ユリウス通算日");
	P2("       %e：年通算週");
	P2("       \\t：タブ  \\n：改行");
	PRGB21();
	P2("   -N");
	PRGB91();
	P2("       改行しない");
	NL();
	PRGB11();
	P2(" (備考)");
	PRGB91();
	P2("   ・ユリウス暦 （-4712/01/01～1582/10/04）");
	P2("   ・グレゴリオ暦（1582/10/15～9999/12/31）");
	PRGB12();
	P0("    (注１) ");
	PRGB91();
	P2("空白暦 1582/10/5～1582/10/14 は、\"1582/10/4\" として取扱う。");
	PRGB12();
	P0("    (注２) ");
	PRGB91();
	P2("BC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。");
	PRGB12();
	P0("    (注３) ");
	PRGB91();
	P2("プログラム上は、修正ユリウス暦を使用。");
	NL();
	PRGB92();
	LN();
	PRGB00();

	ifree(_format);
	ifree(_cmd);
}
