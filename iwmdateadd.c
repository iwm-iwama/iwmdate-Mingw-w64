//------------------------------------------------------------------------------
#define  IWM_VERSION         "iwmdateadd_20220313"
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
	iExecSec_init();  //=> $ExecSecBgn
	iCLI_getARGV();   //=> $CMD, $ARGV, $ARGC
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

	INT *iAryDt    = icalloc_INT(6); // y, m, d, h, n, s
	INT *iAryDtAdd = icalloc_INT(6); // �}y, �}m, �}d, �}h, �}n, �}s

	// [0]
	/*
		"." "now" => ���ݎ�
	*/
	if(iCLI_getOptMatch(0, ".", "now"))
	{
		iAryDt = idate_now_to_iAryYmdhns_localtime();
	}
	else
	{
		iAryDt = idate_MBS_to_iAryYmdhns($ARGV[0]);
	}

	MBS *p1 = 0;

	// [1..]
	for(INT _i1 = 1; _i1 < $ARGC; _i1++)
	{
		// -y
		if((p1 = iCLI_getOptValue(_i1, "-y=", NULL)))
		{
			iAryDtAdd[0] += inum_atoi(p1);
		}

		// -m
		if((p1 = iCLI_getOptValue(_i1, "-m=", NULL)))
		{
			iAryDtAdd[1] += inum_atoi(p1);
		}

		// -d
		if((p1 = iCLI_getOptValue(_i1, "-d=", NULL)))
		{
			iAryDtAdd[2] += inum_atoi(p1);
		}

		// -w
		if((p1 = iCLI_getOptValue(_i1, "-w=", NULL)))
		{
			iAryDtAdd[2] += inum_atoi(p1) * 7;
		}

		// -h
		if((p1 = iCLI_getOptValue(_i1, "-h=", NULL)))
		{
			iAryDtAdd[3] += inum_atoi(p1);
		}

		// -n
		if((p1 = iCLI_getOptValue(_i1, "-n=", NULL)))
		{
			iAryDtAdd[4] += inum_atoi(p1);
		}

		// -s
		if((p1 = iCLI_getOptValue(_i1, "-s=", NULL)))
		{
			iAryDtAdd[5] += inum_atoi(p1);
		}

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

	iAryDt = idate_add(
		iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5],
		iAryDtAdd[0], iAryDtAdd[1], iAryDtAdd[2], iAryDtAdd[3], iAryDtAdd[4], iAryDtAdd[5]
	);

	P0(
		idate_format_ymdhns(
			_Format,
			iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5]
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
	P2("\033[48;2;80;80;250m �����̑O����v�Z \033[49m");
	NL();
	PRGB02();
	P ("\033[48;2;250;80;80m %s [Date] [Option] \033[49m\n\n", $CMD);
	PRGB11();
	P2(" (�g�p��)");
	PRGB91();
	P ("   %s \033[38;2;255;150;150m\"2000/1/1\" \033[38;2;150;150;255m-y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n", $CMD);
	PRGB02();
	P2("\033[48;2;250;80;80m [Date] \033[49m");
	PRGB91();
	P2("   \"now\"  \".\"  (���ݓ���)");
	P2("   \"+2000/01/01\"  \"+2000-01-01\"");
	P2("   \"+2000/01/01 00:00:00\"  \"+2000-01-01 00:00:00\"");
	NL();
	PRGB02();
	P2("\033[48;2;250;80;80m [Option] \033[49m");
	PRGB21();
	P2("   -y=[�}�N]  -m=[�}��]  -d=[�}��]  -w=[�}�T]");
	PRGB21();
	P2("   -h=[�}��]  -n=[�}��]  -s=[�}�b]");
	NL();
	PRGB21();
	P2("   -format=STR | -f=STR");
	PRGB91();
	P ("       ��STR�����w��̂Ƃ� \"%s\"\n", DATE_FORMAT);
	P2("       %g�F+/-�\\��");
	P2("       %y�F�N(0000)  %m�F��(00)  %d�F��(00)");
	P2("       %h�F��(00)  %n�F��(00)  %s�F�b(00)");
	P2("       %a�F�j��  %A�F�j����");
	P2("       %c�F�N�ʎZ��  %C�F�C�������E�X�ʎZ��  %J�F�����E�X�ʎZ��");
	P2("       %e�F�N�ʎZ�T");
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
