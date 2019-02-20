//=============================================================================
//
// エフェクト処理 [Effect.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Effect.h"
#include "Player.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"
#include "Transition.h"
#include "Tutorial.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define EffectMax (100)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// ボスに攻撃されたとき、炎のエフェクトの回転を計算する
void CalculateFireRot(int EffectID);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
EFFECTCONTROLLER		EffectCtrl;
EFFECT					Effect[EffectMax];

static const EFK_CHAR* EffectFileName[] =
{
	(const EFK_CHAR*)L"data/Effects/HPHeal/HPHeal.efk",
	(const EFK_CHAR*)L"data/Effects/HitEffect/HitBlood.efk",
	(const EFK_CHAR*)L"data/Effects/HitEffect/HitFire.efk",
	(const EFK_CHAR*)L"data/Effects/HitEffect/HitFire_Phase2.efk",
	(const EFK_CHAR*)L"data/Effects/HitEffect/HitLight.efk",
	(const EFK_CHAR*)L"data/Effects/LockOn/LockOn.efk",
	(const EFK_CHAR*)L"data/Effects/SummonSword/SummonSword.efk",
	(const EFK_CHAR*)L"data/Effects/ExplodeFire/ExplodeFire.efk",
	(const EFK_CHAR*)L"data/Effects/ExplodeFire/ExplodeFire_Phase2.efk",
	(const EFK_CHAR*)L"data/Effects/FireWall/FireWall.efk",
	(const EFK_CHAR*)L"data/Effects/FireWall/FireWall_Phase2.efk",
	(const EFK_CHAR*)L"data/Effects/DashSmoke/DashSmoke.efk",
	(const EFK_CHAR*)L"data/Effects/FirePillar/FirePillar.efk",
	(const EFK_CHAR*)L"data/Effects/PowerUp/PowerUp.efk",
	(const EFK_CHAR*)L"data/Effects/Tutorial/BossDebut.efk",
	(const EFK_CHAR*)L"data/Effects/BossDeath/BossDeath.efk",
	(const EFK_CHAR*)L"data/Effects/MagicCircle/MagicCircle.efk",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Effect_No = 0;

	for (Effect_No = 0; Effect_No < EffectMax; Effect_No++)
	{
		Effect[Effect_No].Pos = PositionZero;
		Effect[Effect_No].ID = -1;
		Effect[Effect_No].EffectType = -1;
		Effect[Effect_No].Use = false;
	}

	// 初めて初期化
	if (FirstInit == true)
	{
		EffectCtrl.Manager = NULL;
		EffectCtrl.Render = NULL;
		EffectCtrl.Effect = NULL;
		EffectCtrl.EffectNum = sizeof(EffectFileName) / sizeof(const EFK_CHAR*);

		// 描画用インスタンスの生成
		EffectCtrl.Render = ::EffekseerRendererDX9::Renderer::Create(Device, 30000);

		// エフェクト管理用インスタンスの生成
		EffectCtrl.Manager = ::Effekseer::Manager::Create(30000);

		// 描画用インスタンスから描画機能を設定
		EffectCtrl.Manager->SetSpriteRenderer(EffectCtrl.Render->CreateSpriteRenderer());
		EffectCtrl.Manager->SetRibbonRenderer(EffectCtrl.Render->CreateRibbonRenderer());
		EffectCtrl.Manager->SetRingRenderer(EffectCtrl.Render->CreateRingRenderer());
		EffectCtrl.Manager->SetTrackRenderer(EffectCtrl.Render->CreateTrackRenderer());
		EffectCtrl.Manager->SetModelRenderer(EffectCtrl.Render->CreateModelRenderer());

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		EffectCtrl.Manager->SetTextureLoader(EffectCtrl.Render->CreateTextureLoader());
		EffectCtrl.Manager->SetModelLoader(EffectCtrl.Render->CreateModelLoader());

		// メモリを確保
		EffectCtrl.Effect = (Effekseer::Effect**)calloc(EffectCtrl.EffectNum, sizeof(Effekseer::Effect*));
		if (EffectCtrl.Effect == NULL)
		{
			MessageBox(0, "Alloc Effect Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// エフェクトの読込
		for (int i = 0; i < EffectCtrl.EffectNum; i++)
		{
			if (FAILED(SafeLoadEffect(EffectCtrl.Manager, &EffectCtrl.Effect[i], EffectFileName[i], "Other")))
			{
				return E_FAIL;
			}
		}
	}

	// 全エフェクト停止
	EffectCtrl.Manager->StopAllEffects();
	EffectCtrl.Manager->Update();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
{
	// エフェクトの停止
	EffectCtrl.Manager->StopAllEffects();

	// エフェクトの破棄
	SafeFree(EffectCtrl.Effect);

	// 先にエフェクト管理用インスタンスを破棄
	EffectCtrl.Manager->Destroy();

	// 次に描画用インスタンスを破棄
	EffectCtrl.Render->Destroy();

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	static int Count = 0;
	int Effect_No = 0;
	int EffectID = 0;
	int GameStage = GetGameStage();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CUBE *Cube = GetCube();
	CAMERA_3RD *Camera = GetCamera_3rd();

	for (Effect_No = 0; Effect_No < EffectMax; Effect_No++)
	{
		if (Effect[Effect_No].Use == true)
		{
			EffectID = Effect[Effect_No].ID;
			if (EffectCtrl.Manager->GetShown(EffectID) == false)
			{
				if (Effect[Effect_No].EffectType == BossDeath)
				{
					Count = 0;
					Boss->Exist = false;
					SetTransition(Fadein);
				}
				EffectCtrl.Manager->StopEffect(Effect[Effect_No].ID);
				Effect[Effect_No].Use = false;
				continue;
			}

			switch (Effect[Effect_No].EffectType)
			{
			case HPHeal:
				EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(Player->Pos));
				break;
			case HitBlood:
				EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(Boss->CenterPos));
				break;
			case HitFire:
			case HitFire_Phase2:
				CalculateFireRot(EffectID);
				break;
			case HitLight:
				EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(Cube->Pos));
				break;
			case LockOn:
				if (Camera->InLockOn == true)
				{
					if (GameStage == Stage_Tutorial)
					{
						EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(Cube->Pos));
					}
					else if (GameStage == Stage_Game)
					{
						EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(Boss->CenterPos));
					}
				}
				else
				{
					EffectCtrl.Manager->StopEffect(Effect[Effect_No].ID);
					Effect[Effect_No].Use = false;
				}
				break;
			case BossDebut:
				Count++;
				if (Count >= 300)
				{
					Cube->EffectOver = true;
					Effect[Effect_No].Use = false;
					Count = 0;
				}
				break;
			case BossDeath:
				Count++;
				if (Count == 130)
				{
					StopSound(SE3D, SE3D_BossDeathWind);
					Set3DSound(Effect[Effect_No].Pos, SE3D_ExplosionWind, E_DS8_FLAG_NONE, true);
				}
				if (Count >= 150)
				{
					Boss->Exist = false;
				}
				break;
			case MagicCircle:
				if (Cube->Exist == false)
				{
					EffectCtrl.Manager->StopEffect(Effect[Effect_No].ID);
				}
				break;
			default:
				break;
			}
		}
	}

	// エフェクトの更新処理を行う
	EffectCtrl.Manager->Update();

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
{
	CAMERA_3RD *Camera = GetCamera_3rd();

	// 投影行列を設定
	EffectCtrl.Render->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z));

	// カメラ行列を設定
	EffectCtrl.Render->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(DXtoEffekVec(Camera->Pos), DXtoEffekVec(Camera->At), DXtoEffekVec(Camera->Up)));

	// エフェクトの描画開始処理を行う。
	EffectCtrl.Render->BeginRendering();

	// エフェクトの描画を行う。
	EffectCtrl.Manager->Draw();

	// エフェクトの描画終了処理を行う。
	EffectCtrl.Render->EndRendering();

	return;
}

