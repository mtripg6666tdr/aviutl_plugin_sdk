//----------------------------------------------------------------------------------
//		サンプルYUY2色変換プラグイン(色変換プラグイン)  for AviUtl ver0.99h以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "color.h"


//---------------------------------------------------------------------
//		色変換プラグイン構造体定義
//---------------------------------------------------------------------
COLOR_PLUGIN_TABLE color_plugin_table = {
	NULL,												//	フラグ
	"サンプルYUY2変換",									//	プラグインの名前
	"サンプルYUY2変換 version 0.01 By ＫＥＮくん",		//	プラグインの情報
	NULL,												//	DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,												//	DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_pixel2yc,										//	DIB形式の画像からからPIXEL_YC形式の画像に変換します (NULLなら呼ばれません)
	func_yc2pixel,										//	PIXEL_YC形式の画像からからDIB形式の画像に変換します (NULLなら呼ばれません)
};


//---------------------------------------------------------------------
//		色変換プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C COLOR_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetColorPluginTable( void )
{
	return &color_plugin_table;
}
//	※GetColorPluginTableYUY2でYUY2フィルタモードにも対応できます


//---------------------------------------------------------------------
//		入力変換
//---------------------------------------------------------------------
BOOL func_pixel2yc( COLOR_PROC_INFO *cpip )
{
	int			x,y;
	PIXEL_YC	*ycp;
	BYTE		*pixelp;

	//	YUY2以外はAviUtl本体の変換に任せる
	if( cpip->format != MAKEFOURCC('Y','U','Y','2') ) return FALSE;

	//	ITU-R BT.601として変換
	for(y=0;y<cpip->h;y++) {
		ycp    = (PIXEL_YC *)( (BYTE *)cpip->ycp + y*cpip->line_size );
		pixelp = (BYTE *)cpip->pixelp + y*((cpip->w+1)/2*4);
		for(x=0;x<cpip->w;x+=2) {
			ycp->y  = (pixelp[0]-16 )*4096/219;
			ycp->cb = (pixelp[1]-128)*4096/224;
			ycp->cr = (pixelp[3]-128)*4096/224;
			ycp++;
			ycp->y  = (pixelp[2]-16 )*4096/219;
			ycp->cb = (pixelp[1]-128)*4096/224;
			ycp->cr = (pixelp[3]-128)*4096/224;
			ycp++;
			pixelp+=4;
		}
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		出力変換
//---------------------------------------------------------------------
BYTE pack_byte( int val )
{
	if(      val < 0   ) val = 0;
	else if( val > 255 ) val = 255;
	return (BYTE)val;
}
BOOL func_yc2pixel( COLOR_PROC_INFO *cpip )
{
	int			x,y;
	PIXEL_YC	*ycp;
	BYTE		*pixelp;

	//	YUY2以外はAviUtl本体の変換に任せる
	if( cpip->format != MAKEFOURCC('Y','U','Y','2') ) return FALSE;

	//	ITU-R BT.601として変換
	for(y=0;y<cpip->h;y++) {
		ycp    = (PIXEL_YC *)( (BYTE *)cpip->ycp + y*cpip->line_size );
		pixelp = (BYTE *)cpip->pixelp + y*((cpip->w+1)/2*4);
		for(x=0;x<cpip->w;x+=2) {
			pixelp[0] = pack_byte( ycp->y *219/4096 + 16  );
			pixelp[1] = pack_byte( ycp->cb*224/4096 + 128 );
			pixelp[3] = pack_byte( ycp->cr*224/4096 + 128 );
			ycp++;
			pixelp[2] = pack_byte( ycp->y *219/4096 + 16  );
			ycp++;
			pixelp+=4;
		}
	}

	return TRUE;
}


