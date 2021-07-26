//----------------------------------------------------------------------------------
//		サンプル表示プラグイン(フィルタプラグイン)  for AviUtl ver0.98c以降
//----------------------------------------------------------------------------------
#include <windows.h>

#include "filter.h"

#define	LOUPE_W			32
#define	LOUPE_H			32
#define	WINDOW_W		(LOUPE_W*7+6)
#define	WINDOW_H		(LOUPE_H*7+21)

typedef struct {
	short	x,y;
} SHORTPOINT;
static void				*buf0=NULL;
static BITMAPINFO		bmi;
static SHORTPOINT		pos;


//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE|FILTER_FLAG_MAIN_MESSAGE|FILTER_FLAG_WINDOW_SIZE|FILTER_FLAG_DISP_FILTER|FILTER_FLAG_EX_INFORMATION,
	WINDOW_W,WINDOW_H,
	"簡易ルーペ",
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
	"簡易ルーペ version 0.01 by ＫＥＮくん",
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
}

void mem_free( void )
{
	if( buf0 ) {
		VirtualFree(buf0,0,MEM_RELEASE);
		buf0 = NULL;
	}
}


//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init( FILTER *fp )
{
	return TRUE;
}


//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit( FILTER *fp )
{
	mem_free();
	return TRUE;
}


//---------------------------------------------------------------------
//		表示
//---------------------------------------------------------------------
void disp( FILTER *fp,void *editp,SHORTPOINT *pp )
{
	HDC					hdc0;
	RECT				rc;
	static int		 	editing = FALSE;

	GetClientRect(fp->hwnd,&rc);
	hdc0 = GetDC(fp->hwnd);

	if( fp ) editing = fp->exfunc->is_editing(editp);
	if( buf0 && editing ) {
		if( pp ) {
			pos.x = pp->x-LOUPE_W/2;
			pos.y = pp->y-LOUPE_H/2;
		}
		if(      pos.x < 0                              ) pos.x = 0;
		else if( pos.x > bmi.bmiHeader.biWidth-LOUPE_W  ) pos.x = bmi.bmiHeader.biWidth-LOUPE_W;
		if(      pos.y < 0                              ) pos.y = 0;
		else if( pos.y > bmi.bmiHeader.biHeight-LOUPE_H ) pos.y = bmi.bmiHeader.biHeight-LOUPE_H;
		SetStretchBltMode(hdc0,STRETCH_DELETESCANS);
		StretchDIBits(hdc0,0,0,rc.right,rc.bottom,pos.x,bmi.bmiHeader.biHeight-pos.y-LOUPE_H,LOUPE_W,LOUPE_H,buf0,&bmi,DIB_RGB_COLORS,SRCCOPY);
	} else {
		FillRect(hdc0,&rc,(HBRUSH)(COLOR_ACTIVEBORDER+1));
	}

	ReleaseDC(fp->hwnd,hdc0);
}


//---------------------------------------------------------------------
//		フィルタされた画像をバッファにコピー
//---------------------------------------------------------------------
BOOL func_proc( FILTER *fp,FILTER_PROC_INFO *fpip )
{
	int		w,h;

	if( buf0 ) {
		fp->exfunc->get_pixel_filtered_ex(fpip->editp,fpip->frame,buf0,&w,&h,NULL);
		ZeroMemory(&bmi,sizeof(bmi));
		bmi.bmiHeader.biSize        = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth       = w;
		bmi.bmiHeader.biHeight      = h;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 24;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biPlanes      = 1;
		disp(fp,fpip->editp,NULL);
	}

	return TRUE;
}


//---------------------------------------------------------------------
//		WndProc
//---------------------------------------------------------------------
BOOL func_WndProc( HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam,void *editp,FILTER *fp )
{
	//	TRUEを返すと全体が再描画される

	switch(message) {
		case WM_PAINT:
		case WM_FILTER_CHANGE_EDIT:
			disp(fp,editp,NULL);
			break;
		case WM_FILTER_CHANGE_WINDOW:
			if( fp->exfunc->is_filter_window_disp(fp) ) {
				mem_alloc(fp);
				return TRUE;
			} else {
				mem_free();
			}
			break;
		case WM_FILTER_MAIN_MOUSE_MOVE:
			disp(fp,editp,(SHORTPOINT *)&lparam);
			break;
	}
	return FALSE;
}


