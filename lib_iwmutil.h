/////////////////////////////////////////////////////////////////////////////////////////
#define  LIB_IWMUTIL_VERSION   "lib_iwmutil_20191121"
#define  LIB_IWMUTIL_COPYLIGHT "Copyright (C)2008-2019 iwm-iwama"
/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	�T���v��
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/*
#include "lib_iwmutil.h"

//-----------
// ���L�ϐ�
//-----------
UINT $execMS   = 0;
MBS  *$program = NULL;
MBS  **$args   = {NULL};
UINT $argsSize = 0;

INT
main()
{
	// ���s�J�n����
	$execMS = iExecSec_init();

	// �R�}���h���^����
	$program = iCmdline_getCmd();
	$args = iCmdline_getArgs();
	$argsSize = $IWM_uAryUsed;

	// ����������

	// (��) test.exe -size=640,480 -text1="where name like 'who?'" -text2="����������","��,��,��,��,��","'��'��'��'��'��'"

	MBS **as1 = {NULL};
	MBS **as2 = {NULL};

	P2($program);

	for(INT _i1 = 0; _i1 < $argsSize; _i1++)
	{
		NL();
		P2($args[_i1]);	

		as1 = ija_split($args[_i1], "=", "", FALSE);
		INT _i11 = $IWM_uAryUsed;

		MBS *sLabel = as1[0];

		for(INT _i2 = 1; _i2 < _i11; _i2++)
		{
			as2 = ija_split(as1[_i2], ",", "\"\"\'\'", TRUE);

			// "-size"
			if(imb_cmpp(sLabel, "-size"))
			{
				P("0> %s\n", as2[0]);
				P("1> %s\n", as2[1]);
			}

			// "-text1"
			if(imb_cmpp(sLabel, "-text1"))
			{
				P("0> %s\n", as2[0]);
			}

			// "-text2"
			if(imb_cmpp(sLabel, "-text2"))
			{
				P("0> %s\n", as2[0]);
				P("1> %s\n", as2[1]);
				P("2> %s\n", as2[2]);
			}

			ifree(as2);
		}
		ifree(as1);
	}

	NL();

	// �������܂�

	// �f�o�b�O
	icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

	// ���s����
	P("(+%.3fsec)\n\n", iExecSec_next($execMS));
	imain_end();
}
*/

/////////////////////////////////////////////////////////////////////////////////////////
#include <conio.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <shlwapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	���ʒ萔
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
#define  IMAX_PATHW                              (UINT)(MAX_PATH+2) // windef.h�Q��
#define  IMAX_PATHA                              (UINT)(2*IMAX_PATHW)
#define  IGET_ARGS_LEN_MAX                       2048
#define  IVA_LIST_MAX                            64       // va_xxx()�̏���l

#define  MBS                                     CHAR     // imx_xxx()�^MBCS
#define  WCS                                     WCHAR    // iwx_xxx()�^UTF-16
#define  U8N                                     CHAR     // iux_xxx()�^UTF-8

#define  INT64                                   LONGLONG // %I64d

#define  NULL_DEVICE                             "NUL"    // "NUL" = "/dev/null"

#define  EOD                                     NULL

#define  ISO_FORMAT_DATE                         "%.4d-%02d-%02d" // "yyyy-mm-dd"
#define  ISO_FORMAT_TIME                         "%02d:%02d:%02d" // "hh:nn:ss"
#define  ISO_FORMAT_DATETIME                     "%.4d-%02d-%02d %02d:%02d:%02d"

#define  IDATE_FORMAT_STD                        "%G%y-%m-%d %h:%n:%s"
#define  IDATE_FORMAT_DIFF                       "%g%y-%m-%d %h:%n:%s"

#define  ceil8(n)                                (UINT)((((n)>>3)+2)<<3)

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	���s����̊֐�����̕Ԓl�i���F�֐����őΉ����Ă��邱�Ɓj
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
BOOL     $IWM_bSuccess;      // ��������=TRUE�^�����Ώۂ��s�݂Ȃ�=FALSE
UINT     $IWM_uAryUsed;      // �Q�����z��̗v�f��
UINT     $IWM_uWords;        // �������^MBS,WCS�����

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	���s�J�n����
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
UINT     iExecSec(CONST UINT microSec);
#define  iExecSec_init()                         (UINT)iExecSec(0)
#define  iExecSec_next(microSec)                 (DOUBLE)(iExecSec(microSec))/1000

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	�������m��
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
VOID     *icalloc(UINT n,UINT size,BOOL aryOn);
VOID     *irealloc(VOID *ptr,UINT n,UINT size);

/*
	MBS�^[] = [0...] + NULL
	(��) MBS[]=[0]"ABC",[1]"DEF",[2]NULL
*/
#define  icalloc_MBS(n)                          (MBS*)icalloc(n,sizeof(MBS),FALSE)
#define  irealloc_MBS(pM,n)                      (MBS*)irealloc(pM,n,sizeof(MBS))

#define  icalloc_MBS_ary(n)                      (MBS**)icalloc(n,sizeof(MBS*),TRUE)
#define  irealloc_MBS_ary(pM,n)                  (MBS**)irealloc(pM,n,sizeof(MBS*))