//=============================================================================
// エフェクトを設置する
//=============================================================================
int SetEffect(D3DXVECTOR3 Pos, int EffectType)
{
	int Effect_No = 0;
	BOSS *Boss = GetBoss();

	for (Effect_No = 0; Effect_No < EffectMax; Effect_No++)
	{
		if (Effect[Effect_No].Use == false)
		{
			Effect[Effect_No].ID = EffectCtrl.Manager->Play(EffectCtrl.Effect[EffectType], Pos.x, Pos.y, Pos.z);
			Effect[Effect_No].Pos = Pos;
			Effect[Effect_No].EffectType = EffectType;
			Effect[Effect_No].Use = true;
			if (EffectType == FireWall)
			{
				EffectCtrl.Manager->SetRotation(Effect[Effect_No].ID, 0.0f, Boss->Rot.y, 0.0f);
			}
			else if (EffectType == MagicCircle)
			{
				EffectCtrl.Manager->SetRotation(Effect[Effect_No].ID, D3DXToRadian(90.0f), 0.0f, 0.0f);
			}
			else if (EffectType == ExplodeFire || EffectType == ExplodeFire_Phase2)
			{
				EffectCtrl.Manager->SetLocation(Effect[Effect_No].ID, Pos.x, 0.0f, Pos.z);
			}
			break;
		}
	}

	return Effect[Effect_No].ID;
}

