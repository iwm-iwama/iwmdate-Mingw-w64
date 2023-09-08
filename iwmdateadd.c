//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdateadd_20230828"
#define   IWM_COPYRIGHT       "Copyright (C)2008-2023 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      print_version();
VOID      print_help();

#define   DATE_FORMAT         L"%g%y-%m-%d" // (注)%g付けないと全て正数表示

// 出力フォーマット
//   -f=STR | -format=STR
WS *_Format = DATE_FORMAT;

// 改行するとき TRUE
//   -N
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

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

	WS *wp1 = 0;

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
		// "0" など
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
					iAryDt = idate_WsToiAryYmdhns($ARGV[_i1]);
				}
			}
		}
	}

	// Err
	if(! bAryDtLock)
	{
		P(IESC_ERR1);
		P1W(L"[Err] 引数 Date を入力してください!");
		P(IESC_RESET);
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
	P1(IESC_STR2);
	LN(80);
	P(
		" %s\n"
		"    Ver.%s+%s\n"
		, IWM_COPYRIGHT, IWM_VERSION, LIB_IWMUTIL_VERSION
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);
	MS *_format = W2M(DATE_FORMAT);

	print_version();
	P(
		IESC_TITLE1	" 日時の前後を計算 "
		IESC_RESET	"\n"
		IESC_STR1	"    %s"
		IESC_OPT1	" [Date]"
		IESC_OPT2	" [Option]\n"
		IESC_LBL1	"        or\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [Date]\n\n"
		IESC_LBL1	" (例)\n"
		IESC_STR1	"    %s"
		IESC_OPT1	" \"2000/1/1\""
		IESC_OPT2	" -y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n"
		, _cmd, _cmd, _cmd
	);
	P1(
		IESC_OPT1	" [Date]\n"
		IESC_STR1	"    now  .  (現在日時)\n"
					"    \"+2000/1/1\"  \"+2000-1-1\"\n"
					"    \"+2000/1/1 00:00:00\"  \"+2000-1-1 00:00:00\"\n\n"
	);
	P(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -y=[±年]  -m=[±月]  -d=[±日]  -w=[±週]  -h=[±時]  -n=[±分]  -s=[±秒]\n\n"
					"    -format=STR | -f=STR\n"
		IESC_STR1	"        ※STRが無指定のとき \"%s\"\n"
					"        %%g：+/-表示\n"
					"        %%y：年(0000)  %%m：月(00)  %%d：日(00)\n"
					"        %%h：時(00)  %%n：分(00)  %%s：秒(00)\n"
					"        %%a：曜日  %%A：曜日数\n"
					"        %%c：年通算日  %%C：修正ユリウス通算日  %%J：ユリウス通算日\n"
					"        %%e：年通算週\n"
					"        \\t：タブ  \\n：改行\n\n"
		IESC_OPT21	"    -N\n"
		IESC_STR1	"        改行しない\n\n"
		, _format
	);
	P1(
		IESC_LBL1	" (備考)\n"
		IESC_STR1	"    ・ユリウス暦 （-4712/01/01～1582/10/04）\n"
					"    ・グレゴリオ暦（1582/10/15～9999/12/31）\n"
		IESC_OPT22	"      (注１)"
		IESC_STR1	" 空白暦 1582/10/05～1582/10/14 は、\"1582/10/04\" として取扱う。\n"
		IESC_OPT22	"      (注２)"
		IESC_STR1	" BC暦は、\"-1/01/01\" を \"0/01/01\" として取扱う。\n"
		IESC_OPT22	"      (注３)"
		IESC_STR1	" プログラム上は、修正ユリウス暦を使用。\n\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);

	ifree(_format);
	ifree(_cmd);
}
