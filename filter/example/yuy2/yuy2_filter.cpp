//---------------------------------------------------------------------------------------------
//		YUY2フィルタモード対応サンプルフィルタ(フィルタプラグイン)  for AviUtl ver0.99a以降
//---------------------------------------------------------------------------------------------
//		YUY2フィルタモードについてはAviUtlの正式な機能ではなく
//		テスト的なおまけ機能として考えていますので
//		試してみたい人のみ対応させてください。
//---------------------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	0														//	トラックバーの数
#define	CHECK_N	0														//	チェックボックスの数

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
	"YUY2フィルタモード対応サンプルフィルタ",	//	フィルタの名前
	TRACK_N,					//	トラックバーの数 (0なら名前初期値等もNULLでよい)
	NULL,						//	トラックバーの名前郡へのポインタ
	NULL,						//	トラックバーの初期値郡へのポインタ
	NULL,NULL,					//	トラックバーの数値の下限上限 (NULLなら全て0～256)
	CHECK_N,					//	チェックボックスの数 (0なら名前初期値等もNULLでよい)
	NULL,						//	チェックボックスの名前郡へのポインタ
	NULL,						//	チェックボックスの初期値郡へのポインタ
	func_proc,					//	フィルタ処理関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定が変更されたときに呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,						//	設定ウィンドウにウィンドウメッセージが来た時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,NULL,					//	システムで使いますので使用しないでください
	NULL,						//  拡張データ領域へのポインタ (FILTER_FLAG_EX_DATAが立っている時に有効)
	NULL,						//  拡張データサイズ (FILTER_FLAG_EX_DATAが立っている時に有効)
	"YUY2フィルタモード対応サンプルフィルタ version 0.01 by ＫＥＮくん",
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
//		YUY2フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTableYUY2( void )
{
	return &filter;
}


//---------------------------------------------------------------------
//		フィルタ処理関数
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
//
//	fpip->w 				: 実際の画像の横幅
//	fpip->h 				: 実際の画像の縦幅
//	fpip->max_w				: 画像領域の横幅
//	fpip->max_h				: 画像領域の縦幅
//	fpip->ycp_edit			: 画像領域へのポインタ
//	fpip->ycp_temp			: テンポラリ領域へのポインタ
//	fpip->yc_size;			: 画像領域の画素のバイトサイズ ( 通常フィルタモード時 = 6 , YUY2フィルタモード時 = 2 )
//	fpip->line_size;		: 画像領域の幅のバイトサイズ

//	フィルタモードによって画像領域の画素のフォーマットは違います。
//	通常フィルタモード時は型通りのPIXEL_YC形式ですが
//	YUY2フィルタモード時は内容がYUY2形式のデータになります。
//	YUY2形式では横方向の解像度が2の倍数のみの対応になります。
//
	int			x,y;
	PIXEL_YC	*ycp,*ycp2,*ycp3;
	BYTE		*p,*p2,*p3;

	//	縦方向を1/2に縮小
	if( fpip->yc_size == 2 ) {
		//	YUY2モード時
		for(y=0;y<fpip->h/2;y++) {
			p = (BYTE *)fpip->ycp_edit + y*fpip->line_size;
			p2 = (BYTE *)fpip->ycp_edit + y*2*fpip->line_size;
			p3 = p2 + fpip->line_size;
			for(x=0;x<fpip->w;x+=2) {
				p[0] = (unsigned char)(( p2[0] + p3[0] )/2);	//	Y0
				p[1] = (unsigned char)(( p2[1] + p3[1] )/2);	//	U0
				p[2] = (unsigned char)(( p2[2] + p3[2] )/2);	//	Y1
				p[3] = (unsigned char)(( p2[3] + p3[3] )/2);	//	V0
				p  += 4;
				p2 += 4;
				p3 += 4;
			}
		}
	} else {
		//	通常モード時
		for(y=0;y<fpip->h/2;y++) {
			ycp = fpip->ycp_edit + y*fpip->max_w;
			ycp2 = fpip->ycp_edit + y*2*fpip->max_w;
			ycp3 = ycp2 + fpip->max_w;
			for(x=0;x<fpip->w;x++) {
				ycp->y  = (short)(( ycp2->y  + ycp3->y  )/2);
				ycp->cb = (short)(( ycp2->cb + ycp3->cb )/2);
				ycp->cr = (short)(( ycp2->cr + ycp3->cr )/2);
				ycp++;
				ycp2++;
				ycp3++;
			}
		}
	}

	//	幅を半分にしたので fpip->w を半分にする
	fpip->h /= 2;

	return TRUE;
}


