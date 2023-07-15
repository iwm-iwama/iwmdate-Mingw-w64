//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdateadd_20230713"
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
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
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

	INT *iAryDt = { 0 };
	INT *iAryDtAdd = icalloc_INT(6); // ±y, ±m, ±d, ±h, ±n, ±s

	// 代入されたらロック
	BOOL bAryDtLock = FALSE;

	// Main Loop
	for(INT _i1 = 0; _i1 < $ARGC; _i1++)
	{
		// -y
		if((wp1 = iCLI_getOptValue(_i1, L"-y=", NULL)))
		{
			iAryDtAdd[0] += _wtoi(wp1);
		}
		// -m
		else if((wp1 = iCLI_getOptValue(_i1, L"-m=", NULL)))
		{
			iAryDtAdd[1] += _wtoi(wp1);
		}
		// -d
		else if((wp1 = iCLI_getOptValue(_i1, L"-d=", NULL)))
		{
			iAryDtAdd[2] += _wtoi(wp1);
		}
		// -w
		else if((wp1 = iCLI_getOptValue(_i1, L"-w=", NULL)))
		{
			iAryDtAdd[2] += _wtoi(wp1) * 7;
		}
		// -h
		else if((wp1 = iCLI_getOptValue(_i1, L"-h=", NULL)))
		{
			iAryDtAdd[3] += _wtoi(wp1);
		}
		// -n
		else if((wp1 = iCLI_getOptValue(_i1, L"-n=", NULL)))
		{
			iAryDtAdd[4] += _wtoi(wp1);
		}
		// -s
		else if((wp1 = iCLI_getOptValue(_i1, L"-s=", NULL)))
		{
			iAryDtAdd[5] += _wtoi(wp1);
		}
		// -f | -format
		else if((wp1 = iCLI_getOptValue(_i1, L"-f=", L"-format=")))
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
			if(! bAryDtLock)
			{
				if(iCLI_getOptMatch(_i1, L".", L"now"))
				{
					bAryDtLock = TRUE;
					iAryDt = idate_nowToiAryYmdhns_localtime();
				}
				else if(idate_chk_ymdhnsW($ARGV[_i1]))
				{
					bAryDtLock = TRUE;
					iAryDt = idate_WCSToiAryYmdhns($ARGV[_i1]);
				}
			}
		}
	}

	// Err
	if(! bAryDtLock)
	{
		P(CLR_ERR1);
		P1W(L"[Err] 引数 Date を入力してください!");
		P(CLR_RESET);
		NL();
		imain_end();
	}

	INT *iAryDt2 = idate_add(
		iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5],
		iAryDtAdd[0], iAryDtAdd[1], iAryDtAdd[2], iAryDtAdd[3], iAryDtAdd[4], iAryDtAdd[5]
	);

	wp1 = idate_format_ymdhns(_Format, iAryDt2[0], iAryDt2[1], iAryDt2[2], iAryDt2[3], iAryDt2[4], iAryDt2[5]);
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
	MBS *_cmd = W2M($CMD);
	MBS *_format = W2M(DATE_FORMAT);

	print_version();
	P("%s 日時の前後を計算 %s\n", CLR_TITLE1, CLR_RESET);
	P("%s    %s %s[Date] %s[Option]\n", CLR_STR1, _cmd, CLR_OPT1, CLR_OPT2);
	P("%s        or\n", CLR_LBL1);
	P("%s    %s %s[Option] %s[Date]\n", CLR_STR1, _cmd, CLR_OPT2, CLR_OPT1);
	P("\n");
	P("%s (例)\n", CLR_LBL1);
	P("%s    %s %s\"2000/1/1\" %s-y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n", CLR_STR1, _cmd, CLR_OPT1, CLR_OPT2);
	P("\n");
	P("%s [Date]\n", CLR_OPT1);
	P("%s    now  .  (現在日時)\n", CLR_STR1);
	P("%s    \"+2000/01/01\"  \"+2000-01-01\"\n", CLR_STR1);
	P("%s    \"+2000/01/01 00:00:00\"  \"+2000-01-01 00:00:00\"\n", CLR_STR1);
	P("\n");
	P("%s [Option]\n", CLR_OPT2);
	P("%s    -y=[±年]  -m=[±月]  -d=[±日]  -w=[±週]  -h=[±時]  -n=[±分]  -s=[±秒]\n", CLR_OPT21);
	P("\n");
	P("%s    -format=STR | -f=STR\n", CLR_OPT21);
	P("%s        ※STRが無指定のとき \"%s\"\n", CLR_STR1, _format);
	P("%s        %%g：+/-表示\n", CLR_STR1);
	P("%s        %%y：年(0000)  %%m：月(00)  %%d：日(00)\n", CLR_STR1);
	P("%s        %%h：時(00)  %%n：分(00)  %%s：秒(00)\n", CLR_STR1);
	P("%s        %%a：曜日  %%A：曜日数\n", CLR_STR1);
	P("%s        %%c：年通算日  %%C：修正ユリウス通算日  %%J：ユリウス通算日\n", CLR_STR1);
	P("%s        %%e：年通算週\n", CLR_STR1);
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