#define  icalloc_WCS(n)                          (WCS*)icalloc(n,sizeof(WCS),FALSE)
#define  irealloc_WCS(pW,n)                      (WCS*)irealloc(pW,n,sizeof(WCS))

#define  icalloc_INT(n)                          (INT*)icalloc(n,sizeof(INT),FALSE)
#define  irealloc_INT(ptr,n)                     (INT*)irealloc(ptr,n,sizeof(INT))

#define  icalloc_UINT(n)                         (UINT*)icalloc(n,sizeof(UINT),FALSE)
#define  irealloc_UINT(ptr,n)                    (UINT*)irealloc(ptr,n,sizeof(UINT))

#define  icalloc_INT64(n)                        (INT64*)icalloc(n,sizeof(INT64),FALSE)
#define  irealloc_INT64(ptr,n)                   (INT64*)irealloc(ptr,n,sizeof(INT64))

#define  icallocDBL(n)                           (DOUBLE*)icalloc(n,sizeof(DOUBLE),FALSE)
#define  ireallocDBL(ptr,n)                      (DOUBLE*)irealloc(ptr,n,sizeof(DOUBLE))

VOID     icalloc_err(VOID *ptr);

VOID     icalloc_free(VOID *ptr);
VOID     icalloc_freeAll();
VOID     icalloc_mapSweep();

#define  ifree(ptr)                              (VOID)icalloc_free(ptr);(VOID)icalloc_mapSweep();
#define  ifree_all()                             (VOID)icalloc_freeAll()
#define  imain_end()                             ifree_all();exit(EXIT_SUCCESS)

VOID     icalloc_mapPrint1();
VOID     icalloc_mapPrint2();
#define  icalloc_mapPrint()                      P8();icalloc_mapPrint1();icalloc_mapPrint2()

#define  ierr_end(msg)                           P("Err: %s\n",msg);imain_end()

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	printf()�n
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
VOID     P(CONST MBS *format,...);
VOID     PR(MBS *pM,INT repeat);
VOID     P20B(MBS *pM);
VOID     P20X(MBS *pM);
VOID     P20XW(WCS *pW);

#define  PC(pM)                                  putchar(*pM)
#define  PP(pM)                                  P("[%p] ",pM)
#define  PX(pM)                                  P("|%#hx|",*pM)
#define  NL()                                    putchar('\n')
#define  LN()                                    PR("-",72);NL()

#define  P20(pM)                                 P("%s",pM)
#define  P30(num)                                P("%I64d",(INT64)num)
#define  P40(num)                                P("%.8f",(DOUBLE)num)
#define  P80()                                   P("L%4u: ",__LINE__)

#define  P2(pM)                                  P20(pM);NL()
#define  P3(num)                                 P30(num);NL()
#define  P4(num)                                 P40(num);NL()
#define  P8()                                    P80();NL()
#define  P9(repeat)                              PR("\n",repeat)

#define  P820(pM)                                P80();P20(pM)
#define  P830(num)                               P80();P30(num)
#define  P840(num)                               P80();P40(num)

#define  P82(pM)                                 P80();P2(pM)
#define  P83(num)                                P80();P3(num)
#define  P84(num)                                P80();P4(num)

#define  P22(pM1,pM2)                            P("\"%s\"\t\"%s\"\n",pM1,pM2)
#define  P23(pM,num)                             P("%s%I64d\n",pM,(INT64)num)
#define  P24(pM,num)                             P("%s%f\n",pM,(DOUBLE)num)

#define  P32(num,pM)                             P("(%I64d)\t\"%s\"\n",(INT64)num,pM)
#define  P33(num1,num2)                          P("(%I64d)\t(%I64d)\n",(INT64)num1,(INT64)num2)
#define  P34(num1,num2)                          P("(%I64d)\t(%.8f)\n",(INT64)num1,(DOUBLE)num2)
#define  P44(num1,num2)                          P("(%.8f)\t(%.8f)\n",(DOUBLE)num1,(DOUBLE)num2)

#define  P822(pM1,pM2)                           P80();P22(pM1,pM2)
#define  P823(pM,num)                            P80();P23(pM,num)
#define  P824(pM,num)                            P80();P24(pM,num)
#define  P832(num,pM)                            P80();P32(num,pM)
#define  P833(num1,num2)                         P80();P33(num1,num2)
#define  P834(num1,num2)                         P80();P34(num1,num2)
#define  P844(num1,num2)                         P80();P44(num1,num2)

#define  P2B1(pM)                                P20B(pM);NL()
#define  P2B2(repeat,pM)                         PR(" ",repeat);P20B(pM);NL()
#define  P82B1(pM)                               P80();P2B1(pM)
#define  P82B2(repeat,pM)                        P80();P2B2(repeat,pM)

#define  P82X(pM)                                P80();P20X(pM);NL()
#define  P82XW(pW)                               P80();P20XW(pW);NL()

MBS      *ims_conv_escape(MBS *pM);
MBS      *ims_sprintf(FILE *oFp,MBS *format,...);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	MBS�^WCS�^U8N�ϊ�
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
WCS      *icnv_M2W(MBS *pM);
#define  M2W(pM)                                 (WCS*)icnv_M2W(pM)

