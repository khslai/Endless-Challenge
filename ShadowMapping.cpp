//=============================================================================
//
// シャドウマッピング処理 [ShadowMapping.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "ShadowMapping.h"
#include "Light.h"
#include "Stage.h"
#include "Player.h"
#include "Boss.h"
#include "Equipment.h"
#include "D3DXAnimation.h"
#include "ThirdPersonCamera.h"
#include "Tutorial.h"
#include "Input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// シェーダーのパス
#define TexCreatorShader	_T("data/Shaders/TexCreatorShader.fx")
// デプステクスチャのサイズ
#define	ShadowMap_Size		(2048)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 描画開始宣言
void TexCreator_Begin(void);
// シェーダーのパラメータを設定する
void SetTexCreatorParam(void);
// 描画終了宣言
void TexCreator_End(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
TEXCREATOR			TexCreator;
LPDIRECT3DSURFACE9	ShadowMapSurface;		// シャドウマップバッファ
LPDIRECT3DSURFACE9	ShadowMapDepthSurface;	// シャドウマップ深度バッファ
LPDIRECT3DSURFACE9	SceneSurface;			// 描画シーンバッファ
LPDIRECT3DSURFACE9	SceneDepthSurface;		// 描画シーン深度バッファ
static LPD3DXSPRITE	Sprite = NULL;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitShadowMapping(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	PLAYER *Player = GetPlayer();

	// 初めて初期化
	if (FirstInit == true)
	{
		HRESULT hr;
		D3DXCreateSprite(Device, &Sprite);

		// リソースにあるZ値プロットシェーダプログラムを読み込む
		if (FAILED(D3DXCreateEffectFromFile(
			Device,
			TexCreatorShader,
			NULL,
			NULL,
			0,
			NULL,
			&TexCreator.Effect,
			NULL)))
		{
			return E_FAIL;
		}

		// 指定のZ値テクスチャを生成
		hr = D3DXCreateTexture(
			Device,
			ShadowMap_Size, ShadowMap_Size,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_R32F,
			D3DPOOL_DEFAULT,
			&TexCreator.ShadowMapTexture);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// サーフェイス取得
		TexCreator.ShadowMapTexture->GetSurfaceLevel(0, &ShadowMapSurface);

		// 描画デバイスに定義されているバッファの能力を取得
		IDirect3DSurface9 *pSurf;
		Device->GetDepthStencilSurface(&pSurf);
		D3DSURFACE_DESC Desc;
		pSurf->GetDesc(&Desc);
		pSurf->Release();

		// 独自深度バッファを作成
		hr = Device->CreateDepthStencilSurface(
			ShadowMap_Size, ShadowMap_Size,
			Desc.Format,
			Desc.MultiSampleType,
			Desc.MultiSampleQuality,
			FALSE,
			&ShadowMapDepthSurface,
			NULL);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// エフェクト内の各種パラメータハンドルを取得
		TexCreator.Para_WVPMatrix = TexCreator.Effect->GetParameterByName(NULL, "WorldViewProj_Light");
		TexCreator.Technique = TexCreator.Effect->GetTechniqueByName("ShadowTexture");

		if (!TexCreator.Para_WVPMatrix || !TexCreator.Technique)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitShadowMapping(void)
{
	// 描画サーフェイスの開放
	SafeRelease(ShadowMapSurface);
	SafeRelease(ShadowMapDepthSurface);
	SafeRelease(SceneSurface);
	SafeRelease(SceneDepthSurface);
	SafeRelease(Sprite);

	SafeRelease(TexCreator.Effect);				// Z値プロットエフェクト
	SafeRelease(TexCreator.ShadowMapTexture);	// デプステクスチャ

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateShadowMapping(void)
{

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawShadowTexture(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	STAGE *Stage = GetStage();
	CUBE *Cube = GetCube();
	int GameStage = GetGameStage();

	// Z値テクスチャにライト方向から描画

	// 描画開始
	TexCreator_Begin();

	// プレイヤー
	DrawAnimation(Player->Animation, &Player->WorldMatrix, true);

	if (GameStage != Stage_Title)
	{
		// プレイヤーの剣
		TexCreator.WorldMatrix = Player->Sword->WorldMatrix;
		SetTexCreatorParam();
		for (int i = 0; i < (int)Player->Sword->MaterialNum; i++)
		{
			TexCreator.Effect->BeginPass(0);
			Player->Sword->Mesh->DrawSubset(i);
			TexCreator.Effect->EndPass();
		}

		if (Boss->Exist == true)
		{
			// ボス
			DrawAnimation(Boss->Animation, &Boss->WorldMatrix, true);

			// ボスの剣
			TexCreator.WorldMatrix = Boss->Sword->WorldMatrix;
			SetTexCreatorParam();
			for (int i = 0; i < (int)Boss->Sword->MaterialNum; i++)
			{
				TexCreator.Effect->BeginPass(0);
				Boss->Sword->Mesh->DrawSubset(i);				// メッシュ描画
				TexCreator.Effect->EndPass();
			}
		}
	}

	if (Cube->Exist == true)
	{
		// 謎の立方体
		TexCreator.WorldMatrix = Cube->WorldMatrix;
		SetTexCreatorParam();
		for (int i = 0; i < (int)Cube->MaterialNum; i++)
		{
			TexCreator.Effect->BeginPass(0);
			Cube->Mesh->DrawSubset(i);
			TexCreator.Effect->EndPass();
		}
	}

	// 描画終了
	TexCreator_End();

	// 小さい分割画面を使て、デプステクスチャを表示する
#if 0
	D3DXMATRIX SpriteScaleMat, TransMatrix;
	float Ratio = (float)Screen_Width / 2048.0f;	// 画面に対する比率を計算
	D3DXMatrixScaling(&SpriteScaleMat, Ratio / 3, Ratio / 3, 1.0f);
	D3DXMatrixTranslation(&TransMatrix, 0.0f, 400.0f, 0.0f);
	D3DXMatrixMultiply(&SpriteScaleMat, &SpriteScaleMat, &TransMatrix);
	Sprite->SetTransform(&SpriteScaleMat);
	Sprite->Begin(0);
	Sprite->Draw(TexCreator.ShadowMapTexture, NULL, NULL, NULL, WHITE(255));
	Sprite->End();
#endif

	return;
}

//=============================================================================
// 描画開始宣言
//=============================================================================
void TexCreator_Begin(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// デバイスが持っているバッファを一時保存
	Device->GetRenderTarget(0, &SceneSurface);
	Device->GetDepthStencilSurface(&SceneDepthSurface);

	// デバイスにZ値テクスチャサーフェイスと深度バッファを設定
	Device->SetRenderTarget(0, ShadowMapSurface);
	Device->SetDepthStencilSurface(ShadowMapDepthSurface);

	// 各サーフェイスを初期化
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

	// プログラマブルシェーダのテクニックを設定
	TexCreator.Effect->SetTechnique(TexCreator.Technique);

	// シェーダの開始を宣言
	UINT Tmp;
	TexCreator.Effect->Begin(&Tmp, 0);

	return;
}

//=============================================================================
// シェーダーのパラメータを設定する
//=============================================================================
void SetTexCreatorParam(void)
{
	D3DXMATRIX WVPMatrix;
	D3DXMATRIX LightViewMatrix = GetLightViewMatrix();
	D3DXMATRIX LightProjMatrix = GetLightProjMatrix();
	WVPMatrix = TexCreator.WorldMatrix * LightViewMatrix * LightProjMatrix;
	TexCreator.Effect->SetMatrix(TexCreator.Para_WVPMatrix, &WVPMatrix);

	return;
}

//=============================================================================
// 描画終了宣言
//=============================================================================
void TexCreator_End(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	TexCreator.Effect->End();

	// デバイスに元のサーフェイスを戻す
	Device->SetRenderTarget(0, SceneSurface);
	Device->SetDepthStencilSurface(SceneDepthSurface);

	SceneSurface = NULL;
	SceneDepthSurface = NULL;

	// 固定機能に戻す
	Device->SetVertexShader(NULL);
	Device->SetPixelShader(NULL);

	// 各サーフェイスを初期化
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, WHITE(255), 1.0f, 0);

	return;
}

//=============================================================================
// デプステクスチャのポインタを取得する
//=============================================================================
LPDIRECT3DTEXTURE9 *GetShadowMapTexture(void)
{
	return &TexCreator.ShadowMapTexture;
}
