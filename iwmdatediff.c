//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmdatediff_20230809"
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

	WS *wp1 = 0;

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
		// "0" など
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
					iAryDtBgn = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch(_i1, L"cjd", NULL))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch(_i1, L"jd", NULL))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[_i1]))
				{
					bAryDtBgnLock = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns($ARGV[_i1]);
				}
			}
			else if(! bAryDtEndLock)
			{
				if(iCLI_getOptMatch((_i1), L".", L"now"))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch((_i1), L"cjd", NULL))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch((_i1), L"jd", NULL))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[(_i1)]))
				{
					bAryDtEndLock = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns($ARGV[(_i1)]);
				}
			}
		}
	}

	// Err
	if(! bAryDtBgnLock || ! bAryDtEndLock)
	{
		P(ICLR_ERR1);
		P1W(L"[Err] 引数 Date1, Date2 を入力してください!");
		P(ICLR_RESET);
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
	P(ICLR_STR2);
	LN(80);
	P(" %s\n", IWM_COPYRIGHT);
	P("    Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN(80);
	P(ICLR_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);
	MS *_format = W2M(DATE_FORMAT);

	print_version();
	P("%s 日時差を計算 %s\n", ICLR_TITLE1, ICLR_RESET);
	P("%s    %s %s[Date1] [Date2] %s[Option]\n", ICLR_STR1, _cmd, ICLR_OPT1, ICLR_OPT2);
	P("%s        or\n", ICLR_LBL1);
	P("%s    %s %s[Option] %s[Date1] [Date2]\n", ICLR_STR1, _cmd, ICLR_OPT2, ICLR_OPT1);
	P("\n");
	P("%s (例)\n", ICLR_LBL1);
	P("%s    %s %s now \"2000/01/01\" %s-f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n", ICLR_STR1, _cmd, ICLR_OPT1, ICLR_OPT2);
	P("\n");
	P("%s [Date1] [Date2]\n", ICLR_OPT1);
	P("%s    now  .  (現在日時)\n", ICLR_STR1);
	P("%s    cjd     (修正ユリウス開始日 -4712/01/01 00:00:00)\n", ICLR_STR1);
	P("%s    jd      (ユリウス開始日     -4712/01/01 12:00:00)\n", ICLR_STR1);
	P("%s    \"+2000/01/01"  "+2000-01-01\"\n", ICLR_STR1);
	P("%s    \"+2000/01/01 00:00:00"  "+2000-01-01 00:00:00\"\n", ICLR_STR1);
	P("\n");
	P("%s [Option]\n", ICLR_OPT2);
	P("%s    -format=STR | -f=STR\n", ICLR_OPT21);
	P("%s        ※STRが無指定のとき \"%s\"\n", ICLR_STR1, _format);
	P("%s        %%g：+/-表示\n", ICLR_STR1);
	P("%s        %%y：年  %%m：月  %%d：日  %%h：時  %%n：分  %%s：秒\n", ICLR_STR1);
	P("%s        通算  %%Y：年  %%M：月  %%D：日\n", ICLR_STR1);
	P("%s              %%H：時  %%N：分  %%S：秒\n", ICLR_STR1);
	P("%s              %%W：週  %%w：週余日\n", ICLR_STR1);
	P("%s        \\t：タブ  \\n：改行\n", ICLR_STR1);
	P("%s    -N\n", ICLR_OPT21);
	P("%s        改行しない\n", ICLR_STR1);
	P("\n");
	P("%s (備考)\n", ICLR_LBL1);
	P("%s    ・ユリウス暦 （-4712/01/01～1582/10/04）\n", ICLR_STR1);
	P("%s    ・グレゴリオ暦（1582/10/15～9999/12/31）\n", ICLR_STR1);
	P("%s      (注１) %s空白暦 1582/10/5～1582/10/14 は、\"1582/10/4\" として取扱う。\n", ICLR_OPT22, ICLR_STR1);
	P("%s      (注２) %sBC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。\n", ICLR_OPT22, ICLR_STR1);
	P("%s      (注３) %sプログラム上は、修正ユリウス暦を使用。\n", ICLR_OPT22, ICLR_STR1);
	P("\n");
	P(ICLR_STR2);
	LN(80);
	P(ICLR_RESET);

	ifree(_format);
	ifree(_cmd);
}
