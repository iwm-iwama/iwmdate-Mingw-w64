//------------------------------------------------------
#define IWMDATEDIFF_VERSION "iwmdatediff_20170109"
#define IWMDATEDIFF_COPYRIGHT "(C)2008-2017 iwm-iwama"
//------------------------------------------------------
#include "lib_iwmutil.h"

INT *getAryTime(MBS *ymd,INT *localtime);
VOID version();
VOID help();

MBS *_program=0;

#define DATE_FORMAT "%g%y-%m-%d"

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

	INT *localtime=idate_now_to_iAryYmdhns_localtime();

	// ymd1
	MBS *_ymd1=*(args+0);
	INT *ai1=getAryTime(_ymd1,localtime);

	// ymd2
	MBS *_ymd2=*(args+1);
	INT *ai2=getAryTime(_ymd2,localtime);

	// diff[8]
	INT *iAryDiff=idate_diff(*(ai1+0),*(ai1+1),*(ai1+2),*(ai1+3),*(ai1+4),*(ai1+5),*(ai2+0),*(ai2+1),*(ai2+2),*(ai2+3),*(ai2+4),*(ai2+5));

	// -format
	MBS *_format=0;
	ap1=iargs_option(args,"-format","-f");
		_format=($IWM_bSuccess ?
			ims_clone(*ap1) :
			DATE_FORMAT
		);
	ifree(ap1);

	P("%s",idate_format_diff(_format,iAryDiff[0],iAryDiff[1],iAryDiff[2],iAryDiff[3],iAryDiff[4],iAryDiff[5],iAryDiff[6],iAryDiff[7]));

	// -N 改行しない
	ap1=iargs_option(args,"-N",NULL);
		if(!$IWM_bSuccess){
			NL();
		}
	ifree(ap1);

	imain_end();
}

INT
*getAryTime(
	MBS *ymd,
	INT *localtime
){
	INT *rtn=0;
	if(imb_cmppi(ymd,"now") || *ymd=='.'){
		rtn=icalloc_INT(6);
		INT i1=0;
		for(i1=0;i1<6;i1++) *(rtn+i1)=*(localtime+i1);
	}
	else{
		rtn=idate_MBS_to_iAryYmdhns(ymd);
	}
	return rtn;
}

VOID
version(){
	LN();
	P2(IWMDATEDIFF_COPYRIGHT);
	P ("  Ver.%s+%s\n",IWMDATEDIFF_VERSION,LIB_IWMUTIL_VERSION);
}

VOID
help(){
	version();
	LN();
	P2("＜使用法＞");
	P ("  %s [日付1] [日付2] [オプション] \n",_program);
	NL();
	P2("＜日付＞");
	P2("  \".\" \"now\" => 現在日時");
	P2("  \"2008/12/10\" \"2008-12-10\"");
	NL();
	P2("＜オプション＞");
	P2("  -format | -f STR");
	P ("      ※STRが無指定のとき、\"%s\"と同義\n",DATE_FORMAT);
	P2("      %g:+/-表\示");
	P2("      %y:年  %m:月  %d:日  %h:時  %n:分  %s:秒");
	P2("      通算(満)  %M:月  %D:日  %H:時  %N:分  %S:秒  %W:週  %w:週余日");
	P2("      \\t:タブ  \\n:改行");
	P2("  -N");
	P2("      改行しない.");
	NL();
	P2("＜使用例＞");
	P ("  %s 2000/1/1 2008/12/10 -f \"%%D通算日 %%W通算週%%w日\"\n",_program);
	NL();
	P2("＜備考＞");
	P2("  ユリウス暦（-4712/01/01～1582/10/04）、");
	P2("  グレゴリオ暦（1582/10/15～9999/12/31）に対応。");
	P2("  (注1) 空白暦（1582/10/5～1582/10/14）は、\"1582/10/4\" として取扱う。");
	P2("  (注2) BC暦は、\"-1/1/1\" を \"0/1/1\" として取扱う。");
	P2("  (注3) プログラム上は、\"JD通日でなくCJD通日\" を使用。");
	LN();
}
