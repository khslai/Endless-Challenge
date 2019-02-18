//=============================================================================
//
// 炎の剣処理 [FireSword.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "FireSword.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	Texture_SlashTrack	_T("data/Texture/Track.png")	// 読み込むテクスチャファイル名
#define Surface_Max			(100 * 2)
#define AlphaDecreaseSpeed	(0.1f)

static const EFK_CHAR* FireSwordFileName[] =
{
	(const EFK_CHAR*)L"data/Effects/FireSword/FireSword.efk",
	(const EFK_CHAR*)L"data/Effects/FireSword/FireSword_Phase2.efk",
	(const EFK_CHAR*)L"data/Effects/FireSword/SlashFire.efk",
	(const EFK_CHAR*)L"data/Effects/FireSword/SlashFire_Phase2.efk",
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 頂点情報の作成
static HRESULT MakeVtxBuffer(void);
// 頂点情報の更新
static void UpdateVtxBuffer(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
EFFECTCONTROLLER		FireSwordCtrl;						// エフェクトコントローラー
LPDIRECT3DVERTEXBUFFER9 SlashTrackVtxBuffer = NULL;			// 頂点バッファへのポインタ
LPDIRECT3DTEXTURE9		SlashTrackTexture = NULL;			// テクスチャへのポインタ
SURFACE					Surface[Surface_Max];				// サーフェイス

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFireSword(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	for (int Surface_No = 0; Surface_No < Surface_Max; Surface_No++)
	{
		Surface[Surface_No].Vtx_LeftUp = PositionZero;
		Surface[Surface_No].Vtx_RightUp = PositionZero;
		Surface[Surface_No].Vtx_LeftDown = PositionZero;
		Surface[Surface_No].Vtx_RightDown = PositionZero;
		Surface[Surface_No].Color = WHITE(255);
		Surface[Surface_No].IsBack = false;
		Surface[Surface_No].Use = false;
	}

	// 初めて初期化
	if (FirstInit == true)
	{
		FireSwordCtrl.Manager = NULL;
		FireSwordCtrl.Render = NULL;
		FireSwordCtrl.Effect = NULL;
		FireSwordCtrl.EffectNum = sizeof(FireSwordFileName) / sizeof(const EFK_CHAR*);

		// 描画用インスタンスの生成
		FireSwordCtrl.Render = ::EffekseerRendererDX9::Renderer::Create(Device, 10000);

		// エフェクト管理用インスタンスの生成
		FireSwordCtrl.Manager = ::Effekseer::Manager::Create(10000);

		// 描画用インスタンスから描画機能を設定
		FireSwordCtrl.Manager->SetSpriteRenderer(FireSwordCtrl.Render->CreateSpriteRenderer());
		FireSwordCtrl.Manager->SetRibbonRenderer(FireSwordCtrl.Render->CreateRibbonRenderer());
		FireSwordCtrl.Manager->SetRingRenderer(FireSwordCtrl.Render->CreateRingRenderer());
		FireSwordCtrl.Manager->SetTrackRenderer(FireSwordCtrl.Render->CreateTrackRenderer());
		FireSwordCtrl.Manager->SetModelRenderer(FireSwordCtrl.Render->CreateModelRenderer());

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		FireSwordCtrl.Manager->SetTextureLoader(FireSwordCtrl.Render->CreateTextureLoader());
		FireSwordCtrl.Manager->SetModelLoader(FireSwordCtrl.Render->CreateModelLoader());

		// メモリを確保
		FireSwordCtrl.Effect = (Effekseer::Effect**)calloc(FireSwordCtrl.EffectNum, sizeof(Effekseer::Effect*));
		if (FireSwordCtrl.Effect == NULL)
		{
			MessageBox(0, "Alloc FireSword Effect Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// エフェクトの読込
		for (int i = 0; i < FireSwordCtrl.EffectNum; i++)
		{
			if (FAILED(SafeLoadEffect(FireSwordCtrl.Manager, &FireSwordCtrl.Effect[i], FireSwordFileName[i], "FireSword")))
			{
				return E_FAIL;
			}
		}

		// テクスチャの読み込み
		if (FAILED(SafeLoadTexture(Texture_SlashTrack, &SlashTrackTexture, "FireSword")))
		{
			return E_FAIL;
		}

		// 頂点情報の作成
		if (FAILED(MakeVtxBuffer()))
		{
			return E_FAIL;
		}
	}

	// 全エフェクト停止
	FireSwordCtrl.Manager->StopAllEffects();
	FireSwordCtrl.Manager->Update();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFireSword(void)
{
	// エフェクトの停止
	FireSwordCtrl.Manager->StopAllEffects();

	// エフェクトの破棄
	SafeFree(FireSwordCtrl.Effect);

	// 先にエフェクト管理用インスタンスを破棄
	FireSwordCtrl.Manager->Destroy();

	// 次に描画用インスタンスを破棄
	FireSwordCtrl.Render->Destroy();

	// テクスチャの開放
	SafeRelease(SlashTrackTexture);

	// 頂点バッファの開放
	SafeRelease(SlashTrackVtxBuffer);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFireSword(void)
{
	// エフェクトの更新処理を行う
	FireSwordCtrl.Manager->Update();

	UpdateVtxBuffer();

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFireSword(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX WorldMatrix;
	CAMERA_3RD *Camera = GetCamera_3rd();
	SURFACE	*SurfacePtr = NULL;
	int Surface_No = 0;

	// 投影行列を設定
	FireSwordCtrl.Render->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z));

	// カメラ行列を設定
	FireSwordCtrl.Render->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(DXtoEffekVec(Camera->Pos), DXtoEffekVec(Camera->At), DXtoEffekVec(Camera->Up)));

	// エフェクトの描画開始処理を行う。
	FireSwordCtrl.Render->BeginRendering();

	// エフェクトの描画を行う。
	FireSwordCtrl.Manager->Draw();

	// エフェクトの描画終了処理を行う。
	FireSwordCtrl.Render->EndRendering();

	// 加算合成
	Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);	// 結果 = 転送先(DEST) + 転送元(SRC)
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ラインティングを無効にする
	Device->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (Surface_No = 0; Surface_No < Surface_Max; Surface_No++)
	{
		SurfacePtr = &Surface[Surface_No];
		if (SurfacePtr->Use == true)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&WorldMatrix);

			// ワールドマトリックスの設定
			Device->SetTransform(D3DTS_WORLD, &WorldMatrix);

			// 頂点バッファをレンダリングパイプラインに設定
			Device->SetStreamSource(0, SlashTrackVtxBuffer, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			Device->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			Device->SetTexture(0, SlashTrackTexture);

			// ポリゴンの描画
			Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, (4 * Surface_No), POLYGON_NUM);
		}
	}

	// ラインティングを有効にする
	Device->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 通常ブレンド
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	return;
}

//=============================================================================
// 剣を纏う炎を設置する
//=============================================================================
int SetFireSword(D3DXVECTOR3 Pos, D3DXVECTOR3 Direction)
{
	int EffectID = -1;
	BOSS *Boss = GetBoss();

	if (Boss->Phase == Phase1)
	{
		EffectID = FireSwordCtrl.Manager->Play(FireSwordCtrl.Effect[FireSword], Pos.x, Pos.y, Pos.z);
	}
	else if (Boss->Phase == Phase2)
	{
		EffectID = FireSwordCtrl.Manager->Play(FireSwordCtrl.Effect[FireSword_Phase2], Pos.x, Pos.y, Pos.z);
	}

	// 向き回転
	// 回転軸
	D3DXVECTOR3 RotAxis;
	// 回転方向正規化
	D3DXVec3Normalize(&Direction, &Direction);
	// 回転ラジアン計算
	float RotRadian = acosf(D3DXVec3Dot(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), &Direction));
	// 回転軸計算
	D3DXVec3Cross(&RotAxis, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &Direction);
	if (RotAxis == PositionZero)
	{
		RotAxis = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	}
	D3DXVec3Normalize(&RotAxis, &RotAxis);
	// エフェクト回転
	FireSwordCtrl.Manager->SetRotation(EffectID, DXtoEffekVec(RotAxis), RotRadian - D3DX_PI);

	return EffectID;
}

