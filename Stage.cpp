//=============================================================================
//
// ステージ処理 [Stage.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Stage.h"
#include "Light.h"
#include "ThirdPersonCamera.h"
#include "ShadowMapping.h"
#include "Input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 読み込むモデル名
#define	Model_Stage			"data/Model/Stage.x"	
// 読み込むシェーダー名
#define DepthBufferShader	_T("data/Shaders/DepthBufferShader.fx")


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 描画開始宣言
void DepthBuffer_Begin(void);
// シェーダーのパラメータを設定する
void SetDepthBufferParam(void);
// 描画終了宣言
void DepthBuffer_End(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
STAGE			Stage;			// ステージ
DEPTHBUFFER		DepthBuffer;	// 深度バッファ

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStage(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 位置・回転・スケールの初期設定
	Stage.Pos = PositionZero;
	Stage.Rot = PositionZero;
	Stage.Scale = DefaultScale;

	// 初めて初期化
	if (FirstInit == true)
	{
		Stage.Texture = NULL;
		Stage.MaterialBuffer = NULL;
		Stage.Mesh = NULL;
		Stage.MaterialNum = 0;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(Model_Stage,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,						// メッシュの作成オプションを指定
			Device,									// IDirect3DDevice9インターフェイスへのポインタ
			NULL,									// 隣接性データを含むバッファへのポインタ
			&Stage.MaterialBuffer,					// マテリアルデータを含むバッファへのポインタ
			NULL,									// エフェクトインスタンスの配列を含むバッファへのポインタ
			&Stage.MaterialNum,						// D3DXMATERIAL構造体の数
			&Stage.Mesh)))							// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		// モデル用のテクスチャのメモリ空間を配る
		Stage.Texture = (LPDIRECT3DTEXTURE9*)calloc(Stage.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Stage.Texture == NULL)
		{
			MessageBox(0, "Alloc Stage Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// テクスチャを読み込む
		if (FAILED(SafeLoadModelTexture(Stage.Texture, Stage.MaterialBuffer, Stage.MaterialNum, "Stage")))
		{
			return E_FAIL;
		}

		// リソースにある深度バッファシャドウシェーダプログラムを読み込む
		if (FAILED(D3DXCreateEffectFromFile(
			Device,
			DepthBufferShader,
			NULL,
			NULL,
			0,
			NULL,
			&DepthBuffer.Effect,
			NULL)))
		{
			return E_FAIL;
		}

		DepthBuffer.Para_WorldMatrix = DepthBuffer.Effect->GetParameterByName(NULL, "WorldMatrix");
		DepthBuffer.Para_WVPMatrix = DepthBuffer.Effect->GetParameterByName(NULL, "WorldViewProj");
		DepthBuffer.Para_WVPMatrix_Light = DepthBuffer.Effect->GetParameterByName(NULL, "WorldViewProj_Light");
		DepthBuffer.Para_CosTheta = DepthBuffer.Effect->GetParameterByName(NULL, "CosTheta");
		DepthBuffer.Para_ShadowMapTexture = DepthBuffer.Effect->GetParameterByName(NULL, "ShadowMapTex");
		DepthBuffer.Technique = DepthBuffer.Effect->GetTechniqueByName("RenderShadow");

		if (!DepthBuffer.Para_WorldMatrix || !DepthBuffer.Para_WVPMatrix || !DepthBuffer.Para_WVPMatrix_Light ||
			!DepthBuffer.Para_CosTheta || !DepthBuffer.Para_ShadowMapTexture || !DepthBuffer.Technique)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStage(void)
{
	// テクスチャの開放
	for (int i = 0; i < (int)Stage.MaterialNum; i++)
	{
		SafeRelease(Stage.Texture[i]);
	}

	// マテリアルの開放
	SafeRelease(Stage.MaterialBuffer);

	// メッシュの開放
	SafeRelease(Stage.Mesh);

	SafeRelease(DepthBuffer.Effect);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStage(void)
{

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStage(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;

	//=================================================
	// ステージモデル
	//=================================================
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&Stage.WorldMatrix);

	// スケールを反映
	D3DXMatrixScaling(&ScaleMatrix, Stage.Scale.x, Stage.Scale.y, Stage.Scale.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &ScaleMatrix);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Stage.Rot.y, Stage.Rot.x, Stage.Rot.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &RotMatrix);

	// 移動を反映
	D3DXMatrixTranslation(&TransMatrix, Stage.Pos.x, Stage.Pos.y, Stage.Pos.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &TransMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &Stage.WorldMatrix);

	// マテリアル情報に対するポインタを取得
	pD3DXMat = (D3DXMATERIAL*)Stage.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)Stage.MaterialNum; nCntMat++)
	{
		// マテリアルの設定
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャの設定
		Device->SetTexture(0, Stage.Texture[nCntMat]);

		// 描画
		Stage.Mesh->DrawSubset(nCntMat);
	}

	//=================================================
	// 地面の影
	//=================================================
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);

	// 描画開始
	DepthBuffer_Begin();

	// シェーダーのパラメータを設定する
	SetDepthBufferParam();

	// 描画
	for (int nCntMat = 0; nCntMat < (int)Stage.MaterialNum; nCntMat++)
	{
		DepthBuffer.Effect->BeginPass(0);
		Stage.Mesh->DrawSubset(nCntMat);
		DepthBuffer.Effect->EndPass();
	}

	// 描画終了
	DepthBuffer_End();

	// 通常ブレンド
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return;
}

