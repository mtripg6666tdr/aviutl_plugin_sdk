//---------------------------------------------------------------------
//		サンプルインターレース解除プラグイン  for AviUtl ver0.98以降
//---------------------------------------------------------------------
#include	<windows.h>

#include	"filter.h"


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
#define	TRACK_N	0
#define	CHECK_N	0

FILTER_DLL filter = {
	FILTER_FLAG_INTERLACE_FILTER|FILTER_FLAG_EX_INFORMATION|FILTER_FLAG_NO_CONFIG,
	0,0,
	"サンプル解除",
	TRACK_N,NULL,NULL,
	NULL,NULL,
	CHECK_N,NULL,NULL,
	func_proc,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,NULL,
	NULL,
	NULL,
	"サンプルインターレース解除 version 0.01 by ＫＥＮくん",
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	func_is_saveframe
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
//
//	fp->track[n]			: トラックバーの数値
//	fp->check[n]			: チェックボックスの数値
//	fpip->w 				: 実際の画像の横幅
//	fpip->h 				: 実際の画像の縦幅
//	fpip->max_w				: 画像領域の横幅
//	fpip->max_h				: 画像領域の縦幅
//	fpip->ycp_edit			: 画像領域へのポインタ
//	fpip->ycp_temp			: テンポラリ領域へのポインタ
//	fpip->ycp_edit[n].y		: 画素(輝度    )データ (     0 ～ 4095 )
//	fpip->ycp_edit[n].cb	: 画素(色差(青))データ ( -2048 ～ 2047 )
//	fpip->ycp_edit[n].cr	: 画素(色差(赤))データ ( -2048 ～ 2047 )
//
//	インターレース解除フィルタは画像サイズを変えたり
//	画像領域とテンポラリ領域を入れ替えたりは出来ません。
//
//	画像領域に初期画像データは入っていません。
//	get_ycp_source_cache()を使って自分で読み込む必要があります。
//
	int			y;
	PIXEL_YC	*ycp,*ycp2,*p0;

	//	インターレース解除フィルタのfpip->ycp_editにはデータが入っていないので
	//	get_ycp_source_cache()を使って自分で読み込む。
	p0 = (PIXEL_YC *)fp->exfunc->get_ycp_source_cache(fpip->editp,fpip->frame,0);

	//	奇数解除
	for(y=0;y<fpip->h;y++) {
		ycp  = p0 + (y-(y&1))*fpip->max_w;
		ycp2 = fpip->ycp_edit + y*fpip->max_w;
		memcpy(ycp2,ycp,sizeof(PIXEL_YC)*fpip->w);
	}

	return TRUE;
}

BOOL func_is_saveframe( FILTER *fp,void *editp,int saveno,int frame,int fps,int edit_flag,int inter )
{
//	saveno		: セーブする範囲の先頭からのフレーム番号
//	frame		: 編集フレーム番号
//	fps			: フレームレートの変更の設定値 (30,24,20,15,10)
//	edit_flag	: 編集フラグ
//	inter		: フレームのインターレース
//
//	TRUEを返すと保存フレーム、FALSEを返すと間引きフレームになります。
//
	if( saveno*fps/30 - (saveno+1)*fps/30 ) return TRUE;
	return FALSE;
}


