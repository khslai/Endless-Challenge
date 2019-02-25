//=============================================================================
//
// ステージヘッダー [Stage.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _STAGE_H_
#define _STAGE_H_

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// ステージ構造体
typedef struct
{
	D3DXVECTOR3			Pos;					// モデルの位置
	D3DXVECTOR3			Rot;					// モデルの向き(回転)
	D3DXVECTOR3			Scale;					// モデルの大きさ(スケール)
	D3DXMATRIX			WorldMatrix;			// ワールドマトリックス
	LPDIRECT3DTEXTURE9	*Texture;				// テクスチャへのポインタ
	LPD3DXMESH			Mesh;					// メッシュ情報へのポインタ
	LPD3DXBUFFER		MaterialBuffer;			// マテリアル情報へのポインタ
	DWORD				MaterialNum;			// マテリアル情報の数
}STAGE;

// デプスバッファ構造体
typedef struct
{
	LPDIRECT3DTEXTURE9	*ShadowMapTexture;		// シャドウマップテクスチャ
	LPD3DXEFFECT		Effect;					// 深度バッファシャドウエフェクト
	D3DXHANDLE			Para_WorldMatrix;		// ワールド変換行列ハンドル
	D3DXHANDLE			Para_WVPMatrix;			// World x View x Proj行列ハンドル
	D3DXHANDLE			Para_WVPMatrix_Light;	// ライトのWorld x View x Proj行列ハンドル
	D3DXHANDLE			Para_CosTheta;			// シャドウ角度ハンドル
	D3DXHANDLE			Para_ShadowMapTexture;	// シャドウマップテクスチャハンドル
	D3DXHANDLE			Technique;				// テクニックへのハンドル
}DEPTHBUFFER;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitStage(bool FirstInit);
// 終了処理
void UninitStage(void);
// 更新処理
void UpdateStage(void);
// 描画処理
void DrawStage(void);
// ステージのポインタを取得する
STAGE *GetStage(void);

#endif
