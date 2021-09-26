//------------------------------------------------------------------------------
#define  IWM_VERSION         "iwmdateadd_20210924"
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
	iCLI_getARGV();      //=> $IWM_CMD, $IWM_ARGV, $IWM_ARGC
	iConsole_getColor(); //=> $IWM_ColorDefault, $IWM_StdoutHandle
	iExecSec_init();     //=> $IWM_ExecSecBgn

	// -h | -help
	if(! $IWM_ARGC || imb_cmpp($IWM_ARGV[0], "-h") || imb_cmpp($IWM_ARGV[0], "-help"))
	{
		print_help();
		imain_end();
	}

	// -v | -version
	if(imb_cmpp($IWM_ARGV[0], "-v") || imb_cmpp($IWM_ARGV[0], "-version"))
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
	if(imb_cmpp($IWM_ARGV[0], ".") || imb_cmpp($IWM_ARGV[0], "now"))
	{
		iAryDt = idate_now_to_iAryYmdhns_localtime();
	}
	else
	{
		iAryDt = idate_MBS_to_iAryYmdhns($IWM_ARGV[0]);
	}

	// [1..]
	for(INT _i1 = 1; _i1 < $IWM_ARGC; _i1++)
	{
		MBS **_as1 = ija_split($IWM_ARGV[_i1], "=");

		// -y
		if(imb_cmpp(_as1[0], "-y"))
		{
			iAryDtAdd[0] += inum_atoi(_as1[1]);
		}

		// -m
		if(imb_cmpp(_as1[0], "-m"))
		{
			iAryDtAdd[1] += inum_atoi(_as1[1]);
		}

		// -d
		if(imb_cmpp(_as1[0], "-d"))
		{
			iAryDtAdd[2] += inum_atoi(_as1[1]);
		}

		// -w
		if(imb_cmpp(_as1[0], "-w"))
		{
			iAryDtAdd[2] += inum_atoi(_as1[1]) * 7;
		}

		// -h
		if(imb_cmpp(_as1[0], "-h"))
		{
			iAryDtAdd[3] += inum_atoi(_as1[1]);
		}

		// -n
		if(imb_cmpp(_as1[0], "-n"))
		{
			iAryDtAdd[4] += inum_atoi(_as1[1]);
		}

		// -s
		if(imb_cmpp(_as1[0], "-s"))
		{
			iAryDtAdd[5] += inum_atoi(_as1[1]);
		}

		// -f | -format
		if(imb_cmpp(_as1[0], "-f") || imb_cmpp(_as1[0], "-format"))
		{
			_Format = ims_clone(_as1[1]);
		}

		// -N
		if(imb_cmpp(_as1[0], "-N"))
		{
			_NL = FALSE;
		}

		ifree(_as1);
	}

	iAryDt = idate_add(
		iAryDt[0], iAryDt[1], iAryDt[2], iAryDt[3], iAryDt[4], iAryDt[5],
		iAryDtAdd[0], iAryDtAdd[1], iAryDtAdd[2], iAryDtAdd[3], iAryDtAdd[4], iAryDtAdd[5]
	);

	P(
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
	PZ(COLOR01, " �����̑O����v�Z \n\n");
	PZ(COLOR11, " %s [���t] [�I�v�V����] \n\n", $IWM_CMD);
	PZ(COLOR12, " (�g�p��)\n");
	PZ(COLOR91, "   %s \"2000/1/1\" -y=8 -m=11 -d=9 -f=\"%%g%%y-%%m-%%d(%%a) %%h:%%n:%%s\"\n\n", $IWM_CMD);
	PZ(COLOR21, " [���t]\n");
	PZ(COLOR91, NULL);
	P2("   \"now\" \".\" (���ݓ���)");
	P2("   \"+2000/01/01\" \"+2000-01-01\"");
	P2("   \"+2000/01/01 00:00:00\" \"+2000-01-01 00:00:00\"\n");
	PZ(COLOR21, " [�I�v�V����]\n");
	PZ(COLOR22, NULL);
	P2("   -y=[�}�N] -m=[�}��] -d=[�}��] -w=[�}�T] -h=[�}��] -n=[�}��] -s=[�}�b]\n");
	P2("   -format=STR | -f=STR");
	PZ(COLOR91, NULL);
	P("       ��STR�����w��̂Ƃ� \"%s\"\n", DATE_FORMAT);
	P2("       %g : +/-�\\��");
	P2("       %y : �N(0000)  %m : ��(00)  %d : ��(00)");
	P2("       %h : ��(00)  %n : ��(00)  %s : �b(00)");
	P2("       %a : �j��  %A : �j����");
	P2("       %c : �N�ʎZ��  %C : �C�������E�X�ʎZ��  %J : �����E�X�ʎZ��");
	P2("       %e : �N�ʎZ�T");
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
