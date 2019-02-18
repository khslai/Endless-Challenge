//=============================================================================
//
// スカイボックス処理 [SkyBox.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "SkyBox.h"
#include "Player.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"
#include "Input.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	Texture_SkyBox					_T("data/Texture/SkyBox.png")	// 読み込むテクスチャファイル名
#define Texture_SkyBox_Divide_X			(4)								// テクスチャ内分割数（横)
#define Texture_SkyBox_Divide_Y			(3)								// テクスチャ内分割数（縦)
#define Surface_Num						(6)								// サーフェイスの数
#define SkyBoxLength					(5000.0f)						// 立方体の長さ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 頂点情報の作成
HRESULT MakeVertexSkyBox(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 SkyBoxVtxBuffer = NULL;			// 頂点バッファへのポインタ
LPDIRECT3DTEXTURE9		SkyBoxTexture = NULL;			// テクスチャへのポインタ
D3DXMATRIX				WorldMatrix_SkyBox;				// ワールドマトリックス
SKYBOX					SkyBox;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSkyBox(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 位置・回転・スケールの初期設定
	SkyBox.Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SkyBox.Rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	SkyBox.Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	// 初めて初期化
	if (FirstInit == true)
	{
		// 頂点情報の作成
		if (FAILED(MakeVertexSkyBox()))
		{
			return E_FAIL;
		}

		// テクスチャの読み込み
		if (FAILED(SafeLoadTexture(Texture_SkyBox, &SkyBoxTexture, "SkyBox")))
		{
			return E_FAIL;
		}
	}
	
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSkyBox(void)
{
	// テクスチャの開放
	SafeRelease(SkyBoxTexture);

	// 頂点バッファの開放
	SafeRelease(SkyBoxVtxBuffer);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSkyBox(void)
{
	CAMERA_3RD *Camera = GetCamera_3rd();

	// 中心はカメラの注視点
	SkyBox.Pos = Camera->At;
	// 空の回転
	SkyBox.Rot.y += 0.0001f;

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSkyBox(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&WorldMatrix_SkyBox);

	// スケールを反映
	D3DXMatrixScaling(&ScaleMatrix, SkyBox.Scale.x, SkyBox.Scale.y, SkyBox.Scale.z);
	D3DXMatrixMultiply(&WorldMatrix_SkyBox, &WorldMatrix_SkyBox, &ScaleMatrix);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, SkyBox.Rot.y, SkyBox.Rot.x, SkyBox.Rot.z);
	D3DXMatrixMultiply(&WorldMatrix_SkyBox, &WorldMatrix_SkyBox, &RotMatrix);

	// 移動を反映
	D3DXMatrixTranslation(&TransMatrix, SkyBox.Pos.x, SkyBox.Pos.y, SkyBox.Pos.z);
	D3DXMatrixMultiply(&WorldMatrix_SkyBox, &WorldMatrix_SkyBox, &TransMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &WorldMatrix_SkyBox);

	// 頂点バッファをデバイスのデータストリームにバインド
	Device->SetStreamSource(0, SkyBoxVtxBuffer, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	Device->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	Device->SetTexture(0, SkyBoxTexture);

	// ポリゴンの描画
	for (int i = 0; i < Surface_Num; i++)
	{
		Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * i, POLYGON_NUM);
	}

	return;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexSkyBox(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int i = 0;
	int x = 0, y = 0;
	float sizeX = 1.0f / Texture_SkyBox_Divide_X;
	float sizeY = 1.0f / Texture_SkyBox_Divide_Y;

	// オブジェクトの頂点バッファを生成
	if (FAILED(Device->CreateVertexBuffer(sizeof(VERTEX_3D) * VERTEX_NUM * Surface_Num,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
		FVF_VERTEX_3D,				// 使用する頂点フォーマット
		D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
		&SkyBoxVtxBuffer,			// 頂点バッファインターフェースへのポインタ
		NULL)))						// NULLに設定
	{
		return E_FAIL;
	}

	// 頂点バッファの中身を埋める
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	SkyBoxVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	// 前
	pVtx[0].vtx = D3DXVECTOR3(-(SkyBoxLength / 2), (SkyBoxLength / 2), (SkyBoxLength / 2));
	pVtx[1].vtx = D3DXVECTOR3((SkyBoxLength / 2), (SkyBoxLength / 2), (SkyBoxLength / 2));
	pVtx[2].vtx = D3DXVECTOR3(-(SkyBoxLength / 2), -(SkyBoxLength / 2), (SkyBoxLength / 2));
	pVtx[3].vtx = D3DXVECTOR3((SkyBoxLength / 2), -(SkyBoxLength / 2), (SkyBoxLength / 2));
	// 後ろ
	pVtx[4].vtx = D3DXVECTOR3((SkyBoxLength / 2), (SkyBoxLength / 2), -(SkyBoxLength / 2));
	pVtx[5].vtx = D3DXVECTOR3(-(SkyBoxLength / 2), (SkyBoxLength / 2), -(SkyBoxLength / 2));
	pVtx[6].vtx = D3DXVECTOR3((SkyBoxLength / 2), -(SkyBoxLength / 2), -(SkyBoxLength / 2));
	pVtx[7].vtx = D3DXVECTOR3(-(SkyBoxLength / 2), -(SkyBoxLength / 2), -(SkyBoxLength / 2));
	// 上
	pVtx[8].vtx = pVtx[5].vtx;
	pVtx[9].vtx = pVtx[4].vtx;
	pVtx[10].vtx = pVtx[0].vtx;
	pVtx[11].vtx = pVtx[1].vtx;
	// 右
	pVtx[12].vtx = pVtx[1].vtx;
	pVtx[13].vtx = pVtx[4].vtx;
	pVtx[14].vtx = pVtx[3].vtx;
	pVtx[15].vtx = pVtx[6].vtx;
	// 底
	pVtx[16].vtx = pVtx[2].vtx;
	pVtx[17].vtx = pVtx[3].vtx;
	pVtx[18].vtx = pVtx[7].vtx;
	pVtx[19].vtx = pVtx[6].vtx;
	// 左
	pVtx[20].vtx = pVtx[5].vtx;
	pVtx[21].vtx = pVtx[0].vtx;
	pVtx[22].vtx = pVtx[7].vtx;
	pVtx[23].vtx = pVtx[2].vtx;


	// 法線ベクトルの設定
	for (i = 0; i < VERTEX_NUM * Surface_Num; i++)
	{
		pVtx[i].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	}

	// 反射光の設定
	for (i = 0; i < VERTEX_NUM * Surface_Num; i++)
	{
		pVtx[i].diffuse = WHITE(255);
	}

	// テクスチャ座標の設定

	// 前
	x = 1;
	y = 1;
	pVtx[0].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[1].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[2].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[3].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	// 後ろ
	x = 3;
	y = 1;
	pVtx[4].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[5].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[6].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[7].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	// 上
	x = 1;
	y = 0;
	pVtx[8].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[9].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[10].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[11].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	// 右
	x = 2;
	y = 1;
	pVtx[12].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[13].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[14].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[15].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	// 底
	x = 1;
	y = 2;
	pVtx[16].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[17].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[18].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[19].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	// 左
	x = 0;
	y = 1;
	pVtx[20].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[21].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + 0.001f);
	pVtx[22].tex = D3DXVECTOR2((float)(x)* sizeX + 0.001f, (float)(y)* sizeY + sizeY - 0.001f);
	pVtx[23].tex = D3DXVECTOR2((float)(x)* sizeX + sizeX - 0.001f, (float)(y)* sizeY + sizeY - 0.001f);

	// 頂点データをアンロックする
	SkyBoxVtxBuffer->Unlock();

	return S_OK;
}
