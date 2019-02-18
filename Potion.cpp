//=============================================================================
//
// ポーションモデル処理 [Potion.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Potion.h"
#include "Player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	Model_Potion	"data/Model/Potion.x"	// 読み込むモデル名


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
POTION Potion;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPotion(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 位置・回転・スケールの初期設定
	Potion.Pos = PositionZero;
	Potion.Rot = D3DXVECTOR3(D3DXToRadian(-90.0f), 0.0f, 0.0f);
	Potion.Scale = DefaultScale;

	// 初めて初期化
	if (FirstInit == true)
	{
		Potion.Texture = NULL;
		Potion.MaterialBuffer = NULL;
		Potion.Mesh = NULL;
		Potion.MaterialNum = 0;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(Model_Potion,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,						// メッシュの作成オプションを指定
			Device,									// IDirect3DDevice9インターフェイスへのポインタ
			NULL,									// 隣接性データを含むバッファへのポインタ
			&Potion.MaterialBuffer,					// マテリアルデータを含むバッファへのポインタ
			NULL,									// エフェクトインスタンスの配列を含むバッファへのポインタ
			&Potion.MaterialNum,					// D3DXMATERIAL構造体の数
			&Potion.Mesh)))							// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		// モデル用のテクスチャのメモリ空間を配る
		Potion.Texture = (LPDIRECT3DTEXTURE9*)calloc(Potion.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Potion.Texture == NULL)
		{
			MessageBox(0, "Alloc Potion Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// テクスチャを読み込む
		if (FAILED(SafeLoadModelTexture(Potion.Texture, Potion.MaterialBuffer, Potion.MaterialNum, "Potion")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPotion(void)
{
	// テクスチャの開放
	for (int i = 0; i < (int)Potion.MaterialNum; i++)
	{
		SafeRelease(Potion.Texture[i]);
	}

	// マテリアルの開放
	SafeRelease(Potion.MaterialBuffer);

	// メッシュの開放
	SafeRelease(Potion.Mesh);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePotion(void)
{

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPotion(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 DefaultMat;
	D3DXMATRIX PotionMatrix;
	PLAYER *Player = GetPlayer();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&Potion.WorldMatrix);

	// スケールを反映
	D3DXMatrixScaling(&ScaleMatrix, Potion.Scale.x, Potion.Scale.y, Potion.Scale.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &ScaleMatrix);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Potion.Rot.y, Potion.Rot.x, Potion.Rot.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &RotMatrix);

	// 移動を反映
	D3DXMatrixTranslation(&TransMatrix, Potion.Pos.x, Potion.Pos.y, Potion.Pos.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &TransMatrix);

	// 左手のボーンのマトリクスを探す
	PotionMatrix = GetBoneMatrix(Player->Animation, "PotionBone");
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &PotionMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &Potion.WorldMatrix);

	// 現在のマテリアルを取得
	Device->GetMaterial(&DefaultMat);

	// マテリアル情報に対するポインタを取得
	pD3DXMat = (D3DXMATERIAL*)Potion.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)Potion.MaterialNum; nCntMat++)
	{
		// マテリアルの設定
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// テクスチャの設定
		Device->SetTexture(0, Potion.Texture[nCntMat]);

		// 描画
		Potion.Mesh->DrawSubset(nCntMat);
	}

	// マテリアルをデフォルトに戻す
	Device->SetMaterial(&DefaultMat);

	return;
}
