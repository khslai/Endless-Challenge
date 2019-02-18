//=============================================================================
//
// デバッグ表示ヘッダー [DebugProcess.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _DEBUGPROCESS_H_
#define _DEBUGPROCESS_H_

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitDebugProcess(bool FirstInit);
// 終了処理
void UninitDebugProcess(void);
// 更新処理
void UpdateDebugProcess(void);
// 描画処理
void DrawDebugProcess(void);
// デバッグメッセージ表示
void PrintDebugMsg(const char *fmt,...);

#endif