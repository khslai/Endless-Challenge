//=============================================================================
//
// ファイアボール処理 [FireBall.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "FireBall.h"
#include "Effect.h"
#include "Player.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"
#include "AttackRange.h"
#include "DebugProcess.h"
#include "Input.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BulletRadius			(3.0f)
#define BulletShotCount			(90)
#define BulletDeadCount			(120)
#define BezierDrawPoint			(120)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void BulletMove(int Bullet_No, int Time);
void CalculateBezier(int Bullet_No);
void MoveCheck(int Bullet_No);
void SetFireBallEffect(int Bullet_No, D3DXVECTOR3 Pos, int EffectType);
void DrawLine(D3DXVECTOR3 P1, D3DXVECTOR3 P2, D3DCOLOR Color);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
EFFECTCONTROLLER		FireBallCtrl;
BULLET					Bullet[BulletMax];

static const EFK_CHAR* FireBallFileName[] =
{
	(const EFK_CHAR*)L"data/Effects/FireBall/Fire.efk",
	(const EFK_CHAR*)L"data/Effects/FireBall/Burnning.efk",
	(const EFK_CHAR*)L"data/Effects/FireBall/Explode.efk",
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFireBall(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Bullet_No = 0;

	for (Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
	{
		Bullet[Bullet_No].Pos = PositionZero;
		Bullet[Bullet_No].Direction = PositionZero;
		Bullet[Bullet_No].Radius = BulletRadius;
		Bullet[Bullet_No].State = BulletInit;
		Bullet[Bullet_No].EffectID = -1;
		Bullet[Bullet_No].SphereID = -1;
		Bullet[Bullet_No].Use = false;
	}

	// 初めて初期化
	if (FirstInit == true)
	{
		FireBallCtrl.Manager = NULL;
		FireBallCtrl.Render = NULL;
		FireBallCtrl.Effect = NULL;
		FireBallCtrl.EffectNum = sizeof(FireBallFileName) / sizeof(const EFK_CHAR*);

		// 描画用インスタンスの生成
		FireBallCtrl.Render = ::EffekseerRendererDX9::Renderer::Create(Device, 2000);

		// エフェクト管理用インスタンスの生成
		FireBallCtrl.Manager = ::Effekseer::Manager::Create(10000);

		// 描画用インスタンスから描画機能を設定
		FireBallCtrl.Manager->SetSpriteRenderer(FireBallCtrl.Render->CreateSpriteRenderer());
		FireBallCtrl.Manager->SetRibbonRenderer(FireBallCtrl.Render->CreateRibbonRenderer());
		FireBallCtrl.Manager->SetRingRenderer(FireBallCtrl.Render->CreateRingRenderer());
		FireBallCtrl.Manager->SetTrackRenderer(FireBallCtrl.Render->CreateTrackRenderer());
		FireBallCtrl.Manager->SetModelRenderer(FireBallCtrl.Render->CreateModelRenderer());

		// 描画用インスタンスからテクスチャの読込機能を設定
		// 独自拡張可能、現在はファイルから読み込んでいる。
		FireBallCtrl.Manager->SetTextureLoader(FireBallCtrl.Render->CreateTextureLoader());
		FireBallCtrl.Manager->SetModelLoader(FireBallCtrl.Render->CreateModelLoader());

		// メモリを確保
		FireBallCtrl.Effect = (Effekseer::Effect**)calloc(FireBallCtrl.EffectNum, sizeof(Effekseer::Effect*));
		if (FireBallCtrl.Effect == NULL)
		{
			MessageBox(0, "Alloc FireBall Effect Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// エフェクトの読込
		for (int i = 0; i < FireBallCtrl.EffectNum; i++)
		{
			if (FAILED(SafeLoadEffect(FireBallCtrl.Manager, &FireBallCtrl.Effect[i], FireBallFileName[i], "FireBall")))
			{
				return E_FAIL;
			}
		}
	}

	// 全エフェクト停止
	FireBallCtrl.Manager->StopAllEffects();
	FireBallCtrl.Manager->Update();

	return S_OK;
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitFireBall(void)
{
	// エフェクトの停止
	FireBallCtrl.Manager->StopAllEffects();

	// エフェクトの破棄
	SafeFree(FireBallCtrl.Effect);

	// 先にエフェクト管理用インスタンスを破棄
	FireBallCtrl.Manager->Destroy();

	// 次に描画用インスタンスを破棄
	FireBallCtrl.Render->Destroy();

	return;
}


//=============================================================================
// 更新処理
//=============================================================================
void UpdateFireBall(void)
{
	PLAYER *Player = GetPlayer();
	int Bullet_No = 0;

	for (Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
	{
		if (Bullet[Bullet_No].Use == true)
		{
			Bullet[Bullet_No].Count++;
			switch (Bullet[Bullet_No].State)
			{
				// 弾待機中
			case Standby:

				// 弾発射
				if ((Bullet[Bullet_No].Count % Bullet[Bullet_No].ShotTime) == 0)
				{
					Bullet[Bullet_No].State = Shot;
					Bullet[Bullet_No].Count = 0;
					CalculateBezier(Bullet_No);
				}
				break;

				// 弾移動中
			case Shot:

				if (Bullet[Bullet_No].Count <= BezierDrawPoint)
				{
					if (Bullet[Bullet_No].Count <= 60.0f)
					{
						Bullet[Bullet_No].DestPos = Player->CenterPos;
					}
					BulletMove(Bullet_No, Bullet[Bullet_No].Count);
				}
				else
				{
					Bullet[Bullet_No].Pos += Bullet[Bullet_No].Direction * Bullet[Bullet_No].Speed;
				}
				MoveCheck(Bullet_No);
				FireBallCtrl.Manager->SetLocation(Bullet[Bullet_No].EffectID, DXtoEffekVec(Bullet[Bullet_No].Pos));
				break;

				// 直撃命中エフェクト
			case SetBurnningEffect:

				SetFireBallEffect(Bullet_No, Player->Pos, Burnning);
				Bullet[Bullet_No].Use = false;
				break;

				// 消えるまで待つ
			case WaitDisappear:

				if (Bullet[Bullet_No].Count >= BulletDeadCount)
				{
					Bullet[Bullet_No].State = DisappearExplode;
				}
				break;

				// 踏まれる爆発
			case BurnningExplode:
				// 消える爆発
			case DisappearExplode:

				// 爆発効果音
				Set3DSound(Bullet[Bullet_No].Pos, SE3D_FireBallExplode, E_DS8_FLAG_NONE, true);
				// 爆発エフェクト
				SetFireBallEffect(Bullet_No, Bullet[Bullet_No].Pos, Explode);
				Bullet[Bullet_No].State = WaitExplodeOver;
				Bullet[Bullet_No].SphereID = SetRangeSphere(Bullet[Bullet_No].Pos, ExplodeRange);
				break;

				// 爆発が終わる
			case WaitExplodeOver:

				if (FireBallCtrl.Manager->GetShown(Bullet[Bullet_No].EffectID) == false)
				{
					FireBallCtrl.Manager->StopEffect(Bullet[Bullet_No].EffectID);
					Bullet[Bullet_No].Use = false;
					DeleteRangeSphere(Bullet[Bullet_No].SphereID);
				}
				break;

			case OverStageRange:

				Bullet[Bullet_No].Pos += Bullet[Bullet_No].Direction * Bullet[Bullet_No].Speed;
				FireBallCtrl.Manager->SetLocation(Bullet[Bullet_No].EffectID, DXtoEffekVec(Bullet[Bullet_No].Pos));
				if ((Bullet[Bullet_No].Count % 30) == 0)
				{
					FireBallCtrl.Manager->StopEffect(Bullet[Bullet_No].EffectID);
					Bullet[Bullet_No].Use = false;
					DeleteRangeSphere(Bullet->SphereID);
				}
				break;

			default:
				break;
			}
		}
	}

	// エフェクトの更新処理を行う
	FireBallCtrl.Manager->Update();

	return;
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawFireBall(void)
{
	CAMERA_3RD *Camera = GetCamera_3rd();

	//for (int Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
	//{
	//	if (Bullet[Bullet_No].Use == true)
	//	{
	//		DrawLine(Bullet[Bullet_No].StartPos, Bullet[Bullet_No].BezierPoint[0], BLACK(255));
	//		DrawLine(Bullet[Bullet_No].StartPos, Bullet[Bullet_No].BezierPoint[1], BLACK(255));
	//	}
	//}

	// 投影行列を設定
	FireBallCtrl.Render->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z));

	// カメラ行列を設定
	FireBallCtrl.Render->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(DXtoEffekVec(Camera->Pos), DXtoEffekVec(Camera->At), DXtoEffekVec(Camera->Up)));

	// エフェクトの描画開始処理を行う。
	FireBallCtrl.Render->BeginRendering();

	// エフェクトの描画を行う。
	FireBallCtrl.Manager->Draw();

	// エフェクトの描画終了処理を行う。
	FireBallCtrl.Render->EndRendering();

	return;
}

//=============================================================================
// ベジェ曲線の点を計算
//=============================================================================
void CalculateBezier(int Bullet_No)
{
	PLAYER *Player = GetPlayer();
	int PositionRate = 0;
	float Distance = 0.0f;
	float Rot_Yaxis = 0.0f;
	D3DXMATRIX RotMatrix;

	// 目標座標設定
	Bullet[Bullet_No].DestPos = Player->CenterPos;

	// 方向ベクトル計算
	Bullet[Bullet_No].Direction = Player->CenterPos - Bullet[Bullet_No].StartPos;
	// プレイヤーと弾の間の長さ
	Distance = D3DXVec3Length(&Bullet[Bullet_No].Direction);

	Rot_Yaxis = -40.0f + Bullet_No * 20.0f;

	// プレイヤーと弾の位置によって、回転の方向が変わる
	if (Bullet[Bullet_No].Pos.z >= Player->Pos.z)
	{
		PositionRate = 1;
	}
	else
	{
		PositionRate = -1;
	}

	// 初期化
	D3DXMatrixIdentity(&RotMatrix);
	// 方向ベクトル回転
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, D3DXToRadian(Rot_Yaxis), D3DXToRadian(45.0f * PositionRate), D3DXToRadian(0.0f));
	D3DXVec3TransformCoord(&Bullet[Bullet_No].Direction, &Bullet[Bullet_No].Direction, &RotMatrix);
	D3DXVec3Normalize(&Bullet[Bullet_No].Direction, &Bullet[Bullet_No].Direction);
	// ベジェ曲線のP1を計算
	Bullet[Bullet_No].BezierPoint[0] = Bullet[Bullet_No].Pos + Bullet[Bullet_No].Direction * Distance * 0.33f;

	// 初期化
	D3DXMatrixIdentity(&RotMatrix);
	// 方向ベクトル回転
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, D3DXToRadian(Rot_Yaxis), D3DXToRadian(30.0f * PositionRate), D3DXToRadian(0.0f));
	D3DXVec3TransformCoord(&Bullet[Bullet_No].Direction, &Bullet[Bullet_No].Direction, &RotMatrix);
	D3DXVec3Normalize(&Bullet[Bullet_No].Direction, &Bullet[Bullet_No].Direction);
	// ベジェ曲線のP2を計算
	Bullet[Bullet_No].BezierPoint[1] = Bullet[Bullet_No].Pos + Bullet[Bullet_No].Direction * Distance * 0.66f;

	return;
}

//=============================================================================
// 弾の移動計算
//=============================================================================
void BulletMove(int Bullet_No, int Time)
{
	float f1, f2, f3, f4;
	float deltaT = 1.0f / BezierDrawPoint;
	float T;
	D3DXVECTOR3 Pos[4] = {
	Bullet[Bullet_No].StartPos,
	Bullet[Bullet_No].BezierPoint[0],
	Bullet[Bullet_No].BezierPoint[1],
	Bullet[Bullet_No].DestPos };

	// B(t)=P0*(1−t)^3 + 3*P1*t*(1−t)^2 + 3*P2*t^2*(1−t) + P3*t^3
	if (Time <= BezierDrawPoint)
	{
		T = Time * deltaT;
		f1 = powf((1 - T), 3);
		f2 = 3 * T * powf((1 - T), 2);
		f3 = 3 * powf(T, 2) * (1 - T);
		f4 = powf(T, 3);
		Bullet[Bullet_No].Pos.x = f1 * Pos[0].x + f2 * Pos[1].x + f3 * Pos[2].x + f4 * Pos[3].x;
		Bullet[Bullet_No].Pos.y = f1 * Pos[0].y + f2 * Pos[1].y + f3 * Pos[2].y + f4 * Pos[3].y;
		Bullet[Bullet_No].Pos.z = f1 * Pos[0].z + f2 * Pos[1].z + f3 * Pos[2].z + f4 * Pos[3].z;

		if (Time == BezierDrawPoint - 1)
		{
			Bullet[Bullet_No].PrePos = Bullet[Bullet_No].Pos;
		}
		else if (Time == BezierDrawPoint)
		{
			Bullet[Bullet_No].Direction = Bullet[Bullet_No].Pos - Bullet[Bullet_No].PrePos;
			Bullet[Bullet_No].Speed = D3DXVec3Length(&Bullet[Bullet_No].Direction);
			D3DXVec3Normalize(&Bullet[Bullet_No].Direction, &Bullet[Bullet_No].Direction);
		}
	}

	return;
}


//=============================================================================
// 地面、壁との衝突判定
//=============================================================================
void MoveCheck(int Bullet_No)
{
	float StageRange = Stage_Size + 100.0f;

	// ステージ範囲内
	if (Bullet[Bullet_No].Pos.x < Stage_Size && Bullet[Bullet_No].Pos.x > -Stage_Size &&
		Bullet[Bullet_No].Pos.z < Stage_Size && Bullet[Bullet_No].Pos.z > -Stage_Size)
	{
		// 床に当たる
		if (Bullet[Bullet_No].Pos.y > 1000.0f || Bullet[Bullet_No].Pos.y <= 3.0f)
		{
			Set3DSound(Bullet[Bullet_No].Pos, SE3D_HitFire, E_DS8_FLAG_NONE, false);

			Bullet[Bullet_No].State = WaitDisappear;
			Bullet[Bullet_No].Count = 0;
			Bullet[Bullet_No].SphereID = SetRangeSphere(Bullet[Bullet_No].Pos, BurnningRange);
		}
	}
	// ステージ範囲を超える
	else
	{
		Bullet[Bullet_No].State = OverStageRange;
		Bullet[Bullet_No].Count = 0;
	}

	return;
}

//=============================================================================
// 弾を設置する
//=============================================================================
void SetFireBall(D3DXVECTOR3 Pos)
{
	int Bullet_No = 0;

	for (Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
	{
		if (Bullet[Bullet_No].Use == false)
		{
			Bullet[Bullet_No].Pos = Pos;
			Bullet[Bullet_No].PrePos = Pos;
			Bullet[Bullet_No].StartPos = Pos;
			Bullet[Bullet_No].DestPos = PositionZero;
			Bullet[Bullet_No].BezierPoint[0] = PositionZero;
			Bullet[Bullet_No].BezierPoint[1] = PositionZero;
			Bullet[Bullet_No].Direction = PositionZero;
			Bullet[Bullet_No].ShotTime = BulletShotCount + Bullet_No * 10;
			Bullet[Bullet_No].Count = 0;
			Bullet[Bullet_No].Speed = 0;
			Bullet[Bullet_No].State = Standby;
			Bullet[Bullet_No].Use = true;

			// エフェクト
			SetFireBallEffect(Bullet_No, Bullet[Bullet_No].Pos, BulletFire);

			// 効果音
			Set3DSound(Pos, SE3D_SetFireBall, E_DS8_FLAG_NONE, true);

			break;
		}
	}

	return;
}

//=============================================================================
// 弾のエフェクトを設置する
//=============================================================================
void SetFireBallEffect(int Bullet_No, D3DXVECTOR3 Pos, int EffectType)
{
	if (EffectType >= 0 && EffectType < FireBallCtrl.EffectNum)
	{
		if (Bullet[Bullet_No].Use == true)
		{
			// 前のエフェクトを停止
			if (Bullet[Bullet_No].EffectID != -1)
			{
				FireBallCtrl.Manager->StopEffect(Bullet[Bullet_No].EffectID);
				DeleteRangeSphere(Bullet[Bullet_No].SphereID);
			}

			// 新しいエフェクト設置
			Bullet[Bullet_No].EffectID = FireBallCtrl.Manager->Play(FireBallCtrl.Effect[EffectType], Pos.x, Pos.y, Pos.z);
		}
	}

	return;
}

//=============================================================================
// 弾を取得する
//=============================================================================
BULLET *GetBullet(int Bullet_No)
{
	return &Bullet[Bullet_No];
}