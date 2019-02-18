//=============================================================================
//
// 装備モデルヘッダー [Equipment.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _EQUIPMENT_H_
#define _EQUIPMENT_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3			Pos;				// モデルの位置
	D3DXVECTOR3			Rot;				// モデルの向き(回転)
	D3DXVECTOR3			Scale;				// モデルの大きさ(スケール)
	LPDIRECT3DTEXTURE9	*Texture;			// テクスチャへのポインタ
	LPD3DXMESH			Mesh;				// メッシュ情報へのポインタ
	LPD3DXBUFFER		MaterialBuffer;		// マテリアル情報へのポインタ
	DWORD				MaterialNum;		// マテリアル情報の数
	D3DXMATRIX			WorldMatrix;		// ワールドマトリックス
	CAPSULE				HitCapsule;			// 当たり判定用カプセル
	int					EffectID;			// エフェクト番号
}SWORD;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitEquipment(bool FirstInit);
// 終了処理
void UninitEquipment(void);
// 更新処理
void UpdateEquipment(void);
// 描画処理
void DrawEquipment(void);
// 剣のポインタを取得する
SWORD *GetSword(const char* Owner);

#endif
