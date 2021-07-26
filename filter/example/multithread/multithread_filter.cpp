//---------------------------------------------------------------------------------------------
//		マルチスレッド対応サンプルフィルタ(フィルタプラグイン)  for AviUtl ver0.99a以降
//---------------------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	3														//	トラックバーの数
TCHAR	*track_name[] =		{	"track0",	"track1",	"track2"	};	//	トラックバーの名前
int		track_default[] =	{	512		,	0		,	0			};	//	トラックバーの初期値
int		track_s[] =			{	-999	,	-999	,	-999		};	//	トラックバーの下限値
int		track_e[] =			{	+999	,	+999	,	+999		};	//	トラックバーの上限値

#define	CHECK_N	4														//	チェックボックスの数
TCHAR	*check_name[] = 	{	"thread1"	,"thread2"	,"thread3"	,"thread4",	};	//	チェックボックスの名前
int		check_default[] = 	{	1			,1			,1			,1			};	//	チェックボックスの初期値 (値は0か1)

FILTER_DLL filter = {
	FILTER_FLAG_EX_INFORMATION,	//	フィルタのフラグ
								//	FILTER_FLAG_ALWAYS_ACTIVE		: フィルタを常にアクティブにします
								//	FILTER_FLAG_CONFIG_POPUP		: 設定をポップアップメニューにします
								//	FILTER_FLAG_CONFIG_CHECK		: 設定をチェックボックスメニューにします
								//	FILTER_FLAG_CONFIG_RADIO		: 設定をラジオボタンメニューにします
								//	FILTER_FLAG_EX_DATA				: 拡張データを保存出来るようにします。
								//	FILTER_FLAG_PRIORITY_HIGHEST	: フィルタのプライオリティを常に最上位にします
								//	FILTER_FLAG_PRIORITY_LOWEST		: フィルタのプライオリティを常に最下位にします
								//	FILTER_FLAG_WINDOW_THICKFRAME	: サイズ変更可能なウィンドウを作ります
								//	FILTER_FLAG_WINDOW_SIZE			: 設定ウィンドウのサイズを指定出来るようにします
								//	FILTER_FLAG_DISP_FILTER			: 表示フィルタにします
								//	FILTER_FLAG_EX_INFORMATION		: フィルタの拡張情報を設定できるようにします
								//	FILTER_FLAG_NO_CONFIG			: 設定ウィンドウを表示しないようにします
								//	FILTER_FLAG_AUDIO_FILTER		: オーディオフィルタにします
								//	FILTER_FLAG_RADIO_BUTTON		: チェックボックスをラジオボタンにします
								//	FILTER_FLAG_WINDOW_HSCROLL		: 水平スクロールバーを持つウィンドウを作ります
								//	FILTER_FLAG_WINDOW_VSCROLL		: 垂直スクロールバーを持つウィンドウを作ります
								//	FILTER_FLAG_IMPORT				: インポートメニューを作ります
								//	FILTER_FLAG_EXPORT				: エクスポートメニューを作ります
	0,0,						//	設定ウインドウのサイズ (FILTER_FLAG_WINDOW_SIZEが立っている時に有効)
	"マルチスレッドサンプルフィルタ",			//	フィルタの名前
	TRACK_N,					//	トラックバーの数 (0なら名前初期値等もNULLでよい)
	track_name,					//	トラックバーの名前郡へのポインタ
	track_default,				//	トラックバーの初期値郡へのポインタ
	track_s,track_e,			//	トラックバーの数値の下限上限 (NULLなら全て0～256)
	CHECK_N,					//	チェックボックスの数 (0なら名前初期値等もNULLでよい)
	check_name,					//	チェックボックスの名前郡へのポインタ
	check_default,				//	チェックボックスの初期値郡へのポインタ
	func_proc,					//	フィルタ処理関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定が変更されたときに呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定ウィンドウにウィンドウメッセージが来た時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,NULL,					//	システムで使いますので使用しないでください
	NULL,						//  拡張データ領域へのポインタ (FILTER_FLAG_EX_DATAが立っている時に有効)
	NULL,						//  拡張データサイズ (FILTER_FLAG_EX_DATAが立っている時に有効)
	"マルチスレッドサンプルフィルタ version 0.01 by ＫＥＮくん",
								//  フィルタ情報へのポインタ (FILTER_FLAG_EX_INFORMATIONが立っている時に有効)
	NULL,						//	セーブが開始される直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	セーブが終了した直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
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
void multi_thread_func( int thread_id,int thread_num,void *param1,void *param2 )
{
//	thread_id	: スレッド番号 ( 0 ～ thread_num-1 )
//	thread_num	: スレッド数 ( 1 ～ )
//	param1		: 汎用パラメータ
//	param2		: 汎用パラメータ
//
//	この関数内からWin32APIや外部関数(rgb2yc,yc2rgbは除く)を使用しないでください。
//
	FILTER *fp				= (FILTER *)param1;
	FILTER_PROC_INFO *fpip	= (FILTER_PROC_INFO *)param2;

	int			x,y,y_start,y_end;
	PIXEL_YC	*ycp;

	//	チェックボックスが外れているスレッドは処理しない
	if( fp->check[thread_id] == 0 ) return;

	//	スレッド毎の画像を処理する場所を計算する
	y_start = ( fpip->h * thread_id     ) / thread_num;
	y_end   = ( fpip->h * (thread_id+1) ) / thread_num;

	//	各要素にトラックバーの値を足し込む
	for(y=y_start;y<y_end;y++) {
		ycp = fpip->ycp_edit + y*fpip->max_w;
		for(x=0;x<fpip->w;x++) {
			ycp->y  += (short)fp->track[0];
			ycp->cb += (short)fp->track[1];
			ycp->cr += (short)fp->track[2];
			ycp++;
		}
	}
}


//---------------------------------------------------------------------
//		フィルタ処理関数
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
	//	マルチスレッドでフィルタ処理関数を呼ぶ
	fp->exfunc->exec_multi_thread_func(multi_thread_func,(void *)fp,(void *)fpip);

	//	もし画像領域ポインタの入れ替えや解像度変更等の
	//	fpip の内容を変える場合はこちらの関数内で処理をする

	return TRUE;
}


