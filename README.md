# AviUtl Plugin SDK  for ver0.99m  
<div style="text-align: right;">by ＫＥＮくん</div>

### **※これは非公式のSDKのミラーです。**  
ミラーを公開するにあたり、見やすくするために、このREADMEを書き換え、フォルダ構造を一新しています。

## はじめに 
AviUtlのPluginを作成するためのSDKです。
Pluginにより画像フィルタや他のファイル形式の入出力など
さまざまな機能をAviUtlに追加することが出来ます。

## 内容
各種ヘッダーファイルとサンプルプラグインが含まれています。
各種サンプルプラグインは作成するときの参考にしてみてください。

## フィルタプラグイン (.auf) - `filter`フォルダ内
画像の加工編集などのプラグインです。
フォルダ内の直下に[`filter.h`](filter/filter.h)が含まれており、[`example`](filter/example)内にサンプルが配置されています。

- video  
  	画像を加工するプラグインです。

- audio  
		音量を変更するプラグインです。

- disp  
		前後のフレームを表示するプラグインです。

- loupe  
		ルーペ表示するプラグインです。

- edit  
		フレームの削除や張り付けなどの編集をするプラグインです。

- interlace  
		インターレースの解除をするプラグインです。

- importexport  
		24fps周期の情報をファイルにインポート＆エクスポートするプラグインです。

- multithread  
		マルチスレッドで画像を加工するプラグインです。

- yuy2  
		YUY2フィルタモードで画像を加工するプラグインです。

## 入力プラグイン (.aui) - `input`フォルダ内
他のファイル形式を読み込むプラグインです。
フォルダ内の直下に[`input.h`](input/input.h)が含まれており、[`example`](input/example)内にサンプルが配置されています。

- avi_input  
		AVIファイルを読み込むプラグインです。

## 出力プラグイン (.auo) - `output`フォルダ内
他のファイル形式に出力するプラグインです。
フォルダ内の直下に[`output.h`](output/output.h)が含まれており、[`example`](output/example)内にサンプルが配置されています。

- bmp_output  
		連番BMPファイルで出力するプラグインです。

## 色変換プラグイン (.auc) - `color`フォルダ内
画像データ入出力時の色空間を変換をするプラグインです。
フォルダ内の直下に[`color.h`](color/color.h)が含まれており、[`example`](color/example)内にサンプルが配置されています。

- yuy2_color  
		色空間を変換するプラグインです。

## 言語拡張リソース (.aul) - `language`フォルダ内
対応言語を拡張するためのリソースです。
[`example`](language/example)内にサンプルが配置されています。

- jp_resource  
		日本語のリソースです。


## 使用上の注意

このプログラムはフリーソフトウェアです。
このプログラムの使用によって何らかの障害が発生した場合でも、
作者は一切の責任を負わないものとします。


## ライセンス

AviUtlPluginSDK は MIT License の元で公開されています。
ライセンスについては [LICENSE](LICENSE) をご覧ください。

## 

