//----------------------------------------------------------------------------------
//		サンプル編集プラグイン(フィルタプラグイン)  for AviUtl ver0.99i以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"
#include "edit_filter.h"

#define	WINDOW_W		320
#define	WINDOW_H		240


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_DISP_FILTER|FILTER_FLAG_WINDOW_HSCROLL|FILTER_FLAG_WINDOW_THICKFRAME|FILTER_FLAG_ALWAYS_ACTIVE|FILTER_FLAG_WINDOW_SIZE|FILTER_FLAG_PRIORITY_LOWEST|FILTER_FLAG_EX_INFORMATION,
	WINDOW_W,WINDOW_H,
	"簡易編集",
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	func_WndProc,
	NULL,NULL,
	NULL,
	NULL,
	"簡易編集 version 0.07 by ＫＥＮくん",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	func_project_load,
	func_project_save,
};


//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport) * __stdcall GetFilterTable( void )
{
	return &filter;
}


//---------------------------------------------------------------------
//		表示
//---------------------------------------------------------------------
void disp( HWND hwnd,FILTER *fp,void *editp,int n,int l,int w,int h )
{
	HDC					hdc;
	RECT				rc;
	BITMAPINFO			bmi;
	TCHAR				b[MAX_PATH];
	static TCHAR		*inter_txt[] = {
		"","反転","奇数","偶数","二重化","自動"
	};

	if( !fp->exfunc->is_filter_window_disp(fp) ) return;

	GetClientRect(hwnd,&rc);

	//	フレームの表示
	hdc = GetDC(hwnd);
	SetStretchBltMode(hdc,STRETCH_DELETESCANS);
	ZeroMemory(&bmi,sizeof(bmi));
	bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth       = w;
	bmi.bmiHeader.biHeight      = h;
	bmi.bmiHeader.biPlanes      = 1;
	bmi.bmiHeader.biBitCount    = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes      = 1;
	if( fp->exfunc->is_editing(editp) && l ) {
		StretchDIBits(hdc,0,0,rc.right,rc.bottom,0,0,w,h,fp->exfunc->get_pixelp(editp,n),&bmi,DIB_RGB_COLORS,SRCCOPY);
	} else {
		FillRect(hdc,&rc,(HBRUSH)(COLOR_ACTIVEBORDER+1));
	}
	ReleaseDC(hwnd,hdc);

	//	タイトルバーの表示
	if( fp->exfunc->is_editing(editp) && l ) {
		FRAME_STATUS	fs;
		fp->exfunc->get_frame_status(editp,n,&fs);
		wsprintf(b,"%s  [%d/%d]  %s %s",filter.name,n+1,l,fp->exfunc->get_config_name(editp,fs.config),inter_txt[fs.inter]);
		if( fp->exfunc->is_keyframe(editp,n)   ) lstrcat(b,"*");
		if( fp->exfunc->is_recompress(editp,n) ) lstrcat(b,"!");
		if( !fp->exfunc->is_saveframe(editp,n) ) lstrcat(b,"X");
		if( fs.edit_flag&EDIT_FRAME_EDIT_FLAG_KEYFRAME  ) lstrcat(b,"K");
		if( fs.edit_flag&EDIT_FRAME_EDIT_FLAG_MARKFRAME ) lstrcat(b,"M");
		if( fs.edit_flag&EDIT_FRAME_EDIT_FLAG_DELFRAME  ) lstrcat(b,"D");
		if( fs.edit_flag&EDIT_FRAME_EDIT_FLAG_NULLFRAME ) lstrcat(b,"N");
	} else {
		wsprintf(b,"%s",filter.name);
	}
	SetWindowText(hwnd,b);
}


//---------------------------------------------------------------------
//		スクロールバーの設定
//---------------------------------------------------------------------
void set_scrollbar( HWND hwnd,int n,int l )
{
	SCROLLINFO		si;

	si.cbSize = sizeof(si);
	si.fMask  = SIF_DISABLENOSCROLL|SIF_PAGE|SIF_POS|SIF_RANGE;
	si.nMin   = 0;
	si.nMax   = l-1;
	si.nPage  = 1;
	si.nPos   = n;
	SetScrollInfo(hwnd,SB_HORZ,&si,TRUE);

	return;
}


