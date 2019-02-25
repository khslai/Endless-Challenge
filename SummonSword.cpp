//=============================================================================
//
// 剣の召喚処理 [SummonSword.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "SummonSword.h"
#include "Player.h"
#include "Boss.h"
#include "FireSword.h"
#include "Effect.h"
#include "Tutorial.h"
#include "Input.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	Model_BossSword		"data/Model/BossSword.x"	// 読み込むモデル名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 剣を設置する
void SetSummonSword(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
SUMMONSWORD			SummonSword;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSummonSword(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sword_No = 0;

	// 位置・回転・スケールの初期設定
	SummonSword.Pos = PositionZero;
	SummonSword.Rot = PositionZero;
	SummonSword.Scale = DefaultScale;
	SummonSword.EffectID = -1;
	SummonSword.State = -1;
	SummonSword.Count = 0;
	SummonSword.Use = false;
	SummonSword.BurnedFire = false;

	// 初めて初期化
	if (FirstInit == true)
	{
		SummonSword.Texture = NULL;
		SummonSword.MaterialBuffer = NULL;
		SummonSword.Mesh = NULL;
		SummonSword.MaterialNum = 0;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(Model_BossSword,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,							// メッシュの作成オプションを指定
			Device,										// IDirect3DDevice9インターフェイスへのポインタ
			NULL,										// 隣接性データを含むバッファへのポインタ
			&SummonSword.MaterialBuffer,		// マテリアルデータを含むバッファへのポインタ
			NULL,										// エフェクトインスタンスの配列を含むバッファへのポインタ
			&SummonSword.MaterialNum,			// D3DXMATERIAL構造体の数
			&SummonSword.Mesh)))				// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		// モデル用のテクスチャのメモリ空間を配る
		SummonSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(SummonSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (SummonSword.Texture == NULL)
		{
			MessageBox(0, "Alloc BossSword Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// テクスチャを読み込む
		if (FAILED(SafeLoadModelTexture(SummonSword.Texture, SummonSword.MaterialBuffer, SummonSword.MaterialNum, "BossSword")))
		{
			return E_FAIL;
		}

		// 当たり判定カプセルを生成
		if (FAILED(
			CreateCapsule(&SummonSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 90.0f, 2.0f, false)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSummonSword(void)
{
	// テクスチャの開放
	for (int i = 0; i < (int)SummonSword.MaterialNum; i++)
	{
		SafeRelease(SummonSword.Texture[i]);
	}

	// マテリアルの開放
	SafeRelease(SummonSword.MaterialBuffer);

	// メッシュの開放
	SafeRelease(SummonSword.Mesh);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateSummonSword(void)
{
	PLAYER *Player = GetPlayer();
	EFFECTCONTROLLER *FireSwordCtrl = GetFireSwordCtrl();
	EFFECTCONTROLLER *EffectCtrl = GetEffectCtrl();
	float RotRadian = 0.0f;
	int TutorialState = GetTutorialState();

	if (TutorialState == Tutorial_SetSword)
	{
		SetTutorialState(Tutorial_WaitRolling);
		SetSummonSword();
		return;
	}

	if (SummonSword.Use == true)
	{
		SummonSword.Count++;

		switch (SummonSword.State)
		{
		case SetSummonEffect:

			// エフェクトを設置する
			SummonSword.EffectID = SetEffect(SummonSword.HitCapsule.P2, SummonSwordEffect);

			// エフェクトの回転を計算する
			RotRadian = acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &Player->Direction));
			if (Player->Direction.x >= 0.0f)
			{
				EffectCtrl->Manager->SetRotation(SummonSword.EffectID, D3DXToRadian(90.0f), RotRadian, D3DXToRadian(0.0f));
			}
			else
			{
				EffectCtrl->Manager->SetRotation(SummonSword.EffectID, D3DXToRadian(90.0f), -RotRadian, D3DXToRadian(0.0f));
			}

			SummonSword.State = SetFireEffect;
			SummonSword.Count = 0;
			break;

		case SetFireEffect:

			if (SummonSword.BurnedFire == false && SummonSword.Count >= 90)
			{
				SummonSword.EffectID = SetFireSword(SummonSword.Pos, D3DXVECTOR3(0.0f, -1.0f, 0.0f));
				SummonSword.BurnedFire = true;
				SummonSword.State = WaitRolling;
				SummonSword.Count = 0;
			}
			break;

		case WaitRolling:

			if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A))
			{
				ChangeAnimation(Player->Animation, Rolling, 1.5f, true);
				SummonSword.State = AttackStart;
				Set3DSound(SummonSword.Pos, SE3D_BossSlash, E_DS8_FLAG_NONE, true);
				Set3DSound(SummonSword.Pos, SE3D_SlashFire, E_DS8_FLAG_NONE, true);
			}
			break;

		case AttackStart:

			SummonSword.Rot.z += 0.1f;
			SetSlashFire(SummonSword.HitCapsule.P2, SummonSword.HitCapsule.PreP2);
			SetSurface(SummonSword.HitCapsule.PreP1, SummonSword.HitCapsule.P1,
				SummonSword.HitCapsule.PreP2, SummonSword.HitCapsule.P2, FIRECOLOR(200));
			if (SummonSword.HitCapsule.P2.y <= 0.0f)
			{
				SummonSword.HitCapsule.P2.y = 0.0f;
				SetEffect(SummonSword.HitCapsule.P2, ExplodeFire);
				Set3DSound(SummonSword.HitCapsule.P2, SE3D_ExplodeFire, E_DS8_FLAG_NONE, true);
				FireSwordCtrl->Manager->StopEffect(SummonSword.EffectID);
				SummonSword.Use = false;
			}
			break;

		default:
			break;
		}

		// 炎の剣のエフェクト回転を計算する
		if (SummonSword.BurnedFire == true)
		{
			FireSwordCtrl->Manager->SetLocation(SummonSword.EffectID, DXtoEffekVec(SummonSword.HitCapsule.P2));
			D3DXVECTOR3 RotAxis;
			D3DXVECTOR3 Direction = SummonSword.HitCapsule.Direction;
			D3DXVec3Normalize(&Direction, &Direction);
			float RotRadian = acosf(D3DXVec3Dot(&D3DXVECTOR3(1.0f, 0.0f, 0.0f), &Direction));
			D3DXVec3Cross(&RotAxis, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), &Direction);
			D3DXVec3Normalize(&RotAxis, &RotAxis);
			FireSwordCtrl->Manager->SetRotation(SummonSword.EffectID, DXtoEffekVec(RotAxis), RotRadian);
		}
	}

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawSummonSword(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATRIX CapsuleMatrix;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 DefaultMat;

	if (SummonSword.Use == true)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&SummonSword.WorldMatrix);
		D3DXMatrixIdentity(&CapsuleMatrix);
		CapsuleMatrix._31 = 0.0f;
		CapsuleMatrix._32 = -1.0f;
		CapsuleMatrix._33 = 0.0f;

		// スケールを反映
		D3DXMatrixScaling(&ScaleMatrix, SummonSword.Scale.x, SummonSword.Scale.y, SummonSword.Scale.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &ScaleMatrix);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, SummonSword.Rot.y, SummonSword.Rot.x, SummonSword.Rot.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &RotMatrix);

		// 移動を反映
		D3DXMatrixTranslation(&TransMatrix, SummonSword.Pos.x, SummonSword.Pos.y, SummonSword.Pos.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &TransMatrix);

		// ワールドマトリックスの設定
		Device->SetTransform(D3DTS_WORLD, &SummonSword.WorldMatrix);

		// 現在のマテリアルを取得
		Device->GetMaterial(&DefaultMat);

		// マテリアル情報に対するポインタを取得
		pD3DXMat = (D3DXMATERIAL*)SummonSword.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)SummonSword.MaterialNum; nCntMat++)
		{
			// マテリアルの設定
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// テクスチャの設定
			Device->SetTexture(0, SummonSword.Texture[nCntMat]);

			// 描画
			SummonSword.Mesh->DrawSubset(nCntMat);
		}

		// マテリアルをデフォルトに戻す
		Device->SetMaterial(&DefaultMat);

		// 立方体のカプセルを描画する
		D3DXMatrixMultiply(&CapsuleMatrix, &CapsuleMatrix, &SummonSword.WorldMatrix);
		DrawCapsule(&SummonSword.HitCapsule, &CapsuleMatrix);
	}

	return;
}

//=============================================================================
// 剣を設置する
//=============================================================================
void SetSummonSword(void)
{
	PLAYER *Player = GetPlayer();

	// 最初から始まる
	if (SummonSword.Use == false)
	{
		SummonSword.Use = true;
		SummonSword.BurnedFire = false;
		SummonSword.Count = 0;
		SummonSword.State = SetSummonEffect;
		SummonSword.Pos.x = Player->CenterPos.x + sinf(Player->DestAngle + D3DX_HalfPI) * 50.0f;
		SummonSword.Pos.y = Player->CenterPos.y;
		SummonSword.Pos.z = Player->CenterPos.z + cosf(Player->DestAngle + D3DX_HalfPI) * 50.0f;

		// 剣のモデル回転
		SummonSword.Rot.y = Player->DestAngle;
	}

	return;
}

//=============================================================================
// ポインタを取得する
//=============================================================================
SUMMONSWORD *GetSummonSword()
{
	return &SummonSword;
}