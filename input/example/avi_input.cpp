//----------------------------------------------------------------------------------
//		サンプルAVI(vfw経由)入力プラグイン  for AviUtl ver0.98以降
//----------------------------------------------------------------------------------
#include	<windows.h>
#include	<vfw.h>

#include	"input.h"


//---------------------------------------------------------------------
//		入力プラグイン構造体定義
//---------------------------------------------------------------------
INPUT_PLUGIN_TABLE input_plugin_table = {
	INPUT_PLUGIN_FLAG_VIDEO|INPUT_PLUGIN_FLAG_AUDIO,	//	フラグ
														//	INPUT_PLUGIN_FLAG_VIDEO	: 画像をサポートする
														//	INPUT_PLUGIN_FLAG_AUDIO	: 音声をサポートする
	"AVI File Reader (sample)",							//	プラグインの名前
	"AviFile (*.avi)\0*.avi\0",							//	入力ファイルフィルタ
	"AVI File Reader version 0.03 By ＫＥＮくん",		//	プラグインの情報
	func_init,											//	DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_exit,											//	DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
	func_open,											//	入力ファイルをオープンする関数へのポインタ
	func_close,											//	入力ファイルをクローズする関数へのポインタ
	func_info_get,										//	入力ファイルの情報を取得する関数へのポインタ
	func_read_video,									//	画像データを読み込む関数へのポインタ
	func_read_audio,									//	音声データを読み込む関数へのポインタ
	func_is_keyframe,									//	キーフレームか調べる関数へのポインタ (NULLなら全てキーフレーム)
	func_config,										//	入力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
};


//---------------------------------------------------------------------
//		入力プラグイン構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C INPUT_PLUGIN_TABLE __declspec(dllexport) * __stdcall GetInputPluginTable( void )
{
	return &input_plugin_table;
}


//---------------------------------------------------------------------
//		ファイルハンドル構造体
//---------------------------------------------------------------------
typedef struct {
	int				flag;
	PAVIFILE		pfile;
	PAVISTREAM		pvideo,paudio;
	AVIFILEINFO		fileinfo;
	AVISTREAMINFO	videoinfo,audioinfo;
	void			*videoformat;
	LONG			videoformatsize;
	void			*audioformat;
	LONG			audioformatsize;
} FILE_HANDLE;
#define FILE_HANDLE_FLAG_VIDEO		1
#define FILE_HANDLE_FLAG_AUDIO		2


//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init( void )
{
	AVIFileInit();
	return TRUE;
}


//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit( void )
{
	AVIFileExit();
	return TRUE;
}


//---------------------------------------------------------------------
//		ファイルオープン
//---------------------------------------------------------------------
INPUT_HANDLE func_open( LPSTR file )
{
	FILE_HANDLE		*fp;
	int				i;
	PAVISTREAM		pas;
	AVISTREAMINFO	asi;

	fp = (FILE_HANDLE *)GlobalAlloc(GMEM_FIXED,sizeof(FILE_HANDLE));
	if( fp == NULL ) return NULL;
	ZeroMemory(fp,sizeof(FILE_HANDLE));

	if( AVIFileOpen(&fp->pfile,file,OF_READ,NULL) != 0 ) {
		GlobalFree(fp);
		return NULL;
	}

	if( AVIFileInfo(fp->pfile,&fp->fileinfo,sizeof(fp->fileinfo)) == 0 ) {
		for(i=0;i<fp->fileinfo.dwStreams;i++) {
			if( AVIFileGetStream(fp->pfile,&pas,0,i) == 0 ) {
				AVIStreamInfo(pas,&asi,sizeof(asi));
				if( asi.fccType == streamtypeVIDEO ) {
					//	ビデオストリームの設定
					fp->pvideo = pas;
					fp->videoinfo = asi;
					AVIStreamFormatSize(fp->pvideo,0,&fp->videoformatsize);
					fp->videoformat = (FILE_HANDLE *)GlobalAlloc(GMEM_FIXED,fp->videoformatsize);
					if( fp->videoformat ) {
						AVIStreamReadFormat(fp->pvideo,0,fp->videoformat,&fp->videoformatsize);
						fp->flag |= FILE_HANDLE_FLAG_VIDEO;
					} else {
						AVIStreamRelease(pas);
					}
				} else if( asi.fccType == streamtypeAUDIO ) {
					//	オーディオストリームの設定
					fp->paudio = pas;
					fp->audioinfo = asi;
					AVIStreamFormatSize(fp->paudio,0,&fp->audioformatsize);
					fp->audioformat = (FILE_HANDLE *)GlobalAlloc(GMEM_FIXED,fp->audioformatsize);
					if( fp->videoformat ) {
						AVIStreamReadFormat(fp->paudio,0,fp->audioformat,&fp->audioformatsize);
						fp->flag |= FILE_HANDLE_FLAG_AUDIO;
					} else {
						AVIStreamRelease(pas);
					}
				} else {
					AVIStreamRelease(pas);
				}
			}
		}
	}

	return fp;
}