//---------------------------------------------------------------------
//		WndProc
//---------------------------------------------------------------------
#define	COPY_MODE_VIDEO	0
#define	COPY_MODE_AUDIO	1
#define	COPY_MODE_ALL	2
BOOL func_WndProc( HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam,void *editp,FILTER *fp )
{
	//	TRUEを返すと全体が再描画される

	int				i;
	static	int		frame,frame_n,frame_w,frame_h;
	static	int		copy_frame,copy_mode;
	FILE_INFO		fi;
	TCHAR			txt[256],txt2[256];

	switch(message) {
		//	描画要求
		case WM_PAINT:
		case WM_FILTER_CHANGE_WINDOW:
			disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
			break;

		//	コマンド
		case WM_COMMAND:
			if( fp->exfunc->is_editing(editp) != TRUE ) break;	//	編集中でなければ終了
		    switch(LOWORD(wparam)) {

				//	コピー
				case MID_EDIT_VIDEO_COPY:
					copy_frame = frame;
					copy_mode  = COPY_MODE_VIDEO;
					break;
				case MID_EDIT_AUDIO_COPY:
					copy_frame = frame;
					copy_mode  = COPY_MODE_AUDIO;
					break;
				case MID_EDIT_COPY:
					copy_frame = frame;
					copy_mode  = COPY_MODE_ALL;
					break;

				//	ペースト
				case MID_EDIT_PASTE:
					fp->exfunc->set_undo(editp);
					if( copy_mode == COPY_MODE_VIDEO ) {
						fp->exfunc->copy_video(editp,frame,copy_frame);
					} else if( copy_mode == COPY_MODE_AUDIO ) {
						fp->exfunc->copy_audio(editp,frame,copy_frame);
					} else {
						fp->exfunc->copy_frame(editp,frame,copy_frame);
					}
					return TRUE;	//	編集内容を更新したので再描画要求

				//	削除
				case MID_EDIT_DELETE:
					fp->exfunc->set_undo(editp);
					if( copy_frame >= frame ) copy_frame--;
					for(i=frame;i<frame_n-1;i++) {
						fp->exfunc->copy_frame(editp,i,i+1);
					}
					fp->exfunc->set_frame_n(editp,frame_n-1);
					return TRUE;	//	編集内容を更新したので再描画要求

				//	挿入
				case MID_EDIT_INSERT:
					fp->exfunc->set_undo(editp);
					if( copy_frame >= frame ) copy_frame++;
					for(i=frame_n;i>frame;i--) {
						fp->exfunc->copy_frame(editp,i,i-1);
					}
					fp->exfunc->copy_frame(editp,frame,copy_frame);
					fp->exfunc->set_frame_n(editp,frame_n+1);
					return TRUE;	//	編集内容を更新したので再描画要求

				case MID_EDIT_FILE_INFO:
					fp->exfunc->get_file_info(editp,&fi);
					ZeroMemory(txt,sizeof(txt));
					if( fi.flag&FILE_INFO_FLAG_VIDEO ) {
						wsprintf(txt,"ファイル名 : %s\nサイズ : %dx%d\nフレームレート : %d.%03dfps",
							fi.name,
							fi.w,
							fi.h,
							(int)((double)fi.video_rate/fi.video_scale),
							(int)((double)fi.video_rate*1000/fi.video_scale)%1000
						);
					}
					ZeroMemory(txt2,sizeof(txt2));
					if( fi.flag&FILE_INFO_FLAG_AUDIO ) {
						wsprintf(txt2,"\nサンプリングレート : %dkHz\nチャンネル数 : %dch",
							fi.audio_rate,
							fi.audio_ch
						);
					}
					lstrcat(txt,txt2);
					MessageBox(hwnd,txt,"ファイルの情報",MB_OK);
					break;
			}
			break;

		//	スクロールバー制御
		case WM_HSCROLL:
		    switch(LOWORD(wparam)) {
				case SB_LINEDOWN:
					frame++;
					if( frame > frame_n-1 ) frame = frame_n-1;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
				case SB_LINEUP:
					frame--;
					if( frame < 0 ) frame = 0;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
				case SB_PAGEDOWN:
					frame+=10;
					if( frame > frame_n-1 ) frame = frame_n-1;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
				case SB_PAGEUP:
					frame-=10;
					if( frame < 0 ) frame = 0;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
				case SB_THUMBTRACK:
					SCROLLINFO	si;
					MSG			msg;
					si.cbSize=sizeof(si);
					si.fMask=SIF_ALL;
					GetScrollInfo(hwnd,SB_HORZ,&si);
					if( PeekMessage(&msg,hwnd,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_NOREMOVE) ) break;
					frame = si.nTrackPos;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
 			        break;
			}
	        break;

		//	キー入力
		case WM_KEYDOWN:
		    switch(LOWORD(wparam)) {
				case VK_RIGHT:
					frame++;
					if( frame > frame_n-1 ) frame = frame_n-1;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
				case VK_LEFT:
					frame--;
					if( frame < 0 ) frame = 0;
					set_scrollbar(hwnd,frame,frame_n);
					disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
   			        break;
			}
	        break;

		case WM_FILTER_FILE_OPEN:
			frame = 0;
			copy_frame = copy_mode = 0;
			fp->exfunc->get_frame_size(editp,&frame_w,&frame_h);
		case WM_FILTER_UPDATE:
			if( fp->exfunc->is_editing(editp) != TRUE ) break;	//	編集中でなければ終了
			frame_n = fp->exfunc->get_frame_n(editp);
			set_scrollbar(hwnd,frame,frame_n);
			disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
			break;

		case WM_FILTER_INIT:
			HMENU	hmenu;
			hmenu = LoadMenu(fp->dll_hinst,"FILTER");
			SetMenu(hwnd,hmenu);
			DrawMenuBar(hwnd);
		case WM_FILTER_FILE_CLOSE:
			frame = frame_n = 0;
			set_scrollbar(hwnd,frame,frame_n);
			disp(hwnd,fp,editp,frame,frame_n,frame_w,frame_h);
			break;
	}

	return FALSE;
}


//---------------------------------------------------------------------
//		プロジェクトファイルへのデータ保存サンプル
//---------------------------------------------------------------------
typedef struct {
	int		a,b,c;
} PROJECT_FILE_DATA;
static PROJECT_FILE_DATA project_file_data;

BOOL func_project_load( FILTER *fp,void *editp,void *data,int size )
{
	if( size == sizeof(project_file_data) ) {
		memcpy(&project_file_data,data,sizeof(project_file_data));
	}
	return TRUE;
}

BOOL func_project_save( FILTER *fp,void *editp,void *data,int *size )
{
	*size = sizeof(project_file_data);
	if( data ) {
		memcpy(data,&project_file_data,sizeof(project_file_data));
	}
	return TRUE;
}