U8N      *icnv_W2U(WCS *pW);
#define  W2U(pW)                                 (U8N*)icnv_W2U(pW)

U8N      *icnv_M2U(MBS *pM);
#define  M2U(pM)                                 (U8N*)icnv_M2U(pM)

WCS      *icnv_U2W(U8N *pU);
#define  U2W(pU)                                 (WCS*)icnv_U2W(pU)

MBS      *icnv_W2M(WCS *pW);
#define  W2M(pW)                                 (MBS*)icnv_W2M(pW)

MBS      *icnv_U2M(U8N *pU);
#define  U2M(pU)                                 (MBS*)icnv_U2M(pU)

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	�����񏈗�
		p : return Pointer
		s : return String
		1byte     MBS : imp_xxx(), imp_xxx()
		1 & 2byte MBS : ijp_xxx(), ijs_xxx()
		UTF-8     U8N : iup_xxx(), ius_xxx()
		UTF-16    WCS : iwp_xxx(), iws_xxx()
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
UINT     imi_len(MBS *pM);
UINT     iji_len(MBS *pM);
UINT     iui_len(U8N *pU);
UINT     iwi_len(WCS *pW);

MBS      *imp_forwardN(MBS *pM,UINT sizeM);
MBS      *ijp_forwardN(MBS *pM,UINT sizeJ);
U8N      *iup_forwardN(U8N *pU,UINT sizeU);

MBS      *imp_sod(MBS *pM);
#define  imp_eod(pM)                             (MBS*)(pM+imi_len(pM))
WCS      *iwp_sod(WCS *pW);
#define  iwp_eod(pW)                             (WCS*)(pW+iwi_len(pW))

MBS      *ims_upper(MBS *pM);
MBS      *ims_lower(MBS *pM);
WCS      *iws_upper(WCS *pW);
WCS      *iws_lower(WCS *pW);

UINT     iji_plen(MBS *pBgn,MBS *pEnd);

MBS      *imp_cpy(MBS *to,MBS *from);
WCS      *iwp_cpy(WCS *to,WCS *from);
#define  ijp_cpy(to,from)                        (MBS*)imp_pcpy(to,from,CharNextA(from))

MBS      *imp_pcpy(MBS *to,MBS *from1,MBS *from2);
WCS      *iwp_pcpy(WCS *to,WCS *from1,WCS *from2);

#define  imp_ncpy(to,from,size)                  (MBS*)imp_pcpy(to,from,from+size)
#define  iwp_ncpy(to,from,size)                  (WCS*)iwp_pcpy(to,from,from+size)
#define  ijp_ncpy(to,from,sizeJ)                 (MBS*)imp_pcpy(to,from,ijp_forwardN(from,sizeJ))

MBS      *ims_clone(MBS *from);
WCS      *iws_clone(WCS *from);

MBS      *ims_pclone(MBS *from1,MBS *from2);
WCS      *iws_pclone(WCS *from1,WCS *from2);

#define  ims_nclone(from1,size)                  (MBS*)ims_pclone(from1,(from1+size))
#define  iws_nclone(from1,size)                  (WCS*)iws_pclone(from1,(from1+size))

MBS      *ims_cat_pclone(MBS *to,MBS *from1,MBS *from2);
WCS      *iws_cat_pclone(WCS *to,WCS *from1,WCS *from2);

MBS      *ims_cat_clone3(MBS *from1,MBS *from2,MBS *from3,MBS *from4);
#define  ims_cat_clone(from1,from2)              (MBS*)ims_cat_clone3(from1,from2,NULL,NULL)
WCS      *iws_cat_clone3(WCS *from1,WCS *from2,WCS *from3,WCS *from4);
#define  iws_cat_clone(from1,from2)              (WCS*)iws_cat_clone3(from1,from2,NULL,NULL)

MBS      *ims_ncat_clone(MBS *pM,...);

MBS      *ijs_sub_clone(MBS *pM,INT start,INT sizeJ);
#define  ijs_sub_cloneL(pM,sizeJ)                (MBS*)ijs_sub_clone(pM,0,sizeJ)
#define  ijs_sub_cloneR(pM,sizeJ)                (MBS*)ijs_sub_clone(pM,-sizeJ,sizeJ)

BOOL     imb_cmp(MBS *pM,MBS *search,BOOL perfect,BOOL icase);
#define  imb_cmpf(pM,search)                     (BOOL)imb_cmp(pM,search,FALSE,FALSE)
#define  imb_cmpfi(pM,search)                    (BOOL)imb_cmp(pM,search,FALSE,TRUE)
#define  imb_cmpp(pM,search)                     (BOOL)imb_cmp(pM,search,TRUE,FALSE)
#define  imb_cmppi(pM,search)                    (BOOL)imb_cmp(pM,search,TRUE,TRUE)
#define  imb_cmp_leq(pM,search,icase)            (BOOL)imb_cmp(search,pM,FALSE,icase)
#define  imb_cmp_leqf(pM,search)                 (BOOL)imb_cmp_leq(pM,search,FALSE)
#define  imb_cmp_leqfi(pM,search)                (BOOL)imb_cmp_leq(pM,search,TRUE)

