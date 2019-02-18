//=============================================================================
//
// ステージ遷移ヘッダー [Transition.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _TRANSITION_H_
#define _TRANSITION_H_


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitTransition(bool FirstInit);
// 終了処理
void UninitTransition(void);
// 更新処理
void UpdateTransition(void);
// 描画処理
void DrawTransition(void);
// フェードの状態設定
void SetTransition(int State);
// フェードの状態取得
int GetTransition();

#endif
