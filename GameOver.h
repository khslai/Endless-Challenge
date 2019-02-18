//=============================================================================
//
// ゲームオーバーヘッダー [GameOver.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _GAMEOVER_H_
#define _GAMEOVER_H_


//*****************************************************************************
// マクロ定義
//*****************************************************************************
// ゲームオーバー選択肢状態
enum GameOverSelectState
{
	BGMStart,					// BGM鳴らすのが始まる
	Display,					// GameOver文字表示中
	GameOver_Restart,			// 最初から
	GameOver_RestartCheck,		// 最初からの確認
	GameOver_ToTitle,			// タイトルへ
	GameOver_ToTitleCheck,		// タイトルへの確認
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitGameOver(bool FirstInit);
// 終了処理
void UninitGameOver(void);
// 更新処理
void UpdateGameOver(void);
// 描画処理
void DrawGameOver(void);
// 選択肢の状態を取得する
SELECT *GetGameOverSelect(void);

#endif
