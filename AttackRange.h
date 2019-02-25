//=============================================================================
//
// 当たり範囲表示ヘッダー [AttackRange.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _ATTACKRANGE_H_
#define _ATTACKRANGE_H_


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************
// 当たり範囲構造体
typedef struct
{
	LPD3DXMESH			Mesh;			// メッシュ情報へのポインタ
	D3DXVECTOR3			Pos;			// 座標
	D3DXVECTOR3			Rot;			// 回転
	D3DXMATRIX			WorldMatrix;	// ワールドマトリックス
	bool				Use;			// 使用フラグ
}RANGESPHERE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitAttackRange(bool FirstInit);
// 終了処理
void UninitAttackRange(void);
// 更新処理
void UpdateAttackRange(void);
// 描画処理
void DrawAttackRange(void);
// 当たり範囲を設置する
int SetRangeSphere(D3DXVECTOR3 Pos, float Radius);
// 当たり範囲を削除する
void DeleteRangeSphere(int Sphere_No);

#endif
