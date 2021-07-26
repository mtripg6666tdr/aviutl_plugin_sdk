//----------------------------------------------------------------------------------
//		サンプルビデオフィルタ(フィルタプラグイン)  for AviUtl ver0.99e以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	3														//	トラックバーの数
TCHAR	*track_name[] =		{	"track0",	"track1",	"track2"	};	//	トラックバーの名前
int		track_default[] =	{	0,			0,			0			};	//	トラックバーの初期値
int		track_s[] =			{	-999,		-999,		-999		};	//	トラックバーの下限値
int		track_e[] =			{	+999,		+999,		+999		};	//	トラックバーの上限値

#define	CHECK_N	2														//	チェックボックスの数
TCHAR	*check_name[] = 	{	"check0",	"check1"	};				//	チェックボックスの名前
int		check_default[] = 	{	0,			0			};				//	チェックボックスの初期値 (値は0か1)

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
	"サンプルフィルタ",			//	フィルタの名前
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
	"サンプルフィルタ version 0.06 by ＫＥＮくん",
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
//	下記のようにすると1つのaufファイルで複数のフィルタ構造体を渡すことが出来ます
/*
FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
{
	return (FILTER_DLL **)&filter_list;
}
*/


//---------------------------------------------------------------------
//		フィルタ処理関数
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
//
//	fp->track[n]			: トラックバーの数値
//	fp->check[n]			: チェックボックスの数値
//	fpip->w 				: 実際の画像の横幅
//	fpip->h 				: 実際の画像の縦幅
//	fpip->max_w				: 画像領域の横幅
//	fpip->max_h				: 画像領域の縦幅
//	fpip->ycp_edit			: 画像領域へのポインタ
//	fpip->ycp_temp			: テンポラリ領域へのポインタ
//	fpip->ycp_edit[n].y		: 画素(輝度    )データ (     0 ～ 4096 )
//	fpip->ycp_edit[n].cb	: 画素(色差(青))データ ( -2048 ～ 2048 )
//	fpip->ycp_edit[n].cr	: 画素(色差(赤))データ ( -2048 ～ 2048 )
//
//  画素データは範囲外に出ていることがあります。
//  また範囲内に収めなくてもかまいません。
//
//	画像サイズを変えたいときは fpip->w や fpip->h を変えます。
//
//	テンポラリ領域に処理した画像を格納したいときは
//	fpip->ycp_edit と fpip->ycp_temp を入れ替えます。
//
	int			x,y;
	PIXEL_YC	*ycp,*ycp2;

	//	各要素にトラックバーの値を足し込む
	for(y=0;y<fpip->h;y++) {
		ycp = fpip->ycp_edit + y*fpip->max_w;
		for(x=0;x<fpip->w;x++) {
			ycp->y  += (short)fp->track[0];
			ycp->cb += (short)fp->track[1];
			ycp->cr += (short)fp->track[2];
			ycp++;
		}
	}

	//	check0がチェックされていたら横方向を1/2に縮小
	if( fp->check[0] ) {
		for(y=0;y<fpip->h;y++) {
			ycp  = fpip->ycp_edit + y*fpip->max_w;
			ycp2 = fpip->ycp_temp + y*fpip->max_w;
			for(x=0;x<fpip->w/2;x++) {
				ycp2->y  = (short)(( ycp[0].y  + ycp[1].y  )/2);
				ycp2->cb = (short)(( ycp[0].cb + ycp[1].cb )/2);
				ycp2->cr = (short)(( ycp[0].cr + ycp[1].cr )/2);
				ycp+=2;
				ycp2++;
			}
		}
		//	幅を半分にしたので fpip->w を半分にする
		fpip->w /= 2;
		//	fpip->ycp_temp に処理したデータを書き込んだので
		//	fpip->ycp_edit と fpip->ycp_temp を入れ替える
		ycp = fpip->ycp_edit;
		fpip->ycp_edit = fpip->ycp_temp;
		fpip->ycp_temp = ycp;
	}

	//	check1がチェックされていたら画像を1/2に縮小したものを中央に半透明で表示
	if( fp->check[1] ) {
		//	fpip->ycp_temp に fpip->ycp_edit を1/2に縮小したものを作成
		fp->exfunc->resize_yc(fpip->ycp_temp,fpip->w/2,fpip->h/2,fpip->ycp_edit,0,0,fpip->w,fpip->h);
		//	fpip->ycp_temp を fpip->ycp_edit の中央に半透明で描画
		fp->exfunc->copy_yc(fpip->ycp_edit,fpip->w/4,fpip->w/4,fpip->ycp_temp,0,0,fpip->w/2,fpip->h/2,2048);
	}

	return TRUE;
}


