//------------------------------------------------------------------------------
#define  IWM_VERSION         "iwmdatediff_20220320"
#define  IWM_COPYRIGHT       "Copyright (C)2008-2022 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil.h"

INT  main();
VOID print_version();
VOID print_help();

// ���Z�b�g
#define  PRGB00()            P0("\033[0m")
// ���x��
#define  PRGB01()            P0("\033[38;2;255;255;0m")    // ��
#define  PRGB02()            P0("\033[38;2;255;255;255m")  // ��
// ���͗�^��
#define  PRGB11()            P0("\033[38;2;255;255;100m")  // ��
#define  PRGB12()            P0("\033[38;2;255;220;150m")  // ��
#define  PRGB13()            P0("\033[38;2;100;100;255m")  // ��
// �I�v�V����
#define  PRGB21()            P0("\033[38;2;80;255;255m")   // ��
#define  PRGB22()            P0("\033[38;2;255;100;255m")  // �g��
// �{��
#define  PRGB91()            P0("\033[38;2;255;255;255m")  // ��
#define  PRGB92()            P0("\033[38;2;200;200;200m")  // ��

#define  DATE_FORMAT         "%g%y-%m-%d" // (��)%g�t���Ȃ��ƑS�Đ����\��

#define  CJD                 "-4712-01-01 00:00:00"
#define  JD                  "-4712-01-01 12:00:00"

/*
	�o�̓t�H�[�}�b�g
	-f=STR | -format=STR
*/
MBS *_Format = DATE_FORMAT;
/*
	���s����Ƃ� TRUE
	-N
*/
BOOL _NL = TRUE;

INT
main()
{
	// lib_iwmutil ������
	iExecSec_init();       //=> $ExecSecBgn
	iCLI_getCommandLine(); //=> $CMD, $ARGC, $ARGV, $ARGS
	iConsole_EscOn();

	// -h | -help
	if(! $ARGC || iCLI_getOptMatch(0, "-h", "-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(iCLI_getOptMatch(0, "-v", "-version"))
	{
		print_version();
		imain_end();
	}

	INT *iAryDtBgn = icalloc_INT(6); // y, m, d, h, n, s
	INT *iAryDtEnd = icalloc_INT(6); // y, m, d, h, n, s

	// [0], [1]
	/*
		"." "now" => ���ݎ�
		"cjd"     => �C�������E�X�J�n��
		"jd"      => �����E�X�J�n��
	*/
	if(iCLI_getOptMatch(0, ".", "now"))
	{
		iAryDtBgn = idate_now_to_iAryYmdhns_localtime();
	}
	else if(iCLI_getOptMatch(0, "cjd", NULL))
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns(CJD);
	}
	else if(iCLI_getOptMatch(0, "jd", NULL))
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns(JD);
	}
	else
	{
		iAryDtBgn = idate_MBS_to_iAryYmdhns($ARGS[0]);
	}

	if(iCLI_getOptMatch(1, ".", "now"))
	{
		iAryDtEnd = idate_now_to_iAryYmdhns_localtime();
	}
	else if(iCLI_getOptMatch(1, "cjd", NULL))
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns(CJD);
	}
	else if(iCLI_getOptMatch(1, "jd", NULL))
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns(JD);
	}
	else
	{
		iAryDtEnd = idate_MBS_to_iAryYmdhns($ARGS[1]);
	}

	MBS *p1 = 0;

	// [2..]
	for(INT _i1 = 2; _i1 < $ARGC; _i1++)
	{
		// -f | -format
		if((p1 = iCLI_getOptValue(_i1, "-f=", "-format=")))
		{
			_Format = ims_clone(p1);
		}

		// -N
		if(iCLI_getOptMatch(_i1, "-N", NULL))
		{
			_NL = FALSE;
		}
	}

	// diff[8]
	INT *iAryDiff = idate_diff(
		iAryDtBgn[0], iAryDtBgn[1], iAryDtBgn[2], iAryDtBgn[3], iAryDtBgn[4], iAryDtBgn[5],
		iAryDtEnd[0], iAryDtEnd[1], iAryDtEnd[2], iAryDtEnd[3], iAryDtEnd[4], iAryDtEnd[5]
	);

	P0(
		idate_format_diff(
			_Format,
			iAryDiff[0], iAryDiff[1], iAryDiff[2], iAryDiff[3], iAryDiff[4], iAryDiff[5], iAryDiff[6], iAryDiff[7]
		)
	);

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
	PRGB92();
	LN();
	P (" %s\n", IWM_COPYRIGHT);
	P ("   Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
	PRGB00();
}

VOID
print_help()
{
	print_version();
	PRGB01();
	P2("\033[48;2;50;50;200m ���������v�Z \033[49m");
	NL();
	PRGB02();
	P ("\033[48;2;200;50;50m %s [Date1] [Date2] [Option] \033[49m\n\n", $CMD);
	PRGB11();
	P2(" (�g�p��)");
	PRGB91();
	P ("   %s \033[38;2;255;150;150mnow \"2000/01/01\" \033[38;2;150;150;255m-f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n\n", $CMD);
	PRGB02();
	P2("\033[48;2;200;50;50m [Date1] [Date2] \033[49m");
	PRGB91();
	P2("   now  .  (���ݓ���)");
	P2("   cjd     (�C�������E�X�J�n�� -4712/01/01 00:00:00)");
	P2("   jd      (�����E�X�J�n��     -4712/01/01 12:00:00)");
	P2("   \"+2000/01/01\"  \"+2000-01-01\"");
	P2("   \"+2000/01/01 00:00:00\"  \"+2000-01-01 00:00:00\"");
	NL();
	PRGB02();
	P2("\033[48;2;200;50;50m [Option] \033[49m");
	PRGB21();
	P2("   -format=STR | -f=STR");
	PRGB91();
	P ("       ��STR�����w��̂Ƃ� \"%s\"\n", DATE_FORMAT);
	P2("       %g�F+/-�\\��");
	P2("       %y�F�N  %m�F��  %d�F��  %h�F��  %n�F��  %s�F�b");
	P2("       �ʎZ  %Y�F�N  %M�F��  %D�F��");
	P2("             %H�F��  %N�F��  %S�F�b");
	P2("             %W�F�T  %w�F�T�]��");
	P2("       \\t�F�^�u  \\n�F���s");
	PRGB21();
	P2("   -N");
	PRGB91();
	P2("       ���s���Ȃ�");
	NL();
	PRGB11();
	P2(" (���l)");
	PRGB91();
	P2("   �E�����E�X�� �i-4712/01/01�`1582/10/04�j");
	P2("   �E�O���S���I��i1582/10/15�`9999/12/31�j");
	PRGB12();
	P0("    (���P) ");
	PRGB91();
	P2("�󔒗� 1582/10/5�`1582/10/14 �́A\"1582/10/4\" �Ƃ��Ď戵���B");
	PRGB12();
	P0("    (���Q) ");
	PRGB91();
	P2("BC��́A\"-1/1/1\" �� \"0/1/1\" �Ƃ��Ď戵���B");
	PRGB12();
	P0("    (���R) ");
	PRGB91();
	P2("�v���O������́A�C�������E�X����g�p�B");
	NL();
	PRGB92();
	LN();
	PRGB00();
}
