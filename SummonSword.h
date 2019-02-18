//=============================================================================
//
// 剣の召喚ヘッダー [SummonSword.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _SUMMONSWORD_H_
#define _SUMMONSWORD_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum SummonSwordState
{
	SetSummonEffect,		// 剣の召喚のエフェクトを設置する
	SetFireEffect,			// 剣の炎のエフェクトを設置する
	WaitRolling,			// プレイヤーの回避を待っている
	AttackStart,			// プレイヤーが回避する瞬間攻撃開始
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3				Pos;				// 座標
	D3DXVECTOR3				Rot;				// 向き(回転)
	D3DXVECTOR3				Scale;				// 大きさ(スケール)
	LPDIRECT3DTEXTURE9		*Texture;			// テクスチャへのポインタ
	LPD3DXMESH				Mesh;				// メッシュ情報へのポインタ
	LPD3DXBUFFER			MaterialBuffer;		// マテリアル情報へのポインタ
	DWORD					MaterialNum;		// マテリアル情報の数
	D3DXMATRIX				WorldMatrix;		// ワールドマトリックス
	CAPSULE					HitCapsule;			// 当たり判定用カプセル
	int						EffectID;			// エフェクト番号
	int						Count;				// 存在カウント
	int						State;				// 状態
	bool					Use;				// 使用フラグ
	bool					BurnedFire;			// 剣の炎を設置したかのフラグ
}SUMMONSWORD;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitSummonSword(bool FirstInit);
// 終了処理
void UninitSummonSword(void);
// 更新処理
void UpdateSummonSword(void);
// 描画処理
void DrawSummonSword(void);
// ポインタを取得する
SUMMONSWORD *GetSummonSword(void);

#endif
