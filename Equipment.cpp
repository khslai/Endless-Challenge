//=============================================================================
//
// 装備モデル処理 [Equipment.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Equipment.h"
#include "Player.h"
#include "Boss.h"
#include "D3DXAnimation.h"
#include "CapsuleMesh.h"
#include "ThirdPersonCamera.h"
#include "Effect.h"
#include "FireSword.h"
#include "AttackRange.h"
#include "Input.h"
#include "DebugProcess.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 読み込むモデル名
#define	Model_PlayerSword	"data/Model/PlayerSword.x"
#define	Model_BossSword		"data/Model/BossSword.x"


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
SWORD		PlayerSword;
SWORD		BossSword;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEquipment(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 位置・回転・スケールの初期設定
	PlayerSword.Pos = D3DXVECTOR3(-3.0f, 0.0f, 6.0f);
	PlayerSword.Rot = D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(90.0f));
	PlayerSword.Scale = DefaultScale;

	BossSword.Pos = D3DXVECTOR3(0.0f, 0.0f, 20.0f);
	BossSword.Rot = D3DXVECTOR3(D3DXToRadian(45.0f), D3DXToRadian(90.0f), D3DXToRadian(-90.0f));
	BossSword.Scale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);

	// 初めて初期化
	if (FirstInit == true)
	{
		PlayerSword.Texture = NULL;
		PlayerSword.MaterialBuffer = NULL;
		PlayerSword.Mesh = NULL;
		PlayerSword.MaterialNum = 0;

		BossSword.Texture = NULL;
		BossSword.MaterialBuffer = NULL;
		BossSword.Mesh = NULL;
		BossSword.MaterialNum = 0;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(Model_PlayerSword,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,							// メッシュの作成オプションを指定
			Device,										// IDirect3DDevice9インターフェイスへのポインタ
			NULL,										// 隣接性データを含むバッファへのポインタ
			&PlayerSword.MaterialBuffer,				// マテリアルデータを含むバッファへのポインタ
			NULL,										// エフェクトインスタンスの配列を含むバッファへのポインタ
			&PlayerSword.MaterialNum,					// D3DXMATERIAL構造体の数
			&PlayerSword.Mesh)))						// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		if (FAILED(D3DXLoadMeshFromX(Model_BossSword,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,							// メッシュの作成オプションを指定
			Device,										// IDirect3DDevice9インターフェイスへのポインタ
			NULL,										// 隣接性データを含むバッファへのポインタ
			&BossSword.MaterialBuffer,					// マテリアルデータを含むバッファへのポインタ
			NULL,										// エフェクトインスタンスの配列を含むバッファへのポインタ
			&BossSword.MaterialNum,						// D3DXMATERIAL構造体の数
			&BossSword.Mesh)))							// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		// モデル用のテクスチャのメモリ空間を配る
		PlayerSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(PlayerSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (PlayerSword.Texture == NULL)
		{
			MessageBox(0, "Alloc PlayerSword Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		BossSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(BossSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (BossSword.Texture == NULL)
		{
			MessageBox(0, "Alloc BossSword Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// テクスチャを読み込む
		if (FAILED(SafeLoadModelTexture(PlayerSword.Texture, PlayerSword.MaterialBuffer, PlayerSword.MaterialNum, "PlayerSword")))
		{
			return E_FAIL;
		}

		if (FAILED(SafeLoadModelTexture(BossSword.Texture, BossSword.MaterialBuffer, BossSword.MaterialNum, "BossSword")))
		{
			return E_FAIL;
		}

		// 当たり判定カプセルを生成
		if (FAILED(
			CreateCapsule(&PlayerSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 55.0f, 1.5f, false)))
		{
			return E_FAIL;
		}

		if (FAILED(
			CreateCapsule(&BossSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 120.0f, 2.0f, false)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEquipment(void)
{
	int i = 0;

	// テクスチャの開放
	for (i = 0; i < (int)PlayerSword.MaterialNum; i++)
	{
		SafeRelease(PlayerSword.Texture[i]);
	}
	for (i = 0; i < (int)BossSword.MaterialNum; i++)
	{
		SafeRelease(BossSword.Texture[i]);
	}

	// マテリアルの開放
	SafeRelease(PlayerSword.MaterialBuffer);
	SafeRelease(BossSword.MaterialBuffer);

	// メッシュの開放
	SafeRelease(PlayerSword.Mesh);
	SafeRelease(BossSword.Mesh);

	// カプセルの開放
	UninitCapsule(&PlayerSword.HitCapsule);
	UninitCapsule(&BossSword.HitCapsule);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEquipment(void)
{

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEquipment(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 DefaultMat;
	D3DXMATRIX PlayerSwordMatrix;
	D3DXMATRIX BossSwordMatrix;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CAMERA_3RD *Camera = GetCamera_3rd();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&PlayerSword.WorldMatrix);

	// スケールを反映
	D3DXMatrixScaling(&ScaleMatrix, PlayerSword.Scale.x, PlayerSword.Scale.y, PlayerSword.Scale.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &ScaleMatrix);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, PlayerSword.Rot.y, PlayerSword.Rot.x, PlayerSword.Rot.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &RotMatrix);

	// 移動を反映
	D3DXMatrixTranslation(&TransMatrix, PlayerSword.Pos.x, PlayerSword.Pos.y, PlayerSword.Pos.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &TransMatrix);

	// 剣を握るボーンのマトリクスを探す
	PlayerSwordMatrix = GetBoneMatrix(Player->Animation, "SwordPosBone");
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &PlayerSwordMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &PlayerSword.WorldMatrix);

	// 現在のマテリアルを取得
	Device->GetMaterial(&DefaultMat);

	// マテリアル情報に対するポインタを取得
	pD3DXMat = (D3DXMATERIAL*)PlayerSword.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)PlayerSword.MaterialNum; nCntMat++)
	{
		// マテリアルの設定
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャの設定
		Device->SetTexture(0, PlayerSword.Texture[nCntMat]);

		// 描画
		PlayerSword.Mesh->DrawSubset(nCntMat);
	}

	// マテリアルをデフォルトに戻す
	Device->SetMaterial(&DefaultMat);

	// 剣のカプセルを描画する
	DrawCapsule(&PlayerSword.HitCapsule, &PlayerSwordMatrix);

	//========================================================================
	//========================================================================
	//========================================================================

	if (Boss->Exist == true)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&BossSword.WorldMatrix);

		// スケールを反映
		D3DXMatrixScaling(&ScaleMatrix, BossSword.Scale.x, BossSword.Scale.y, BossSword.Scale.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &ScaleMatrix);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, BossSword.Rot.y, BossSword.Rot.x, BossSword.Rot.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &RotMatrix);

		// 移動を反映
		D3DXMatrixTranslation(&TransMatrix, BossSword.Pos.x, BossSword.Pos.y, BossSword.Pos.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &TransMatrix);

		// 剣を握るボーンのマトリクスを探す
		BossSwordMatrix = GetBoneMatrix(Boss->Animation, "GreatSword");
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &BossSwordMatrix);

		// ワールドマトリックスの設定
		Device->SetTransform(D3DTS_WORLD, &BossSword.WorldMatrix);

		// 現在のマテリアルを取得
		Device->GetMaterial(&DefaultMat);

		// マテリアル情報に対するポインタを取得
		pD3DXMat = (D3DXMATERIAL*)BossSword.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)BossSword.MaterialNum; nCntMat++)
		{
			// マテリアルの設定
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// テクスチャの設定
			Device->SetTexture(0, BossSword.Texture[nCntMat]);

			// 描画
			BossSword.Mesh->DrawSubset(nCntMat);
		}

		// マテリアルをデフォルトに戻す
		Device->SetMaterial(&DefaultMat);

		// 剣のカプセルを描画する
		DrawCapsule(&BossSword.HitCapsule, &BossSwordMatrix);
	}

	return;
}

//=============================================================================
// ポインタを取得する
//=============================================================================
SWORD *GetSword(const char* Owner)
{
	if (strcmp(Owner, "Player") == 0)
	{
		return &PlayerSword;
	}
	else if (strcmp(Owner, "Boss") == 0)
	{
		return &BossSword;
	}
	else
	{
		MessageBox(0, "Can't Find Sword Owner", "Error", 0);
		return NULL;
	}
}
