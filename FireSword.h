//=============================================================================
//
// 炎の剣ヘッダー [FireSword.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _FIRESWORD_H_
#define _FIRESWORD_H_

#include "Effect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// エフェクトの種類
enum FireSwordEffect
{
	FireSword,
	FireSword_Phase2,
	SlashFire,
	SlashFire_Phase2,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// 剣の軌跡構造体
typedef struct
{
	D3DXVECTOR3			Vtx_LeftUp;			// 頂点(左上)
	D3DXVECTOR3			Vtx_RightUp;		// 頂点(右上)
	D3DXVECTOR3			Vtx_LeftDown;		// 頂点(左下)
	D3DXVECTOR3			Vtx_RightDown;		// 頂点(右下)
	D3DXCOLOR			Color;				// 軌跡の色
	bool				IsBack;				// 裏面かのフラグ
	bool				Use;				// 使用フラグ
}SURFACE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitFireSword(bool FirstInit);
// 終了処理
void UninitFireSword(void);
// 更新処理
void UpdateFireSword(void);
// 描画処理
void DrawFireSword(void);
// 剣を纏う炎を設置する
int SetFireSword(D3DXVECTOR3 Pos, D3DXVECTOR3 Direction);
// 斬撃の炎を設置する
int SetSlashFire(D3DXVECTOR3 Pos, D3DXVECTOR3 PrePos);
// 斬撃の軌跡を設置する
void SetSurface(D3DXVECTOR3 Vtx_LeftUp, D3DXVECTOR3 Vtx_RightUp, D3DXVECTOR3 Vtx_LeftDown, D3DXVECTOR3 Vtx_RightDown, D3DXCOLOR Color);
// エフェクトコントローラーを取得する
EFFECTCONTROLLER *GetFireSwordCtrl(void);

#endif