BOOL     iwb_cmp(WCS *pW,WCS *search,BOOL perfect,BOOL icase);
#define  iwb_cmpf(pW,search)                     (BOOL)iwb_cmp(pW,search,FALSE,FALSE)
#define  iwb_cmpfi(pW,search)                    (BOOL)iwb_cmp(pW,search,FALSE,TRUE)
#define  iwb_cmpp(pW,search)                     (BOOL)iwb_cmp(pW,search,TRUE,FALSE)
#define  iwb_cmppi(pW,search)                    (BOOL)iwb_cmp(pW,search,TRUE,TRUE)
#define  iwb_cmp_leq(pW,search,icase)            (BOOL)iwb_cmp(search,pW,FALSE,icase)
#define  iwb_cmp_leqf(pW,search)                 (BOOL)iwb_cmp_leq(pW,search,FALSE)
#define  iwb_cmp_leqfi(pW,search)                (BOOL)iwb_cmp_leq(pW,search,TRUE)

WCS      *iwp_cmpSunday(WCS *pW,WCS *search,BOOL icase);

MBS      *ijp_bypass(MBS *pM,MBS *from,MBS *to);

UINT     iji_searchCntA(MBS *pM,MBS *search,BOOL icase);
#define  iji_searchCnt(pM,search)                (UINT)iji_searchCntA(pM,search,FALSE)
#define  iji_searchCnti(pM,search)               (UINT)iji_searchCntA(pM,search,TRUE)

UINT     iwi_searchCntW(WCS *pW,WCS *search,BOOL icase);
#define  iwi_searchCnt(pW,search)                (UINT)iwi_searchCntW(pW,search,FALSE)
#define  iwi_searchCnti(pW,search)               (UINT)iwi_searchCntW(pW,search,TRUE)

UINT     iji_searchCntLA(MBS *pM,MBS *search,BOOL icase,INT option);
#define  iji_searchCntL(pM,search)               (UINT)iji_searchCntLA(pM,search,FALSE,0)
#define  iji_searchCntLi(pM,search)              (UINT)iji_searchCntLA(pM,search,TRUE,0)
#define  imi_searchLenL(pM,search)               (UINT)iji_searchCntLA(pM,search,FALSE,1)
#define  imi_searchLenLi(pM,search)              (UINT)iji_searchCntLA(pM,search,TRUE,1)
#define  iji_searchLenL(pM,search)               (UINT)iji_searchCntLA(pM,search,FALSE,2)
#define  iji_searchLenLi(pM,search)              (UINT)iji_searchCntLA(pM,search,TRUE,2)

UINT     iwi_searchCntLW(WCS *pW,WCS *search,BOOL icase,INT option);
#define  iwi_searchCntL(pW,search)               (UINT)iwi_searchCntLW(pW,search,FALSE,0)
#define  iwi_searchCntLi(pW,search)              (UINT)iwi_searchCntLW(pW,search,TRUE,0)
#define  iwi_searchLenL(pW,search)               (UINT)iwi_searchCntLW(pW,search,FALSE,1)
#define  iwi_searchLenLi(pW,search)              (UINT)iwi_searchCntLW(pW,search,TRUE,1)

UINT     iji_searchCntRA(MBS *pM,MBS *search,BOOL icase,INT option);
#define  iji_searchCntR(pM,search)               (UINT)iji_searchCntRA(pM,search,FALSE,0)
#define  iji_searchCntRi(pM,search)              (UINT)iji_searchCntRA(pM,search,TRUE,0)
#define  imi_searchLenR(pM,search)               (UINT)iji_searchCntRA(pM,search,FALSE,1)
#define  imi_searchLenRi(pM,search)              (UINT)iji_searchCntRA(pM,search,TRUE,1)
#define  iji_searchLenR(pM,search)               (UINT)iji_searchCntRA(pM,search,FALSE,2)
#define  iji_searchLenRi(pM,search)              (UINT)iji_searchCntRA(pM,search,TRUE,2)

UINT     iwi_searchCntRW(WCS *pW,WCS *search,BOOL icase,INT option);
#define  iwi_searchCntR(pW,search)               (UINT)iwi_searchCntRW(pW,search,FALSE,0)
#define  iwi_searchCntRi(pW,search)              (UINT)iwi_searchCntRW(pW,search,TRUE,0)
#define  iwi_searchLenR(pW,search)               (UINT)iwi_searchCntRW(pW,search,FALSE,1)
#define  iwi_searchLenRi(pW,search)              (UINT)iwi_searchCntRW(pW,search,TRUE,1)

MBS      *ijp_searchLA(MBS *pM,MBS *search,BOOL icase);
#define  ijp_searchL(pM,search)                  (MBS*)ijp_searchLA(pM,search,FALSE)
#define  ijp_searchLi(pM,search)                 (MBS*)ijp_searchLA(pM,search,TRUE)

MBS      *ijp_searchRA(MBS *pM,MBS *search,BOOL icase);
#define  ijp_searchR(pM,search)                  (MBS*)ijp_searchRA(pM,search,FALSE)
#define  ijp_searchRi(pM,search)                 (MBS*)ijp_searchRA(pM,search,TRUE)

