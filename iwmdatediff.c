//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdatediff_20230412"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2023 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT  main();
VOID print_version();
VOID print_help();

#define   CLR_RESET           "\033[0m"
#define   CLR_TITLE1          "\033[38;2;250;250;250m\033[104m" // 白／青
#define   CLR_OPT1            "\033[38;2;250;150;150m"          // 赤
#define   CLR_OPT2            "\033[38;2;150;150;250m"          // 青
#define   CLR_OPT21           "\033[38;2;80;250;250m"           // 水
#define   CLR_OPT22           "\033[38;2;250;100;250m"          // 紅紫
#define   CLR_LBL1            "\033[38;2;250;250;100m"          // 黄
#define   CLR_LBL2            "\033[38;2;100;100;250m"          // 青
#define   CLR_STR1            "\033[38;2;225;225;225m"          // 白
#define   CLR_STR2            "\033[38;2;175;175;175m"          // 銀
#define   CLR_ERR1            "\033[38;2;200;0;0m"              // 紅

#define   DATE_FORMAT         L"%g%y-%m-%d" // (注)%g付けないと全て正数表示

#define   CJD                 L"-4712-01-01 00:00:00"
#define   JD                  L"-4712-01-01 12:00:00"

// 出力フォーマット
//   -f=STR | -format=STR
WCS *_Format = DATE_FORMAT;

// 改行するとき TRUE
//   -N
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil 初期化
	iExecSec_init();       //=> $ExecSecBgn
	iCLI_getCommandLine(); //=> $CMD, $ARGC, $ARGV
	iConsole_EscOn();

	// -h | -help
	if($ARGC < 2 || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
		imain_end();
	}

	WCS *wp1 = 0;

	INT *iAryDtBgn = { 0 };
	INT *iAryDtEnd = { 0 };

	// 代入されたらロック
	BOOL bAryDtBgnLock = FALSE;
	BOOL bAryDtEndLock = FALSE;

	// Main Loop
	for(INT _i1 = 0; _i1 < $ARGC; _i1++)
	{
		// -f | -format
		if((wp1 = iCLI_getOptValue(_i1, L"-f=", L"-format=")))
		{
			_Format = wp1;
		}
		// -N
		else if(iCLI_getOptMatch(_i1, L"-N", NULL))
		{
			_NL = FALSE;
		}
		// -1/1/1 など
		else
		{
			/*
				連続する日付 [date1] [date2] のみ有効
				位置は [0..1] or [$ARGC-2..$ARGC-1]
					"." "now" => 現在時
					"cjd"     => 修正ユリウス開始時
					"jd"      => ユリウス開始時
			*/
			if(! bAryDtBgnLock)
			{
				if(iCLI_getOptMatch(_i1, L".", L"now"))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_now_to_iAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch(_i1, L"cjd", NULL))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WCS_to_iAryYmdhns(CJD);
				}
				else if(iCLI_getOptMatch(_i1, L"jd", NULL))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WCS_to_iAryYmdhns(JD);
				}
				else if(idate_chk_ymdhnsW($ARGV[_i1]))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WCS_to_iAryYmdhns($ARGV[_i1]);
				}
			}
			else if(! bAryDtEndLock)
			{
				if(iCLI_getOptMatch((_i1), L".", L"now"))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_now_to_iAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch((_i1), L"cjd", NULL))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WCS_to_iAryYmdhns(CJD);
				}
				else if(iCLI_getOptMatch((_i1), L"jd", NULL))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WCS_to_iAryYmdhns(JD);
				}
				else if(idate_chk_ymdhnsW($ARGV[(_i1)]))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WCS_to_iAryYmdhns($ARGV[(_i1)]);
				}
			}
		}
	}

	// Err
	if(! bAryDtBgnLock || ! bAryDtEndLock)
	{
		P(CLR_ERR1);
		P1W(L"[Err] 引数 Date1, Date2 を入力してください!");
		P(CLR_RESET);
		NL();
		imain_end();
	}

	// diff[8]
	INT *iAryDiff = idate_diff(
		iAryDtBgn[0], iAryDtBgn[1], iAryDtBgn[2], iAryDtBgn[3], iAryDtBgn[4], iAryDtBgn[5],
		iAryDtEnd[0], iAryDtEnd[1], iAryDtEnd[2], iAryDtEnd[3], iAryDtEnd[4], iAryDtEnd[5]
	);

	wp1 = idate_format_diff(_Format, iAryDiff[0], iAryDiff[1], iAryDiff[2], iAryDiff[3], iAryDiff[4], iAryDiff[5], iAryDiff[6], iAryDiff[7]);
		P1W(wp1);
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
	P(CLR_STR2);
	LN();
	P(" %s\n", IWM_COPYRIGHT);
	P("    Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
	P(CLR_RESET);
}

