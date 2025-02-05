//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2008-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmdatediff"
#define   IWM_UPDATE          "20250130"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      print_version();
VOID      print_help();

// (注)%g付けないと全て正数表示
#define   DATE_FORMAT         "%g%y-%m-%d"
#define   DATE_FORMATW        L"" DATE_FORMAT ""

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

	INT *aiDateBgn = 0;
	INT *aiDateEnd = 0;

	// 日付が代入されたらTRUE
	BOOL bDateFlg1 = FALSE;
	BOOL bDateFlg2 = FALSE;

	// Main Loop
	for(UINT _u1 = 0; _u1 < $ARGC; _u1++)
	{
		// -f | -format
		if((wp1 = iCLI_getOptValue(_u1, L"-f=", L"-format=")))
		{
			_Format = wp1;
		}
		// -N
		else if(iCLI_getOptMatch(_u1, L"-N", NULL))
		{
			_NL = FALSE;
		}
		// "0" など
		else
		{
			// 連続する日付 [date1] [date2] のみ有効
			// 位置は [0..1] or [$ARGC-2..$ARGC-1]
			//   "." "now" => 現在時
			//   "cjd"     => 修正ユリウス開始時
			//   "jd"      => ユリウス開始時
			if(! bDateFlg1)
			{
				if(iCLI_getOptMatch(_u1, L".", L"now"))
				{
					bDateFlg1 = TRUE;
					aiDateBgn = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch(_u1, L"cjd", NULL))
				{
					bDateFlg1 = TRUE;
					aiDateBgn = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch(_u1, L"jd", NULL))
				{
					bDateFlg1 = TRUE;
					aiDateBgn = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[_u1]))
				{
					bDateFlg1 = TRUE;
					aiDateBgn = idate_WsToiAryYmdhns($ARGV[_u1]);
				}
			}
			else if(! bDateFlg2)
			{
				if(iCLI_getOptMatch((_u1), L".", L"now"))
				{
					bDateFlg2 = TRUE;
					aiDateEnd = idate_nowToiAryYmdhns_localtime();
				}
				else if(iCLI_getOptMatch((_u1), L"cjd", NULL))
				{
					bDateFlg2 = TRUE;
					aiDateEnd = idate_WsToiAryYmdhns(CJD_START);
				}
				else if(iCLI_getOptMatch((_u1), L"jd", NULL))
				{
					bDateFlg2 = TRUE;
					aiDateEnd = idate_WsToiAryYmdhns(JD_START);
				}
				else if(idate_chk_ymdhnsW($ARGV[(_u1)]))
				{
					bDateFlg2 = TRUE;
					aiDateEnd = idate_WsToiAryYmdhns($ARGV[(_u1)]);
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
	$struct_iDV *IDV = iDV_alloc();
		idate_diff(
			IDV,
			aiDateBgn[0], aiDateBgn[1], aiDateBgn[2], aiDateBgn[3], aiDateBgn[4], aiDateBgn[5],
			aiDateEnd[0], aiDateEnd[1], aiDateEnd[2], aiDateEnd[3], aiDateEnd[4], aiDateEnd[5]
		);
		wp1 = idate_format(_Format, IDV->sign, IDV->y, IDV->m, IDV->d, IDV->h, IDV->n, IDV->s, IDV->days);
			P1W(wp1);
		ifree(wp1);
	iDV_free(IDV);

	ifree(aiDateEnd);
	ifree(aiDateBgn);

	if(_NL)
	{
		NL();
	}

	///idebug_map(); ifree_all(); idebug_map();

	imain_end();
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P1(
		"\033[2G"	IWM_COPYRIGHT	"\n"
		"\033[5G"	IWM_FILENAME	"_"	IWM_UPDATE	" + "	LIB_IWMUTIL_FILENAME	"\n"
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	print_version();
	P1(
		"\033[1G"	IESC_TITLE1	" 日時差を計算 "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" [Date1] [Date2]"	IESC_OPT2	" [Option]"	"\n"
		"\033[9G"	IESC_LBL1	"or"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [Date1] [Date2]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例)"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" now \"2000/1/1\""	IESC_OPT2	" -f=\"%g%y-%m-%d %h:%n:%s\""	"\n"
		"\n"
		"\033[2G"	IESC_OPT1	"[Date1] [Date2]"	"\n"
		IESC_STR1
		"\033[5G"	"now  .  (現在日時)"	"\n"
		"\033[5G"	"cjd     (修正ユリウス開始日 -4712/01/01 00:00:00)"	"\n"
		"\033[5G"	"jd      (ユリウス開始日     -4712/01/01 12:00:00)"	"\n"
		"\033[5G"	"\"+2000/1/1\"  \"+2000-1-1\""	"\n"
		"\033[5G"	"\"+2000/1/1 00:00:00\"  \"+2000-1-1 00:00:00\""	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		"\033[5G"	IESC_OPT21	"-format=STR | -f=STR"	"\n"
		IESC_STR1
		"\033[9G"	"※STRが無指定のとき \"" DATE_FORMAT "\""	"\n"
		"\033[9G"	"%g：+/-表示"	"\n"
		"\033[9G"	"%y：年  %m：月  %d：日  %h：時  %n：分  %@：秒"	"\n"
		"\033[9G"	"通算  %Y：年  %M：月  %D：日"	"\n"
		"\033[15G"	"%H：時  %N：分  %S：秒"	"\n"
		"\033[15G"	"%W：週  %w：週余日"	"\n"
		"\033[9G"	"\\t：タブ  \\n：改行"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-N"	"\n"
		"\033[9G"	IESC_STR1	"改行しない"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(備考)"	"\n"
		IESC_STR1
		"\033[5G"	"・ユリウス暦 （-4712/01/01～1582/10/04）"	"\n"
		"\033[5G"	"・グレゴリオ暦（1582/10/15～9999/12/31）"	"\n"
		"\033[7G"	IESC_OPT22	"(注１)"	IESC_STR1	" 空白暦 1582/10/05～1582/10/14 は、\"1582/10/04\" として取扱う。"	"\n"
		"\033[7G"	IESC_OPT22	"(注２)"	IESC_STR1	" BC暦は、\"-1/01/01\" を \"0/01/01\" として取扱う。"	"\n"
		"\033[7G"	IESC_OPT22	"(注３)"	IESC_STR1	" プログラム上は、修正ユリウス暦を使用。"	"\n"
		"\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
