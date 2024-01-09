//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2008-2023 iwm-iwama"
#define   IWM_VERSION         "iwmdatediff_20231230"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      print_version();
VOID      print_help();

// (注)%g付けないと全て正数表示
#define   DATE_FORMATM        "%g%y-%m-%d"
#define   DATE_FORMATW        L"%g%y-%m-%d"

// 出力フォーマット
//   -f=STR | -format=STR
WS *_Format = DATE_FORMATW;

// 改行するとき TRUE
//   -N
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	///iCLI_VarList();

	// -h | --help
	if($ARGC < 2 || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_help();
		imain_end();
	}

	// -v | --version
	if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
		imain_end();
	}

	WS *wp1 = 0;

	INT *iAryDtBgn = { 0 };
	INT *iAryDtEnd = { 0 };

	// 日付が代入されたらTRUE
	BOOL bDateFlg1 = FALSE;
	BOOL bDateFlg2 = FALSE;

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
			if(! bDateFlg1)
			{
				if(iCLI_getOptMatch(_i1, L".", L"now"))
				{
					bDateFlg1 = TRUE;
					iAryDtBgn = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch(_i1, L"cjd", NULL))
				{
					bDateFlg1 = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch(_i1, L"jd", NULL))
				{
					bDateFlg1 = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[_i1]))
				{
					bDateFlg1 = TRUE;
					iAryDtBgn = idate_WsToiAryYmdhns($ARGV[_i1]);
				}
			}
			else if(! bDateFlg2)
			{
				if(iCLI_getOptMatch((_i1), L".", L"now"))
				{
					bDateFlg2 = TRUE;
					iAryDtEnd = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch((_i1), L"cjd", NULL))
				{
					bDateFlg2 = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch((_i1), L"jd", NULL))
				{
					bDateFlg2 = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[(_i1)]))
				{
					bDateFlg2 = TRUE;
					iAryDtEnd = idate_WsToiAryYmdhns($ARGV[(_i1)]);
				}
			}
		}
	}

	// Err
	if(! bDateFlg1 || ! bDateFlg2)
	{
		P2(
			IESC_FALSE1
			"[Err] 引数 Date1, Date2 を入力してください!"
			IESC_RESET
		);
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

	///icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

	imain_end();
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P(
		" %s\n"
		"    %s+%s\n"
		,
		IWM_COPYRIGHT,
		IWM_VERSION,
		LIB_IWMUTIL_VERSION
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	MS *_cmd = "iwmdatediff.exe";
	MS *_format = DATE_FORMATM;

	print_version();
	P(
		IESC_TITLE1	" 日時差を計算 "
		IESC_RESET	"\n"
		IESC_STR1	"    %s"
		IESC_OPT1	" [Date1] [Date2]"
		IESC_OPT2	" [Option]\n"
		IESC_LBL1	"        or\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [Date1] [Date2]\n\n"
		IESC_LBL1	" (例)\n"
		IESC_STR1	"    %s"
		IESC_OPT1	" now \"2000/1/1\""
		IESC_OPT2	" -f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n\n"
		,
		_cmd,
		_cmd,
		_cmd
	);
	P1(
		IESC_OPT1	" [Date1] [Date2]\n"
		IESC_STR1	"    now  .  (現在日時)\n"
					"    cjd     (修正ユリウス開始日 -4712/01/01 00:00:00)\n"
					"    jd      (ユリウス開始日     -4712/01/01 12:00:00)\n"
					"    \"+2000/1/1\"  \"+2000-1-1\"\n"
					"    \"+2000/1/1 00:00:00\"  \"+2000-1-1 00:00:00\"\n\n"
	);
	P(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -format=STR | -f=STR\n"
		IESC_STR1	"        ※STRが無指定のとき \"%s\"\n"
					"        %%g：+/-表示\n"
					"        %%y：年  %%m：月  %%d：日  %%h：時  %%n：分  %@：秒\n"
					"        通算  %%Y：年  %%M：月  %%D：日\n"
					"              %%H：時  %%N：分  %%S：秒\n"
					"              %%W：週  %%w：週余日\n"
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
}
