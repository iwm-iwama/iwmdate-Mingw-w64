//------------------------------------------------------------------------------
#define  IWM_VERSION         "iwmdatediff_20211129"
#define  IWM_COPYRIGHT       "Copyright (C)2008-2021 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil.h"

INT  main();
VOID print_version();
VOID print_help();

// [�����F] + ([�w�i�F] * 16)
//  0 = Black    1 = Navy     2 = Green    3 = Teal
//  4 = Maroon   5 = Purple   6 = Olive    7 = Silver
//  8 = Gray     9 = Blue    10 = Lime    11 = Aqua
// 12 = Red     13 = Fuchsia 14 = Yellow  15 = White

// �^�C�g��
#define  COLOR01             (15 + ( 9 * 16))
// ���͗�^��
#define  COLOR11             (15 + (12 * 16))
#define  COLOR12             (13 + ( 0 * 16))
#define  COLOR13             (12 + ( 0 * 16))
// ����
#define  COLOR21             (14 + ( 0 * 16))
#define  COLOR22             (11 + ( 0 * 16))
// ����
#define  COLOR91             (15 + ( 0 * 16))
#define  COLOR92             ( 7 + ( 0 * 16))

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
	iCLI_getARGV();      //=> $CMD, $ARGV, $ARGC
	iConsole_getColor(); //=> $ColorDefault, $StdoutHandle
	iExecSec_init();     //=> $ExecSecBgn

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
		iAryDtBgn = idate_MBS_to_iAryYmdhns($ARGV[0]);
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
		iAryDtEnd = idate_MBS_to_iAryYmdhns($ARGV[1]);
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
	PZ(COLOR92, NULL);
	LN();
	P(" %s\n", IWM_COPYRIGHT);
	P("   Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN();
	PZ(-1, NULL);
}

VOID
print_help()
{
	print_version();
	PZ(COLOR01, " ���������v�Z \n\n");
	PZ(COLOR11, " %s [���t1] [���t2] [�I�v�V����] \n\n", $CMD);
	PZ(COLOR12, " (�g�p��)\n");
	PZ(COLOR91, "   %s \"now\" \"2000/01/01\" -f=\"%%g%%y-%%m-%%d %%h:%%n:%%s\"\n\n", $CMD);
	PZ(COLOR21, " [���t1] [���t2]\n");
	PZ(COLOR91, NULL);
	P2("   \"now\" \".\" (���ݓ���)");
	P2("   \"cjd\"     (�C�������E�X�J�n�� -4712/01/01 00:00:00)");
	P2("   \"jd\"      (�����E�X�J�n��     -4712/01/01 12:00:00)");
	P2("   \"+2000/01/01\" \"+2000-01-01\"");
	P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"\n");
	PZ(COLOR21, " [�I�v�V����]\n");
	PZ(COLOR22, "   -format=STR | -f=STR\n");
	PZ(COLOR91, NULL);
	P("       ��STR�����w��̂Ƃ� \"%s\"\n", DATE_FORMAT);
	P2("       %g : +/-�\\��");
	P2("       %y : �N  %m : ��  %d : ��  %h : ��  %n : ��  %s : �b");
	P2("       �ʎZ  %Y : �N  %M : ��  %D : ��");
	P2("             %H : ��  %N : ��  %S : �b");
	P2("             %W : �T  %w : �T�]��");
	P2("       \\t : �^�u  \\n : ���s");
	PZ(COLOR22, "   -N\n");
	PZ(COLOR91, "       ���s���Ȃ�\n\n");
	PZ(COLOR12, " (���l)\n");
	PZ(COLOR91, NULL);
	P2("   �E�����E�X�� �i-4712/01/01�`1582/10/04�j");
	P2("   �E�O���S���I��i1582/10/15�`9999/12/31�j");
	PZ(COLOR13, "    (���P) ");
	PZ(COLOR91, "�󔒗� 1582/10/5�`1582/10/14 �́A\"1582/10/4\" �Ƃ��Ď戵���B\n");
	PZ(COLOR13, "    (���Q) ");
	PZ(COLOR91, "BC��́A\"-1/1/1\" �� \"0/1/1\" �Ƃ��Ď戵���B\n");
	PZ(COLOR13, "    (���R) ");
	PZ(COLOR91, "�v���O������́A�C�������E�X����g�p�B\n\n");
	PZ(COLOR92, NULL);
	LN();
	PZ(-1, NULL);
}
