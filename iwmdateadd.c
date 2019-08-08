//-----------------------------------------------------
#define IWMDATEADD_VERSION   "iwmdateadd_20170109"
#define IWMDATEADD_COPYRIGHT "(C)2008-2017 iwm-iwama"
//-----------------------------------------------------
#include "lib_iwmutil.h"

VOID version();
VOID help();

MBS *_program=0;
MBS *_option[6]={"-y","-m","-d","-h","-n","-s"};

#define DATE_FORMAT "%g%y-%m-%d" // (注)%g付けないと全て正数表示

INT
main(){
	_program=iCmdline_getCmd();
	MBS **args=iCmdline_getArgs();
	MBS **ap1={0};

	// help
	//  オプションなしのとき表示
	//  "-h" <= hour なので使わない
	if(!**(args+0)){
		help();
		imain_end();
	}

	// version
	ap1=iargs_option(args,"-version","-v");
		if($IWM_bSuccess){
			version();
			LN();
			imain_end();
		}
	ifree(ap1);

	INT i1=0;

	// _ymd
	INT *iAryAdd=0;// y,m,d,h,n,s
	MBS *_ymd=*(args);
		iAryAdd=(imb_cmppi(_ymd,"now") || *_ymd=='.' ?
			idate_now_to_iAryYmdhns_localtime() :
			idate_MBS_to_iAryYmdhns(_ymd)
		);

	// ±時間
	INT *iAryAddCnt=icalloc_INT(6);// ±y,±m,±d,±h,±n,±s
	for(i1=0;i1<6;i1++){
		ap1=iargs_option(args,*(_option+i1),NULL);
			if($IWM_bSuccess){
				*(iAryAddCnt+i1)=atoi(*ap1);
			}
		ifree(ap1);
	}
	ap1=iargs_option(args,"-w",NULL);
		if($IWM_bSuccess){
			*(iAryAddCnt+2)+=(atoi(*ap1)*7);
		}
	ifree(ap1);

	// add
	iAryAdd=idate_add(
		*(iAryAdd+0),*(iAryAdd+1),*(iAryAdd+2),*(iAryAdd+3),*(iAryAdd+4),*(iAryAdd+5),
		*(iAryAddCnt+0),*(iAryAddCnt+1),*(iAryAddCnt+2),*(iAryAddCnt+3),*(iAryAddCnt+4),*(iAryAddCnt+5)
	);

	// -format
	MBS *_format=0;
	ap1=iargs_option(args,"-format","-f");
		_format=($IWM_bSuccess ?
			ims_clone(*ap1) :
			DATE_FORMAT
		);
	ifree(ap1);

	// 表示
	P(
		idate_format_ymdhns(
			_format,
			*(iAryAdd+0),*(iAryAdd+1),*(iAryAdd+2),*(iAryAdd+3),*(iAryAdd+4),*(iAryAdd+5)
		)
	);

	// -N 改行しない
	ap1=iargs_option(args,"-N",NULL);
		if(!$IWM_bSuccess){
			NL();
		}
	ifree(ap1);

	imain_end();
}

VOID
version(){
	LN();
	P2(IWMDATEADD_COPYRIGHT);
	P ("  Ver.%s+%s\n",IWMDATEADD_VERSION,LIB_IWMUTIL_VERSION);
}

VOID
help(){
	version();
	LN();
	P2("＜使用法＞");
	P ("  %s [日付] [オプション]\n",_program);
	NL();
	P2("＜日付＞");
	P2("  \".\" \"now\" => 現在日時");
	P2("  \"+2008/12/10\" \"+2008-12-10\"");
	P2("  \"+2008/12/10 00:00:00\" \"+2008-12-10 00:00:00\"");
	NL();
	P2("＜オプション＞");
	P2("  -y [±年] -m [±月] -d [±日] -w [±週] -h [±時] -n [±分] -s [±秒]");
	NL();
	P2("  -format | -f STR");
	P ("      ※STRが無指定のとき、\"%s\"と同義\n",DATE_FORMAT);
	P2("      %g:+/-表\示");
	P2("      %y:年(0000)  %Y:年(下2桁)  %m:月(00)  %d:日(00)");
	P2("      %h:時(00)  %n:分(00)  %s:秒(00)");
	P2("      %a:曜日  %A:曜日数");
	P2("      %c:年通算日  %C:CJD通算日");
	P2("      %e:年通算週  %E:CJD通算週");
	P2("      \\t:タブ  \\n:改行");
	P2("  -N");
	P2("      改行しない.");
	NL();
	P2("＜使用例＞");
	P ("  %s 2000/1/1 -y 8 -m 11 -d 9 -f \"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n",_program);
	NL();
	P2("＜備考＞");
	P2("  ユリウス暦（-4712/01/01～1582/10/04）、");
	P2("  グレゴリオ暦（1582/10/15～9999/12/31）に対応。");
	P2("  (注1) 空白暦（1582/10/5～1582/10/14）は、\"1582/10/4\" として取扱う。");
	P2("  (注2) BC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。");
	P2("  (注3) プログラム上は、\"JD通日でなくCJD通日\" を使用。");
	LN();
}
