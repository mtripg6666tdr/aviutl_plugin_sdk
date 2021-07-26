//----------------------------------------------------------------------------------
//		サンプルBMP出力プラグイン(出力プラグイン)  for AviUtl ver0.98以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "output.h"
#include "bmp_output.h"


//---------------------------------------------------------------------
//		出力プラグイン構造体定義
//---------------------------------------------------------------------
OUTPUT_PLUGIN_TABLE output_plugin_table = {
	NULL,												//	フラグ
	"連番BMP出力",										//	プラグインの名前
	"BmpFile (*.bmp)\0*.bmp\0AllFile (*.*)\0*.*\0",		//	出力ファイルのフィルタ
	"連番BMP出力 version 0.06 By ＫＥＮくん",			//	プラグインの情報
	NULL,												//	DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	NULL,												//	DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_output,										//	出力時に呼ばれる関数へのポインタ
	func_config,										//	出力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_config_get,									//	出力設定データを取得する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_config_set,									//	出力設定データを設定する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
};


//---------------------------------------------------------------------
//		出力プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C OUTPUT_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetOutputPluginTable( void )
{
	return &output_plugin_table;
}


//---------------------------------------------------------------------
//		出力プラグイン内部変数
//---------------------------------------------------------------------
typedef struct {
	TCHAR	name[256];
} CONFIG;
static CONFIG	config = {"_%04d"};


//---------------------------------------------------------------------
//		出力プラグイン出力関数
//---------------------------------------------------------------------
//
//	oip->flag;				// フラグ
//							// OUTPUT_INFO_FLAG_VIDEO	: 画像データあり
//							// OUTPUT_INFO_FLAG_AUDIO	: 音声データあり
//	oip->w,oip->h;			// 縦横サイズ
//	oip->rate,oip->scale;	// フレームレート
//	oip->n;					// フレーム数
//	oip->size;				// １フレームのバイト数
//	oip->audio_rate;		// 音声サンプリングレート
//	oip->audio_ch;			// 音声チャンネル数
//	oip->audio_n;			// 音声サンプリング数
//	oip->audio_size;		// 音声１サンプルのバイト数
//	oip->savefile;			// セーブファイル名へのポインタ
//
//	void *oip->func_get_video( int frame );
//							// DIB形式(RGB24bit)の画像データへのポインタを取得します。
//							// frame	: フレーム番号
//							// 戻り値	: データへのポインタ
//	void *oip->func_get_audio( int start,int length,int *readed );
//							// 16bitPCM形式の音声データへのポインタを取得します。
//							// start	: 開始サンプル番号
//							// length	: 読み込むサンプル数
//							// readed	: 読み込まれたサンプル数
//							// 戻り値	: データへのポインタ
//	BOOL oip->func_is_abort( void );
//							// 中断するか調べます。
//							// 戻り値	: TRUEなら中断
//	BOOL oip->func_rest_time_disp( int now,int total );
//							// 残り時間を表示させます。
//							// now		: 処理しているフレーム番号
//							// total	: 処理する総フレーム数
//							// 戻り値	: TRUEなら成功
//	int oip->func_get_flag( int frame );
//							//	フラグを取得します。
//							//	frame	: フレーム番号
//							//	戻り値	: フラグ
//							//  OUTPUT_INFO_FRAME_FLAG_KEYFRAME		: キーフレーム推奨
//							//  OUTPUT_INFO_FRAME_FLAG_COPYFRAME	: コピーフレーム推奨
//	BOOL oip->func_update_preview( void );
//							//	プレビュー画面を更新します。
//							//	最後にfunc_get_videoで読み込まれたフレームが表示されます。
//							//	戻り値	: TRUEなら成功
//
BOOL func_output( OUTPUT_INFO *oip )
{
	int					i;
	TCHAR				path[MAX_PATH],name[MAX_PATH],ext[MAX_PATH],buf[MAX_PATH],*p,*p2,*p3;
	HANDLE				fp;
	DWORD				dw;
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmih;
	void				*pixelp;

	//	BMPヘッダー作成
	ZeroMemory(&bmfh,sizeof(bmfh));
	bmfh.bfType    = 'B'+'M'*256;
	bmfh.bfSize    = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+oip->size;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	ZeroMemory(&bmih,sizeof(bmih));
	bmih.biSize        = sizeof(bmih);
	bmih.biWidth       = oip->w;
	bmih.biHeight      = oip->h;
	bmih.biPlanes      = 1;
	bmih.biBitCount    = 24;
	bmih.biCompression = BI_RGB;

	//	連番ファイル名の準備
	lstrcpy(path,oip->savefile);
	p2 = p3 = NULL;
	for(p=path;*p;p++) {
		if(*p=='\\') p2=p+1;
		if(*p=='.' ) p3=p;
	}
	if( p2 == NULL ) p2 = path;
	if( p3 == NULL ) p3 = p;
	lstrcpy(ext,p3);
	*p3 = NULL;
	lstrcpy(name,p2);

	//	BMP出力
	for(i=0;i<oip->n;i++) {
		if( oip->func_is_abort() ) break;
		oip->func_rest_time_disp(i,oip->n);
		pixelp = oip->func_get_video_ex(i,NULL);
		wsprintf(buf,config.name,i);
		wsprintf(p2,"%s%s%s",name,buf,ext);
		if( (fp=CreateFile(path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) == INVALID_HANDLE_VALUE ) return FALSE;
		WriteFile(fp,&bmfh,sizeof(bmfh),&dw,NULL);
		WriteFile(fp,&bmih,sizeof(bmih),&dw,NULL);
		WriteFile(fp,pixelp,oip->size,&dw,NULL);
		CloseHandle(fp);
		if( dw == 0 ) return FALSE;
		oip->func_update_preview();
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		出力プラグイン設定関数
//---------------------------------------------------------------------
LRESULT CALLBACK func_config_proc( HWND hdlg,UINT umsg,WPARAM wparam, LPARAM lparam )
{
	switch( umsg ) {
		case WM_INITDIALOG:
			SetDlgItemText(hdlg,IDC_EDIT0,config.name);
			return TRUE;
		case WM_COMMAND:
			switch( LOWORD(wparam) ) {
				case IDCANCEL:
					EndDialog(hdlg,LOWORD(wparam));
					break;
				case IDOK:
					GetDlgItemText(hdlg,IDC_EDIT0,config.name,sizeof(config.name));
					EndDialog(hdlg,LOWORD(wparam));
					break;
			}
			break;
	}
	return FALSE;
}

BOOL func_config( HWND hwnd,HINSTANCE dll_hinst )
{
	DialogBox(dll_hinst,"CONFIG",hwnd,(DLGPROC)func_config_proc);
	return TRUE;
}

int func_config_get( void *data,int size )
{
	if( data ) memcpy(data,&config,sizeof(config));
	return sizeof(config);
}

int func_config_set( void *data,int size )
{
	if( size != sizeof(config) ) return NULL;
	memcpy(&config,data,size);
	return size;
}


