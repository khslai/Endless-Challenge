//=============================================================================
//
// カメラヘッダー [ThirdPersonCamera.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _THIRDPERSONCAMERA_H_
#define _THIRDPERSONCAMERA_H_

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// カメラ構造体
typedef struct
{
	D3DXVECTOR3		Pos;					// 座標
	D3DXVECTOR3		Up;						// 上方向ベクトル
	D3DXVECTOR3		At;						// 注視点
	D3DXVECTOR3		Direction;				// 向き
	float			Distance;				// 座標と注視点の距離
	float			Angle_Yaxis;			// 回転角度（Y軸を基準）
	float			Angle_Xaxis;			// 回転角度（X軸を基準）
	float			Rot;					// 回転
	bool			InReset;				// リセットフラグ
	bool			InLockOn;				// ロックオンフラグ
}CAMERA_3RD;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitCamera_3rd(bool FirstInit);
// 終了処理
void UninitCamera_3rd(void);
// 更新処理
void UpdateCamera_3rd(void);
// カメラを設置する
void SetCamera_3rd(void);
// カメラのポインタを取得する
CAMERA_3RD *GetCamera_3rd(void);
// カメラのビュー行列を取得する
D3DXMATRIX GetViewMatrix(void);
// カメラの射影変換行列を取得する
D3DXMATRIX GetProjMatrix(void);

#endif
