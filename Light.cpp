//=============================================================================
//
// ライト処理 [Light.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Light.h"
#include "DebugProcess.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DXMATRIX LightView, LightProj;	// ライトビュー変換・射影変換
D3DXVECTOR3 Pos = PositionZero;		// ライト座標
float Distance = 0.0f;				// 座標と注視点の距離
float Angle = 0.0f;					// ライトの角度

//=============================================================================
// ライトの初期化処理
//=============================================================================
HRESULT InitLight(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXVECTOR3 NormalizePos;
	int GameStage = GetGameStage();

	// 位置
	if (GameStage == Stage_Title)
	{
		Pos = D3DXVECTOR3(-560.0f, 480.0f, 560.0f);
	}
	else
	{
		Pos = D3DXVECTOR3(-1250.0f, 800.0f, 1250.0f);
	}

	// 角度
	D3DXVec3Normalize(&NormalizePos, &Pos);
	Angle = acosf(D3DXVec3Dot(&NormalizePos, &D3DXVECTOR3(1.0f, 0.0f, 0.0f)));

	// 距離
	Distance = D3DXVec3Length(&D3DXVECTOR3(Pos - PositionZero));

	// ビュー行列
	D3DXMatrixLookAtLH(&LightView, &Pos, &PositionZero, &UpVector);

	// 射影変換行列
	D3DXMatrixPerspectiveFovLH(&LightProj, D3DXToRadian(90.0f), 1.0f, 10.0f, 10000.0f);

	return S_OK;
}

//=============================================================================
// ライトの更新処理
//=============================================================================
void UpdateLight(void)
{
	D3DXVECTOR3 Move = PositionZero;

	// ライトの回転
	Angle -= 0.0001f;
	if (Angle >= D3DX_2PI)
	{
		Angle -= D3DX_2PI;
	}
	Pos.x = cosf(Angle) * Distance;
	Pos.z = sinf(Angle) * Distance;

	D3DXMatrixLookAtLH(&LightView, &Pos, &PositionZero, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

#if _DEBUG
	PrintDebugMsg("LightPos : %f , %f , %f\n", Pos.x, Pos.y, Pos.z);
#endif

	return;
}

//=============================================================================
// ライトのビュー行列を取得する
//=============================================================================
D3DMATRIX GetLightViewMatrix(void)
{
	return LightView;
}

//=============================================================================
// ライトの射影変換行列を取得する
//=============================================================================
D3DMATRIX GetLightProjMatrix(void)
{
	return LightProj;
}

//=============================================================================
// ライトの座標を取得する
//=============================================================================
D3DXVECTOR3 GetLightPos(void)
{
	return Pos;
}
