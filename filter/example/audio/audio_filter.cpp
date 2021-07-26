//----------------------------------------------------------------------------------
//		サンプルオーディオフィルタ(フィルタプラグイン)  for AviUtl ver0.96c以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	1		//	トラックバーの数
static TCHAR	*track_name[] =		{	"レベル"	};
static int		track_default[] =	{	256			};

#define	CHECK_N	0		//	チェックボックスの数

FILTER_DLL filter = {
	FILTER_FLAG_PRIORITY_HIGHEST|FILTER_FLAG_AUDIO_FILTER|FILTER_FLAG_EX_INFORMATION,
	0,0,
	"音量の調節",
	TRACK_N,track_name,track_default,
	NULL,NULL,
	NULL,NULL,NULL,
	func_proc,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,NULL,
	NULL,
	NULL,
	"音量の調節 version 0.03 by ＫＥＮくん",
	NULL,
	NULL,
};


//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}


//---------------------------------------------------------------------
//		フィルタ処理関数
//---------------------------------------------------------------------
static BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
	int		i;

	//	音量を変更する
	for(i=0;i<fpip->audio_n*fpip->audio_ch;i++) {
		fpip->audiop[i] = (short)(fpip->audiop[i]*fp->track[0]/256);
	}

	return TRUE;
}