INT      icmpOperator_extractHead(MBS *pM);
MBS      *icmpOperator_toHeadA(INT operator);
BOOL     icmpOperator_chk_INT(INT i1,INT i2,INT operator);
BOOL     icmpOperator_chk_INT64(INT64 i1,INT64 i2,INT operator);
BOOL     icmpOperator_chkDBL(DOUBLE d1,DOUBLE d2,INT operator);

MBS      **ija_split(MBS *pM,MBS *tokens,MBS *quotes,BOOL quote_cut);
#define  ija_token(pM,tokens)                    (MBS**)ija_split(pM,tokens,"",FALSE)
MBS      **ija_token_eod(MBS *pM);
MBS      **ija_token_zero(MBS *pM);

MBS      *ijs_rm_quote(MBS *pM,MBS *quoteS,MBS *quoteE,BOOL icase,BOOL one_to_one);

MBS      *ims_addTokenNStr(MBS *pM);

MBS      *ijs_cut(MBS *pM,MBS *rmLs,MBS *rmRs);
#define  ijs_cutL(pM,rmLs)                       (MBS*)ijs_cut(pM,rmLs,NULL)
#define  ijs_cutR(pM,rmRs)                       (MBS*)ijs_cut(pM,NULL,rmRs)

MBS      *ijs_cutAry(MBS *pM,MBS **aryLs,MBS **aryRs);
MBS      *ijs_trim(MBS *pM);
MBS      *ijs_trimL(MBS *pM);
MBS      *ijs_trimR(MBS *pM);
MBS      *ijs_chomp(MBS *pM);

MBS      *ijs_replace(MBS *from,MBS *before,MBS *after);

MBS      *ijs_simplify(MBS *pM,MBS *search);
WCS      *iws_simplify(WCS *pW,WCS *search);

BOOL     imb_shiftL(MBS *pM,UINT byte);
BOOL     imb_shiftR(MBS *pM,UINT byte);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	�����֌W
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
#define  inum_chkM(pM)                           (BOOL)((*pM>='0'&&*pM<='9')||*pM=='+'||*pM=='-'||*pM=='.'?TRUE:FALSE)
#define  inum_chk2M(pM)                          (BOOL)(*pM>='0'&&*pM<='9'?TRUE:FALSE)
INT      inum_atoi(MBS *pM);
INT64    inum_atoi64(MBS *pM);
INT64    inum_atoi64Ex(MBS *pM);
DOUBLE   inum_atof(MBS *pM);

UINT     inum_posSize(INT64 num);
DOUBLE   inum_posToDec(DOUBLE num,INT shift);

INT      inum_bitwiseCmpINT(INT iBase,INT iDest);

/*�yCopyright (C) 1997 - 2002,Makoto Matsumoto and Takuji Nishimura,All rights reserved.�z
	http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c
	http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt.html
*/
VOID     MT_initByAry(BOOL fixOn);
UINT     MT_genrandUint32();
VOID     MT_freeAry();

INT      MT_irand_INT(INT posMin,INT posMax);
DOUBLE   MT_irandDBL(INT posMin,INT posMax,UINT decRound);
MBS      *MT_irand_words(UINT size,BOOL ext);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Command Line
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
MBS      *iCmdline_getCmd();
MBS      **iCmdline_getArgs();

MBS      *iCmdline_getsA(CONST UINT sizeM);
MBS      *iCmdline_getsJ(CONST UINT sizeJ);

MBS      *iCmdline_esEncode(MBS *pM);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Array
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
MBS      **ima_null();
WCS      **iwa_null();

UINT     iary_size(MBS **ary);
#define  iargs_size(ary)                         (UINT)iary_size((MBS**)ary)

UINT     iary_Mlen(MBS **ary);
#define  iargs_Mlen(ary)                         (UINT)iary_Mlen((MBS**)ary)

UINT     iary_Jlen(MBS **ary);
#define  iargs_Jlen(ary)                         (UINT)iary_Jlen(((MBS**)ary)

MBS      **iargs_option(MBS **ary,MBS *op1,MBS *op2);

INT      iary_qsort_cmp(CONST VOID *p1,CONST VOID *p2,BOOL asc);
INT      iary_qsort_cmpAsc(CONST VOID *p1,CONST VOID *p2);
INT      iary_qsort_cmpDesc(CONST VOID *p1,CONST VOID *p2);
#define  iary_sortAsc(ary)                       (VOID)qsort((MBS*)ary,iary_size(ary),sizeof(MBS**),iary_qsort_cmpAsc)
#define  iary_sortDesc(ary)                      (VOID)qsort((MBS*)ary,iary_size(ary),sizeof(MBS**),iary_qsort_cmpDesc)
MBS      *iary_toA(MBS **ary,MBS *token);

MBS      **iary_simplify(MBS **ary,BOOL icase);
MBS      **iary_higherDir(MBS **ary,UINT depth);

MBS      **iary_clone(MBS **ary);
MBS      **iary_new(MBS *pM,...);

