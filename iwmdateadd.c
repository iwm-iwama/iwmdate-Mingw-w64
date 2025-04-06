//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2008-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmdateadd"
#define   IWM_UPDATE          "20250312"
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
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_version();
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

	INT *aiAdd = icalloc_INT(6); // ±y, ±m, ±d, ±h, ±n, ±s
	INT *aiDate = 0;

	// 日付が代入されたらTRUE
	BOOL bDateFlg = FALSE;

	// Main Loop
	for(UINT _u1 = 0; _u1 < $ARGC; _u1++)
	{
		// -y
		if((wp1 = iCLI_getOptValue(_u1, L"-y=", NULL)))
		{
			aiAdd[0] += _wtoi(wp1);
		}
		// -m
		else if((wp1 = iCLI_getOptValue(_u1, L"-m=", NULL)))
		{
			aiAdd[1] += _wtoi(wp1);
		}
		// -d
		else if((wp1 = iCLI_getOptValue(_u1, L"-d=", NULL)))
		{
			aiAdd[2] += _wtoi(wp1);
		}
		// -w
		else if((wp1 = iCLI_getOptValue(_u1, L"-w=", NULL)))
		{
			aiAdd[2] += _wtoi(wp1) * 7;
		}
		// -h
		else if((wp1 = iCLI_getOptValue(_u1, L"-h=", NULL)))
		{
			aiAdd[3] += _wtoi(wp1);
		}
		// -n
		else if((wp1 = iCLI_getOptValue(_u1, L"-n=", NULL)))
		{
			aiAdd[4] += _wtoi(wp1);
		}
		// -s
		else if((wp1 = iCLI_getOptValue(_u1, L"-s=", NULL)))
		{
			aiAdd[5] += _wtoi(wp1);
		}
		// -f | -format
		else if((wp1 = iCLI_getOptValue(_u1, L"-f=", L"-format=")))
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
			if(! bDateFlg)
			{
				if(iCLI_getOptMatch(_u1, L".", L"now"))
				{
					bDateFlg = TRUE;
					aiDate = idate_nowToiAryYmdhns_localtime();
				}
				else if(idate_chk_ymdhnsW($ARGV[_u1]))
				{
					bDateFlg = TRUE;
					aiDate = idate_WsToiAryYmdhns($ARGV[_u1]);
				}
			}
		}
	}

	// Err
	if(! bDateFlg)
	{
		P2(
			IESC_FALSE1
			"[Err] 引数 Date を入力してください!"
			IESC_RESET
		);
		imain_end();
	}

	$struct_iDV *IDV = iDV_alloc();
		idate_add(
			IDV,
			aiDate[0], aiDate[1], aiDate[2], aiDate[3], aiDate[4], aiDate[5],
			aiAdd[0], aiAdd[1], aiAdd[2], aiAdd[3], aiAdd[4], aiAdd[5]
		);
		wp1 = idate_format_ymdhns(_Format, IDV->y, IDV->m, IDV->d, IDV->h, IDV->n, IDV->s);
			P1W(wp1);
		ifree(wp1);
	iDV_free(IDV);

	ifree(aiDate);
	ifree(aiAdd);

	if(_NL)
	{
		NL();
	}

	// Debug
	///idebug_map();
	///ifree_all();
	///idebug_map();

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
	P1(
		"\033[1G"	IESC_TITLE1	" 日時の前後を計算 "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" [Date]"	IESC_OPT2	" [Option]"	"\n"
		"\033[9G"	IESC_LBL1	"or"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [Date]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例)"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT1	" \"2000/1/1\""	IESC_OPT2	" -y=8 -m=11 -d=9 -f=\"%g%y-%m-%d(%a) %h:%n:%s\""	"\n"
		"\n"
		"\033[2G"	IESC_OPT1	"[Date]"	"\n"
		IESC_STR1
		"\033[5G"	"now  .  (現在日時)"	"\n"
		"\033[5G"	"\"+2000/1/1\"  \"+2000-1-1\""	"\n"
		"\033[5G"	"\"+2000/1/1 00:00:00\"  \"+2000-1-1 00:00:00\""	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		IESC_OPT21
		"\033[5G"	"-y=[±年]  -m=[±月]  -d=[±日]  -w=[±週]  -h=[±時]  -n=[±分]  -s=[±秒]"	"\n"
		"\n"
		"\033[5G"	"-format=STR | -f=STR"	"\n"
		IESC_STR1
		"\033[9G"	"※STRが無指定のとき \"" DATE_FORMAT "\""	"\n"
		"\033[9G"	"%g：+/-表示"	"\n"
		"\033[9G"	"%y：年(0000)  %m：月(00)  %d：日(00)"	"\n"
		"\033[9G"	"%h：時(00)  %n：分(00)  %s：秒(00)"	"\n"
		"\033[9G"	"%a：曜日  %A：曜日数"	"\n"
		"\033[9G"	"%c：年通算日  %C：修正ユリウス通算日  %J：ユリウス通算日"	"\n"
		"\033[9G"	"%e：年通算週"	"\n"
		"\033[9G"	"\\t：タブ  \\n：改行"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-N"	"\n"
		"\033[9G"	IESC_STR1	"改行しない"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(備考)"	"\n"
		IESC_STR1
		"\033[5G"	"・ユリウス暦   -4712/01/01..1582/10/04"	"\n"
		"\033[5G"	"・グレゴリオ暦  1582/10/15..9999/12/31"	"\n"
		"\033[7G"	IESC_OPT22	"(注１)"	IESC_STR1	" 空白暦 1582/10/05..1582/10/14 は前後関係を考慮し、1582/10/04 または 1582/10/15 として扱う。"	"\n"
		"\033[7G"	IESC_OPT22	"(注２)"	IESC_STR1	" BC暦は、-1/01/01 を 0/01/01 として扱う。"	"\n"
		"\033[7G"	IESC_OPT22	"(注３)"	IESC_STR1	" プログラム上は、修正ユリウス暦を使用。"	"\n"
		"\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