//=============================================================================
// ボスは二段階に入る時のエフェクト
//=============================================================================
void TurnPhaseEffect(void)
{
	BOSS *Boss = GetBoss();

	int EffectID = EffectCtrl.Manager->Play(EffectCtrl.Effect[ExplodeFire_Phase2],
		Boss->Pos.x, Boss->Pos.y, Boss->Pos.z);
	EffectCtrl.Manager->SetScale(EffectID, 2.0f, 2.0f, 2.0f);

	Set3DSound(Boss->Pos, SE3D_ExplosionWind, E_DS8_FLAG_NONE, true);

	return;
}

//=============================================================================
// ボスに攻撃されたとき、炎のエフェクトの回転を計算する
//=============================================================================
void CalculateFireRot(int EffectID)
{
	PLAYER *Player = GetPlayer();
	// エフェクト回転方向
	D3DXVECTOR3 Direction;
	// 回転軸
	D3DXVECTOR3 RotAxis;
	// ボーン座標
	D3DXVECTOR3 BonePos;
	// 回転ラジアン
	float RotRadian = 0.0f;
	// ボーンマトリクス
	D3DMATRIX HipMatrix;

	// ボーンのマトリクスを取得
	HipMatrix = GetBoneMatrix(Player->Animation, "Hips");
	// ボーンの座標
	BonePos.x = HipMatrix._41;
	BonePos.y = HipMatrix._42;
	BonePos.z = HipMatrix._43;
	// ボーンの向き
	Direction.x = HipMatrix._31;
	Direction.y = HipMatrix._32;
	Direction.z = HipMatrix._33;
	D3DXVec3Normalize(&Direction, &Direction);
	// 回転ラジアン計算
	RotRadian = acosf(D3DXVec3Dot(&UpVector, &Direction));
	// エフェクトのデフォルト方向は上
	D3DXVec3Cross(&RotAxis, &UpVector, &Direction);
	D3DXVec3Normalize(&RotAxis, &RotAxis);
	// エフェクト座標
	EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(BonePos));
	// エフェクト回転
	EffectCtrl.Manager->SetRotation(EffectID, DXtoEffekVec(RotAxis), RotRadian + D3DX_PI);

	return;
}

//=============================================================================
// エフェクトコントローラーを取得する
//=============================================================================
EFFECTCONTROLLER *GetEffectCtrl(void)
{
	return &EffectCtrl;
}

//=============================================================================
// エフェクトを読み込む
//=============================================================================
HRESULT SafeLoadEffect(Effekseer::Manager *Manager, Effekseer::Effect **Effect, const EFK_CHAR* SrcPath, const char* ErrorSrc)
{
	char Message[256];
	// エフェクトファイルの中のテクスチャの数
	int TextureNum = 0;

	// エフェクトファイルを読み込む
	(*Effect) = Effekseer::Effect::Create(Manager, SrcPath);
	// 失敗したら
	if ((*Effect) == NULL)
	{
		sprintf(Message, "Load %s Effect Failed！", ErrorSrc);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	// エフェクトファイルの中のテクスチャの数を取得する
	TextureNum = (*Effect)->GetColorImageCount();
	for (int i = 0; i < TextureNum; i++)
	{
		// テクスチャのポインタを取得する
		Effekseer::TextureData *Texture = (*Effect)->GetColorImage(i);
		// テクスチャを読み込むが失敗したら
		if (Texture == NULL)
		{
			sprintf(Message, "Load %s Effect Texture Failed！", ErrorSrc);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	return S_OK;
}