VOID     iary_print(MBS **ary);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	File/Dir����(WIN32_FIND_DATAA)
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	MBS      fullnameA[IMAX_PATHA];// (��) D:\���\iwama.txt
	UINT     iFname;               // MBS= 8�^WCS= 6
	UINT     iExt;                 // MBS=13�^WCS=11
	UINT     iEnd;                 // MBS=17�^WCS=15
	UINT     iAttr;                // 32
	UINT     iFtype;               // 2 : �s��=0�^Dir=1�^File=2
	DOUBLE   cjdCtime;             // (DWORD)dwLowDateTime,(DWORD)dwHighDateTime
	DOUBLE   cjdMtime;             // ��
	DOUBLE   cjdAtime;             // ��
	INT64    iFsize;               // byte (4GB�Ή�)
} $struct_iFinfoA;

typedef struct{
	WCS      fullnameW[IMAX_PATHW];// (��) D:\���\iwama.txt
	UINT     iFname;               // MBS= 8�^WCS= 6
	UINT     iExt;                 // MBS=13�^WCS=11
	UINT     iEnd;                 // MBS=17�^WCS=15
	UINT     iAttr;                // 32
	UINT     iFtype;               // 2 : �s��=0�^Dir=1�^File=2
	DOUBLE   cjdCtime;             // (DWORD)dwLowDateTime,(DWORD)dwHighDateTime
	DOUBLE   cjdMtime;             // ��
	DOUBLE   cjdAtime;             // ��
	INT64    iFsize;               // byte (4GB�Ή�)
} $struct_iFinfoW;

$struct_iFinfoA *iFinfo_allocA();
$struct_iFinfoW *iFinfo_allocW();

VOID     iFinfo_clearA($struct_iFinfoA *FI);
VOID     iFinfo_clearW($struct_iFinfoW *FI);

BOOL     iFinfo_initA($struct_iFinfoA *FI,WIN32_FIND_DATAA *F,MBS *dir,UINT dirLenA,MBS *name);
BOOL     iFinfo_initW($struct_iFinfoW *FI,WIN32_FIND_DATAW *F,WCS *dir,UINT dirLenW,WCS *name);
BOOL     iFinfo_init2A($struct_iFinfoA *FI,MBS *path);

VOID     iFinfo_freeA($struct_iFinfoA *FI);
VOID     iFinfo_freeW($struct_iFinfoW *FI);

MBS      *iFinfo_attrToA(UINT iAttr);
UINT     iFinfo_attrAtoUINT(MBS *sAttr);

MBS      *iFinfo_ftypeToA(UINT iFtype);

INT      iFinfo_depthA($struct_iFinfoA *FI);
INT      iFinfo_depthW($struct_iFinfoW *FI);

INT64    iFinfo_fsize(MBS *Fn);

MBS      *iFinfo_ftimeToA(FILETIME ftime);
DOUBLE   iFinfo_ftimeToCjd(FILETIME ftime);

FILETIME iFinfo_ymdhnsToFtime(INT wYear,INT wMonth,INT wDay,INT wHour,INT wMinute,INT wSecond,BOOL reChk);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	File/Dir����
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct{
	UINT size;
	MBS *ptr;
} $struct_ifreadBuf;

FILE     *ifopen(MBS *Fn,MBS *mode);
#define  ifclose(Fp)                             fclose(Fp)

MBS      *ifreadLine(FILE *iFp,BOOL rmCrlf);

$struct_ifreadBuf *ifreadBuf_alloc(INT64 fsize);
UINT     ifreadBuf(FILE *Fp,$struct_ifreadBuf *Buf);
VOID     ifreadBuf_free($struct_ifreadBuf *Buf);
#define  ifreadBuf_getPtr(Buf)                   (MBS*)(Buf->ptr)

#define  ifwrite(Fp,ptr,size)                    (UINT)fwrite(ptr,size,1,Fp)

BOOL     iFchk_existPathA(MBS *path);

UINT     iFchk_typePathA(MBS *path);

BOOL     iFchk_Bfile(MBS *fn);
#define  iFchk_Tfile(fn)                         (BOOL)(iFchk_typePathA(fn)==2 && !iFchk_Bfile(fn) ? TRUE : FALSE)

#define  ichk_attrDirFile(attr)                  (UINT)(((UINT)attr & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 2)

MBS      *iFget_extPathname(MBS *path,UINT option);

MBS      *iFget_Apath(MBS *path);
#define  iFget_pwd()                             (MBS*)iFget_Apath(".")

MBS      *iFget_AdirA(MBS *path);
WCS      *iFget_AdirW(WCS *path);
MBS      *iFget_RdirA(MBS *path);
WCS      *iFget_RdirW(WCS *path);

#define  irm_file(path)                          (BOOL)DeleteFile(path)
#define  irm_dir(path)                           (BOOL)RemoveDirectory(path)

BOOL     imk_dir(MBS *path);

