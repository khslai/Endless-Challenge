//=============================================================================
//
// スカイボックスヘッダー [SkyBox.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

typedef struct
{
	D3DXVECTOR3				Pos;			// 位置
	D3DXVECTOR3				Rot;			// 向き(回転)
	D3DXVECTOR3				Scale;			// 大きさ(スケール)
}SKYBOX;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitSkyBox(bool FirstInit);
// 終了処理
void UninitSkyBox(void);
// 更新処理
void UpdateSkyBox(void);
// 描画処理
void DrawSkyBox(void);

#endif
