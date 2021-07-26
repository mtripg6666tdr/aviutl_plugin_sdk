//----------------------------------------------------------------------------------
//		サンプル表示プラグイン(フィルタプラグイン)  for AviUtl ver0.98c以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"

#define	WINDOW_W		(200+6)
#define	WINDOW_H		(150+21)

static void				*buf0=NULL,*buf1=NULL;
static BITMAPINFO		bmi;
static HWND				hwnd0,hwnd1;

LRESULT CALLBACK dlg_proc( HWND hdlg,UINT umsg,WPARAM wparam, LPARAM lparam );


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE|FILTER_FLAG_MAIN_MESSAGE|FILTER_FLAG_WINDOW_SIZE|FILTER_FLAG_DISP_FILTER|FILTER_FLAG_EX_INFORMATION,
	WINDOW_W,WINDOW_H,
	"前後表示",
	NULL,NULL,NULL,
	NULL,NULL,
	NULL,NULL,NULL,
	func_proc,
	func_init,
	func_exit,
	NULL,
	func_WndProc,
	NULL,NULL,
	NULL,
	NULL,
	"前後表示 version 0.01 by ＫＥＮくん",
	NULL,NULL,
	NULL,NULL,NULL,NULL,
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
//		メモリ管理
//---------------------------------------------------------------------
void mem_alloc( FILTER *fp )
{
	int			size;
	SYS_INFO 	si;

	fp->exfunc->get_sys_info(NULL,&si);
	size = si.vram_w*si.vram_h*3;
	if( buf0 == NULL ) buf0 = VirtualAlloc(NULL,size,MEM_COMMIT,PAGE_READWRITE);
	if( buf1 == NULL ) buf1 = VirtualAlloc(NULL,size,MEM_COMMIT,PAGE_READWRITE);
}

void mem_free( void )
{
	if( buf0 ) {
		VirtualFree(buf0,0,MEM_RELEASE);
		buf0 = NULL;
	}
	if( buf1 ) {
		VirtualFree(buf1,0,MEM_RELEASE);
		buf1 = NULL;
	}
}


//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init( FILTER *fp )
{
	hwnd0 = CreateDialog(fp->dll_hinst,"DLG",fp->hwnd,(DLGPROC)dlg_proc);
	hwnd1 = fp->hwnd;
	SetWindowPos(hwnd0,NULL,0,0,WINDOW_W,WINDOW_H,SWP_NOMOVE|SWP_NOZORDER);

	return TRUE;
}


//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit( FILTER *fp )
{
	DestroyWindow(hwnd0);
	mem_free();
	return TRUE;
}


//---------------------------------------------------------------------
//		表示
//---------------------------------------------------------------------
void disp( FILTER *fp,void *editp )
{
	HDC					hdc0,hdc1;
	RECT				rc;
	static int		 	editing = FALSE;

	if( !IsWindowVisible(hwnd1) ) return;

	GetClientRect(hwnd0,&rc);
	hdc0 = GetDC(hwnd0);
	hdc1 = GetDC(hwnd1);

	if( fp ) editing = fp->exfunc->is_editing(editp);
	if( buf0 && buf1 && editing ) {
		SetStretchBltMode(hdc0,STRETCH_DELETESCANS);
		SetStretchBltMode(hdc1,STRETCH_DELETESCANS);
		StretchDIBits(hdc0,0,0,rc.right,rc.bottom,0,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,buf0,&bmi,DIB_RGB_COLORS,SRCCOPY);
		StretchDIBits(hdc1,0,0,rc.right,rc.bottom,0,0,bmi.bmiHeader.biWidth,bmi.bmiHeader.biHeight,buf1,&bmi,DIB_RGB_COLORS,SRCCOPY);
	} else {
		FillRect(hdc0,&rc,(HBRUSH)(COLOR_ACTIVEBORDER+1));
		FillRect(hdc1,&rc,(HBRUSH)(COLOR_ACTIVEBORDER+1));
	}

	ReleaseDC(hwnd0,hdc0);
	ReleaseDC(hwnd1,hdc1);
}


//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
LRESULT CALLBACK dlg_proc( HWND hdlg,UINT umsg,WPARAM wparam, LPARAM lparam )
{
	switch( umsg ) {
		case WM_PAINT:
			disp(NULL,NULL);
			break;
	}
	return FALSE;
}


//---------------------------------------------------------------------
//		フィルタされた画像をバッファにコピー
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
	if( buf0 && buf1 ) {
		ZeroMemory(&bmi,sizeof(bmi));
		bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth       = fpip->org_w;
		bmi.bmiHeader.biHeight      = fpip->org_h;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biPlanes      = 1;
		fp->exfunc->get_pixel_source(fpip->editp,fp->exfunc->get_frame(fpip->editp)-1,buf0,NULL);
		fp->exfunc->get_pixel_source(fpip->editp,fp->exfunc->get_frame(fpip->editp)+1,buf1,NULL);
		disp(fp,fpip->editp);
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		WndProc
//---------------------------------------------------------------------
BOOL func_WndProc( HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam,void *editp,FILTER *fp )
{
	//	TRUEを返すと全体が再描画される

	RECT	rc;

	switch(message) {
		case WM_PAINT:
		case WM_FILTER_CHANGE_EDIT:
			disp(fp,editp);
			break;
		case WM_FILTER_CHANGE_WINDOW:
			if( fp->exfunc->is_filter_window_disp(fp) ) {
				ShowWindow(hwnd0,SW_SHOW);
				mem_alloc(fp);
				return TRUE;
			} else {
				ShowWindow(hwnd0,SW_HIDE);
				mem_free();
			}
			break;
		case WM_FILTER_MAIN_MOVESIZE:
			GetWindowRect((HWND)lparam,&rc);
			SetWindowPos(hwnd0,NULL,rc.left-WINDOW_W,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
			SetWindowPos(hwnd1,NULL,rc.right        ,rc.top,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
			break;
	}

	return FALSE;
}