## 履歴 
```

2000/10/16	[auo]出力プラグインを追加。
		[aul]言語拡張リソースを追加。

2000/10/21	[auf]ダイアログを開いてファイル名を取得する関数を追加。
		[auf]インポート＆エクスポート機能を追加しサンプルソースを付属した。
		[auf]フィルタの拡張情報にのフィルタ名を含めれるようにした。
		[aul]AviUtl(ver0.96c)で拡張されたリソースを追加。
		[auo]出力プラグインの内部情報の設定関数を追加。
		[auo]出力フレームのフラグを取得する関数を追加。

2000/11/20	[auo]出力プラグインでプレビューを表示する関数を追加。
		[auf]INIファイルに読み書きする関数を追加。
		[aul]言語拡張リソースのSTRINGTABLEでのフォント指定を削除。

2000/12/3	[auf]指定編集フレームのソースファイル情報を取得する関数を追加。

2001/1/11 	[auf]get_pixel_filtered()の機能を拡張。
		[auf]セーブ中のみフィルタを有効時にget_pixel_filtered()がフィルタ前の画像になっていたのを修正。
		[auf]セーブ中のみフィルタを有効時に表示フィルタのfunc_proc()が実行されていなかったのを修正。
		[auf]外部関数にget_sys_info()を追加。
		[auf]外部関数にget_filterp()を追加。

2002/1/5	[aui]入力プラグインを追加。
		[aul]VFPlugin情報を入力プラグイン情報に変更。
		[aul]ブレンド読み込みのリソースを削除。
		[aul]メニューリソースの配置を少し変更。
		[aul]オーディオフィルタ順序設定のリソースを追加。
		[aul]入力プラグインのリソースを追加。
		[auf]YCbCrの範囲を変更。

2002/1/21	[auf]YCbCrの範囲を変更。
		[auf]外部関数にget_ycp_filtering()を追加。
		[auf]外部関数にget_audio_filtering()を追加。

2002/1/28	[aul]STRINGTABLEの109-112を追加。
		[aul]全てのフィルタをOFFにするのリソースを追加。
		[aul]メニューリソースの配置を少し変更。
		[auf]FILTER_PROC_INFOにフィールドオーダーのフラグを追加。
		[auf]外部関数にset_ycp_filtering_cache_size()を追加。
		[auf]外部関数にget_ycp_filtering_cache()を追加。

2002/2/10	[aui]INPUT_INFOにhandlerを追加。
		[aul]STRINGTABLEの1035を追加。

2002/5/3	[aul]STRINGTABLEの113-124を追加。
		[aul]環境をプロファイルに名前を変更。
		[aul]MENUにシステムの設定のリソースを追加。
		[aul]MENUにコーデックの設定のリソースを追加。
		[aul]MENUに選択範囲に貼り付けのリソースを追加。
		[aul]MENUにプロファイルに圧縮の設定を保持するのリソースを追加。
		[aul]MENUにバッチ登録を保持するのリソースを追加。
		[aul]LOAD_OPTIONダイアログに選択範囲に読み込むを追加。
		[aul]SAVE_OPTIONダイアログにインターリーブを追加し配置を少し変更。
		[aul]GHOSTダイアログのリソースを変更。
		[aul]BATCHダイアログのリソースを変更。
		[aul]再生ウィンドウのオーバーレイ表示のリソースを追加。
		[aul]MENUのTransportCodecServerのリソースを削除。
		[aul]MENUの奇数ラインを１フレーム遅らせるのリソースを削除。
		[aul]MENUの圧縮の設定を保存するのリソースを削除。
		[auf]インターレース解除フィルタ機能を追加しサンプルソースを付属した。
		[auf]フィルタ構造体にfunc_is_saveframe()を追加。
		[auf]フィルタ構造体にex_data_defを追加。
		[auf]外部関数にget_ycp_source_cache()を追加。
		[auf]外部関数にget_disp_pixelp()を追加。
		[auf]外部関数にget_pixel_source()を追加。
		[auf]外部関数にget_pixel_filtered_ex()を追加。
		[auf]FILTER_PROC_INFOに解除方法反転のフラグを追加。
		[auf]FILTER_PROC_INFOのpixelpを無効にした。
		[aui]画像をYUY2で入力出来るようにした。
		[auo]外部関数にfunc_get_video_ex()を追加。

2002/5/7	[aul]SYSTEMダイアログのリソースを追加。
		[aul]CODECダイアログのリソースを追加。

2002/6/24	[aul]STRINGTABLEの125-126を追加。
		[aul]STRINGTABLEの68,80を変更。
		[aul]SYSTEMダイアログのリソースを変更。
		[aul]MENUに任意フレーム数移動のリソースを追加。
		[aul]MENUの５フレーム移動のリソースを削除。

2002/8/5	[auf]フィルタ構造体のフラグにFILTER_FLAG_NO_INIT_DATAを追加。
		[auf]フィルタ構造体のフラグにFILTER_FLAG_MAIN_MESSAGEを追加。
		[auf]サンプルフィルタを少し変更。
		[aul]STRINGTABLEの127,1036-1037を追加。
		[aul]SYSTEMダイアログのリソースを変更。
		[aul]BATCHダイアログのリソースを変更。
		[aul]MENUの環境設定、バッチ出力、ショートカット部分のリソースを変更。
		[aul]SAVEFILEダイアログのリソースを削除。
		[aul]INTERLEAVEダイアログのリソースを削除。

2002/8/12	[auf]外部関数にget_ycp_filtering_cache_ex()を追加。

2003/8/16	[auf]func_update()にどの項目が変更されたかの引数を追加。
		[auf]SYS_INFOにいくつかの項目を追加。
		[aul]STRINGTABLEの1281,1038を追加。
		[aul]STRINGTABLEの62,1001,1006,1008,1016,1017を変更。
		[aul]STRINGTABLEの20,47,48,65,70,71,72,103,104,117,118,119,120,121,126,127,1027を削除。
		[aul]MENUの24fps周期の編集の項目を削除
		[aul]MENUの拡大表示の項目を変更
		[aul]SYSTEMダイアログのリソースを変更。
		[aul]GHOSTダイアログのリソースを削除
		[aul]INTER24ダイアログのリソースを削除

2007/11/03	[auf]外部関数にexec_multi_thread_func()を追加。
		[auf]multithread_filterサンプルを追加。
		[auf]yuy2_filterサンプルを追加。

2008/1/12	[auf]フィルタ構造体にfunc_project_load(),func_project_save()を追加。
		[auf]フィルタ構造体にfunc_modify_title()を追加。
		[auf]func_WndProcのメッセージにWM_FILTER_MAIN_MOUSE_DBLCLKを追加。
		[auf]FILTERのcheck_defaultでボタンを作る設定を追加。
		[auf]FILTERにdll_pathの項目を追加。

2008/2/17	[auf]外部関数にcreate_yc(),delete_yc()を追加。
		[auf]外部関数にresize_yc()を追加。
		[auf]外部関数にcopy_yc()を追加。
		[auf]外部関数にdraw_text()を追加。
		[auf]サンプルフィルタを少し変更。
		[auf]設定ウィンドウのサイズ指定方法を追加。

2008/5/18	[aui]画像をPIXEL_YCで入力出来るようにした。
		[auo]func_get_video_ex()でPIXEL_YCの画像データを取得できるようにした。
		[auf]FILE_INFOにframe_nの項目を追加。
		[auf]外部関数にavi_file_open(),avi_file_close(),avi_file_read_video(),avi_file_read_audio()を追加。

2008/5/22	[auf]外部関数のrgb2yc(),yc2rgb()をMULTI_THREAD_FUNC関数内から呼べるようにした。
		[auf]SYS_INFOにhfontの項目を追加。

2008/7/29	[auo]サンプルソース(bmp_output)の不具合を修正。
		[auo]output.hに戻り値がデータポインタになっている外部関数の説明を追記。
		[auf]SYS_INFOにbuildの項目を追加。
		[auf]外部関数のavi_file_open()にいくつかのflagを追加。
		[auf]外部関数にavi_file_get_video_pixelp(),get_avi_file_filter()を追加。
		[auf]FILE_INFOにvideo_decode_format,video_decode_bitの項目を追加。
		[auf]1つのaufファイルで複数のフィルタを登録できるようにした。
		[auf]サンプルソース(video_filter)に複数のフィルタを登録する方法のコメントを追加。

2008/8/30	[auf]外部関数のavi_file_open()にいくつかのflagを追加。
		[auf]外部関数にavi_file_read_audio_sample(),avi_file_set_audio_sample_rate()を追加。

2009/1/24	[auo]OUTPUT_INFOのflagにOUTPUT_INFO_FLAG_BATCHを追加。

2009/5/30	[auf]外部関数にget_frame_status_table()を追加。
		[auf]サンプルソース(edit_filter)にプロジェクトファイルへデータを保存する方法を追加。
		[auc]色変換プラグインのサンプルソース(yuy2_color)を追加。

2009/9/19	[auf]外部関数にset_undo()を追加。
		[auf]サンプルソース(edit_filter)にアンドゥ設定の処理を追加。
		[auf]外部関数にadd_menu_item()を追加。

2010/2/22	[auf]外部関数にedit_open(),edit_close(),edit_output(),set_config()を追加。
		[auf]WM_FILTER_FILE_UPDATEを追加。
		[auf]プロファイルに関する説明を修正。

2011/12/3	PluginSDKをMITライセンスとして説明を追加。

2012/1/3	[auf]60FPS読み込み時にfunc_is_keyframe()のフレーム番号が正しくなかったのを修正。
		[aui]WAVE_FORMAT_EXTENSIBLE(KSDATAFORMAT_SUBTYPE_PCM)32bit2ch以下のPCMを入力出来るようにした。
		[aui]INPUT_INFOのflagにINPUT_INFO_FLAG_VIDEO_RANDOM_ACCESSを追加。
		[auf]func_WndProcのメッセージをいくつか追加。
		[aul]AviUtl(ver0.99k)のリソースに対応。

2012/5/2	[auc]COLOR_PROC_INFOのformatの説明にPIXEL_YCが抜けていたのを修正。
		[aul]SYSTEMダイアログのリソースを変更。

2012/6/19	[aul]SYSTEMダイアログのリソースを変更。


```