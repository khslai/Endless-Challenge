//=============================================================================
//
// シャドウマッピングヘッダー [ShadowMapping.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _SHADOWMAPPING_H_
#define _SHADOWMAPPING_H_

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// デプステクスチャ構造体
typedef struct
{
	LPDIRECT3DTEXTURE9	ShadowMapTexture;		// シャドウマップテクスチャ
	LPD3DXEFFECT		Effect;					// Z値プロットエフェクト
	D3DXMATRIX			WorldMatrix;			// ワールド変換行列
	D3DXHANDLE			Para_WVPMatrix;			// ワールド x ビュー x 射影変換行列ハンドル
	D3DXHANDLE			Technique;				// テクニックへのハンドル
}TEXCREATOR;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitShadowMapping(bool FirstInit);
// 終了処理
void UninitShadowMapping(void);
// 更新処理
void UpdateShadowMapping(void);
// 描画処理
void DrawShadowTexture(void);
// デプステクスチャのポインタを取得する
LPDIRECT3DTEXTURE9 *GetShadowMapTexture(void);

#endif