MBS      *imk_tempfile(MBS *prefix);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Windows System
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
BOOL     iwin_set_priority(INT class);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Console
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
UINT     iConsole_getBgcolor();
VOID     iConsole_setTextColor(UINT rgb);
#define  iConsole_setColor(textcolor,bgcolor)    (VOID)iConsole_setTextColor(textcolor+(bgcolor*16))
#define  iConsole_setTitle(pM)                   (VOID)SetConsoleTitleA(pM)

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	Clipboard
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
BOOL     iClipboard_erase();
BOOL     iClipboard_setText(MBS *pM);
MBS      *iClipboard_getText();
BOOL     iClipboard_addText(MBS *pM);
MBS      *iClipboard_getDropFn(UINT option);

/////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------------------
	��
---------------------------------------------------------------------------------------*/
/////////////////////////////////////////////////////////////////////////////////////////
/*
	���͂��߂�
		�����E�X��    �F"B.C.4713-01-01 12:00" �` "A.C.1582-10-04 23:59"
		�O���S���E�X��F"A.C.1582-10-15 00:00" �` ����

		����͏�L�̂Ƃ��肾���A�����E�X��ȑO��(�����E�X��ɑ���)�v�Z�\�ł���B
		�wB.C.��x�̎�舵���ɂ��ẮA��q�w��B.C.��x�ɑ���B
		�Ȃ��A�h����ɂ��ẮA��q�w�e��̕ϐ��x���g�p�̂��ƁB

	����B.C.��
		(����) "-4713-01-01"�`"-0001-12-31"
		(��)   "-4712-01-01"�`"+0000-12-31" // ����+�P�N

	���e��̕ϐ�
		��CJD����Ɍv�Z�B
		JD  : Julian Day               :-4712-01-01 12:00:00�J�n
		CJD : Chronological Julian Day :-4712-01-01 00:00:00�J�n :JD-0.5 
		MJD : Modified Julian Day      : 1858-11-17 00:00:00�J�n :JD-2400000.5
		LD  : Lilian Day               : 1582-10-15 00:00:00�J�n :JD-2299159.5
*/
#define  CJD_TO_JD                               (DOUBLE)(0.5)
#define  CJD_TO_MJD                              (DOUBLE)(2400000.5-CJD_TO_JD)
#define  CJD_TO_LD                               (DOUBLE)(2299159.5-CJD_TO_JD)

#define	 CJD_FORMAT                              "%.8f"

/*
	CJD��(=JD��-0.5)�̍ŏI�� NS_before[]
	NS�� (=GD��)    �̊J�n�� NS_after[]

	JD��͖{���ABC.4713-1-1 12:00���N�_�Ƃ��邪�A
	�v�Z��A00:00���N�_(=CJD��)�Ƃ��Ĉ���.
	<cf> idate_jdToCjd(JD)

	�N�_�́A���ɂ���ĈႤ
	<ITALY>
		CJD:2299160	YMD:1582-10-04
		CJD:2299161	YMD:1582-10-15

	<ENGLAND>
		CJD:2361221	YMD:1752-09-02
		CJD:2361222	YMD:1752-09-14
*/

// CJD�ʓ�
#define  idate_cjd_print(CJD)                    P(CJD_FORMAT,CJD)

// CJD�ʓ�����AJD�ʓ���Ԃ�
#define  idate_cjdToJd(CJD)                      (DOUBLE)CJD-CJD_TO_JD
#define  idate_cjdToJd_print(CJD)                P(CJD_FORMAT,idate_cjdToJd(CJD))

// JD�ʓ�����ACJD�ʓ���Ԃ�
#define  idate_jdToCjd(JD)                       (DOUBLE)JD+CJD_TO_JD
#define  idate_jdToCjd_print(JD)                 P(CJD_FORMAT,idate_jdToCjd(JD))

// CJD�ʓ�����AMJD�ʓ���Ԃ�
#define  idate_cjdToMjd(CJD)                     (DOUBLE)CJD-CJD_TO_MJD
#define  idate_cjdToMjd_print(CJD)               P(CJD_FORMAT,idate_cjdToMjd(CJD))

// MJD�ʓ�����ACJD�ʓ���Ԃ�
#define  idate_mjdToCjd(MJD)                     (DOUBLE)MJD+CJD_TO_MJD
#define  idate_mjdToCjd_print(MJD)               P(CJD_FORMAT,idate_mjdToCjd(MJD))

// CJD�ʓ�����ALD�ʓ���Ԃ�
#define  idate_cjdToLd(CJD)                      (DOUBLE)CJD-CJD_TO_LD
#define  idate_cjdToLd_print(CJD)                P(CJD_FORMAT,idate_cjdToLd(CJD))

// LD�ʓ�����ACJD�ʓ���Ԃ�
#define  idate_ldToCjd(LD)                       (DOUBLE)LD+CJD_TO_LD
#define  idate_ldToCjd_print(LD)                 P(CJD_FORMAT,idate_ldToCjd(LD))

BOOL     idate_chk_uruu(INT i_y);

INT      *idate_cnv_month(INT i_y,INT i_m,INT from_m,INT to_m);
// 1-12��
#define  idate_cnv_month1(i_y,i_m)               (INT*)idate_cnv_month(i_y,i_m,1,12)
// 0-11��
#define  idate_cnv_month2(i_y,i_m)               (INT*)idate_cnv_month(i_y,i_m,0,11)

INT      idate_month_end(INT i_y,INT i_m);
BOOL     idate_chk_month_end(INT i_y,INT i_m,INT i_d,BOOL reChk);

