//=============================================================================
//
// 一時停止ヘッダー [Pause.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _PAUSE_H_
#define _PAUSE_H_

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 一時停止選択肢状態
enum PauseSelectState
{
	Pause_Resume,			// 続く
	Pause_Restart,			// 最初から
	Pause_ToTitle,			// 最初からの確認
	Pause_RestartCheck,		// タイトルへ
	Pause_ToTitleCheck,		// タイトルへの確認
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitPause(bool FirstInit);
// 終了処理
void UninitPause(void);
// 更新処理
void UpdatePause(void);
// 描画処理
void DrawPause(void);
// 選択肢の状態を取得する
SELECT *GetPauseSelect(void);

#endif
