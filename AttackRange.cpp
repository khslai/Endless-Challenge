//=============================================================================
//
// 当たり範囲表示処理 [AttackRange.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "AttackRange.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define Sphere_Max	(100)	// 表示できる最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
RANGESPHERE			RangeSphere[Sphere_Max];
D3DMATERIAL9		Material;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAttackRange(bool FirstInit)
{

#if _DEBUG
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sphere_No = 0;

	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		RangeSphere[Sphere_No].Mesh = NULL;
		RangeSphere[Sphere_No].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		RangeSphere[Sphere_No].Rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		RangeSphere[Sphere_No].Use = false;
	}

	if (FirstInit == true)
	{
		Material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
		Material.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		Material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		Material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	}
#endif

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAttackRange(void)
{
	for (int Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		SafeRelease(RangeSphere[Sphere_No].Mesh);
	}

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAttackRange(void)
{

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAttackRange(void)
{

#if _DEBUG
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DMATERIAL9 DefaultMat;
	int Sphere_No = 0;
	bool DrawWireFrame = GetDrawWireFrame();

	// 描画処理
	if (DrawWireFrame == true)
	{
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		if (RangeSphere[Sphere_No].Use == true)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&RangeSphere[Sphere_No].WorldMatrix);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&RotMatrix, RangeSphere[Sphere_No].Rot.y, RangeSphere[Sphere_No].Rot.x, RangeSphere[Sphere_No].Rot.z);
			D3DXMatrixMultiply(&RangeSphere[Sphere_No].WorldMatrix, &RangeSphere[Sphere_No].WorldMatrix, &RotMatrix);

			// 移動を反映
			D3DXMatrixTranslation(&TransMatrix, RangeSphere[Sphere_No].Pos.x, RangeSphere[Sphere_No].Pos.y, RangeSphere[Sphere_No].Pos.z);
			D3DXMatrixMultiply(&RangeSphere[Sphere_No].WorldMatrix, &RangeSphere[Sphere_No].WorldMatrix, &TransMatrix);

			// ワールドマトリックスの設定
			Device->SetTransform(D3DTS_WORLD, &RangeSphere[Sphere_No].WorldMatrix);

			// 現在のマテリアルを取得
			Device->GetMaterial(&DefaultMat);

			Device->SetMaterial(&Material);

			// 描画
			RangeSphere[Sphere_No].Mesh->DrawSubset(0);

			// マテリアルをデフォルトに戻す
			Device->SetMaterial(&DefaultMat);
		}
	}

	// 終了処理
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	return;
}

//=============================================================================
// 当たり範囲を設置する
//=============================================================================
int SetRangeSphere(D3DXVECTOR3 Pos, float Radius)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sphere_No = -1;

#if _DEBUG
	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		if (RangeSphere[Sphere_No].Use == false)
		{
			// 球体メッシュを作成する
			if (RangeSphere[Sphere_No].Mesh == NULL)
			{
				D3DXCreateSphere(Device, Radius, 2, 16, &RangeSphere[Sphere_No].Mesh, NULL);
				RangeSphere[Sphere_No].Rot.z = D3DXToRadian(90.0f);
			}
			RangeSphere[Sphere_No].Pos = Pos;
			RangeSphere[Sphere_No].Pos.y = 1.0f;
			RangeSphere[Sphere_No].Use = true;
			break;
		}
	}
#endif

	return Sphere_No;
}

//=============================================================================
// 当たり範囲を削除する
//=============================================================================
void DeleteRangeSphere(int Sphere_No)
{

#if _DEBUG
	if (Sphere_No >= 0 && Sphere_No < Sphere_Max)
	{
		RangeSphere[Sphere_No].Use = false;
		SafeRelease(RangeSphere[Sphere_No].Mesh);
	}
#endif

	return;
}
