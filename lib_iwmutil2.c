/*
[2011-04-16]
	入口でのチェックは、利用者に『安全』を提供する。
	知り得ている危険に手を抜いてはいけない。たとえ、コード量が増加しようとも。
	『安全』と『速度』は反比例しない。

[2013-01-31] +[2022-09-03]
	マクロで複雑な関数を書くな。デバッグが難しくなる。
	「コードの短さ」より「コードの生産性」を優先する。

[2014-01-03] +[2022-09-03]
	今更ながら・・・、
	自作関数の返り値は以下のルールに拠る。
		NULL   : NULL
		エラー : 新規ポインタ（空データ）
		成功   : 新規ポインタ
	元の引数ポインタを引用して返すのはエラーの元凶。

[2014-01-03] +[2022-09-03]
	動的メモリ確保と初期化／解放をこまめに行えば、
	十分な『安全』と『速度』を今時のハードウェアは提供する。

[2014-02-13] +[2022-09-03]
	使用する変数／Minqw-w64
		INT    : 負値を返す関数
		UINT   : NTFS関連
		INT64  : ファイルサイズ
		DOUBLE : 暦

[2016-08-19] +[2022-09-04]
	大域変数・定数表記
		大域変数    : １文字目は "$"
			$ARGV, $icallocMap など
		#define定数 : 全大文字
			IMAX_PATH, WCS など

[2016-01-27] +[2022-09-03]
	基本関数名ルール
		i = iwm-iwama
		m = MBS(byte基準)／u = U8N／w = WCS
		a = array／b = boolean／n = number／p = pointer／s = strings

[2021-11-18] +[2022-09-03]
	ポインタ *(p + n) と配列 p[n] どちらが速い？
		Mingw-w64（-Os）においては速度低下を感じない。
		従来、高速とされるポインタ型でコーディングしていたが、
		今後は、（速度を要求しない箇所に限定して）可読性を考慮した配列型でコーディングする。

[2022-09-04]
	「iwmutil2」から「入力・内部＝Unicode／出力＝UTF-8(CP65001)」処理系に移行した。
	※「iwmutil」は「入力・内部・出力＝Shift_JIS(CP932)」処理系

*/
#include "lib_iwmutil2.h"
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	大域変数
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
WCS      *$CMD         = L"";   // コマンド名を格納
UINT     $ARGC         = 0;     // 引数配列数
WCS      **$ARGV       = { 0 }; // 引数配列
WCS      **$ARGS       = { 0 }; // $ARGVからダブルクォーテーションを消去したもの
UINT     $CP           = 0;     // 出力コードページ
HANDLE   $StdoutHandle = 0;     // 画面制御用ハンドル
UINT     $ExecSecBgn   = 0;     // 実行開始時間
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Command Line
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------
// コマンド名／引数を取得
//-------------------------
/* (例)
	iCLI_getCommandLine(65001);
	PL2($CMD);
	PL3($ARGC);
	iwa_print($ARGV);
	iwa_print($ARGS); // $ARGVからダブルクォーテーションを除去したもの
*/
// v2022-08-29
VOID
iCLI_getCommandLine(
	UINT codepage // 出力コード (例) 65001 = UTF-8
)
{
	SetConsoleOutputCP(codepage);
	$CP = codepage;

	WCS *str = GetCommandLineW();

	CONST UINT AryMax = 32;
	WCS **argv = icalloc_WCS_ary(AryMax);
	WCS **args = icalloc_WCS_ary(AryMax);

	WCS *pBgn = 0;
	WCS *pEnd = 0;

	WCS *wp1 = 0;
	BOOL bArgc = FALSE;
	$ARGC = 0;

	for(pEnd = str; pEnd < (str + iwn_len(str)) && $ARGC < AryMax; pEnd++)
	{
		for(; *pEnd == ' '; pEnd++);

		if(*pEnd == '\"')
		{
			pBgn = pEnd;
			++pEnd;
			for(; *pEnd && *pEnd != '\"'; pEnd++);
			++pEnd;
			wp1 = iws_pclone(pBgn, pEnd);
		}
		else
		{
			pBgn = pEnd;
			for(; *pEnd && *pEnd != ' '; pEnd++)
			{
				if(*pEnd == '\"')
				{
					++pEnd;
					for(; *pEnd && *pEnd != '\"'; pEnd++);
				}
			}
			wp1 = iws_pclone(pBgn, pEnd);
		}

		if(bArgc)
		{
			argv[$ARGC] = wp1;
			args[$ARGC] = iws_replace(wp1, L"\"", L"", FALSE);
			++$ARGC;
		}
		else
		{
			$CMD = wp1;
			bArgc = TRUE;
		}
	}

	$ARGV = argv;
	$ARGS = args;
}
//-----------------------------------------
// 引数 [Key]+[Value] から [Value] を取得
//-----------------------------------------
/* (例)
	iCLI_getCommandLine(65001);
	// $ARGS[0] => "-w=size <= 1000"
	P2(W2U(iCLI_getOptValue(0, L"-w=", NULL))); //=> "size <= 1000"
*/
// v2022-08-24
WCS
*iCLI_getOptValue(
	UINT argc, // $ARGS[argc]
	WCS *opt1, // (例) "-w="
	WCS *opt2  // (例) "-where=", NULL
)
{
	if(argc >= $ARGC)
	{
		return 0;
	}

	if(! iwn_len(opt1))
	{
		return 0;
	}

	WCS *wp1 = $ARGS[argc];

	if(iwb_cmpf(wp1, opt1))
	{
		return (wp1 + iwn_len(opt1));
	}

	if(iwn_len(opt2) && iwb_cmpf(wp1, opt2))
	{
		return (wp1 + iwn_len(opt2));
	}

	return 0;
}
//--------------------------
// 引数 [Key] と一致するか
//--------------------------
/* (例)
	iCLI_getCommandLine(65001);
	// $ARGV[0] => "-repeat"
	P3(iCLI_getOptMatch(0, L"-repeat", NULL)); //=> TRUE
	// $ARGV[0] => "-w=size <= 1000"
	P3(iCLI_getOptMatch(0, L"-w=", NULL));     //=> FALSE
*/
// v2022-08-29
BOOL
iCLI_getOptMatch(
	UINT argc, // $ARGV[argc]
	WCS *opt1, // (例) "-r"
	WCS *opt2  // (例) "-repeat", NULL
)
{
	if(argc >= $ARGC)
	{
		return FALSE;
	}

	if(! iwn_len(opt1))
	{
		return FALSE;
	}
	else if(! iwn_len(opt2))
	{
		opt2 = opt1;
	}

	WCS *wp1 = $ARGV[argc];

	// 完全一致
	if(iwb_cmpp(wp1, opt1) || iwb_cmpp(wp1, opt2))
	{
		return TRUE;
	}

	return FALSE;
}
// v2022-08-29
VOID
iCLI_VarList()
{
	U8N *uCMD = icnv_W2U($CMD);

	P ("[$CMD]\n    %s\n", uCMD);
	P ("[$ARGC]\n    %d\n", $ARGC);
	P2("[$ARGV]");
	iwa_print($ARGV);
	P2("[$ARGS]");
	iwa_print($ARGS);
	NL();

	ifree(uCMD);
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	実行開始時間
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/* Win32 SDK Reference Help「win32.hlp(1996/11/26)」より
	経過時間はDWORD型で保存されています。
	システムを49.7日間連続して動作させると、
	経過時間は 0 に戻ります。
*/
/* (例)
	iExecSec_init(); //=> $ExecSecBgn
	Sleep(2000);
	P("-- %.6fsec\n\n", iExecSec_next());
	Sleep(1000);
	P("-- %.6fsec\n\n", iExecSec_next());
*/
// v2021-03-19
UINT
iExecSec(
	CONST UINT microSec // 0 のとき Init
)
{
	UINT microSec2 = GetTickCount();
	if(! microSec)
	{
		$ExecSecBgn = microSec2;
	}
	return (microSec2 < microSec ? 0 : (microSec2 - microSec)); // Err = 0
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	メモリ確保
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/*
	icalloc() 用に確保される配列
	IcallocDiv は必要に応じて変更
*/
typedef struct
{
	VOID *ptr; // ポインタ位置
	UINT ary;  // 配列個数（配列以外 = 0）
	UINT size; // アロケート長
	UINT id;   // 順番
}
$struct_icallocMap;

$struct_icallocMap *$icallocMap; // 可変長
UINT $icallocMapSize = 0;        // *$icallocMap のサイズ＋1
UINT $icallocMapEOD = 0;         // *$icallocMap の現在位置＋1
UINT $icallocMapFreeCnt = 0;     // *$icallocMap 中の空白領域
UINT $icallocMapId = 0;          // *$icallocMap の順番
CONST UINT $sizeof_icallocMap = sizeof($struct_icallocMap);
// *$icallocMap の基本区画サイズ(適宜変更 > 0)
#define  IcallocDiv          (1 << 5)
// 確保したメモリ後方に最低4byteの空白を確保
#define  ceilX(n)            ((((n - 5) >> 3) << 3) + (1 << 4))

//---------
// calloc
//---------
/* (例)
	WCS *wp1 = icalloc_WCS(100);
	INT *ai1 = icalloc_INT(100);
*/
// v2022-08-26
VOID
*icalloc(
	UINT n,    // 個数
	UINT size, // 型サイズ sizeof()
	BOOL aryOn // TRUE = 配列
)
{
	UINT size2 = sizeof($struct_icallocMap);

	// 初回 $icallocMap を更新
	if(! $icallocMapSize)
	{
		$icallocMapSize = IcallocDiv;
		$icallocMap = ($struct_icallocMap*)calloc($icallocMapSize, size2);
		icalloc_err($icallocMap);
		$icallocMapId = 0;
	}
	else if($icallocMapSize <= $icallocMapEOD)
	{
		$icallocMapSize += IcallocDiv;
		$icallocMap = ($struct_icallocMap*)realloc($icallocMap, ($icallocMapSize * size2));
		icalloc_err($icallocMap);
	}

	// 引数にポインタ割当
	UINT uSize = ceilX(n * size);
	VOID *rtn = malloc(uSize);
	icalloc_err(rtn);
	memset(rtn, 0, uSize);

	// ポインタ
	($icallocMap + $icallocMapEOD)->ptr = rtn;

	// 配列
	($icallocMap + $icallocMapEOD)->ary = (aryOn ? n : 0);

	// サイズ
	($icallocMap + $icallocMapEOD)->size = uSize;

	// 順番
	++$icallocMapId;
	($icallocMap + $icallocMapEOD)->id = $icallocMapId;
	++$icallocMapEOD;

	return rtn;
}
//----------
// realloc
//----------
// v2022-08-26
VOID
*irealloc(
	VOID *ptr, // icalloc()ポインタ
	UINT n,    // 個数
	UINT size  // 型サイズ sizeof()
)
{
	// 該当なしのときは ptr を返す
	VOID *rtn = ptr;
	UINT uSize = ceilX(n * size);
	// $icallocMap を更新
	UINT u1 = 0;
	while(u1 < $icallocMapEOD)
	{
		if(ptr == ($icallocMap + u1)->ptr && ($icallocMap + u1)->size < uSize)
		{
			// reallocの初期化版
			uSize *= 2; // 次回以降の処理を省略するため２倍確保
			rtn = (VOID*)malloc(uSize);
			icalloc_err(rtn);
			memset(rtn, 0, uSize);
			memcpy(rtn, ptr, ($icallocMap + u1)->size);
			memset(ptr, 0, ($icallocMap + u1)->size);
			free(ptr);
			ptr = 0;
			($icallocMap + u1)->ptr = rtn;
			($icallocMap + u1)->ary = (($icallocMap + u1)->ary ? n : 0);
			($icallocMap + u1)->size = uSize;
			break;
		}
		++u1;
	}
	return rtn;
}
//--------------------------------
// icalloc, ireallocのエラー処理
//--------------------------------
/* (例)
	// 通常
	MBS *mp1 = icalloc_MBS(1000);
	icalloc_err(mp1);
	// 強制的にエラーを発生させる
	icalloc_err(NULL);
*/
// v2016-08-30
VOID
icalloc_err(
	VOID *ptr // icalloc()ポインタ
)
{
	if(! ptr)
	{
		ierr_end("Can't allocate memories!");
	}
}
//----------------------------
// ($icallocMap + n) をfree
//----------------------------
// v2022-09-03
VOID
icalloc_free(
	VOID *ptr // icalloc()ポインタ
)
{
	$struct_icallocMap *map = 0;
	UINT u1 = 0, u2 = 0;
	while(u1 < $icallocMapEOD)
	{
		map = ($icallocMap + u1);
		if(ptr == (map->ptr))
		{
			// 配列から先に free
			if(map->ary)
			{
				// 1次元削除
				u2 = 0;
				while(u2 < (map->ary))
				{
					if(! (*((VOID**)(map->ptr) + u2)))
					{
						break;
					}
					icalloc_free(*((VOID**)(map->ptr) + u2));
					++u2;
				}
				++$icallocMapFreeCnt;

				// memset() + NULL代入 で free() の代替
				// 2次元削除
				// ポインタ配列を消去
				memset(map->ptr, 0, map->size);
				free(map->ptr);
				map->ptr = 0;
				memset(map, 0, $sizeof_icallocMap);
				return;
			}
			else
			{
				free(map->ptr);
				map->ptr = 0;
				memset(map, 0, $sizeof_icallocMap);
				++$icallocMapFreeCnt;
				return;
			}
		}
		++u1;
	}
}
//---------------------
// $icallocMapをfree
//---------------------
// v2016-01-10
VOID
icalloc_freeAll()
{
	// [0]はポインタなので残す
	// [1..]をfree
	while($icallocMapEOD)
	{
		icalloc_free($icallocMap->ptr);
		--$icallocMapEOD;
	}
	$icallocMap = ($struct_icallocMap*)realloc($icallocMap, 0); // free()不可
	$icallocMapSize = 0;
	$icallocMapFreeCnt = 0;
}
//---------------------
// $icallocMapを掃除
//---------------------
// v2022-09-03
VOID
icalloc_mapSweep()
{
	// 毎回呼び出しても影響ない
	UINT uSweep = 0;
	$struct_icallocMap *map1 = 0, *map2 = 0;
	UINT u1 = 0, u2 = 0;

	// 隙間を詰める
	while(u1 < $icallocMapEOD)
	{
		map1 = ($icallocMap + u1);
		if(! (VOID**)(map1->ptr))
		{
			++uSweep; // sweep数
			u2 = u1 + 1;
			while(u2 < $icallocMapEOD)
			{
				map2 = ($icallocMap + u2);
				if((VOID**)(map2->ptr))
				{
					*map1 = *map2; // 構造体コピー
					memset(map2, 0, $sizeof_icallocMap);
					--uSweep; // sweep数
					break;
				}
				++u2;
			}
		}
		++u1;
	}
	// 初期化
	$icallocMapFreeCnt -= uSweep;
	$icallocMapEOD -= uSweep;
}
//---------------------------
// $icallocMapをリスト出力
//---------------------------
// v2022-08-26
VOID
icalloc_mapPrint1()
{
	if(! $icallocMapSize)
	{
		return;
	}

	iConsole_EscOn();

	P0("\x1b[38;2;100;100;255m");
	P0("-1-------------8--------------16--------------24--------------32--------");
	P2("\x1b[38;2;50;255;50m");

	CONST UINT rowsCnt = 32;
	UINT uRowsCnt = rowsCnt;
	UINT u1 = 0, u2 = 0;
	while(u1 < $icallocMapSize)
	{
		while(u1 < uRowsCnt)
		{
			P0("\x1b[49m ");
			if(($icallocMap + u1)->ptr)
			{
				P0("\x1b[48;2;50;255;50m ");
				++u2;
			}
			else
			{
				P0("\x1b[48;2;50;75;50m ");
			}
			++u1;
		}
		P("\x1b[49m ");
		if(u1 > u2)
		{
			P("\x1b[38;2;50;255;50m%7u", u2);
		}
		else
		{
			P0("\x1b[38;2;100;100;255m      *");
		}
		P(" \x1b[38;2;100;100;255m%u\n", u1);
		uRowsCnt += rowsCnt;
	}
	P0("\x1b[0m");
}
// v2022-04-29
VOID
icalloc_mapPrint2()
{
	iConsole_EscOn();

	P0("\x1b[38;2;100;100;255m");
	P0("------- id ---- pointer ---------- array --- size ----------------------");
	P2("\x1b[38;2;255;255;255m");

	$struct_icallocMap *map = 0;
	UINT uUsedCnt = 0, uUsedSize = 0;
	UINT u1 = 0;
	while(u1 < $icallocMapEOD)
	{
		map = ($icallocMap + u1);
		if((map->ptr))
		{
			++uUsedCnt;
			uUsedSize += (map->size);

			if((map->ary))
			{
				// 背景色変更
				P0("\x1b[48;2;150;0;0m");
			}

			P(
				"%-7u %07u [%p] %4u %9u => '%s'",
				(u1 + 1),
				(map->id),
				(map->ptr),
				(map->ary),
				(map->size),
				(map->ptr)
			);

			// 背景色リセット
			P2("\x1b[49m");
		}
		++u1;
	}

	P0("\x1b[38;2;100;100;255m");
	P(
		"------- Usage %-9u ---------- %14u byte -----------------",
		uUsedCnt,
		uUsedSize
	);
	P2("\x1b[0m");
	NL();
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Print関係
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-----------
// printf()
//-----------
/* (例)
	P("abc");         //=> "abc"
	P("%s\n", "abc"); //=> "abc\n"

	// printf()系は遅い。可能であれば P0(), P2() を使用する。
	P0("abc");   //=> "abc"
	P2("abc");   //=> "abc\n"
	QP("abc\n"); //=> "abc\n"
*/
// v2015-01-24
VOID
P(
	MBS *format,
	...
)
{
	va_list va;
	va_start(va, format);
		vfprintf(stdout, format, va);
	va_end(va);
}
//--------------
// Quick Print
//--------------
/* (例)
	iConsole_EscOn();

	INT iMax = 100;
	MBS *rtn = icalloc_MBS(10 * iMax);
	MBS *pEnd = rtn;
	INT iCnt = 0;

	for(INT i1 = 1; i1 <= iMax; i1++)
	{
		pEnd += sprintf(pEnd, "%d\n", i1);

		++iCnt;
		if(iCnt == 30)
		{
			iCnt = 0;

			QP(rtn);
			pEnd = rtn;

			Sleep(2000);
		}
	}
	QP(rtn);
*/
// v2022-09-09
VOID
QP(
	MBS *str // 文字列
)
{
	fflush(stdout);
	WriteFile($StdoutHandle, str, strlen(str), NULL, NULL);
	FlushFileBuffers($StdoutHandle);
}
//-----------------------
// EscapeSequenceへ変換
//-----------------------
/* (例)
	WCS w*p1 = "ABCDE\\n12\\r345";
	// "\\n" を '\n' に変換
	PL2(W2U(iws_conv_escape(wp1))); //=> "ABCDE\n12\r345"
	// ソノママ
	PL2(W2U(wp1)); //=> "ABCDE\\n12\\r345"
*/
// v2022-08-26
WCS
*iws_conv_escape(
	WCS *str // 文字列
)
{
	if(! str)
	{
		return 0;
	}
	WCS *rtn = iws_clone(str);
	INT i1 = 0;
	while(*str)
	{
		if(*str == '\\')
		{
			++str;
			switch(*str)
			{
				case('a'):
					rtn[i1] = '\a';
					break;

				case('b'):
					rtn[i1] = '\b';
					break;

				case('t'):
					rtn[i1] = '\t';
					break;

				case('n'):
					rtn[i1] = '\n';
					break;

				case('v'):
					rtn[i1] = '\v';
					break;

				case('f'):
					rtn[i1] = '\f';
					break;

				case('r'):
					rtn[i1] = '\r';
					break;

				default:
					rtn[i1] = '\\';
					++i1;
					rtn[i1] = *str;
					break;
			}
		}
		else
		{
			rtn[i1] = *str;
		}
		++str;
		++i1;
	}
	rtn[i1] = 0;
	return rtn;
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	WCS／MBS／U8N変換
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
// v2022-08-24
WCS
*icnv_M2W(
	MBS *str
)
{
	if(! str)
	{
		return 0;
	}
	UINT uW = MultiByteToWideChar(CP_OEMCP, 0, str, -1, NULL, 0);
	WCS *wp1 = icalloc_WCS(uW);
	MultiByteToWideChar(CP_OEMCP, 0, str, -1, wp1, uW);
	return wp1;
}
// v2022-08-24
U8N
*icnv_W2U(
	WCS *str
)
{
	if(! str)
	{
		return 0;
	}
	UINT uU = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	U8N *up1 = icalloc_MBS(uU);
	WideCharToMultiByte(CP_UTF8, 0, str, -1, up1, uU, NULL, NULL);
	return up1;
}
// v2022-08-24
WCS
*icnv_U2W(
	U8N *str
)
{
	if(! str)
	{
		return 0;
	}
	UINT uW = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	WCS *wp1 = icalloc_WCS(uW);
	MultiByteToWideChar(CP_UTF8, 0, str, -1, wp1, uW);
	return wp1;
}
// v2022-08-24
MBS
*icnv_W2M(
	WCS *str
)
{
	if(! str)
	{
		return 0;
	}
	UINT uM = WideCharToMultiByte(CP_OEMCP, 0, str, -1, NULL, 0, NULL, NULL);
	MBS *mp1 = icalloc_MBS(uM);
	WideCharToMultiByte(CP_OEMCP, 0, str, -1, mp1, uM, NULL, NULL);
	return mp1;
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	文字列処理
		p : return Pointer
		s : return String
		1byte     MBS : imp_xxx(), ims_xxx()
		UTF-8     U8N : iup_xxx(), ius_xxx()
		UTF-16    WCS : iwp_xxx(), iws_xxx()
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/* (例)
	MBS *mp1 = "あいうえおaiueo";
	PL3(imn_len(mp1)); //=> 15
	WCS *wp1 = M2W(mp1);
	PL3(iwn_len(wp1)); //=> 10
	U8N *up1 = W2U(wp1);
	PL3(iun_len(up1)); //=> 20
*/
// v2022-04-02
UINT
imn_len(
	MBS *str
)
{
	if(! str)
	{
		return 0;
	}
	return strlen(str);
}
// v2022-04-02
UINT
iun_len(
	U8N *str
)
{
	if(! str)
	{
		return 0;
	}
	UINT rtn = 0;
	// BOMを読み飛ばす(UTF-8N は該当しない)
	if(*str == (CHAR)0xEF && str[1] == (CHAR)0xBB && str[2] == (CHAR)0xBF)
	{
		str += 3;
	}
	INT c = 0;
	while(*str)
	{
		if(*str & 0x80)
		{
			// 多バイト文字
			// [0]
			c = (*str & 0xfc);
			c <<= 1;
			++str;
			// [1]
			if(c & 0x80)
			{
				c <<= 1;
				++str;
				// [2]
				if(c & 0x80)
				{
					c <<= 1;
					++str;
					// [3]
					if(c & 0x80)
					{
						++str;
					}
				}
			}
			/* 以下より10%前後速い
				c = (*str & 0xfc);
				while(c & 0x80)
				{
					++str;
					c <<= 1;
				}
			*/
		}
		else
		{
			// 1バイト文字
			++str;
		}
		++rtn;
	}
	return rtn;
}
// v2022-04-02
UINT
iwn_len(
	WCS *str
)
{
	if(! str)
	{
		return 0;
	}
	return wcslen(str);
}
//-------------------------
// コピーした文字長を返す
//-------------------------
/* (例)
	WCS *to = icalloc_WCS(100);
	PL3(iwn_cpy(to, L"abcde")); //=> 5
	ifree(to);
*/
// v2022-09-05
UINT
imn_cpy(
	MBS *to,
	MBS *from
)
{
	if(! from)
	{
		return 0;
	}
	strcpy(to, from);
	return strlen(from);
	/* ↑以下のコードよりやや速い
		MBS *pEnd = from;
		while((*to++ = *pEnd++));
		*to = 0;
		return (pEnd - from - 1);
	*/
}
// v2022-09-05
UINT
iwn_cpy(
	WCS *to,
	WCS *from
)
{
	if(! from)
	{
		return 0;
	}
	wcscpy(to, from);
	return wcslen(from);
	/* ↑以下のコードよりやや速い
		WCS *pEnd = from;
		while((*to++ = *pEnd++));
		*to = 0;
		return (pEnd - from - 1);
	*/
}
/* (例)
	WCS *to = icalloc_WCS(100);
	WCS *wp1 = L"abcde12345";
	PL3(iwn_pcpy(to, wp1 + 3, wp1 + 8)); //=> 5
	ifree(to);
*/
// v2022-08-19
UINT
iwn_pcpy(
	WCS *to,
	WCS *from1,
	WCS *from2
)
{
	if(! from1 || ! from2)
	{
		return 0;
	}
	WCS *pEnd = from1;
	while((pEnd < from2) && (*to++ = *pEnd++));
	*to = 0;
	return (pEnd - from1);
}
//-----------------------
// 新規部分文字列を生成
//-----------------------
/* (例)
	PL2(W2U(iws_clone(L"abcde"))); //=> "abcde"
*/
// v2022-04-01
WCS
*iws_clone(
	WCS *from
)
{
	WCS *to = icalloc_WCS(iwn_len(from));
	WCS *pEnd = to;
	while((*pEnd++ = *from++));
	*pEnd = 0;
	return to;
}
/* (例)
	WCS *from = L"abcde";
	WCS *wp1 = iws_pclone(from, from + 3);
		PL2(W2U(wp1)); //=> "abc"
	ifree(wp1);
*/
// v2022-08-19
WCS
*iws_pclone(
	WCS *from1,
	WCS *from2
)
{
	WCS *to = icalloc_WCS(from2 - from1);
	WCS *pEnd = to;
	while((from1 < from2) && (*pEnd++ = *from1++));
	*pEnd = 0;
	return to;
}
/* (例)
	// 要素を呼び出す度 irealloc する方がスマートだが速度に不安があるので icalloc １回で済ませる。
	MBS *mp1 = ims_cats(3, "123", "abcde", "あいうえお");
		PL2(mp1); //=> "123abcdeあいうえお"
	ifree(mp1);
*/
// v2022-04-02
MBS
*ims_cats(
	UINT size, // 要素数(n+1)
	...        // ary[0..n]
)
{
	UINT u1 = 0, u2 = 0;

	va_list va;
	va_start(va, size);
		u1 = size;
		while(u1--)
		{
			u2 += imn_len(va_arg(va, MBS*));
		}
	va_end(va);

	MBS *rtn = icalloc_MBS(u2);
	MBS *pEnd = rtn;

	va_start(va, size);
		u1 = size;
		while(u1--)
		{
			pEnd += imn_cpy(pEnd, va_arg(va, MBS*));
		}
	va_end(va);

	return rtn;
}
// v2022-08-20
WCS
*iws_cats(
	UINT size, // 要素数(n+1)
	...        // ary[0..n]
)
{
	UINT u1 = 0, u2 = 0;

	va_list va;
	va_start(va, size);
		u1 = size;
		while(u1--)
		{
			u2 += iwn_len(va_arg(va, WCS*));
		}
	va_end(va);

	WCS *rtn = icalloc_WCS(u2);
	WCS *pEnd = rtn;

	va_start(va, size);
		u1 = size;
		while(u1--)
		{
			pEnd += iwn_cpy(pEnd, va_arg(va, WCS*));
		}
	va_end(va);

	return rtn;
}
//--------------------
// sprintf()の拡張版
//--------------------
/* (例)
	MBS *mp1 = ims_sprintf("%s-%s%05d", "ABC", "123", 456);
		PL2(mp1); //=> "ABC-12300456"
	ifree(mp1);
*/
// v2021-11-14
MBS
*ims_sprintf(
	MBS *format,
	...
)
{
	FILE *oFp = fopen("NUL", "wb");
		va_list va;
		va_start(va, format);
			MBS *rtn = icalloc_MBS(vfprintf(oFp, format, va));
			vsprintf(rtn, format, va);
		va_end(va);
	fclose(oFp);
	return rtn;
}
/* (例)
	WCS *wp1 = M2W("あいうえお");
	WCS *wp2 = M2W("ワイド文字：%S\n");
	WCS *wp3 = iws_sprintf(wp2, wp1);
	U8N *up1 = W2U(wp3);
		P2(up1);
	ifree(up1);
	ifree(wp3);
	ifree(wp2);
	ifree(wp1);
*/
// v2022-08-21
WCS
*iws_sprintf(
	WCS *format,
	...
)
{
	FILE *oFp = fopen("NUL", "wb");
		va_list va;
		va_start(va, format);
			INT iSize = vfwprintf(oFp, format, va);
			WCS *rtn = icalloc_WCS(iSize);
			vswprintf(rtn, (iSize + 1), format, va);
		va_end(va);
	fclose(oFp);
	return rtn;
}
//--------------------------------
// lstrcmp()／lstrcmpi()より安全
//--------------------------------
/* (例)
	PL3(iwb_cmp(L"", L"abc", FALSE, FALSE));   //=> FALSE
	PL3(iwb_cmp(L"abc", L"", FALSE, FALSE));   //=> TRUE
	PL3(iwb_cmp(L"", L"", FALSE, FALSE));      //=> TRUE
	PL3(iwb_cmp(NULL, L"abc", FALSE, FALSE));  //=> FALSE
	PL3(iwb_cmp(L"abc", NULL, FALSE, FALSE));  //=> FALSE
	PL3(iwb_cmp(NULL, NULL, FALSE, FALSE));    //=> FALSE
	PL3(iwb_cmp(NULL, L"", FALSE, FALSE));     //=> FALSE
	PL3(iwb_cmp(L"", NULL, FALSE, FALSE));     //=> FALSE
	NL();

	// iwb_cmpf(str, search)
	PL3(iwb_cmp(L"abc", L"AB", FALSE, FALSE)); //=> FALSE
	// iwb_cmpfi(str, search)
	PL3(iwb_cmp(L"abc", L"AB", FALSE, TRUE));  //=> TRUE
	// iwb_cmpp(str, search)
	PL3(iwb_cmp(L"abc", L"AB", TRUE, FALSE));  //=> FALSE
	// iwb_cmppi(str, search)
	PL3(iwb_cmp(L"abc", L"AB", TRUE, TRUE));   //=> FALSE
	NL();

	// searchに１文字でも合致すればTRUEを返す
	// iwb_cmp_leqfi(str,search)
	PL3(iwb_cmp_leqfi(L""   , L".."));         //=> TRUE
	PL3(iwb_cmp_leqfi(L"."  , L".."));         //=> TRUE
	PL3(iwb_cmp_leqfi(L".." , L".."));         //=> TRUE
	PL3(iwb_cmp_leqfi(L"...", L".."));         //=> FALSE
	PL3(iwb_cmp_leqfi(L"...", L""  ));         //=> FALSE
	NL();
*/
// v2022-09-03
BOOL
iwb_cmp(
	WCS *str,     // 検索対象
	WCS *search,  // 検索文字列
	BOOL perfect, // TRUE=長さ一致／FALSE=前方一致
	BOOL icase    // TRUE=大小文字区別しない
)
{
	// NULL は存在しないので FALSE
	if(! str || ! search)
	{
		return FALSE;
	}
	// 例外
	if(! *str && ! *search)
	{
		return TRUE;
	}
	// 検索対象 == "" のときは FALSE
	if(! *str)
	{
		return FALSE;
	}
	// 長さ一致
	if(perfect)
	{
		if(iwn_len(str) == iwn_len(search))
		{
			if(icase)
			{
			}
			else
			{
				return (wcscmp(str, search) ? FALSE : TRUE);
			}
		}
		else
		{
			return FALSE;
		}
	}
	// 大文字小文字区別しない
	if(icase)
	{
		return (wcsnicmp(str, search, iwn_len(search)) ? FALSE : TRUE);
	}
	else
	{
		return (wcsncmp(str, search, iwn_len(search)) ? FALSE : TRUE);
	}
}
// v2022-04-02
UINT
iwn_searchCntW(
	WCS *str,    // 文字列
	WCS *search, // 検索文字列
	BOOL icase   // TRUE=大文字小文字区別しない
)
{
	if(! str || ! search)
	{
		return 0;
	}
	UINT rtn = 0;
	CONST UINT searchLen = iwn_len(search);
	while(*str)
	{
		if(iwb_cmp(str, search, FALSE, icase))
		{
			str += searchLen;
			++rtn;
		}
		else
		{
			++str;
		}
	}
	return rtn;
}
//---------------------------------
// 一致した文字列のポインタを返す
//---------------------------------
/*
	      "\0あいうえお\0"
	         <= TRUE =>
	R_FALSE↑          ↑L_FALSE
*/
/* (例)
	PL2(W2U(iwp_searchLM(L"ABCABCDEFABC", L"ABC", FALSE))); //=> "ABCABCDEFABC"
*/
// v2022-08-20
WCS
*iwp_searchLM(
	WCS *str,    // 文字列
	WCS *search, // 検索文字列
	BOOL icase   // TRUE=大文字小文字区別しない
)
{
	if(! str)
	{
		return str;
	}
	while(*str)
	{
		if(iwb_cmp(str, search, FALSE, icase))
		{
			break;
		}
		++str;
	}
	return str;
}
//-------------------------
// 比較指示子を数字に変換
//-------------------------
/*
	[-2] "<"  | "!>="
	[-1] "<=" | "!>"
	[ 0] "="  | "!<>" | "!><"
	[ 1] ">=" | "!<"
	[ 2] ">"  | "!<="
	[ 3] "!=" | "<>"  | "><"
*/
// v2021-11-17
INT
icmpOperator_extractHead(
	MBS *str
)
{
	INT rtn = INT_MAX; // Errのときは MAX を返す
	if(! str || ! *str || ! (*str == ' ' || *str == '<' || *str == '=' || *str == '>' || *str == '!'))
	{
		return rtn;
	}

	// 先頭の空白のみ特例
	while(*str == ' ')
	{
		++str;
	}
	BOOL bNot = FALSE;
	if(*str == '!')
	{
		++str;
		bNot = TRUE;
	}
	switch(*str)
	{
		// [2]">" | [1]">=" | [3]"><"
		case('>'):
			if(str[1] == '<')
			{
				rtn = 3;
			}
			else
			{
				rtn = (str[1] == '=' ? 1 : 2);
			}
			break;

		// [0]"="
		case('='):
			rtn = 0;
			break;

		// [-2]"<" | [-1]"<=" | [3]"<>"
		case('<'):
			if(str[1] == '>')
			{
				rtn = 3;
			}
			else
			{
				rtn = (str[1] == '=' ? -1 : -2);
			}
			break;
	}
	if(bNot)
	{
		rtn += (rtn > 0 ? -3 : 3);
	}
	return rtn;
}
//-------------------------------------------------------
// icmpOperator_extractHead()で取得した比較指示子で比較
//-------------------------------------------------------
// v2015-12-31
BOOL
icmpOperator_chk_INT64(
	INT64 i1,
	INT64 i2,
	INT operator // [-2..3]
)
{
	if(operator == -2 && i1 < i2)
	{
		return TRUE;
	}
	if(operator == -1 && i1 <= i2)
	{
		return TRUE;
	}
	if(operator == 0 && i1 == i2)
	{
		return TRUE;
	}
	if(operator == 1 && i1 >= i2)
	{
		return TRUE;
	}
	if(operator == 2 && i1 > i2)
	{
		return TRUE;
	}
	if(operator == 3 && i1 != i2)
	{
		return TRUE;
	}
	return FALSE;
}
// v2015-12-26
BOOL
icmpOperator_chkDBL(
	DOUBLE d1,   //
	DOUBLE d2,   //
	INT operator // [-2..3]
)
{
	if(operator == -2 && d1 < d2)
	{
		return TRUE;
	}
	if(operator == -1 && d1 <= d2)
	{
		return TRUE;
	}
	if(operator == 0 && d1 == d2)
	{
		return TRUE;
	}
	if(operator == 1 && d1 >= d2)
	{
		return TRUE;
	}
	if(operator == 2 && d1 > d2)
	{
		return TRUE;
	}
	if(operator == 3 && d1 != d2)
	{
		return TRUE;
	}
	return FALSE;
}
//---------------------------
// 文字列を分割し配列を作成
//---------------------------
/* (例)
	WCS *str = L"-4712/ 1/ 1  00:00:00:::";
	WCS *tokens = L"-/: ";
	WCS **wa1;
	wa1 = iwa_split(str, tokens, TRUE);
		iwa_printU(wa1); //=> {"4712", "1", "1", "00", "00", "00"}
	ifree(wa1);
	wa1 = iwa_split(str, tokens, FALSE);
		iwa_printU(wa1); //=> {"", "4712", "", "1", "", "1", "00", "00", "00", "", ""}
	ifree(wa1);
*/
// v2022-08-25
WCS
**iwa_split(
	WCS *str,     // 文字列
	WCS *tokens,  // 区切り文字群 (例)"-/: " => "-", "/", ":", " "
	BOOL bRmEmpty // TRUE=配列から空白を除く
)
{
	WCS **rtn = {0};

	if(! str || ! *str)
	{
		rtn = icalloc_WCS_ary(1);
		rtn[0] = iws_clone(str);
		return rtn;
	}

	if(! tokens || ! *tokens)
	{
		UINT u1 = 0;
		UINT u2 = iwn_len(str);
		rtn = icalloc_WCS_ary(u2);
		while(u1 < u2)
		{
			rtn[u1] = iws_pclone((str + u1), (str + u1 + 1));
			++u1;
		}
		return rtn;
	}

	UINT uAry = 1;
	rtn = icalloc_WCS_ary(uAry);
	CONST WCS *pEOD = str + iwn_len(str);
	WCS *pBgn = str;
	WCS *pEnd = pBgn;
	while(pEnd < pEOD)
	{
		pEnd += wcscspn(pBgn, tokens);
		if(! bRmEmpty || (bRmEmpty && pBgn < pEnd))
		{
			rtn = irealloc_WCS_ary(rtn, uAry);
			rtn[uAry - 1] = iws_pclone(pBgn, pEnd);
			++uAry;
		}
		++pEnd;
		pBgn = pEnd;
	}
	return rtn;
}
// v2022-08-20
WCS
*iws_addTokenNStr(
	WCS *str
)
{
	if(! str || ! *str)
	{
		return str;
	}

	UINT u1 = iwn_len(str);
	UINT u2 = 0;
	WCS *rtn = icalloc_WCS(u1 * 2);
	WCS *pRtnE = rtn;
	WCS *wp1 = 0;

	// "-000123456.7890" のとき
	// (1) 先頭の [\S*] を探す
	WCS *pBgn = str;
	WCS *pEnd = str;
	while(*pEnd)
	{
		if((*pEnd >= '0' && *pEnd <= '9') || *pEnd == '.')
		{
			break;
		}
		++pEnd;
	}
	pRtnE += iwn_pcpy(pRtnE, pBgn, pEnd);

	// (2) [0-9] 間を探す => "000123456"
	pBgn = pEnd;
	while(*pEnd)
	{
		if(*pEnd < '0' || *pEnd > '9')
		{
			break;
		}
		++pEnd;
	}

	// (2-11) 先頭は [.] か？
	if(*pBgn == '.')
	{
		*pRtnE = '0';
		++pRtnE;
		iwn_cpy(pRtnE, pBgn);
	}

	// (2-21) 連続する 先頭の[0] を調整 => "123456"
	else
	{
		while(*pBgn)
		{
			if(*pBgn != '0')
			{
				break;
			}
			++pBgn;
		}
		if(*(pBgn - 1) == '0' && (*pBgn < '0' || *pBgn > '9'))
		{
			--pBgn;
		}

		// (2-22) ", " 付与 => "123,456"
		wp1 = iws_pclone(pBgn, pEnd);
			u1 = pEnd - pBgn;
			if(u1 > 3)
			{
				u2 = u1 % 3;
				if(u2)
				{
					pRtnE += iwn_pcpy(pRtnE, wp1, wp1 + u2);
				}
				while(u2 < u1)
				{
					if(u2 > 0 && u2 < u1)
					{
						*pRtnE = ',';
						++pRtnE;
					}
					pRtnE += iwn_pcpy(pRtnE, wp1 + u2, wp1 + u2 + 3);
					u2 += 3;
				}
			}
			else
			{
				pRtnE += iwn_cpy(pRtnE, wp1);
			}
		ifree(wp1);

		// (2-23) 残り => ".7890"
		iwn_cpy(pRtnE, pEnd);
	}
	return rtn;
}
//---------------------------
// Dir末尾の "\" を消去する
//---------------------------
// v2022-08-30
WCS
*iws_cutYenR(
	WCS *path
)
{
	if(! path || ! *path)
	{
		return path;
	}
	WCS *rtn = iws_clone(path);
	WCS *pEnd = rtn + iwn_len(rtn) - 1;
	while(*pEnd == L'\\')
	{
		*pEnd = L'\0';
		--pEnd;
	}
	return rtn;
}
//-------------
// 文字列置換
//-------------
/* (例)
	PL2(W2U(iws_replace(L"100YEN yen", L"YEN", L"$", TRUE)));  //=> "100$ $"
	PL2(W2U(iws_replace(L"100YEN yen", L"YEN", L"$", FALSE))); //=> "100$ yen"
*/
// v2022-04-02
WCS
*iws_replace(
	WCS *from,   // 文字列
	WCS *before, // 変換前の文字列
	WCS *after,  // 変換後の文字列
	BOOL icase   // TRUE=大文字小文字区別しない
)
{
	if(! from || ! *from || ! before || ! *before)
	{
		return iws_clone(from);
	}

	WCS *fW = 0;
	WCS *bW = 0;

	if(icase)
	{
		fW = iws_clone(from);
		CharLowerW(fW);
		bW = iws_clone(before);
		CharLowerW(bW);
	}
	else
	{
		fW = from;
		bW = before;
	}

	UINT fromLen = iwn_len(from);
	UINT beforeLen = iwn_len(before);
	UINT afterLen = iwn_len(after);

	// ゼロ長対策
	WCS *rtn = icalloc_WCS(fromLen * (1 + (afterLen / beforeLen)));

	WCS *fWB = fW;
	WCS *rtnE = rtn;

	while(*fWB)
	{
		if(! wcsncmp(fWB, bW, beforeLen))
		{
			rtnE += iwn_cpy(rtnE, after);
			fWB += beforeLen;
		}
		else
		{
			*rtnE++ = *fWB++;
		}
	}

	if(icase)
	{
		ifree(bW);
		ifree(fW);
	}

	return rtn;
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	数字関係
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------
// 文字を無視した位置で数値に変換する
//-------------------------------------
/* (例)
	PL3(inum_wtoi(L"-0123.45")); //=> -123
*/
// v2022-08-20
INT64
inum_wtoi(
	WCS *str // 文字列
)
{
	if(! str || ! *str)
	{
		return 0;
	}
	while(*str)
	{
		if(inum_chkW(str))
		{
			break;
		}
		++str;
	}
	return _wtoi64(str);
}
/* (例)
	PL4(inum_wtof(L"-0123.45")); //=> -123.45000000
*/
// v2022-08-20
DOUBLE
inum_wtof(
	WCS *str // 文字列
)
{
	if(! str || ! *str)
	{
		return 0;
	}
	while(*str)
	{
		if(inum_chkW(str))
		{
			break;
		}
		++str;
	}
	return _wtof(str);
}
//-----------------------------------------------------------------------------------------
// Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura, All rights reserved.
//  A C-program for MT19937, with initialization improved 2002/1/26.
//  Coded by Takuji Nishimura and Makoto Matsumoto.
//-----------------------------------------------------------------------------------------
/*
	http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
	上記コードを元に以下の関数についてカスタマイズを行った。
	MT関連の最新情報（派生版のSFMT、TinyMTなど）については下記を参照のこと
		http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt.html
*/
/* (例)
INT
main()
{
	CONST INT Output = 10; // 出力数
	CONST INT Min    = -5; // 最小値(>=INT_MIN)
	CONST INT Max    =  5; // 最大値(<=INT_MAX)

	MT_init(TRUE); // 初期化

	for(INT i1 = 0; i1 < Output; i1++)
	{
		P4(MT_irand_DBL(Min, Max, 10));
	}

	MT_free(); // 解放

	return 0;
}
*/
/*
	Period parameters
*/
#define  MT_N 624
#define  MT_M 397
#define  MT_MATRIX_A         0x9908b0dfUL // constant vector a
#define  MT_UPPER_MASK       0x80000000UL // most significant w-r bits
#define  MT_LOWER_MASK       0x7fffffffUL // least significant r bits
static UINT MT_u1 = (MT_N + 1);           // MT_u1 == MT_N + 1 means ai1[MT_N] is not initialized
static UINT *MT_au1 = 0;                  // the array forthe state vector
// v2021-11-15
VOID
MT_init(
	BOOL fixOn
)
{
	// 二重Alloc回避
	MT_free();
	MT_au1 = icalloc(MT_N, sizeof(UINT), FALSE);
	// Seed設定
	UINT init_key[4];
	INT *ai1 = idate_now_to_iAryYmdhns_localtime();
		init_key[0] = ai1[3] * ai1[5];
		init_key[1] = ai1[3] * ai1[6];
		init_key[2] = ai1[4] * ai1[5];
		init_key[3] = ai1[4] * ai1[6];
	ifree(ai1);
	// fixOn == FALSE のとき時間でシャッフル
	if(! fixOn)
	{
		init_key[3] &= (INT)GetTickCount();
	}
	INT i = 1, j = 0, k = 4;
	while(k)
	{
		MT_au1[i] = (MT_au1[i] ^ ((MT_au1[i - 1] ^ (MT_au1[i - 1] >> 30)) * 1664525UL)) + init_key[j] + j; // non linear
		MT_au1[i] &= 0xffffffffUL; // for WORDSIZE>32 machines
		++i, ++j;
		if(i >= MT_N)
		{
			MT_au1[0] = MT_au1[MT_N - 1];
			i = 1;
		}
		if(j >= MT_N)
		{
			j = 0;
		}
		--k;
	}
	k = MT_N - 1;
	while(k)
	{
		MT_au1[i] = (MT_au1[i] ^ ((MT_au1[i - 1] ^ (MT_au1[i - 1] >> 30)) * 1566083941UL)) - i; // non linear
		MT_au1[i] &= 0xffffffffUL; // for WORDSIZE>32 machines
		++i;
		if(i >= MT_N)
		{
			MT_au1[0] = MT_au1[MT_N - 1];
			i = 1;
		}
		--k;
	}
	MT_au1[0] = 0x80000000UL; // MSB is 1;assuring non-zero initial array
}
/*
	generates a random number on [0, 0xffffffff]-interval
	generates a random number on [0, 0xffffffff]-interval
*/
// v2022-04-01
UINT
MT_genrand_UINT()
{
	UINT y = 0;
	static UINT mag01[2] = {0x0UL, MT_MATRIX_A};
	if(MT_u1 >= MT_N)
	{
		// generate N words at one time
		INT kk = 0;
		while(kk < MT_N - MT_M)
		{
			y = (MT_au1[kk] & MT_UPPER_MASK) | (MT_au1[kk + 1] & MT_LOWER_MASK);
			MT_au1[kk] = MT_au1[kk + MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
			++kk;
		}
		while(kk < MT_N - 1)
		{
			y = (MT_au1[kk] & MT_UPPER_MASK) | (MT_au1[kk + 1] & MT_LOWER_MASK);
			MT_au1[kk] = MT_au1[kk + (MT_M - MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
			++kk;
		}
		y = (MT_au1[MT_N - 1] & MT_UPPER_MASK) | (MT_au1[0] & MT_LOWER_MASK);
		MT_au1[MT_N - 1] = MT_au1[MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
		MT_u1 = 0;
	}
	y = MT_au1[++MT_u1];
	// Tempering
	y ^= (y >> 11);
	y ^= (y <<  7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}
// v2015-11-15
VOID
MT_free()
{
	ifree(MT_au1);
}
//----------------
// INT乱数を発生
//----------------
/* (例)
	MT_init(TRUE);                        // 初期化
	P("%lld\n", MT_irand_INT64(0, 1000)); // [0..100]
	MT_free();                            // 解放
*/
// v2022-04-01
INT64
MT_irand_INT64(
	INT posMin,
	INT posMax
)
{
	if(posMin > posMax)
	{
		return 0;
	}
	return (MT_genrand_UINT() % (posMax - posMin + 1)) + posMin;
}
//-------------------
// DOUBLE乱数を発生
//-------------------
/* (例)
	MT_init(TRUE);                        // 初期化
	P("%0.5f\n", MT_irand_DBL(0, 10, 5)); // [0.00000..10.00000]
	MT_free();                            // 解放
*/
// v2022-04-01
DOUBLE
MT_irand_DBL(
	INT posMin,
	INT posMax,
	INT decRound // [0..10]／[0]"1", [1]"0.1", .., [10]"0.0000000001"
)
{
	if(posMin > posMax)
	{
		return 0.0;
	}
	if(decRound > 10)
	{
		decRound = 0;
	}
	INT i1 = 1;
	while(decRound > 0)
	{
		i1 *= 10;
		--decRound;
	}
	return (DOUBLE)MT_irand_INT64(posMin, (posMax - 1)) + (DOUBLE)MT_irand_INT64(0, i1) / i1;
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Array
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------
// 配列サイズを取得
//-------------------
/* (例)
	iCLI_getCommandLine(65001);
	PL3(iwa_size($ARGV));
*/
// v2022-08-19
UINT
iwa_size(
	WCS **ary // 引数列
)
{
	UINT rtn = 0;
	while(*ary++)
	{
		++rtn;
	}
	return rtn;
}
//---------------------
// 配列の合計長を取得
//---------------------
/* (例)
	iCLI_getCommandLine(65001);
	PL3(iwa_len($ARGV));
*/
// v2022-08-20
UINT
iwa_len(
	WCS **ary
)
{
	UINT rtn = 0;
	while(*ary)
	{
		rtn += iwn_len(*ary++);
	}
	return rtn;
}
//--------------
// 配列をqsort
//--------------
/* (例)
	// 順ソート／大小文字区別する
	qsort($ARGV, $ARGC, sizeof(WCS*), iwa_qsort_Asc);
	iwa_print($ARGV);
*/
// v2022-09-02
INT
iwa_qsort_Asc(
	CONST VOID *arg1,
	CONST VOID *arg2
)
{
	return wcscmp(*(WCS**)arg1, *(WCS**)arg2);
}
/* (例)
	// 順ソート／大小文字区別しない
	qsort($ARGV, $ARGC, sizeof(WCS*), iwa_qsort_iAsc);
	iwa_print($ARGV);
*/
// v2022-09-02
INT
iwa_qsort_iAsc(
	CONST VOID *arg1,
	CONST VOID *arg2
)
{
	return wcsicmp(*(WCS**)arg1, *(WCS**)arg2);
}
/* (例)
	// 逆順ソート／大小文字区別する
	qsort($ARGV, $ARGC, sizeof(WCS*), iwa_qsort_Desc);
	iwa_print($ARGV);
*/
// v2022-09-02
INT
iwa_qsort_Desc(
	CONST VOID *arg1,
	CONST VOID *arg2
)
{
	return -(wcscmp(*(WCS**)arg1, *(WCS**)arg2));
}
/* (例)
	// 逆順ソート／大小文字区別しない
	qsort($ARGV, $ARGC, sizeof(WCS*), iwa_qsort_iDesc);
	iwa_print($ARGV);
*/
// v2022-09-02
INT
iwa_qsort_iDesc(
	CONST VOID *arg1,
	CONST VOID *arg2
)
{
	return -(wcsicmp(*(WCS**)arg1, *(WCS**)arg2));
}
//---------------------
// 配列を文字列に変換
//---------------------
// v2022-08-20
WCS
*iwa_njoin(
	WCS **ary,  // 配列
	WCS *token, // 区切文字
	UINT start, // 取得位置
	UINT count  // 個数
)
{
	UINT arySize = iwa_size(ary);
	UINT tokenSize = iwn_len(token);
	WCS *rtn = icalloc_WCS(iwa_len(ary) + (arySize * tokenSize));
	WCS *pEnd = rtn;
	UINT u1 = 0;
	while(u1 < arySize && count > 0)
	{
		if(u1 >= start)
		{
			--count;
			pEnd += iwn_cpy(pEnd, ary[u1]);
			pEnd += iwn_cpy(pEnd, token);
		}
		++u1;
	}
	*(pEnd - tokenSize) = 0;
	return rtn;
}
//---------------------------
// 配列から空白／重複を除く
//---------------------------
/* (例)
	WCS *args[] = {L"aaa", L"AAA", L"BBB", L"", L"bbb", NULL};
	WCS **wa1 = { 0 };
	U8N *up1 = 0;
	INT i1 = 0;
	//
	// TRUE = 大小文字区別しない
	//
	wa1 = iwa_simplify(args, TRUE);
	i1 = 0;
	while((wa1[i1]))
	{
		up1 = W2U(wa1[i1]);
			PL2(up1); //=> "aaa", "BBB"
		ifree(up1);
		++i1;
	}
	ifree(wa1);
	//
	// FALSE = 大小文字区別する
	//
	wa1 = iwa_simplify(args, FALSE);
	i1 = 0;
	while((wa1[i1]))
	{
		up1 = W2U(wa1[i1]);
			PL2(up1); //=> "aaa", "AAA", "BBB", "bbb"
		ifree(up1);
		++i1;
	}
	ifree(wa1);
*/
// v2022-08-20
WCS
**iwa_simplify(
	WCS **ary,
	BOOL icase // TRUE=大文字小文字区別しない
)
{
	CONST UINT uArySize = iwa_size(ary);
	UINT u1 = 0, u2 = 0;
	// iAryFlg 生成
	INT *iAryFlg = icalloc_INT(uArySize); // 初期値 = 0
	// 上位へ集約
	u1 = 0;
	while(u1 < uArySize)
	{
		if(*ary[u1] && iAryFlg[u1] > -1)
		{
			iAryFlg[u1] = 1; // 仮
			u2 = u1 + 1;
			while(u2 < uArySize)
			{
				if(icase)
				{
					if(iwb_cmppi(ary[u1], ary[u2]))
					{
						iAryFlg[u2] = -1; // ×
					}
				}
				else
				{
					if(iwb_cmpp(ary[u1], ary[u2]))
					{
						iAryFlg[u2] = -1; // ×
					}
				}
				++u2;
			}
		}
		++u1;
	}
	// rtn作成
	UINT uAryUsed = 0;
	u1 = 0;
	while(u1 < uArySize)
	{
		if(iAryFlg[u1] == 1)
		{
			++uAryUsed;
		}
		++u1;
	}
	WCS **rtn = icalloc_WCS_ary(uAryUsed);
	u1 = u2 = 0;
	while(u1 < uArySize)
	{
		if(iAryFlg[u1] == 1)
		{
			rtn[u2] = iws_clone(ary[u1]);
			++u2;
		}
		++u1;
	}
	ifree(iAryFlg);
	return rtn;
}
//------------
// Dirを抽出
//------------
/* (例)
	WCS *args[] = {L"", L"D:", L"C:\\Windows\\", L"a:", L"c:", L"d:\\TMP", NULL};
	WCS **wa1 = iwa_getDir(args);
		iwa_print(wa1); //=> 'C:\' 'C:\Windows\' 'D:\' 'd:\TMP\'
	ifree(wa1);
*/
// v2022-09-03
WCS
**iwa_getDir(
	WCS **ary
)
{
	// 重複排除
	WCS **wa1 = iwa_simplify(ary, TRUE);
		UINT uArySize = iwa_size(wa1);
		WCS **rtn = icalloc_WCS_ary(uArySize);
		UINT u1 = 0, u2 = 0;

		// Dir名整形／存在するDirのみ抽出
		while(u1 < uArySize)
		{
			if(iFchk_typePathW(wa1[u1]) == 1)
			{
				rtn[u2] = iFget_AdirW(wa1[u1]);
				++u2;
			}
			++u1;
		}
	ifree(wa1);

	// 順ソート／大小文字区別しない
	iwa_sort_iAsc(rtn);

	return rtn;
}
//----------------
// 上位Dirを抽出
//----------------
/* (例)
	WCS *args[] = {L"", L"D:", L"C:\\Windows\\", L"a:", L"c:", L"d:\\TMP", NULL};
	WCS **wa1 = iwa_higherDir(args);
		iwa_print(wa1); //=> 'c:\' 'D:\'
	ifree(wa1);
*/
// v2022-09-03
WCS
**iwa_higherDir(
	WCS **ary
)
{
	WCS **rtn = iwa_getDir(ary);
	UINT uArySize = iwa_size(rtn);
	UINT u1 = 0, u2 = 0;

	// 上位Dirを取得
	while(u1 < uArySize)
	{
		u2 = u1 + 1;
		while(u2 < uArySize)
		{
			// 前方一致／大小文字区別しない
			if(iwb_cmpfi(rtn[u2], rtn[u1]))
			{
				rtn[u2] = L"";
				++u2;
			}
			else
			{
				break;
			}
		}
		u1 = u2;
	}

	// 実データを前に詰める
	u1 = 0;
	while(u1 < uArySize)
	{
		if(! *rtn[u1])
		{
			u2 = u1 + 1;
			while(u2 < uArySize)
			{
				if(*rtn[u2])
				{
					rtn[u1] = rtn[u2];
					rtn[u2] = L"";
					break;
				}
				++u2;
			}
		}
		++u1;
	}

	// 末尾の空白をNULL詰め
	u1 = uArySize;
	while(u1 > 0)
	{
		--u1;
		if(! *rtn[u1])
		{
			rtn[u1] = 0;
		}
	}

	return rtn;
}
//-----------
// 配列一覧
//-----------
/* (例)
	// コマンドライン引数
	iwa_print($ARGV);
	iwa_print($ARGS);
*/
// v2022-08-29
VOID
iwa_print(
	WCS **ary
)
{
	if(! ary)
	{
		return;
	}
	UINT u1 = 0;
	while(*ary)
	{
		U8N *up1 = icnv_W2U(*ary++);
			P(" %4u) %s\n", ++u1, up1);
		ifree(up1);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	File/Dir処理(WIN32_FIND_DATAA)
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-------------------
// ファイル情報取得
//-------------------
/* (例１) 複数取得する場合
VOID
ifind(
	$struct_iFinfoW *FI,
	WCS *dir,
	UINT dirLen
)
{
	WIN32_FIND_DATAW F;

	WCS *wp1 = FI->fullnameW + iwn_cpy(FI->fullnameW, dir);
		*wp1 = '*';
		*(++wp1) = 0;

	U8N *up1 = 0;

	HANDLE hfind = FindFirstFileW(FI->fullnameW, &F);
		// Dir
		iFinfo_initW(FI, &F, dir, dirLen, NULL);
			up1 = W2U(FI->fullnameW);
				P2(up1);
			ifree(up1);
		// File
		do
		{
			/// PL2(F.cFileName);
			if(iFinfo_initW(FI, &F, dir, dirLen, F.cFileName))
			{
				// Dir
				if((FI->uFtype) == 1)
				{
					wp1 = iws_pclone(FI->fullnameW, (FI->fullnameW + FI->uEnd));
						ifind(FI, wp1, FI->uEnd); // Dir(下位)
					ifree(wp1);
				}
				// File
				else
				{
					up1 = W2U(FI->fullnameW);
						P2(up1);
					ifree(up1);
				}
			}
		}
		while(FindNextFileW(hfind, &F));
	FindClose(hfind);
}

// main()
	$struct_iFinfoW *FI = iFinfo_allocW();
		WCS *wPath = L".\\";
		WCS *wDir = iFget_AdirW(wPath);
		if(wDir)
		{
			ifind(FI, wDir, iwn_len(wDir));
		}
		ifree(wDir);
	iFinfo_freeW(FI);
*/
/* (例２) 単一ファイルから情報取得する場合
// main()
	$struct_iFinfoW *FI = iFinfo_allocW();
	WCS *fn = L"lib_iwmutil2.c";
	if(iFinfo_init2W(FI, fn))
	{
		U8N *up1 = W2U(FI->fullnameW);
		U8N *up2 = W2U(idate_format_cjdToW(NULL, FI->cjdMtime));
			P2(up1);
			P("%lld Byte\n", FI->iFsize);
			P("mtime: %s", up2);
			NL();
		ifree(up2);
		ifree(up1);
	}
	iFinfo_freeW(FI);
*/
// v2016-08-09
$struct_iFinfoW
*iFinfo_allocW()
{
	return icalloc(1, sizeof($struct_iFinfoW), FALSE);
}
// v2016-08-09
VOID
iFinfo_clearW(
	$struct_iFinfoW *FI
)
{
	*FI->fullnameW = 0;
	FI->uFname = 0;
	FI->uExt = 0;
	FI->uEnd = 0;
	FI->uAttr = 0;
	FI->uFtype = 0;
	FI->cjdCtime = 0.0;
	FI->cjdMtime = 0.0;
	FI->cjdAtime = 0.0;
	FI->iFsize = 0;
}
//---------------------------
// ファイル情報取得の前処理
//---------------------------
// v2022-04-02
BOOL
iFinfo_initW(
	$struct_iFinfoW *FI,
	WIN32_FIND_DATAW *F,
	WCS *dir,            // "\"を付与して呼ぶ
	UINT dirLenW,
	WCS *name
)
{
	// "\." "\.." は除外
	if(name && *name && iwb_cmp_leqfi(name, L".."))
	{
		return FALSE;
	}

	// FI->uAttr
	FI->uAttr = (UINT)F->dwFileAttributes; // DWORD => UINT

	// <32768
	if((FI->uAttr) >> 15)
	{
		iFinfo_clearW(FI);
		return FALSE;
	}

	// FI->fullnameW
	// FI->uFname
	// FI->uEnd
	WCS *wp1 = FI->fullnameW + iwn_cpy(FI->fullnameW, dir);
	UINT u1 = iwn_cpy(wp1, name);

	// FI->uFtype
	// FI->uExt
	// FI->iFsize
	if((FI->uAttr & FILE_ATTRIBUTE_DIRECTORY))
	{
		if(u1)
		{
			dirLenW += u1 + 1;
			*((FI->fullnameW) + dirLenW - 1) = L'\\'; // "\" 付与
			*((FI->fullnameW) + dirLenW) = 0;         // NULL 付与
		}
		(FI->uFtype) = 1;
		(FI->uFname) = (FI->uExt) = (FI->uEnd) = dirLenW;
		(FI->iFsize) = 0;
	}
	else
	{
		(FI->uFtype) = 2;
		(FI->uFname) = dirLenW;
		(FI->uEnd) = (FI->uFname) + u1;
		(FI->uExt) = PathFindExtensionW(FI->fullnameW) - (FI->fullnameW);
		if((FI->uExt) < (FI->uEnd))
		{
			++(FI->uExt);
		}
		(FI->iFsize) = (INT64)F->nFileSizeLow + (F->nFileSizeHigh ? (INT64)(F->nFileSizeHigh) * MAXDWORD + 1 : 0);
	}

	// JST変換
	// FI->cftime
	// FI->mftime
	// FI->aftime
	if((FI->uEnd) <= 3)
	{
		(FI->cjdCtime) = (FI->cjdMtime) = (FI->cjdAtime) = 2444240.0; // 1980-01-01
	}
	else
	{
		FILETIME ft;
		FileTimeToLocalFileTime(&F->ftCreationTime, &ft);
			(FI->cjdCtime) = iFinfo_ftimeToCjd(ft);
		FileTimeToLocalFileTime(&F->ftLastWriteTime, &ft);
			(FI->cjdMtime) = iFinfo_ftimeToCjd(ft);
		FileTimeToLocalFileTime(&F->ftLastAccessTime, &ft);
			(FI->cjdAtime) = iFinfo_ftimeToCjd(ft);
	}
	return TRUE;
}
BOOL
iFinfo_init2W(
	$struct_iFinfoW *FI, //
	WCS *path            // ファイルパス
)
{
	// 存在チェック
	/// PL3(iFchk_existPathM(path));
	if(! iFchk_existPathW(path))
	{
		return FALSE;
	}
	WCS *path2 = iFget_AdirW(path); // 絶対pathを返す
		INT iFtype = iFchk_typePathW(path2);
		UINT uDirLen = (iFtype == 1 ? iwn_len(path2) : (UINT)(PathFindFileNameW(path2) - path2));
		WCS *pDir = (FI->fullnameW); // tmp
			iwn_pcpy(pDir, path2, path2 + uDirLen);
		WCS *sName = 0;
			if(iFtype == 1)
			{
				// Dir
				iwn_cpy(path2 + uDirLen, L"."); // Dir検索用 "." 付与
			}
			else
			{
				// File
				sName = iws_clone(path2 + uDirLen);
			}
			WIN32_FIND_DATAW F;
			HANDLE hfind = FindFirstFileW(path2, &F);
				iFinfo_initW(FI, &F, pDir, uDirLen, sName);
			FindClose(hfind);
		ifree(sName);
	ifree(path2);
	return TRUE;
}
// v2016-08-09
VOID
iFinfo_freeW(
	$struct_iFinfoW *FI
)
{
	ifree(FI);
}
//---------------------
// ファイル情報を変換
//---------------------
/*
	// 1: READONLY
		FILE_ATTRIBUTE_READONLY
	// 2: HIDDEN
		FILE_ATTRIBUTE_HIDDEN
	// 4: SYSTEM
		FILE_ATTRIBUTE_SYSTEM
	// 16: DIRECTORY
		FILE_ATTRIBUTE_DIRECTORY
	// 32: ARCHIVE
		FILE_ATTRIBUTE_ARCHIVE
	// 64: DEVICE
		FILE_ATTRIBUTE_DEVICE
	// 128: NORMAL
		FILE_ATTRIBUTE_NORMAL
	// 256: TEMPORARY
		FILE_ATTRIBUTE_TEMPORARY
	// 512: SPARSE FILE
		FILE_ATTRIBUTE_SPARSE_FILE
	// 1024: REPARSE_POINT
		FILE_ATTRIBUTE_REPARSE_POINT
	// 2048: COMPRESSED
		FILE_ATTRIBUTE_COMPRESSED
	// 8192: NOT CONTENT INDEXED
		FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
	// 16384: ENCRYPTED
		FILE_ATTRIBUTE_ENCRYPTED
*/
// v2022-08-30
WCS
*iFinfo_attrToW(
	UINT uAttr
)
{
	WCS *rtn = icalloc_WCS(5);
	if(! rtn)
	{
		return 0;
	}
	rtn[0] = (uAttr & FILE_ATTRIBUTE_DIRECTORY ? 'd' : '-'); // 16: Dir
	rtn[1] = (uAttr & FILE_ATTRIBUTE_READONLY  ? 'r' : '-'); //  1: ReadOnly
	rtn[2] = (uAttr & FILE_ATTRIBUTE_HIDDEN    ? 'h' : '-'); //  2: Hidden
	rtn[3] = (uAttr & FILE_ATTRIBUTE_SYSTEM    ? 's' : '-'); //  4: System
	rtn[4] = (uAttr & FILE_ATTRIBUTE_ARCHIVE   ? 'a' : '-'); // 32: Archive
	return rtn;
}
// v2022-09-02
INT
iFinfo_attrWtoINT(
	WCS *sAttr // "r, h, s, d, a" => 55
)
{
	if(! sAttr || ! *sAttr)
	{
		return 0;
	}
	WCS **wa1 = iwa_split(sAttr, NULL, TRUE);
	WCS **wa2 = iwa_simplify(wa1, TRUE);
		WCS *wp1 = iwa_join(wa2, L"");
	ifree(wa2);
	ifree(wa1);
	// 小文字に変換
	CharLowerW(wp1);
	INT rtn = 0;
	WCS *pE = wp1;
	while(*pE)
	{
		// 頻出順
		switch(*pE)
		{
			// 32: ARCHIVE
			case('a'):
				rtn += FILE_ATTRIBUTE_ARCHIVE;
				break;

			// 16: DIRECTORY
			case('d'):
				rtn += FILE_ATTRIBUTE_DIRECTORY;
				break;

			// 4: SYSTEM
			case('s'):
				rtn += FILE_ATTRIBUTE_SYSTEM;
				break;

			// 2: HIDDEN
			case('h'):
				rtn += FILE_ATTRIBUTE_HIDDEN;
				break;

			// 1: READONLY
			case('r'):
				rtn += FILE_ATTRIBUTE_READONLY;
				break;
		}
		++pE;
	}
	ifree(wp1);
	return rtn;
}
WCS
*iFinfo_ftypeToW(
	INT iFtype
)
{
	WCS *rtn = icalloc_WCS(1);
	switch(iFtype)
	{
		case(1): *rtn = 'd'; break;
		case(2): *rtn = 'f'; break;
		default: *rtn = '-'; break;
	}
	return rtn;
}
/*
	(Local)"c:\" => 0
	(Network)"\\localhost\" => 0
*/
// v2022-05-26
INT
iFinfo_depthW(
	$struct_iFinfoW *FI
)
{
	if(! *FI->fullnameW)
	{
		return -1;
	}
	return iwn_searchCnt(FI->fullnameW + 2, L"\\") - 1;
}
//---------------------------
// ファイルサイズ取得に特化
//---------------------------
INT64
iFinfo_fsizeW(
	WCS *Fn // 入力ファイル名
)
{
	$struct_iFinfoW *FI = iFinfo_allocW();
	iFinfo_init2W(FI, Fn);
	INT64 rtn = FI->iFsize;
	iFinfo_freeW(FI);
	return rtn;
}
//---------------
// FileTime関係
//---------------
/*
	基本、FILETIME(UTC)で処理。
	必要に応じて、JST(UTC+9h)に変換した値を渡すこと。
*/
WCS
*iFinfo_ftimeToW(
	FILETIME ft
)
{
	WCS *rtn = icalloc_WCS(32);
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);
	if(st.wYear <= 1980 || st.wYear >= 2099)
	{
		rtn = 0;
	}
	else
	{
		wsprintfW(
			rtn,
			ISO_FORMAT_DATETIME,
			st.wYear,
			st.wMonth,
			st.wDay,
			st.wHour,
			st.wMinute,
			st.wSecond
		);
	}
	return rtn;
}
// v2015-01-03
DOUBLE
iFinfo_ftimeToCjd(
	FILETIME ftime
)
{
	INT64 i1 = ((INT64)ftime.dwHighDateTime << 32) + ftime.dwLowDateTime;
	i1 /= 10000000; // (重要) MicroSecond 削除
	return ((DOUBLE)i1 / 86400) + 2305814.0;
}
// v2021-11-15
FILETIME
iFinfo_ymdhnsToFtime(
	INT i_y,   // 年
	INT i_m,   // 月
	INT i_d,   // 日
	INT i_h,   // 時
	INT i_n,   // 分
	INT i_s,   // 秒
	BOOL reChk // TRUE = 年月日を正規化／FALSE = 入力値を信用
)
{
	SYSTEMTIME st;
	FILETIME ft;
	if(reChk)
	{
		INT *ai = idate_reYmdhns(i_y, i_m, i_d, i_h, i_n, i_s); // 正規化
			i_y = ai[0];
			i_m = ai[1];
			i_d = ai[2];
			i_h = ai[3];
			i_n = ai[4];
			i_s = ai[5];
		ifree(ai);
	}
	st.wYear         = i_y;
	st.wMonth        = i_m;
	st.wDay          = i_d;
	st.wHour         = i_h;
	st.wMinute       = i_n;
	st.wSecond       = i_s;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ft);
	return ft;
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	File/Dir処理
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------
// ファイルが存在するときTRUEを返す
//-----------------------------------
/* (例)
	PL3(iFchk_existPathW(L"."));    //=> 1
	PL3(iFchk_existPathW(L""));     //=> 0
	PL3(iFchk_existPathW(L"\\"));   //=> 1
	PL3(iFchk_existPathW(L"\\\\")); //=> 0
*/
// v2022-08-30
BOOL
iFchk_existPathW(
	WCS *path // ファイルパス
)
{
	if(! path || ! *path)
	{
		return FALSE;
	}
	return (PathFileExistsW(path) ? TRUE : FALSE);
}
//---------------------------------
// Dir／File が実在するかチェック
//---------------------------------
/* (例)
	// 返り値
	//  Err  : 0
	//  Dir  : 1
	//  File : 2
	//
	// 存在チェックはしない
	// 必要に応じて iFchk_existPathM() で前処理
	PL3(iFchk_typePathW(L"."));                       //=> 1
	PL3(iFchk_typePathW(L".."));                      //=> 1
	PL3(iFchk_typePathW(L"\\"));                      //=> 1
	PL3(iFchk_typePathW(L"c:\\windows\\"));           //=> 1
	PL3(iFchk_typePathW(L"c:\\windows\\system.ini")); //=> 2
	PL3(iFchk_typePathW(L"\\\\Network\\"));           //=> 2(不明なときも)
*/
// v2022-08-30
INT
iFchk_typePathW(
	WCS *path // ファイルパス
)
{
	if(! path || ! *path)
	{
		return 0;
	}
	return (PathIsDirectoryW(path) ? 1 : 2);
}
//-------------------------------
// Binary File のときTRUEを返す
//-------------------------------
/* (例)
	PL3(iFchk_BfileW(L"aaa.exe")); //=> TRUE
	PL3(iFchk_BfileW(L"aaa.txt")); //=> FALSE
	PL3(iFchk_BfileW(L"???"));     //=> FALSE (存在しないとき)
*/
// v2022-08-30
BOOL
iFchk_BfileW(
	WCS *Fn
)
{
	FILE *Fp = _wfopen(Fn, L"rb");
	if(! Fp)
	{
		return FALSE;
	}
	UINT cnt = 0, c = 0, u1 = 0;
	// 64byteでは不完全
	while((c = getc(Fp)) != (UINT)EOF && u1 < 128)
	{
		if(! c)
		{
			++cnt;
			break;
		}
		++u1;
	}
	fclose(Fp);
	return (0 < cnt ? TRUE : FALSE);
}
//---------------------
// ファイル名等を抽出
//---------------------
/* (例)
	// 存在しなくても機械的に抽出
	// 必要に応じて iFchk_existPathM() で前処理
	WCS *wp1 = L"c:\\windows\\win.ini";
	PL2(W2U(iFget_extPathnameW(wp1, 0))); //=> "c:\windows\win.ini"
	PL2(W2U(iFget_extPathnameW(wp1, 1))); //=> "win.ini"
	PL2(W2U(iFget_extPathnameW(wp1, 2))); //=> "win"
*/
// v2022-09-05
WCS
*iFget_extPathnameW(
	WCS *path,
	INT option // 1=拡張子付きファイル名／2=拡張子なしファイル名
)
{
	if(! path || ! *path)
	{
		return 0;
	}
	WCS *rtn = icalloc_WCS(iwn_len(path) + 3); // CRLF+NULL
	WCS *pBgn = 0;
	WCS *pEnd = 0;

	// Dir or File ?
	if(PathIsDirectoryW(path))
	{
		if(option == 0)
		{
			iwn_cpy(rtn, path);
		}
	}
	else
	{
		switch(option)
		{
			// path
			case(0):
				iwn_cpy(rtn, path);
				break;

			// name + ext
			case(1):
				pBgn = PathFindFileNameW(path);
				iwn_cpy(rtn, pBgn);
				break;

			// name
			case(2):
				pBgn = PathFindFileNameW(path);
				pEnd = PathFindExtensionW(pBgn);
				iwn_pcpy(rtn, pBgn, pEnd);
				break;
		}
	}
	return rtn;
}
//-------------------------------------
// 相対Dir を 絶対Dir("\"付き) に変換
//-------------------------------------
/* (例)
	// _fullpath() の応用
	PL2(W2U(iFget_AdirW(L".\\")));
*/
// v2022-08-30
WCS
*iFget_AdirW(
	WCS *path // ファイルパス
)
{
	WCS *wp1 = icalloc_WCS(IMAX_PATH);
	WCS *wp2 = 0;
	switch(iFchk_typePathW(path))
	{
		// Dir
		case(1):
			wp2 = iws_cats(2, path, L"\\");
				_wfullpath(wp1, wp2, IMAX_PATH);
			ifree(wp2);
			break;
		// File
		case(2):
			_wfullpath(wp1, path, IMAX_PATH);
			break;
	}
	WCS *rtn = iws_clone(wp1);
	ifree(wp1);
	return rtn;
}
//----------------------------
// 相対Dir を "\" 付きに変換
//----------------------------
/* (例)
	// _fullpath() の応用
	PL2(W2U(iFget_RdirW(L"."))); => ".\\"
*/
// v2022-08-30
WCS
*iFget_RdirW(
	WCS *path // ファイルパス
)
{
	WCS *rtn = 0;
	if(PathIsDirectoryW(path))
	{
		rtn = iws_cutYenR(path);
		*(rtn + iwn_len(rtn)) = L'\\';
	}
	else
	{
		rtn = iws_replace(path, L"\\\\", L"\\", FALSE);
	}
	return rtn;
}
//--------------------
// 複階層のDirを作成
//--------------------
/* (例)
	PL3(imk_dirW(L"aaa\\bbb"));
*/
// v2022-08-30
BOOL
imk_dirW(
	WCS *path // ファイルパス
)
{
	INT flg = 0;
	WCS *wp1 = 0;
	WCS *pBgn = iws_cutYenR(path);
	WCS *pEnd = pBgn;
	while(*pEnd)
	{
		pEnd = iwp_searchL(pEnd, L"\\");
		wp1 = iws_pclone(pBgn, pEnd);
			if(CreateDirectoryW(wp1, 0))
			{
				++flg;
			}
		ifree(wp1);
		++pEnd;
	}
	ifree(pBgn);
	return (flg ? TRUE : FALSE);
}
//-------------------------
// File/Dirをゴミ箱へ移動
//-------------------------
/* (例)
	PL3(imv_trashW(L"Dir", TRUE));   // 空DIrのときのみ、ゴミ箱へ移動する
	PL3(imv_trashW(L"Dir", FALSE));  // DIrをゴミ箱へ移動する
	PL3(imv_trashW(L"File", FALSE)); // Fileをゴミ箱へ移動する
*/
// v2022-08-30
BOOL
imv_trashW(
	WCS *path,     // ファイルパス
	BOOL bFileOnly // TRUE=ファイルのみゴミ箱へ移動する
)
{
	// 存在するか？
	if(! iFchk_existPathW(path))
	{
		return FALSE;
	}
	// Dirか？
	if(bFileOnly && iFchk_typePathW(path) == 1)
	{
		return FALSE;
	}
	// path + "\0" = 末尾"\0\0"
	MBS *mp1 = icnv_W2M(path);
		SHFILEOPSTRUCT sfos;
			ZeroMemory(&sfos, sizeof(SHFILEOPSTRUCT));
			sfos.hwnd = 0;
			sfos.wFunc = FO_DELETE;
			// ファイル名は W2M()
			sfos.pFrom = mp1;
			sfos.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
		INT rtn = SHFileOperation(&sfos);
	ifree(mp1);
	return (rtn ? FALSE : TRUE);
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Console
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//------------------
// RGB色を使用する
//------------------
/* (例)
	// ESC有効化
	iConsole_EscOn();

	// リセット
	//   すべて   \x1b[0m
	//   文字のみ \x1b[39m
	//   背景のみ \x1b[49m

	// SGRによる指定例
	//  文字色 9n
	//  背景色 10n
	//    0 = 黒    1 = 赤    2 = 黄緑  3 = 黄
	//    4 = 青    5 = 紅紫  6 = 水    7 = 白
	P2("\x1b[91m 文字[赤] \x1b[0m");
	P2("\x1b[101m 背景[赤] \x1b[0m");
	P2("\x1b[91;107m 文字[赤]／背景[白] \x1b[0m");

	// RGBによる指定例
	//  文字色   \x1b[38;2;R;G;Bm
	//  背景色   \x1b[48;2;R;G;Bm
	P2("\x1b[38;2;255;255;255m\x1b[48;2;0;0;255m 文字[白]／背景[青] \x1b[0m");
*/
// v2022-03-23
VOID
iConsole_EscOn()
{
	$StdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode = 0;
	GetConsoleMode($StdoutHandle, &consoleMode);
	consoleMode = (consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
	SetConsoleMode($StdoutHandle, consoleMode);
}
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	暦
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
//--------------------------
// 空白暦：1582/10/5-10/14
//--------------------------
// {-4712-1-1からの通算日, yyyymmdd}
INT NS_BEFORE[2] = {2299160, 15821004};
INT NS_AFTER[2]  = {2299161, 15821015};
//---------------------------
// 曜日表示設定 [7] = Err値
//---------------------------
WCS *WDAYS[8] = {L"Su", L"Mo", L"Tu", L"We", L"Th", L"Fr", L"Sa", L"**"};
//-----------------------
// 月末日(-1y12m - 12m)
//-----------------------
INT MDAYS[13] = {31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//---------------
// 閏年チェック
//---------------
/* (例)
	idate_chk_uruu(2012); //=> TRUE
	idate_chk_uruu(2013); //=> FALSE
*/
// v2022-04-29
BOOL
idate_chk_uruu(
	INT i_y // 年
)
{
	if(i_y > (INT)(NS_AFTER[1] / 10000))
	{
		if(! (i_y % 400))
		{
			return TRUE;
		}
		if(! (i_y % 100))
		{
			return FALSE;
		}
	}
	return (! (i_y % 4) ? TRUE : FALSE);
}
//-------------
// 月を正規化
//-------------
/* (例)
	INT *ai = idate_cnv_month(2011, 14, 1, 12);
	for(INT i1 = 0; i1 < 2; i1++)
	{
		PL3(ai[i1]); //=> 2012, 2
	}
	ifree(ai);
*/
// v2021-11-15
INT
*idate_cnv_month(
	INT i_y,    // 年
	INT i_m,    // 月
	INT from_m, // 開始月
	INT to_m    // 終了月
)
{
	INT *rtn = icalloc_INT(2);
	while(i_m < from_m)
	{
		i_m += 12;
		i_y -= 1;
	}
	while(i_m > to_m)
	{
		i_m -= 12;
		i_y += 1;
	}
	rtn[0] = i_y;
	rtn[1] = i_m;
	return rtn;
}
//---------------
// 月末日を返す
//---------------
/* (例)
	idate_month_end(2012, 2); //=> 29
	idate_month_end(2013, 2); //=> 28
*/
// v2021-11-15
INT
idate_month_end(
	INT i_y, // 年
	INT i_m  // 月
)
{
	INT *ai = idate_cnv_month1(i_y, i_m);
	INT i_d = MDAYS[ai[1]];
	// 閏２月末日
	if(ai[1] == 2 && idate_chk_uruu(ai[0]))
	{
		i_d = 29;
	}
	ifree(ai);
	return i_d;
}
//-----------------
// 月末日かどうか
//-----------------
/* (例)
	PL3(idate_chk_month_end(2012, 2, 28, FALSE)); //=> FALSE
	PL3(idate_chk_month_end(2012, 2, 29, FALSE)); //=> TRUE
	PL3(idate_chk_month_end(2012, 1, 60, TRUE )); //=> TRUE
	PL3(idate_chk_month_end(2012, 1, 60, FALSE)); //=> FALSE
*/
// v2021-11-15
BOOL
idate_chk_month_end(
	INT i_y,   // 年
	INT i_m,   // 月
	INT i_d,   // 日
	BOOL reChk // TRUE = 年月日を正規化／FALSE = 入力値を信用
)
{
	if(reChk)
	{
		INT *ai1 = idate_reYmdhns(i_y, i_m, i_d, 0, 0, 0); // 正規化
			i_y = ai1[0];
			i_m = ai1[1];
			i_d = ai1[2];
		ifree(ai1);
	}
	return (i_d == idate_month_end(i_y, i_m) ? TRUE : FALSE);
}
//-------------------------
// strを年月日に分割(int)
//-------------------------
/* (例)
	INT *ai1 = idate_WCS_to_iAryYmdhns(L"-2012-8-12 12:45:00");
	for(INT i1 = 0; i1 < 6; i1++)
	{
		PL3(ai1[i1]); //=> -2012, 8, 12, 12, 45, 0
	}
	ifree(ai1);
*/
// v2022-08-25
INT
*idate_WCS_to_iAryYmdhns(
	WCS *str // (例) "2012-03-12 13:40:00"
)
{
	INT *rtn = icalloc_INT(6);
	INT i1 = 1;
	if(*str == '-')
	{
		i1 = -1;
		++str;
	}
	WCS **as1 = iwa_split(str, L"/.-: ", TRUE);
	INT i2 = 0;
	while(as1[i2])
	{
		rtn[i2] = _wtoi(as1[i2]);
		++i2;
	}
	ifree(as1);
	if(i1 == -1)
	{
		rtn[0] *= -1;
	}
	return rtn;
}
//---------------------
// 年月日を数字に変換
//---------------------
/* (例)
	idate_ymd_num(2012, 6, 17); //=> 20120617
*/
// v2013-03-17
INT
idate_ymd_num(
	INT i_y, // 年
	INT i_m, // 月
	INT i_d  // 日
)
{
	return (i_y * 10000) + (i_m * 100) + (i_d);
}
//-----------------------------------------------
// 年月日をCJDに変換
//   (注)空白日のとき、一律 NS_BEFORE[0] を返す
//-----------------------------------------------
// v2021-11-15
DOUBLE
idate_ymdhnsToCjd(
	INT i_y, // 年
	INT i_m, // 月
	INT i_d, // 日
	INT i_h, // 時
	INT i_n, // 分
	INT i_s  // 秒
)
{
	DOUBLE cjd = 0.0;
	INT i1 = 0, i2 = 0;
	INT *ai = idate_cnv_month1(i_y, i_m);
		i_y = ai[0];
		i_m = ai[1];
	ifree(ai);
	// 1m=>13m, 2m=>14m
	if(i_m <= 2)
	{
		i_y -= 1;
		i_m += 12;
	}
	// 空白日
	i1 = idate_ymd_num(i_y, i_m, i_d);
	if(i1 > NS_BEFORE[1] && i1 < NS_AFTER[1])
	{
		return (DOUBLE)NS_BEFORE[0];
	}
	// ユリウス通日
	cjd = floor((DOUBLE)(365.25 * (i_y + 4716)) + floor(30.6001 * (i_m + 1)) + i_d - 1524.0);
	if((INT)cjd >= NS_AFTER[0])
	{
		i1 = (INT)floor(i_y / 100.0);
		i2 = 2 - i1 + (INT)floor(i1 / 4);
		cjd += (DOUBLE)i2;
	}
	return cjd + ((i_h * 3600 + i_n * 60 + i_s) / 86400.0);
}
//--------------------
// CJDを時分秒に変換
//--------------------
// v2021-11-15
INT
*idate_cjd_to_iAryHhs(
	DOUBLE cjd
)
{
	INT *rtn = icalloc_INT(3);
	INT i_h = 0, i_n = 0, i_s = 0;

	// 小数点部分を抽出
	// [sec][補正前]  [cjd]
	//   0  =  0   -  0.00000000000000000
	//   1  =  1   -  0.00001157407407407
	//   2  =  2   -  0.00002314814814815
	//   3  >  2  NG  0.00003472222222222
	//   4  =  4   -  0.00004629629629630
	//   5  =  5   -  0.00005787037037037
	//   6  >  5  NG  0.00006944444444444
	//   7  =  7   -  0.00008101851851852
	//   8  =  8   -  0.00009259259259259
	//   9  =  9   -  0.00010416666666667
	DOUBLE d1 = (cjd - (INT)cjd);
		d1 += 0.00001; // 補正
		d1 *= 24.0;
	i_h = (INT)d1;
		d1 -= i_h;
		d1 *= 60.0;
	i_n = (INT)d1;
		d1 -= i_n;
		d1 *= 60.0;
	i_s = (INT)d1;
	rtn[0] = i_h;
	rtn[1] = i_n;
	rtn[2] = i_s;

	return rtn;
}
//--------------------
// CJDをYMDHNSに変換
//--------------------
// v2021-11-15
INT
*idate_cjd_to_iAryYmdhns(
	DOUBLE cjd // cjd通日
)
{
	INT *rtn = icalloc_INT(6);
	INT i_y = 0, i_m = 0, i_d = 0;
	INT iCJD = (INT)cjd;
	INT i1 = 0, i2 = 0, i3 = 0, i4 = 0;
	if((INT)cjd >= NS_AFTER[0])
	{
		i1 = (INT)floor((cjd - 1867216.25) / 36524.25);
		iCJD += (i1 - (INT)floor(i1 / 4.0) + 1);
	}
	i1 = iCJD + 1524;
	i2 = (INT)floor((i1 - 122.1) / 365.25);
	i3 = (INT)floor(365.25 * i2);
	i4 = (INT)((i1 - i3) / 30.6001);
	// d
	i_d = (i1 - i3 - (INT)floor(30.6001 * i4));
	// y, m
	if(i4 <= 13)
	{
		i_m = i4 - 1;
		i_y = i2 - 4716;
	}
	else
	{
		i_m = i4 - 13;
		i_y = i2 - 4715;
	}
	// h, n, s
	INT *ai2 = idate_cjd_to_iAryHhs(cjd);
		rtn[0] = i_y;
		rtn[1] = i_m;
		rtn[2] = i_d;
		rtn[3] = ai2[0];
		rtn[4] = ai2[1];
		rtn[5] = ai2[2];
	ifree(ai2);
	return rtn;
}
//----------------------
// CJDをFILETIMEに変換
//----------------------
// v2021-11-15
FILETIME
idate_cjdToFtime(
	DOUBLE cjd // cjd通日
)
{
	INT *ai1 = idate_cjd_to_iAryYmdhns(cjd);
	INT i_y = ai1[0], i_m = ai1[1], i_d = ai1[2], i_h = ai1[3], i_n = ai1[4], i_s = ai1[5];
	ifree(ai1);
	return iFinfo_ymdhnsToFtime(i_y, i_m, i_d, i_h, i_n, i_s, FALSE);
}
//---------
// 再計算
//---------
// v2013-03-21
INT
*idate_reYmdhns(
	INT i_y, // 年
	INT i_m, // 月
	INT i_d, // 日
	INT i_h, // 時
	INT i_n, // 分
	INT i_s  // 秒
)
{
	DOUBLE cjd = idate_ymdhnsToCjd(i_y, i_m, i_d, i_h, i_n, i_s);
	return idate_cjd_to_iAryYmdhns(cjd);
}
//-------------------------------------------
// cjd通日から曜日(日 = 0, 月 = 1...)を返す
//-------------------------------------------
// v2013-03-21
INT
idate_cjd_iWday(
	DOUBLE cjd // cjd通日
)
{
	return (INT)((INT)cjd + 1) % 7;
}
//-----------------------------------------
// cjd通日から 日="Su", 月="Mo", ...を返す
//-----------------------------------------
// v2022-08-20
WCS
*idate_cjd_Wday(
	DOUBLE cjd // cjd通日
)
{
	return WDAYS[idate_cjd_iWday(cjd)];
}
//------------------------------
// cjd通日から年内の通日を返す
//------------------------------
// v2021-11-15
INT
idate_cjd_yeardays(
	DOUBLE cjd // cjd通日
)
{
	INT *ai = idate_cjd_to_iAryYmdhns(cjd);
	INT i1 = ai[0];
	ifree(ai);
	return (INT)(cjd - idate_ymdhnsToCjd(i1, 1, 0, 0, 0, 0));
}
//--------------------------------------
// 日付の前後 [6] = {y, m, d, h, n, s}
//--------------------------------------
/* (例)
	INT *ai = idate_add(2012, 1, 31, 0, 0, 0, 0, 1, 0, 0, 0, 0);
	for(INT i1 = 0; i1 < 6; i1++)
	{
		PL3(ai[i1]); //=> 2012, 2, 29, 0, 0, 0
	}
*/
// v2021-11-15
INT
*idate_add(
	INT i_y,   // 年
	INT i_m,   // 月
	INT i_d,   // 日
	INT i_h,   // 時
	INT i_n,   // 分
	INT i_s,   // 秒
	INT add_y, // 年
	INT add_m, // 月
	INT add_d, // 日
	INT add_h, // 時
	INT add_n, // 分
	INT add_s  // 秒
)
{
	INT *ai1 = 0;
	INT *ai2 = idate_reYmdhns(i_y, i_m, i_d, i_h, i_n, i_s); // 正規化
	INT i1 = 0, i2 = 0, flg = 0;
	DOUBLE cjd = 0.0;

	// 個々に計算
	// 手を抜くと「1582-11-10 -1m, -1d」のとき、1582-10-04(期待値は1582-10-03)となる
	if(add_y != 0 || add_m != 0)
	{
		i1 = (INT)idate_month_end(ai2[0] + add_y, ai2[1] + add_m);
		if(ai2[2] > (INT)i1)
		{
			ai2[2] = (INT)i1;
		}
		ai1 = idate_reYmdhns(ai2[0] + add_y, ai2[1] + add_m, ai2[2], ai2[3], ai2[4], ai2[5]);
		i2 = 0;
		while(i2 < 6)
		{
			ai2[i2] = ai1[i2];
			++i2;
		}
		ifree(ai1);
		++flg;
	}
	if(add_d != 0)
	{
		cjd = idate_ymdhnsToCjd(ai2[0], ai2[1], ai2[2], ai2[3], ai2[4], ai2[5]);
		ai1 = idate_cjd_to_iAryYmdhns(cjd + add_d);
		i2 = 0;
		while(i2 < 6)
		{
			ai2[i2] = ai1[i2];
			++i2;
		}
		ifree(ai1);
		++flg;
	}
	if(add_h != 0 || add_n != 0 || add_s != 0)
	{
		ai1 = idate_reYmdhns(ai2[0], ai2[1], ai2[2], ai2[3] + add_h, ai2[4] + add_n, ai2[5] + add_s);
		i2 = 0;
		while(i2 < 6)
		{
			ai2[i2] = ai1[i2];
			++i2;
		}
		ifree(ai1);
		++flg;
	}
	if(flg)
	{
		ai1 = icalloc_INT(6);
		i2 = 0;
		while(i2 < 6)
		{
			ai1[i2] = ai2[i2];
			++i2;
		}
	}
	else
	{
		ai1 = idate_reYmdhns(ai2[0], ai2[1], ai2[2], ai2[3], ai2[4], ai2[5]);
	}
	ifree(ai2);

	return ai1;
}
//-------------------------------------------------------
// 日付の差 [8] = {sign, y, m, d, h, n, s, days}
//   (注)便宜上、(日付1<=日付2)に変換して計算するため、
//       結果は以下のとおりとなるので注意。
//       ・5/31⇒6/30 : + 1m
//       ・6/30⇒5/31 : -30d
//-------------------------------------------------------
/* (例)
	INT *ai = idate_diff(2012, 1, 31, 0, 0, 0, 2012, 2, 29, 0, 0, 0); //=> sign = 1, y = 0, m = 1, d = 0, h = 0, n = 0, s = 0, days = 29
	for(INT i1 = 0; i1 < 7; i1++)
	{
		PL3(ai[i1]); //=> 2012, 2, 29, 0, 0, 0, 29
	}
*/
// v2021-11-15
INT
*idate_diff(
	INT i_y1, // 年1
	INT i_m1, // 月1
	INT i_d1, // 日1
	INT i_h1, // 時1
	INT i_n1, // 分1
	INT i_s1, // 秒1
	INT i_y2, // 年2
	INT i_m2, // 月2
	INT i_d2, // 日2
	INT i_h2, // 時2
	INT i_n2, // 分2
	INT i_s2  // 秒2
)
{
	INT *rtn = icalloc_INT(8);
	INT i1 = 0, i2 = 0, i3 = 0, i4 = 0;
	DOUBLE cjd = 0.0;
	/*
		正規化1
	*/
	DOUBLE cjd1 = idate_ymdhnsToCjd(i_y1, i_m1, i_d1, i_h1, i_n1, i_s1);
	DOUBLE cjd2 = idate_ymdhnsToCjd(i_y2, i_m2, i_d2, i_h2, i_n2, i_s2);

	if(cjd1 > cjd2)
	{
		rtn[0] = -1; // sign(-)
		cjd  = cjd1;
		cjd1 = cjd2;
		cjd2 = cjd;
	}
	else
	{
		rtn[0] = 1; // sign(+)
	}
	/*
		days
	*/
	rtn[7] = (INT)(cjd2 - cjd1);
	/*
		正規化2
	*/
	INT *ai1 = idate_cjd_to_iAryYmdhns(cjd1);
	INT *ai2 = idate_cjd_to_iAryYmdhns(cjd2);
	/*
		ymdhns
	*/
	rtn[1] = ai2[0] - ai1[0];
	rtn[2] = ai2[1] - ai1[1];
	rtn[3] = ai2[2] - ai1[2];
	rtn[4] = ai2[3] - ai1[3];
	rtn[5] = ai2[4] - ai1[4];
	rtn[6] = ai2[5] - ai1[5];
	/*
		m 調整
	*/
	INT *ai3 = idate_cnv_month2(rtn[1], rtn[2]);
		rtn[1] = ai3[0];
		rtn[2] = ai3[1];
	ifree(ai3);
	/*
		hns 調整
	*/
	while(rtn[6] < 0)
	{
		rtn[6] += 60;
		rtn[5] -= 1;
	}
	while(rtn[5] < 0)
	{
		rtn[5] += 60;
		rtn[4] -= 1;
	}
	while(rtn[4] < 0)
	{
		rtn[4] += 24;
		rtn[3] -= 1;
	}
	/*
		d 調整
		前処理
	*/
	if(rtn[3] < 0)
	{
		rtn[2] -= 1;
		if(rtn[2] < 0)
		{
			rtn[2] += 12;
			rtn[1] -= 1;
		}
	}
	/*
		本処理
	*/
	if(rtn[0] > 0)
	{
		ai3 = idate_add(ai1[0], ai1[1], ai1[2], ai1[3], ai1[4], ai1[5], rtn[1], rtn[2], 0, 0, 0, 0);
			i1 = (INT)idate_ymdhnsToCjd(ai2[0], ai2[1], ai2[2], 0, 0, 0);
			i2 = (INT)idate_ymdhnsToCjd(ai3[0], ai3[1], ai3[2], 0, 0, 0);
		ifree(ai3);
	}
	else
	{
		ai3 = idate_add(ai2[0], ai2[1], ai2[2], ai2[3], ai2[4], ai2[5], -rtn[1], -rtn[2], 0, 0, 0, 0);
			i1 = (INT)idate_ymdhnsToCjd(ai3[0], ai3[1], ai3[2], 0, 0, 0);
			i2 = (INT)idate_ymdhnsToCjd(ai1[0], ai1[1], ai1[2], 0, 0, 0);
		ifree(ai3);
	}
	i3 = idate_ymd_num(ai1[3], ai1[4], ai1[5]);
	i4 = idate_ymd_num(ai2[3], ai2[4], ai2[5]);
	/* 変換例
		"05-31" "06-30" のとき m = 1, d = 0
		"06-30" "05-31" のとき m = 0, d = -30
		※分岐条件は非常にシビアなので安易に変更するな!!
	*/
	if(rtn[0] > 0                                             // +のときのみ
		&& i3 <= i4                                           // (例) "12:00:00 =< 23:00:00"
		&& idate_chk_month_end(ai2[0], ai2[1], ai2[2], FALSE) // (例) 06-"30" は月末日？
		&& ai1[2] > ai2[2]                                    // (例) 05-"31" > 06-"30"
	)
	{
		rtn[2] += 1;
		rtn[3] = 0;
	}
	else
	{
		rtn[3] = i1 - i2 - (INT)(i3 > i4 ? 1 : 0);
	}
	ifree(ai2);
	ifree(ai1);

	return rtn;
}
//--------------------------
// diff()／add()の動作確認
//--------------------------
/* (例) 西暦1年から2000年迄のサンプル100例について評価
	idate_diff_checker(1, 2000, 100);
*/
// v2022-08-26
/*
VOID
idate_diff_checker(
	INT from_year, // 何年から
	INT to_year,   // 何年まで
	UINT uSample   // サンプル抽出数
)
{
	if(from_year > to_year)
	{
		INT i1 = to_year;
		to_year = from_year;
		from_year = i1;
	}
	INT rnd_y = to_year - from_year;
	INT *ai1 = 0, *ai2 = 0, *ai3 = 0, *ai4 = 0;
	INT y1 = 0, y2 = 0, m1 = 0, m2 = 0, d1 = 0, d2 = 0;
	MBS s1[16] = "", s2[16] = "";
	MBS *err = 0;
	P2("\x1b[94m--Cnt----From----------To------------[Sign,    Y,  M,  D]----DateAdd-------Chk----\x1b[39m");
	MT_init(TRUE);
	for(INT i1 = 1; i1 <= uSample; i1++)
	{
		y1 = from_year + (INT)MT_irand_INT64(0, rnd_y);
		y2 = from_year + (INT)MT_irand_INT64(0, rnd_y);
		m1 = 1 + (INT)MT_irand_INT64(0, 11);
		m2 = 1 + (INT)MT_irand_INT64(0, 11);
		d1 = 1 + (INT)MT_irand_INT64(0, 30);
		d2 = 1 + (INT)MT_irand_INT64(0, 30);
		// 再計算
		ai1 = idate_reYmdhns(y1, m1, d1, 0, 0, 0);
		ai2 = idate_reYmdhns(y2, m2, d2, 0, 0, 0);
		// diff & add
		ai3 = idate_diff(ai1[0], ai1[1], ai1[2], 0, 0, 0, ai2[0], ai2[1], ai2[2], 0, 0, 0);
		ai4 = (
			ai3[0] > 0 ?
			idate_add(ai1[0], ai1[1], ai1[2], 0, 0, 0, ai3[1], ai3[2], ai3[3], 0, 0, 0) :
			idate_add(ai1[0], ai1[1], ai1[2], 0, 0, 0, -(ai3[1]), -(ai3[2]), -(ai3[3]), 0, 0, 0)
		);
		// 計算結果の照合
		sprintf(s1, "%d%02d%02d", ai2[0], ai2[1], ai2[2]);
		sprintf(s2, "%d%02d%02d", ai4[0], ai4[1], ai4[2]);
		err = (strcmp(s1, s2) ? "\x1b[95mNG\x1b[39m" : "ok");
		P(
			"%5d   %5d-%02d-%02d   \x1b[96m%5d-%02d-%02d\x1b[39m    [%4d, %4d, %2d, %2d]   \x1b[96m%5d-%02d-%02d\x1b[39m    %s\n",
			i1,
			ai1[0], ai1[1], ai1[2], ai2[0], ai2[1], ai2[2],
			ai3[0], ai3[1], ai3[2], ai3[3], ai4[0], ai4[1], ai4[2],
			err
		);
		ifree(ai4);
		ifree(ai3);
		ifree(ai2);
		ifree(ai1);
	}
	MT_free();
}
*/
/*
	// Ymdhns
	%a : 曜日(例:Su)
	%A : 曜日数(例:0)
	%c : 年内の通算日
	%C : CJD通算日
	%J : JD通算日
	%e : 年内の通算週

	// Diff
	%Y : 通算年
	%M : 通算月
	%D : 通算日
	%H : 通算時
	%N : 通算分
	%S : 通算秒
	%W : 通算週
	%w : 通算週の余日

	// 共通
	%g : Sign "-" "+"
	%G : Sign "-" のみ
	%y : 年(0000)
	%m : 月(00)
	%d : 日(00)
	%h : 時(00)
	%n : 分(00)
	%s : 秒(00)
	%% : "%"
	\a
	\n
	\t
*/
/* (例) ymdhns
	INT *ai1 = idate_reYmdhns(1970, 12, 10, 0, 0, 0);
	WCS *wp1 = idate_format_ymdhns(L"%g%y-%m-%d(%a), %c / %C", ai1[0], ai1[1], ai1[2], ai1[3], ai1[4], ai1[5]);
		PL2(W2U(wp1));
	ifree(wp1);
	ifree(ai1);
*/
/* (例) diff
	INT *ai1 = idate_diff(1970, 12, 10, 0, 0, 0, 2021, 4, 18, 0, 0, 0);
	WCS *wp1 = idate_format_diff(L"%g%y-%m-%d\t%Wweeks\t%Ddays\t%Sseconds", ai1[0], ai1[1], ai1[2], ai1[3], ai1[4], ai1[5], ai1[6], ai1[7]);
		PL2(W2U(wp1));
	ifree(wp1);
	ifree(ai1);
*/
// v2022-08-20
WCS
*idate_format_diff(
	WCS   *format, //
	INT   i_sign,  // 符号／-1 = "-", 1 = "+"
	INT   i_y,     // 年
	INT   i_m,     // 月
	INT   i_d,     // 日
	INT   i_h,     // 時
	INT   i_n,     // 分
	INT   i_s,     // 秒
	INT64 i_days   // 通算日／idate_diff()で使用
)
{
	if(! format)
	{
		return L"";
	}

	CONST UINT BufSizeMax = 512;
	CONST UINT BufSizeDmz = 64;
	WCS *rtn = icalloc_WCS(BufSizeMax + BufSizeDmz);
	WCS *pEnd = rtn;
	UINT uPos = 0;

	// Ymdhns で使用
	DOUBLE cjd = (i_days ? 0.0 : idate_ymdhnsToCjd(i_y, i_m, i_d, i_h, i_n, i_s));
	DOUBLE jd = idate_cjdToJd(cjd);

	// 符号チェック
	if(i_y < 0)
	{
		i_sign = -1;
		i_y = -(i_y);
	}
	while(*format)
	{
		if(*format == '%')
		{
			++format;
			switch(*format)
			{
				// Ymdhns
				case 'a': // 曜日(例:Su)
					pEnd += iwn_cpy(pEnd, idate_cjd_Wday(cjd));
					break;

				case 'A': // 曜日数
					pEnd += swprintf(pEnd, 12, L"%d", idate_cjd_iWday(cjd));
					break;

				case 'c': // 年内の通算日
					pEnd += swprintf(pEnd, 12, L"%d", idate_cjd_yeardays(cjd));
					break;

				case 'C': // CJD通算日
					pEnd += swprintf(pEnd, 12, CJD_FORMAT, cjd);
					break;

				case 'J': // JD通算日
					pEnd += swprintf(pEnd, 12, CJD_FORMAT, jd);
					break;

				case 'e': // 年内の通算週
					pEnd += swprintf(pEnd, 12, L"%d", idate_cjd_yearweeks(cjd));
					break;

				// Diff
				case 'Y': // 通算年
					pEnd += swprintf(pEnd, 12, L"%d", i_y);
					break;

				case 'M': // 通算月
					pEnd += swprintf(pEnd, 12, L"%d", (i_y * 12) + i_m);
					break;

				case 'D': // 通算日
					pEnd += swprintf(pEnd, 12, L"%lld", i_days);
					break;

				case 'H': // 通算時
					pEnd += swprintf(pEnd, 12, L"%lld", (i_days * 24) + i_h);
					break;

				case 'N': // 通算分
					pEnd += swprintf(pEnd, 12, L"%lld", (i_days * 24 * 60) + (i_h * 60) + i_n);
					break;

				case 'S': // 通算秒
					pEnd += swprintf(pEnd, 12, L"%lld", (i_days * 24 * 60 * 60) + (i_h * 60 * 60) + (i_n * 60) + i_s);
					break;

				case 'W': // 通算週
					pEnd += swprintf(pEnd, 12, L"%lld", (i_days / 7));
					break;

				case 'w': // 通算週の余日
					pEnd += swprintf(pEnd, 12, L"%d", (INT)(i_days % 7));
					break;

				// 共通
				case 'g': // Sign "-" "+"
					*pEnd = (i_sign < 0 ? '-' : '+');
					++pEnd;
					break;

				case 'G': // Sign "-" のみ
					if(i_sign < 0)
					{
						*pEnd = '-';
						++pEnd;
					}
					break;

				case 'y': // 年
					pEnd += swprintf(pEnd, 12, L"%04d", i_y);
					break;

				case 'm': // 月
					pEnd += swprintf(pEnd, 12, L"%02d", i_m);
					break;

				case 'd': // 日
					pEnd += swprintf(pEnd, 12, L"%02d", i_d);
					break;

				case 'h': // 時
					pEnd += swprintf(pEnd, 12, L"%02d", i_h);
					break;

				case 'n': // 分
					pEnd += swprintf(pEnd, 12, L"%02d", i_n);
					break;

				case 's': // 秒
					pEnd += swprintf(pEnd, 12, L"%02d", i_s);
					break;

				case '%':
					*pEnd = '%';
					++pEnd;
					break;

				default:
					--format; // else に処理を振る
					break;
			}
			uPos = pEnd - rtn;
		}
		else if(*format == '\\')
		{
			switch(format[1])
			{
				case('a'): *pEnd = '\a';      break;
				case('n'): *pEnd = '\n';      break;
				case('t'): *pEnd = '\t';      break;
				default:   *pEnd = format[1]; break;
			}
			++format;
			++pEnd;
			++uPos;
		}
		else
		{
			*pEnd = *format;
			++pEnd;
			++uPos;
		}
		++format;

		if(BufSizeMax < uPos)
		{
			break;
		}
	}
	return rtn;
}
/* (例)
	PL2(W2U(idate_format_cjdToW(NULL, idate_nowToCjd_localtime())));
*/
// v2022-09-02
WCS
*idate_format_cjdToW(
	WCS *format, // NULL=IDATE_FORMAT_STD
	DOUBLE cjd
)
{
	if(! format)
	{
		format = IDATE_FORMAT_STD;
	}
	INT *ai1 = idate_cjd_to_iAryYmdhns(cjd);
	WCS *rtn = idate_format_ymdhns(format, ai1[0], ai1[1], ai1[2], ai1[3], ai1[4], ai1[5]);
	ifree(ai1);
	return rtn;
}
//---------------------
// CJD を文字列に変換
//---------------------
/*
	大文字 => "yyyy-mm-dd 00:00:00"
	小文字 => "yyyy-mm-dd hh:nn:ss"
		Y, y : 年
		M, m : 月
		W, w : 週
		D, d : 日
		H, h : 時
		N, n : 分
		S, s : 秒
*/
/*
	"[-20d]"  "2015-12-10 00:25:00"
	"[-20D]"  "2015-12-10 00:00:00"
	"[-20d%]" "2015-12-10 %"
	"[]"      "2015-12-30 00:25:00"
	"[%]"     "2015-12-30 %"
	"[Err]"   ""
	"[Err%]"  ""
*/
/* (例)
	WCS *str = L"date>[-1m%] and date<[1M]";
	INT *ai1 = idate_now_to_iAryYmdhns_localtime();
	WCS *wp1 = idate_replace_format_ymdhns(
		str,
		L"[", L"]",
		L"'",
		ai1[0], ai1[1], ai1[2], ai1[3], ai1[4], ai1[5]
	);
	PL2(W2U(wp1));
	ifree(wp1);
	ifree(ai1);
*/
// v2022-08-20
WCS
*idate_replace_format_ymdhns(
	WCS *str,       // 変換対象文字列
	WCS *quoteBgn,  // 囲文字 1文字 (例) "["
	WCS *quoteEnd,  // 囲文字 1文字 (例) "]"
	WCS *add_quote, // 出力文字に追加するquote (例) "'"
	INT i_y,        // ベース年
	INT i_m,        // ベース月
	INT i_d,        // ベース日
	INT i_h,        // ベース時
	INT i_n,        // ベース分
	INT i_s         // ベース秒
)
{
	if(! str)
	{
		return 0;
	}
	WCS *wp1 = 0, *wp2 = 0, *wp3 = 0, *wp4 = 0, *wp5 = 0;
	UINT u1 = iwn_searchCnt(str, quoteBgn);
	WCS *rtn = icalloc_WCS(iwn_len(str) + (32 * u1));
	WCS *pEnd = rtn;

	// quoteBgn がないとき、pのクローンを返す
	if(! u1)
	{
		iwn_cpy(rtn, str);
		return rtn;
	}

	// add_quoteの除外文字
	WCS *pAddQuote = (
		(*add_quote >= '0' && *add_quote <= '9') || *add_quote == '+' || *add_quote == '-' ?
		L"" :
		add_quote
	);
	UINT uQuote1 = iwn_len(quoteBgn), uQuote2 = iwn_len(quoteEnd);
	INT add_y = 0, add_m = 0, add_d = 0, add_h = 0, add_n = 0, add_s = 0;
	INT cntPercent = 0;
	INT *ai = 0;
	BOOL bAdd = FALSE;
	BOOL flg = FALSE;
	BOOL zero = FALSE;
	WCS *ts1 = icalloc_WCS(iwn_len(str));

	// quoteBgn - quoteEnd を日付に変換
	wp1 = wp2 = str;
	while(*wp2)
	{
		// "[" を探す
		// str = "[[", quoteBgn = "["のときを想定しておく
		if(*quoteBgn && iwb_cmpf(wp2, quoteBgn) && ! iwb_cmpf(wp2 + uQuote1, quoteBgn))
		{
			bAdd = FALSE;   // 初期化
			wp2 += uQuote1; // quoteBgn.len
			wp1 = wp2;

			// "]" を探す
			if(*quoteEnd)
			{
				wp2 = iwp_searchL(wp1, quoteEnd);
				iwn_pcpy(ts1, wp1, wp2); // 解析用文字列

				// "[]" の中に数字が含まれているか
				u1 = 0;
				wp3 = ts1;
				while(*wp3)
				{
					if(*wp3 >= '0' && *wp3 <= '9')
					{
						u1 = 1;
						break;
					}
					++wp3;
				}

				// 例外
				wp3 = 0;
				switch((wp2 - wp1))
				{
					case(0): // "[]"
						wp3 = L"y";
						u1 = 1;
						break;

					case(1): // "[%]"
						if(*ts1 == '%')
						{
							wp3 = L"y%";
							u1 = 1;
						}
						break;

					default:
						wp3 = ts1;
						break;
				}
				if(u1)
				{
					zero = FALSE; // "00:00:00" かどうか
					u1 = (INT)inum_wtoi(wp3); // 引数から数字を抽出
					while(*wp3)
					{
						switch(*wp3)
						{
							case 'Y': // 年 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'y': // 年 => "yyyy-mm-dd hh:nn:ss"
								add_y = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'M': // 月 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'm': // 月 => "yyyy-mm-dd hh:nn:ss"
								add_m = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'W': // 週 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'w': // 週 => "yyyy-mm-dd hh:nn:ss"
								add_d = u1 * 7;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'D': // 日 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'd': // 日 => "yyyy-mm-dd hh:nn:ss"
								add_d = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'H': // 時 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'h': // 時 => "yyyy-mm-dd hh:nn:ss"
								add_h = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'N': // 分 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 'n': // 分 => "yyyy-mm-dd hh:nn:ss"
								add_n = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;

							case 'S': // 秒 => "yyyy-mm-dd 00:00:00"
								zero = TRUE;
								[[fallthrough]];
							case 's': // 秒 => "yyyy-mm-dd hh:nn:ss"
								add_s = u1;
								flg = TRUE;
								bAdd = TRUE;
								break;
						}
						// [1y6m] のようなとき [1y] で処理する
						if(flg)
						{
							break;
						}
						++wp3;
					}

					// 略記か日付か
					cntPercent = 0;
					if(bAdd)
					{
						// "Y-s" 末尾の "%" を検索
						while(*wp3)
						{
							if(*wp3 == '%')
							{
								++cntPercent;
							}
							++wp3;
						}

						// ±日時
						ai = idate_add(
							i_y, i_m, i_d, i_h, i_n, i_s,
							add_y, add_m, add_d, add_h, add_n, add_s
						);
						// その日時の 00:00 を求める
						if(zero)
						{
							ai[3] = ai[4] = ai[5] = 0;
						}

						// 初期化
						flg = FALSE;
						add_y = add_m = add_d = add_h = add_n = add_s = 0;
					}
					else
					{
						// "%" が含まれていれば「見なし」処理する
						wp4 = ts1;
						while(*wp4)
						{
							if(*wp4 == '%')
							{
								++cntPercent;
							}
							++wp4;
						}
						ai = idate_WCS_to_iAryYmdhns(ts1);
					}

					// bAddの処理を引き継ぎ
					wp5 = idate_format_ymdhns(
						IDATE_FORMAT_STD,
						ai[0], ai[1], ai[2], ai[3], ai[4], ai[5]
					);
						// "2015-12-30 00:00:00" => "2015-12-30 %"
						if(cntPercent)
						{
							wp4 = wp5;
							while(*wp4 != ' ')
							{
								++wp4;
							}
							*(++wp4) = '%'; // SQLiteの"%"を付与
							*(++wp4) = 0;
						}
						pEnd += iwn_cpy(pEnd, pAddQuote);
						pEnd += iwn_cpy(pEnd, wp5);
						pEnd += iwn_cpy(pEnd, pAddQuote);
					ifree(wp5);
					ifree(ai);
				}
				wp2 += (uQuote2); // quoteEnd.len
				wp1 = wp2;
			}
		}
		else
		{
			pEnd += iwn_pcpy(pEnd, wp2, wp2 + 1);
			++wp2;
		}
		add_y = add_m = add_d = add_h = add_n = add_s = 0;
	}
	ifree(ts1);
	return rtn;
}
//---------------------
// 今日のymdhnsを返す
//---------------------
/* (例)
	// 今日 = 2012-06-19 00:00:00 のとき、
	idate_now_to_iAryYmdhns(0); // System(-9h) => 2012, 6, 18, 15, 0, 0
	idate_now_to_iAryYmdhns(1); // Local       => 2012, 6, 19,  0, 0, 0
*/
// v2021-11-15
INT
*idate_now_to_iAryYmdhns(
	BOOL area // TRUE = LOCAL／FALSE = SYSTEM
)
{
	SYSTEMTIME st;
	if(area)
	{
		GetLocalTime(&st);
	}
	else
	{
		GetSystemTime(&st);
	}
	/* [Pending] 2021-11-15
		下記コードでビープ音を発生することがある。
			INT *rtn = icalloc_INT(n) ※INT系全般／DOUBL系は問題なし
			rtn[n] = 1793..2047
		2021-11-10にコンパイラを MInGW(32bit) から Mingw-w64(64bit) に変更した影響か？
		当面、様子を見る。
	*/
	INT *rtn = icalloc_INT(7);
		rtn[0] = st.wYear;
		rtn[1] = st.wMonth;
		rtn[2] = st.wDay;
		rtn[3] = st.wHour;
		rtn[4] = st.wMinute;
		rtn[5] = st.wSecond;
		rtn[6] = st.wMilliseconds;
	return rtn;
}
//------------------
// 今日のcjdを返す
//------------------
/* (例)
	idate_nowToCjd(0); // System(-9h)
	idate_nowToCjd(1); // Local
*/
// v2021-11-15
DOUBLE
idate_nowToCjd(
	BOOL area // TRUE = LOCAL／FALSE = SYSTEM
)
{
	INT *ai = idate_now_to_iAryYmdhns(area);
	INT y = ai[0], m = ai[1], d = ai[2], h = ai[3], n = ai[4], s = ai[5];
	ifree(ai);
	return idate_ymdhnsToCjd(y, m, d, h, n, s);
}