DOUBLE   idate_MBStoCjd(MBS *pM);

MBS      **idate_MBS_to_mAryYmdhns(MBS *pM);
INT      *idate_MBS_to_iAryYmdhns(MBS *pM);

INT      idate_ymd_num(INT i_y,INT i_m,INT i_d);
DOUBLE   idate_ymdhnsToCjd(INT i_y,INT i_m,INT i_d,INT i_h,INT i_n,INT i_s);

INT      *idate_cjd_to_iAryHns(DOUBLE cjd);
INT      *idate_cjd_to_iAryYmdhns(DOUBLE cjd);

INT      *idate_reYmdhns(INT i_y,INT i_m,INT i_d,INT i_h,INT i_n,INT i_s);

INT      idate_cjd_iWday(DOUBLE cjd);
MBS      *idate_cjd_mWday(DOUBLE cjd);
FILETIME idate_cjdToFtime(DOUBLE cjd);

// �N���̒ʎZ��
INT      idate_cjd_yeardays(DOUBLE cjd);
// cjd1�`cjd2�̒ʎZ��
#define  idate_cjd_days(cjd1,cjd2)               (INT)((INT)cjd2-(INT)cjd1)

// �N���̒ʎZ�T
#define  idate_cjd_yearweeks(cjd)                (INT)((6+idate_cjd_yeardays(cjd))/7)
// cjd1�`cjd2�̒ʎZ�T
#define  idate_cjd_weeks(cjd1,cjd2)              (INT)((idate_cjd_days(cjd1,cjd2)+6)/7)

// [6]={y,m,d,h,n,s}
INT      *idate_add(INT i_y,INT i_m,INT i_d,INT i_h,INT i_n,INT i_s,INT add_y,INT add_m,INT add_d,INT add_h,INT add_n,INT add_s);
// [8]={sign,y,m,d,h,n,s,days}
INT      *idate_diff(INT i_y1,INT i_m1,INT i_d1,INT i_h1,INT i_n1,INT i_s1,INT i_y2,INT i_m2,INT i_d2,INT i_h2,INT i_n2,INT i_s2);

//VOID     idate_diff_checker(INT from_year,INT to_year,INT repeat);

/*
// Ymdhns
	%a : �j��(��:Su)
	%A : �j����(��:0)
	%c : �N���̒ʎZ��(��:001)
	%C : CJD�ʎZ��
	%e : �N���̒ʎZ�T(��:01)
	%E : CJD�ʎZ�T

// Diff
	%M : �ʎZ��
	%D : �ʎZ��
	%H : �ʎZ��
	%N : �ʎZ��
	%S : �ʎZ�b
	%W : �ʎZ�T
	%w : �ʎZ�T�̗]��

// ����
	%g : Sign "-" "+"
	%G : Sign "-" �̂�
	%y : �N(��:2016)
	%Y : �N(��:16)
	%m : ��(��:01)
	%d : ��(��:01)
	%h : ��(��:01)
	%n : ��(��:01)
	%s : �b(��:01)
	%% : "%"
	\a
	\n
	\t
*/
MBS      *idate_format_diff(MBS *format,INT i_sign,INT i_y,INT i_m,INT i_d,INT i_h,INT i_n,INT i_s,INT i_days);
#define  idate_format_ymdhns(format,i_y,i_m,i_d,i_h,i_n,i_s) \
                                                 idate_format_diff(format,0,i_y,i_m,i_d,i_h,i_n,i_s,0)

MBS      *idate_format_iAryToA(MBS *format,INT *ymdhns);
MBS      *idate_format_cjdToA(MBS *format,DOUBLE cjd);
/*
	�啶�� => "yyyy-mm-dd 00:00:00"
	������ => "yyyy-mm-dd hh:nn:ss"
		Y,y : �N
		M,m : ��
		W,w : �T
		D,d : ��
		H,h : ��
		N,n : ��
		S,s : �b
*/
MBS      *idate_replace_format_ymdhns(MBS *pM,MBS *quote1,MBS *quote2,MBS *add_quote,CONST INT i_y,CONST INT i_m,CONST INT i_d,CONST INT i_h,CONST INT i_n,CONST INT i_s);
#define  idate_format_nowToYmdhns(i_y,i_m,i_d,i_h,i_n,i_s) \
                                                 (MBS*)idate_replace_format_ymdhns("[]","[","]","",i_y,i_m,i_d,i_h,i_n,i_s)

INT      *idate_now_to_iAryYmdhns(BOOL area);
#define  idate_now_to_iAryYmdhns_localtime()     (INT*)idate_now_to_iAryYmdhns(TRUE)
#define  idate_now_to_iAryYmdhns_systemtime()    (INT*)idate_now_to_iAryYmdhns(FALSE)

DOUBLE   idate_nowToCjd(BOOL area);
#define  idate_nowToCjd_localtime()              (DOUBLE)idate_nowToCjd(TRUE)
#define  idate_nowToCjd_systemtime()             (DOUBLE)idate_nowToCjd(FALSE)

#define  idate_cjd_sec(cjd)                      (DOUBLE)(cjd)*86400.0