//=============================================================================
// 斬撃の炎を設置する
//=============================================================================
int SetSlashFire(D3DXVECTOR3 Pos, D3DXVECTOR3 PrePos)
{
	int EffectID = -1;
	BOSS *Boss = GetBoss();
	// エフェクトの回転軸
	D3DXVECTOR3 RotAxis;
	// 目的方向
	D3DXVECTOR3 Direction;
	// 回転ラジアン
	float RotRadian = 0.0f;

	if (Boss->Phase == Phase1)
	{
		EffectID = FireSwordCtrl.Manager->Play(FireSwordCtrl.Effect[SlashFire], Pos.x, Pos.y, Pos.z);
	}
	else if (Boss->Phase == Phase2)
	{
		EffectID = FireSwordCtrl.Manager->Play(FireSwordCtrl.Effect[SlashFire_Phase2], Pos.x, Pos.y, Pos.z);
	}

	// 目的方向
	Direction = Pos - PrePos;
	D3DXVec3Normalize(&Direction, &Direction);
	// 回転ラジアン計算
	RotRadian = acosf(D3DXVec3Dot(&UpVector, &Direction));
	// エフェクトのデフォルト方向は上
	D3DXVec3Cross(&RotAxis, &UpVector, &Direction);
	D3DXVec3Normalize(&RotAxis, &RotAxis);
	// エフェクト回転
	FireSwordCtrl.Manager->SetRotation(EffectID, DXtoEffekVec(RotAxis), RotRadian);

	return EffectID;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVtxBuffer(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int i = 0;

	// オブジェクトの頂点バッファを生成
	if (FAILED(Device->CreateVertexBuffer(sizeof(VERTEX_3D) * VERTEX_NUM * Surface_Max,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
		FVF_VERTEX_3D,				// 使用する頂点フォーマット
		D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
		&SlashTrackVtxBuffer,		// 頂点バッファインターフェースへのポインタ
		NULL)))						// NULLに設定
	{
		return E_FAIL;
	}

	// 頂点バッファの中身を埋める
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	SlashTrackVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	for (i = 0; i < VERTEX_NUM * Surface_Max; i++)
	{
		// 頂点座標の設定
		pVtx[i].vtx = PositionZero;

		// 法線ベクトルの設定
		pVtx[i].nor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

		// 反射光の設定
		pVtx[i].diffuse = WHITE(255);

		// テクスチャ座標の設定
		switch (i % 4)
		{
		case 0:
			pVtx[i].tex = D3DXVECTOR2(0.0f, 0.0f);
			break;
		case 1:
			pVtx[i].tex = D3DXVECTOR2(1.0f, 0.0f);
			break;
		case 2:
			pVtx[i].tex = D3DXVECTOR2(0.0f, 1.0f);
			break;
		case 3:
			pVtx[i].tex = D3DXVECTOR2(1.0f, 1.0f);
			break;
		default:
			break;
		}
	}

	// 頂点データをアンロックする
	SlashTrackVtxBuffer->Unlock();

	return S_OK;
}

//=============================================================================
// 斬撃の軌跡の頂点バッファを更新する
//=============================================================================
void UpdateVtxBuffer(void)
{
	int i = 0;
	int Surface_No = 0;

	// 頂点バッファの中身を埋める
	VERTEX_3D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	SlashTrackVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標クリア
	for (i = 0; i < VERTEX_NUM * Surface_Max; i++)
	{
		pVtx[i].vtx = PositionZero;
	}

	// 頂点座標の設定
	for (Surface_No = 0; Surface_No < Surface_Max; Surface_No++)
	{
		if (Surface[Surface_No].Use == true)
		{
			Surface[Surface_No].Color.a -= AlphaDecreaseSpeed;
			if (Surface[Surface_No].Color.a <= 0.0f)
			{
				Surface[Surface_No].Use = false;
				continue;
			}

			pVtx[(Surface_No * 4) + 0].diffuse = Surface[Surface_No].Color;
			pVtx[(Surface_No * 4) + 1].diffuse = Surface[Surface_No].Color;
			pVtx[(Surface_No * 4) + 2].diffuse = Surface[Surface_No].Color;
			pVtx[(Surface_No * 4) + 3].diffuse = Surface[Surface_No].Color;

			if (Surface[Surface_No].IsBack == false)
			{
				pVtx[(Surface_No * 4) + 0].vtx = Surface[Surface_No].Vtx_LeftUp;
				pVtx[(Surface_No * 4) + 1].vtx = Surface[Surface_No].Vtx_RightUp;
				pVtx[(Surface_No * 4) + 2].vtx = Surface[Surface_No].Vtx_LeftDown;
				pVtx[(Surface_No * 4) + 3].vtx = Surface[Surface_No].Vtx_RightDown;
			}
			else
			{
				pVtx[(Surface_No * 4) + 0].vtx = Surface[Surface_No].Vtx_RightUp;
				pVtx[(Surface_No * 4) + 1].vtx = Surface[Surface_No].Vtx_LeftUp;
				pVtx[(Surface_No * 4) + 2].vtx = Surface[Surface_No].Vtx_RightDown;
				pVtx[(Surface_No * 4) + 3].vtx = Surface[Surface_No].Vtx_LeftDown;
			}
		}
	}

	// 頂点データをアンロックする
	SlashTrackVtxBuffer->Unlock();

	return;
}

//=============================================================================
// 斬撃の軌跡を設置する
//=============================================================================
void SetSurface(D3DXVECTOR3 Vtx_LeftUp, D3DXVECTOR3 Vtx_RightUp, D3DXVECTOR3 Vtx_LeftDown, D3DXVECTOR3 Vtx_RightDown, D3DXCOLOR Color)
{
	int Surface_No = 0;

	for (int i = 0; i < 2; i++)
	{
		for (Surface_No = 0; Surface_No < Surface_Max; Surface_No++)
		{
			if (Surface[Surface_No].Use == false)
			{
				Surface[Surface_No].Use = true;
				if (i == 0)
				{
					Surface[Surface_No].IsBack = false;
				}
				else if (i == 1)
				{
					Surface[Surface_No].IsBack = true;
				}
				Surface[Surface_No].Color = Color;
				Surface[Surface_No].Vtx_LeftUp = Vtx_LeftUp;
				Surface[Surface_No].Vtx_RightUp = Vtx_RightUp;
				Surface[Surface_No].Vtx_LeftDown = Vtx_LeftDown;
				Surface[Surface_No].Vtx_RightDown = Vtx_RightDown;
				break;
			}
		}
	}

	return;
}

//=============================================================================
// エフェクトコントローラーを取得する
//=============================================================================
EFFECTCONTROLLER *GetFireSwordCtrl(void)
{
	return &FireSwordCtrl;
}