//---------------------------------------------------------------------
//		ファイルクローズ
//---------------------------------------------------------------------
BOOL func_close( INPUT_HANDLE ih )
{
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;

	if( fp ) {
		if( fp->flag&FILE_HANDLE_FLAG_AUDIO ) {
			AVIStreamRelease(fp->paudio);
			GlobalFree(fp->audioformat);
		}
		if( fp->flag&FILE_HANDLE_FLAG_VIDEO ) {
			AVIStreamRelease(fp->pvideo);
			GlobalFree(fp->videoformat);
		}
		AVIFileRelease(fp->pfile);
		GlobalFree(fp);
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		ファイルの情報
//---------------------------------------------------------------------
BOOL func_info_get( INPUT_HANDLE ih,INPUT_INFO *iip )
{
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;

	iip->flag = 0;

	if( fp->flag&FILE_HANDLE_FLAG_VIDEO ) {
		iip->flag |= INPUT_INFO_FLAG_VIDEO;
		iip->rate = fp->videoinfo.dwRate;
		iip->scale = fp->videoinfo.dwScale;
		iip->n = fp->videoinfo.dwLength;
		iip->format = (BITMAPINFOHEADER *)fp->videoformat;
		iip->format_size = fp->videoformatsize;
		iip->handler = fp->videoinfo.fccHandler;
	}

	if( fp->flag&FILE_HANDLE_FLAG_AUDIO ) {
		iip->flag |= INPUT_INFO_FLAG_AUDIO;
		iip->audio_n = fp->audioinfo.dwLength;
		iip->audio_format = (WAVEFORMATEX *)fp->audioformat;
		iip->audio_format_size = fp->audioformatsize;
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		画像読み込み
//---------------------------------------------------------------------
int func_read_video( INPUT_HANDLE ih,int frame,void *buf )
{
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;
	LONG		videosize;
	LONG		size;

	AVIStreamRead(fp->pvideo,frame,1,NULL,NULL,&videosize,NULL);
	if( AVIStreamRead(fp->pvideo,frame,1,buf,videosize,&size,NULL) ) return 0;
	return size;
}


//---------------------------------------------------------------------
//		音声読み込み
//---------------------------------------------------------------------
int func_read_audio( INPUT_HANDLE ih,int start,int length,void *buf )
{
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;
	LONG		size;
	int			samplesize;

	samplesize = ((WAVEFORMATEX *)fp->audioformat)->nBlockAlign;
	if( AVIStreamRead(fp->paudio,start,length,buf,samplesize*length,NULL,&size) ) return 0;
	return size;
}


//---------------------------------------------------------------------
//		キーフレーム情報
//---------------------------------------------------------------------
BOOL func_is_keyframe( INPUT_HANDLE ih,int frame )
{
	FILE_HANDLE	*fp = (FILE_HANDLE *)ih;

	return AVIStreamIsKeyFrame(fp->pvideo,frame);
}


//---------------------------------------------------------------------
//		設定ダイアログ
//---------------------------------------------------------------------
BOOL func_config( HWND hwnd,HINSTANCE dll_hinst )
{
	MessageBox(hwnd,"サンプルダイアログ","入力設定",MB_OK);

	//	DLLを開放されても設定が残るように保存しておいてください。

	return TRUE;
}


