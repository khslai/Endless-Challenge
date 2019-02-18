//=============================================================================
//
// カプセルヘッダー [CapsuleMesh.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _CAPSULEMESH_H_
#define _CAPSULEMESH_H_

typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 VtxBuffer;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	IdxBuffer;			// インデックスバッファへのポインタ
	D3DXMATRIX				WorldMatrix;		// ワールドマトリックス
	D3DXVECTOR3				P1;					// 左と右側二つの半球体の中心座標
	D3DXVECTOR3				P2;
	D3DXVECTOR3				PreP1;				// iフレイム前の左と右側二つの半球体の中心座標
	D3DXVECTOR3				PreP2;
	D3DXVECTOR3				Pos;				// カプセルの座標
	D3DXVECTOR3				Direction;			// 描画するときの方向ベクトル
	D3DXVECTOR3				DefaultDirVec;		// カプセルを作成するとき設置するベクトル
												// 例えば、DirectionVec = (0.0f,1.0f,0.0f)のとき
												// カプセルを回転させない場合、カプセルは真上に向く
	D3DCOLOR				Color;				// 色
	int						NumOfVtxBuffer;		// バッファの頂点数
	int						NumOfTriangle;		// 三角形ポリゴンの数
	float					Length;				// カプセルの長さ
	float					Radius;				// カプセルの半径
	bool					FromCenter;			// カプセル描画の始点は中心かどうか
} CAPSULE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT CreateCapsule(CAPSULE *Capsule, D3DXVECTOR3 Pos, D3DXVECTOR3 DirectionVec, float Length, float Radius, bool FromCenter);
// 終了処理
void UninitCapsule(CAPSULE *Capsule);
// 更新処理
void UpdateCapsule(CAPSULE *Capsule);
// 描画処理
void DrawCapsule(CAPSULE *Capsule,const D3DXMATRIX *WorldMatrix);

#endif