VOID
print_help()
{
	MBS *_cmd = W2U($CMD);
	MBS *_format = W2U(DATE_FORMAT);

	print_version();
	P("%s 日時差を計算 %s\n", CLR_TITLE1, CLR_RESET);
	P("%s    %s %s[Date1] [Date2] %s[Option]\n", CLR_STR1, _cmd, CLR_OPT1, CLR_OPT2);
	P("%s        or\n", CLR_LBL1);
	P("%s    %s %s[Option] %s[Date1] [Date2]\n", CLR_STR1, _cmd, CLR_OPT2, CLR_OPT1);
	P("\n");
	P("%s (例)\n", CLR_LBL1);
	P("%s    %s %s now \"2000/01/01\" %s-f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n", CLR_STR1, _cmd, CLR_OPT1, CLR_OPT2);
	P("\n");
	P("%s [Date1] [Date2]\n", CLR_OPT1);
	P("%s    now  .  (現在日時)\n", CLR_STR1);
	P("%s    cjd     (修正ユリウス開始日 -4712/01/01 00:00:00)\n", CLR_STR1);
	P("%s    jd      (ユリウス開始日     -4712/01/01 12:00:00)\n", CLR_STR1);
	P("%s    \"+2000/01/01"  "+2000-01-01\"\n", CLR_STR1);
	P("%s    \"+2000/01/01 00:00:00"  "+2000-01-01 00:00:00\"\n", CLR_STR1);
	P("\n");
	P("%s [Option]\n", CLR_OPT2);
	P("%s    -format=STR | -f=STR\n", CLR_OPT21);
	P("%s        ※STRが無指定のとき \"%s\"\n", CLR_STR1, _format);
	P("%s        %%g：+/-表示\n", CLR_STR1);
	P("%s        %%y：年  %%m：月  %%d：日  %%h：時  %%n：分  %%s：秒\n", CLR_STR1);
	P("%s        通算  %%Y：年  %%M：月  %%D：日\n", CLR_STR1);
	P("%s              %%H：時  %%N：分  %%S：秒\n", CLR_STR1);
	P("%s              %%W：週  %%w：週余日\n", CLR_STR1);
	P("%s        \\t：タブ  \\n：改行\n", CLR_STR1);
	P("%s    -N\n", CLR_OPT21);
	P("%s        改行しない\n", CLR_STR1);
	P("\n");
	P("%s (備考)\n", CLR_LBL1);
	P("%s    ・ユリウス暦 （-4712/01/01～1582/10/04）\n", CLR_STR1);
	P("%s    ・グレゴリオ暦（1582/10/15～9999/12/31）\n", CLR_STR1);
	P("%s      (注１) %s空白暦 1582/10/5～1582/10/14 は、\"1582/10/4\" として取扱う。\n", CLR_OPT22, CLR_STR1);
	P("%s      (注２) %sBC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。\n", CLR_OPT22, CLR_STR1);
	P("%s      (注３) %sプログラム上は、修正ユリウス暦を使用。\n", CLR_OPT22, CLR_STR1);
	P("\n");
	P(CLR_STR2);
	LN();
	P(CLR_RESET);

	ifree(_format);
	ifree(_cmd);
}
