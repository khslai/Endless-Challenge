//=============================================================================
//
// ゲージヘッダー [Gauge.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _GAUGE_H_
#define _GAUGE_H_


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************
// ゲージ構造体
typedef	struct
{
	VERTEX_2D	VertexWk[VERTEX_NUM];		// 頂点情報格納ワーク
	int			Type;						// ゲージの種類
}GAUGE;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitGauge(bool FirstInit);
// 終了処理
void UninitGauge(void);
// 更新処理
void UpdateGauge(void);
// 描画処理
void DrawGauge(void);

#endif