//=============================================================================
// 描画開始宣言
//=============================================================================
void DepthBuffer_Begin(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// プログラマブルシェーダに切り替え
	DepthBuffer.Effect->SetTechnique(DepthBuffer.Technique);

	// デプステクスチャを取得する
	DepthBuffer.ShadowMapTexture = GetShadowMapTexture();

	// 開始宣言
	UINT Pass;
	DepthBuffer.Effect->Begin(&Pass, 0);

	return;
}

//=============================================================================
// シェーダーのパラメータを設定する
//=============================================================================
void SetDepthBufferParam(void)
{
	D3DXMATRIX WVPMatrix;
	D3DXMATRIX CameraViewMatrix = GetViewMatrix();
	D3DXMATRIX CameraProjMatrix = GetProjMatrix();
	D3DXMATRIX LightViewMatrix = GetLightViewMatrix();
	D3DXMATRIX LightProjMatrix = GetLightProjMatrix();
	D3DXVECTOR3 LightPos = GetLightPos();
	D3DXVECTOR3 LightDir = PositionZero;

	// ライト方向
	D3DXVec3Normalize(&LightDir, &LightPos);
	LightDir *= -1;
	// 90度
	DepthBuffer.Effect->SetFloat(DepthBuffer.Para_CosTheta, cosf(D3DX_PI / 2));
	DepthBuffer.Effect->SetFloatArray("LightPos", (float*)&LightPos, 3);
	DepthBuffer.Effect->SetFloatArray("LightDirection", (float*)&LightDir, 3);
	DepthBuffer.Effect->SetMatrix(DepthBuffer.Para_WorldMatrix, &Stage.WorldMatrix);

	// テクスチャ
	DepthBuffer.Effect->SetTexture(DepthBuffer.Para_ShadowMapTexture, *DepthBuffer.ShadowMapTexture);

	// カメラ
	WVPMatrix = Stage.WorldMatrix * CameraViewMatrix * CameraProjMatrix;
	DepthBuffer.Effect->SetMatrix(DepthBuffer.Para_WVPMatrix, &WVPMatrix);

	// ライト
	WVPMatrix = Stage.WorldMatrix * LightViewMatrix * LightProjMatrix;
	DepthBuffer.Effect->SetMatrix("WorldViewProj_Light", &WVPMatrix);

	return;
}

//=============================================================================
// 描画終了宣言
//=============================================================================
void DepthBuffer_End(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 描画終了
	DepthBuffer.Effect->End();

	// 固定機能に戻す
	Device->SetVertexShader(NULL);
	Device->SetPixelShader(NULL);

	return;
}

//=============================================================================
// ステージのポインタを取得する
//=============================================================================
STAGE *GetStage(void)
{
	return &Stage;
}