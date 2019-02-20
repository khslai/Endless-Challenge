//=============================================================================
//
// ボス処理 [Boss.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Boss.h"
#include "Player.h"
#include "Equipment.h"
#include "FireSword.h"
#include "ThirdPersonCamera.h"
#include "D3DXAnimation.h"
#include "Transition.h"
#include "Input.h"
#include "Title.h"
#include "DebugProcess.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	Model_Boss			"data/Model/Boss.x"			// 読み込むモデル名
#define BossMoveSpeed		(0.2f)						// 移動速度
#define DefaultHP			(100.0f)					// 最大HP
#define	BossRotInertia		(0.2f)						// 回転慣性係数

// プレイヤーとの距離標準
#define VeryCloseBorderline	(100.0f)
#define CloseBorderline		(200.0f)
#define MediumBorderline	(350.0f)
#define FarBorderline		(550.0f)
enum DistanceStandard
{
	VeryClose,			// VeryCloseBorderline以内
	Close,				// VeryCloseBorderline ~ CloseBorderline
	Medium,				// CloseBorderline ~ MediumBorderline
	Far,				// MediumBorderline ~ FarBorderline
	VeryFar,			// FarBorderline以上
};

typedef struct
{
	int		AnimID;		// モーションID
	int		Priority;	// 優先度
}ATTACKINFO;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// ボスのアニメーションを制御する
static void AnimationManager(void);
// ボスの移動を制御する
void BossMove(void);
// ボスの行動を制御する
void BossAI(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
BOSS				Boss;

// モーションID , 優先度(全部100%)
// とても近距離
ATTACKINFO VeryCloseAttack[] =
{
	{BossAct_LeftRightSlash, 50},
	{BossAct_SurfaceSlash, 50},
};

// 近距離
ATTACKINFO CloseAttack_Phase1[] =
{
	{BossAct_VerticalSlash, 25},
	{BossAct_ComboAttack2, 35},
	{BossAct_TwoHandCombo1, 40},
};
ATTACKINFO CloseAttack_Phase2[] =
{
	{BossAct_VerticalSlash, 15},
	{BossAct_ComboAttack2, 15},
	{BossAct_ComboAttack3, 35},
	{BossAct_TwoHandCombo2, 35},
};

// 中距離
ATTACKINFO MediumAttack_Phase1[] =
{
	{BossAct_UpSlash, 15},
	{BossAct_HorizonSlash, 35},
	{BossAct_RollingSlash, 35},
	{BossAct_ComboAttack1, 15},
};
ATTACKINFO MediumAttack_Phase2[] =
{
	{BossAct_HorizonSlash, 40},
	{BossAct_RollingSlash, 30},
	{BossAct_ComboAttack1, 30},
};

// 遠距離
ATTACKINFO FarAttack_Phase1[] =
{
	{BossAct_Stab, 100},
};
ATTACKINFO FarAttack_Phase2[] =
{
	{BossAct_Stab, 40},
	{BossAct_SlantSlash, 60},
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBoss(bool FirstInit)
{
	int GameStage = GetGameStage();
	int Difficulty = GetDifficulty();
	int Set_No = 0;
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// パラメータの初期設定
	D3DXMatrixIdentity(&Boss.WorldMatrix);
	Boss.Pos = D3DXVECTOR3(0.0f, 0.0f, 300.0f);
	Boss.CenterPos = PositionZero;
	Boss.PrePos = PositionZero;
	Boss.Move = PositionZero;
	Boss.Rot = D3DXVECTOR3(0.0f, D3DXToRadian(180.0f), 0.0f);
	Boss.Scale = DefaultScale;
	Boss.DestAngle = 0.0f;
	Boss.HPDecreaseStart = false;
	Boss.SetFireSword = false;
	Boss.GiveDamage = false;
	Boss.TurnRotation = true;
	Boss.JumpState = Jump_Stop;
	Boss.DistanceState = -1;

	// 難易度によって、HP、行動速度が変わる
	if (Difficulty == Easy)
	{
		Boss.HP_Max = DefaultHP * 0.8f;
		Boss.HP = Boss.HP_Max;
		Boss.PreviousHP = Boss.HP_Max;
		Boss.ActionSpeed = 0.8f;
	}
	else if (Difficulty == Normal)
	{
		Boss.HP_Max = DefaultHP;
		Boss.HP = Boss.HP_Max;
		Boss.PreviousHP = Boss.HP_Max;
		Boss.ActionSpeed = 1.0f;
	}
	else if (Difficulty == AppealDifficulty)
	{
		Boss.HP_Max = DefaultHP * 0.6f;
		Boss.HP = Boss.HP_Max;
		Boss.PreviousHP = Boss.HP_Max;
		Boss.ActionSpeed = 1.0f;
	}

	// 存在フラグ
	if (GameStage == Stage_Game)
	{
		Boss.Exist = true;
	}
	else
	{
		Boss.Exist = false;
	}

	// ボスの段階
	if (Difficulty != AppealDifficulty)
	{
		Boss.Phase = Phase1;
	}
	else
	{
		Boss.Phase = TurnToPhase2;
	}

	// 初めて初期化
	if (FirstInit == true)
	{
		// 剣を取得する
		Boss.Sword = GetSword("Boss");
		if (Boss.Sword == NULL)
		{
			return E_FAIL;
		}

		// アニメーションオブジェクトを作る
		Boss.Animation = (D3DXANIMATION*)calloc(1, sizeof(D3DXANIMATION));
		Boss.Animation->Owner = "Boss";
		Boss.Animation->CurrentAnimID = -1;
		Boss.Animation->NextAnimID = BossAct_Idle;

		// xfileを読み込む
		if (FAILED(Load_xFile(Boss.Animation, Model_Boss, "Boss")))
		{
			return E_FAIL;
		}

		for (Set_No = 0; Set_No < Boss.Animation->AnimSetNum; Set_No++)
		{
			switch (Set_No)
			{
				//									SetName	 AnimSet  ShiftTime  CurWeightTime  Damage
			case BossAct_Idle:
				Boss.Animation->AnimData[Set_No] = { "Idle",				NULL, 0.3f,	0.0f, 0.0f };
				break;
			case BossAct_Walk:
				Boss.Animation->AnimData[Set_No] = { "Walk",				NULL, 0.1f, 0.0f, 0.0f };
				break;
			case BossAct_HitReact:
				Boss.Animation->AnimData[Set_No] = { "HitReact",			NULL, 0.1f,	0.0f, 0.0f };
				break;
			case BossAct_Rolling:
				Boss.Animation->AnimData[Set_No] = { "Rolling",				NULL, 0.1f,	0.0f, 0.0f };
				break;
			case BossAct_BossDeath:
				Boss.Animation->AnimData[Set_No] = { "BossDeath",			NULL, 0.2f,	0.0f, 0.0f };
				break;
			case BossAct_SlantSlash:
				Boss.Animation->AnimData[Set_No] = { "SlantSlash",			NULL, 0.1f,	0.0f, 30.0f };
				break;
			case BossAct_LeftRightSlash:
				Boss.Animation->AnimData[Set_No] = { "LeftRightSlash",		NULL, 0.1f,	0.0f, 30.0f };
				break;
			case BossAct_RollingSlash:
				Boss.Animation->AnimData[Set_No] = { "RollingSlash",		NULL, 0.1f,	0.0f, 60.0f };
				break;
			case BossAct_SurfaceSlash:
				Boss.Animation->AnimData[Set_No] = { "SurfaceSlash",		NULL, 0.1f,	0.0f, 30.0f };
				break;
			case BossAct_UpSlash:
				Boss.Animation->AnimData[Set_No] = { "UpSlash",				NULL, 0.3f,	0.0f, 30.0f };
				break;
			case BossAct_HorizonSlash:
				Boss.Animation->AnimData[Set_No] = { "HorizonSlash",		NULL, 0.1f,	0.0f, 45.0f };
				break;
			case BossAct_VerticalSlash:
				Boss.Animation->AnimData[Set_No] = { "VerticalSlash",		NULL, 0.2f, 0.0f, 60.0f };
				break;
			case BossAct_Stab:
				Boss.Animation->AnimData[Set_No] = { "Stab",				NULL, 0.1f,	0.0f, 35.0f };
				break;
			case BossAct_ComboAttack1:
				Boss.Animation->AnimData[Set_No] = { "ComboAttack1",		NULL, 0.2f, 0.0f, 17.0f };
				break;
			case BossAct_ComboAttack2:
				Boss.Animation->AnimData[Set_No] = { "ComboAttack2",		NULL, 0.2f, 0.0f, 17.0f };
				break;
			case BossAct_ComboAttack3:
				Boss.Animation->AnimData[Set_No] = { "ComboAttack3",		NULL, 0.2f, 0.0f, 20.0f };
				break;
			case BossAct_TwoHandCombo1:
				Boss.Animation->AnimData[Set_No] = { "TwoHandCombo1",		NULL, 0.2f, 0.0f, 20.0f };
				break;
			case BossAct_TwoHandCombo2:
				Boss.Animation->AnimData[Set_No] = { "TwoHandCombo2",		NULL, 0.2f, 0.0f, 20.0f };
				break;
			case BossAct_PowerUp:
				Boss.Animation->AnimData[Set_No] = { "PowerUp",				NULL, 0.1f, 0.0f, 0.0f };
				break;
			default:
				break;
			}
		}

		// AnimationCallbackを設置する
		if (FAILED(InitCallbackKeys_Boss()))
		{
			return E_FAIL;
		}

		// AnimationSetを初期化する
		for (Set_No = 0; Set_No < Boss.Animation->AnimSetNum; Set_No++)
		{
			if (FAILED(InitAnimation(Boss.Animation, Set_No)))
			{
				return E_FAIL;
			}
		}

		// 最初は待機状態
		ChangeAnimation(Boss.Animation, BossAct_Idle, 1.0f, false);

		// 当たり判定カプセルを生成
		if (FAILED(
			CreateCapsule(&Boss.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 60.0f, 15.0f, true)))
		{
			return E_FAIL;
		}
	}
	else
	{
		Boss.Animation->MotionEnd = false;
		Boss.Animation->StartMove = false;
		Boss.Animation->SlashTrack = false;
		Boss.Animation->MotionBlendOver = false;
		Boss.Animation->Cancelable = false;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBoss(void)
{
	// アニメーションをリリース
	UninitAnimation(Boss.Animation);

	// カプセルをリリース
	UninitCapsule(&Boss.HitCapsule);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBoss(void)
{
	EFFECTCONTROLLER *FireSwordCtrl = GetFireSwordCtrl();
	int GameCount = GetGameCount();
	int Transition = GetTransition();
	// ダメージを受けた後カウント
	static int HPDecreaseCount = 0;
	static int Count = 0;

	if (Boss.Exist == false || Transition != TransitionOver)
	{
		// 中心座標更新
		Boss.CenterPos = D3DXVECTOR3(Boss.Pos.x, Boss.Pos.y + 50.0f, Boss.Pos.z);
		return;
	}

	// 前フレイムの座標を記録する
	Boss.PrePos = Boss.Pos;

	// 炎の剣のエフェクト
	if (Boss.SetFireSword == true)
	{
		// エフェクトの座標は剣の端点を設置する
		FireSwordCtrl->Manager->SetLocation(Boss.Sword->EffectID, DXtoEffekVec(Boss.Sword->HitCapsule.P1));
		// エフェクトの回転軸
		D3DXVECTOR3 RotAxis;
		// 目的方向
		D3DXVECTOR3 Direction = Boss.Sword->HitCapsule.Direction;
		D3DXVec3Normalize(&Direction, &Direction);
		// 回転ラジアン計算
		float RotRadian = acosf(D3DXVec3Dot(&RightVector, &Direction));
		// エフェクトのデフォルト方向は右
		D3DXVec3Cross(&RotAxis, &RightVector, &Direction);
		D3DXVec3Normalize(&RotAxis, &RotAxis);
		// エフェクト回転
		FireSwordCtrl->Manager->SetRotation(Boss.Sword->EffectID, DXtoEffekVec(RotAxis), RotRadian - D3DX_PI);
		// 段階変換中、エフェクト停止
		if (Boss.Phase == TurnToPhase2)
		{
			Boss.SetFireSword = false;
			FireSwordCtrl->Manager->StopEffect(Boss.Sword->EffectID);
		}
	}
	else
	{
		if (Boss.Phase == Phase1 || Boss.Phase == Phase2)
		{
			// 炎の剣のエフェクトを設置する
			Boss.Sword->EffectID = SetFireSword(Boss.Sword->HitCapsule.P1, Boss.Sword->HitCapsule.Direction);
			Boss.SetFireSword = true;
		}
	}

	// 第二段階に入る
	if (Boss.Phase == TurnToPhase2 && Boss.Animation->CurrentAnimID != BossAct_PowerUp &&
		(Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk))
	{
		ChangeAnimation(Boss.Animation, BossAct_PowerUp, 1.0f, true);
		// 効果音
		Set3DSound(Boss.Pos, SE3D_PowerUp, E_DS8_FLAG_NONE, true);
		// パワーアップエフェクト設置
		SetEffect(Boss.CenterPos, PowerUp);
	}

	// ボスが倒された
	if (Boss.HP <= 0.0f)
	{
		// 死亡モーション再生
		ChangeAnimation(Boss.Animation, BossAct_BossDeath, 0.5f, false);
		if (Boss.Animation->MotionEnd == true && Boss.Phase != SetDeathEffect)
		{
			Boss.Phase = SetDeathEffect;
			Boss.Animation->MotionEnd = false;

			// 効果音
			Set3DSound(Boss.Pos, SE3D_BossDeathWind, E_DS8_FLAG_NONE, true);
			// 炎の剣のエフェクト停止
			FireSwordCtrl->Manager->StopEffect(Boss.Sword->EffectID);
			// 死亡エフェクト設置
			SetEffect(Boss.CenterPos, BossDeath);
		}
	}

	// ダメージ計算
	if (Boss.PreviousHP > Boss.HP)
	{
		HPDecreaseCount++;
		// HP減少開始
		if (HPDecreaseCount >= 90)
		{
			Boss.HPDecreaseStart = true;
		}
	}
	else
	{
		Boss.PreviousHP = Boss.HP;
		HPDecreaseCount = 0;
	}

	// ボスの行動を制御する
	if (Boss.Animation->CurrentAnimID != BossAct_PowerUp && Boss.Animation->NextAnimID != BossAct_HitReact &&
		Boss.Phase != TurnToPhase2)
	{
		BossAI();
	}

	// アニメーションを更新する
	if (Boss.HP > 0.0f)
	{
		AnimationManager();
	}

	// アニメーション更新
	if (Boss.Phase != SetDeathEffect)
	{
		UpdateAnimation(Boss.Animation, TimePerFrame * Boss.ActionSpeed);
	}

	// ボス移動
	BossMove();

#if _DEBUG
	PrintDebugMsg("BossPos：(%f : %f : %f)\n", Boss.Pos.x, Boss.Pos.y, Boss.Pos.z);
	PrintDebugMsg("Boss Animation：%s\n", Boss.Animation->AnimData[Boss.Animation->CurrentAnimID].SetName);
	double Time = Boss.Animation->AnimController->GetTime();
	double Period = Boss.Animation->AnimData[Boss.Animation->CurrentAnimID].AnimSet->GetPeriod();
	int Frame = (int)(fmod(Time, Period) * 60);
	PrintDebugMsg(" Animation Total Frame：%d\n", (int)(Period * 60));
	PrintDebugMsg(" Animation Frame：%d\n", Frame);
	PrintDebugMsg(" Next Animation：%s\n", Boss.Animation->AnimData[Boss.Animation->NextAnimID].SetName);
#endif

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBoss(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix, CapsuleMatrix;
	D3DMATERIAL9 matDef;

	if (Boss.Exist == false)
	{
		return;
	}

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&Boss.WorldMatrix);

	// スケールを反映
	D3DXMatrixScaling(&ScaleMatrix, Boss.Scale.x, Boss.Scale.y, Boss.Scale.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &ScaleMatrix);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Boss.Rot.y, Boss.Rot.x, Boss.Rot.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &RotMatrix);

	// 移動を反映
	D3DXMatrixTranslation(&TransMatrix, Boss.Pos.x, Boss.Pos.y, Boss.Pos.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &TransMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &Boss.WorldMatrix);

	// 現在のマテリアルを取得
	Device->GetMaterial(&matDef);

	DrawAnimation(Boss.Animation, &Boss.WorldMatrix, false);

	// マテリアルをデフォルトに戻す
	Device->SetMaterial(&matDef);

	// プレイヤーのカプセルを描画する
	CapsuleMatrix = GetBoneMatrix(Boss.Animation, "Hips");
	DrawCapsule(&Boss.HitCapsule, &CapsuleMatrix);

	return;
}

//=============================================================================
// ボスの行動を制御する
//=============================================================================
void BossAI(void)
{
	PLAYER *Player = GetPlayer();
	ATTACKINFO *AttackInfo = NULL;
	// プレイヤーとの距離
	float Distance = D3DXVec3LengthSq(&D3DXVECTOR3(Player->Pos - Boss.Pos));
	int AttackPercent = 0;			// 攻撃するの確率
	int RollingPercent = 0;			// 回避するの確率
	int AttackRand = 0;				// どんな攻撃の確率
	int TotalPriority = 0;			// 総優先度
	int i = 0;
	int AttackInfo_Num = 0;			// 攻撃グループの数
	bool AttackDetermine = false;	// 攻撃確定のフラグ
	static int MotionInterval = 0;	// 攻撃の間隔
	static int Count = 0;			// 加算カウント

	// 待機状態だけ加算
	if (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk)
	{
		Count++;
	}

	// 攻撃間隔は90~120カウントの間
	if (MotionInterval == 0)
	{
		MotionInterval = rand() % (30 + 1) + 90;
	}

	// 行動の判定
	if (Count != 0 && Count % MotionInterval == 0 && (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk) ||
		(Boss.Animation->CurrentAnimID == BossAct_Rolling || Boss.Animation->CurrentAnimID == BossAct_HitReact) && Boss.Animation->MotionEnd == true)
	{
		Count = 0;
		MotionInterval = 0;

		// 今は攻撃されている
		if (Boss.Animation->CurrentAnimID == BossAct_HitReact)
		{
			// 回避する確率を取得
			RollingPercent = rand() % (100 + 1);
		}
		// 攻撃する確率を取得
		AttackPercent = rand() % (100 + 1);
		// 前回の行動は回避、次の行動は攻撃の確率を上がる
		if (Boss.Animation->PreviousAnimID == BossAct_HitReact)
		{
			AttackPercent += 20;
		}

		// 距離によって、攻撃の欲望と技が変わる
		if (Distance <= pow(VeryCloseBorderline, 2))
		{
			AttackPercent += 100;
			Boss.DistanceState = VeryClose;
		}
		else if (Distance > pow(VeryCloseBorderline, 2) && Distance <= pow(CloseBorderline, 2))
		{
			AttackPercent += 80;
			Boss.DistanceState = Close;
		}
		else if (Distance > pow(CloseBorderline, 2) && Distance <= pow(MediumBorderline, 2))
		{
			AttackPercent += 60;
			Boss.DistanceState = Medium;
		}
		else if (Distance > pow(MediumBorderline, 2))
		{
			AttackPercent += 40;
			Boss.DistanceState = Far;
		}

		if (AttackPercent >= 100)
		{
			// 次の行動は攻撃
			AttackDetermine = true;
		}

	}

	// 回避かどうか確認
	if (RollingPercent >= 30)
	{
		// 攻撃より回避が優先
		AttackDetermine = false;
		ChangeAnimation(Boss.Animation, BossAct_Rolling, 1.5f, false);
		// 回避の方向を決定
		int RollingDir = rand() % (2 + 1);
		if (RollingDir == 0)
		{
			// 後ろ
			Boss.DestAngle = Boss.Rot.y - D3DX_PI;
		}
		else if (RollingDir == 1)
		{
			// 右
			Boss.DestAngle = Boss.Rot.y + D3DX_HalfPI;
		}
		else if (RollingDir == 2)
		{
			// 左
			Boss.DestAngle = Boss.Rot.y - D3DX_HalfPI;
		}
	}

	// 攻撃開始
	if (AttackDetermine == true)
	{
		switch (Boss.DistanceState)
		{
		case VeryClose:
			AttackInfo = VeryCloseAttack;
			AttackInfo_Num = sizeof(VeryCloseAttack) / sizeof(ATTACKINFO);
			break;
		case Close:
			if (Boss.Phase == Phase1)
			{
				AttackInfo = CloseAttack_Phase1;
				AttackInfo_Num = sizeof(CloseAttack_Phase1) / sizeof(ATTACKINFO);
			}
			else if (Boss.Phase == Phase2)
			{
				AttackInfo = CloseAttack_Phase2;
				AttackInfo_Num = sizeof(CloseAttack_Phase2) / sizeof(ATTACKINFO);
			}
			break;
		case Medium:
			if (Boss.Phase == Phase1)
			{
				AttackInfo = MediumAttack_Phase1;
				AttackInfo_Num = sizeof(MediumAttack_Phase1) / sizeof(ATTACKINFO);
			}
			else if (Boss.Phase == Phase2)
			{
				AttackInfo = MediumAttack_Phase2;
				AttackInfo_Num = sizeof(MediumAttack_Phase2) / sizeof(ATTACKINFO);
			}
			break;
		case Far:
			if (Boss.Phase == Phase1)
			{
				AttackInfo = FarAttack_Phase1;
				AttackInfo_Num = sizeof(FarAttack_Phase1) / sizeof(ATTACKINFO);
			}
			else if (Boss.Phase == Phase2)
			{
				AttackInfo = FarAttack_Phase2;
				AttackInfo_Num = sizeof(FarAttack_Phase2) / sizeof(ATTACKINFO);
			}
			break;
		default:
			break;
		}

		// 全部攻撃の優先度を加算する
		for (i = 0; i < AttackInfo_Num; i++)
		{
			TotalPriority += AttackInfo[i].Priority;
		}

		// どんな攻撃をランダムにする
		AttackRand = rand() % (TotalPriority + 1);

		// 優先度によってどんな攻撃をする
		for (i = 0, TotalPriority = 0; i < AttackInfo_Num; i++)
		{
			TotalPriority += AttackInfo[i].Priority;
			if (AttackRand <= TotalPriority)
			{
				// 攻撃前は方向調整
				Boss.TurnRotation = true;
				// 攻撃モーションに移す
				ChangeAnimation(Boss.Animation, AttackInfo[i].AnimID, 1.0f, false);
				Boss.Animation->NextAnimID = BossAct_Walk;
				break;
			}
		}
	}
	// プレイヤーに向かって移動
	else if (Boss.Animation->NextAnimID != BossAct_HitReact && Boss.Animation->NextAnimID != BossAct_Rolling)
	{
		Boss.Animation->NextAnimID = BossAct_Walk;
	}

#if _DEBUG
	PrintDebugMsg("Distance : %f\n", sqrtf(Distance));
	PrintDebugMsg("AICount : %d\n", Count);
#endif

	return;
}

//=============================================================================
// ボスのアニメーションを制御する
//=============================================================================
void AnimationManager(void)
{
	int Difficulty = GetDifficulty();
	static int SetFireCount = 0;
	bool AnimChangeable = false;

	// ===============================
	// 現在のアニメーション
	// ===============================
	switch (Boss.Animation->CurrentAnimID)
	{
	case BossAct_SlantSlash:
	case BossAct_LeftRightSlash:
	case BossAct_RollingSlash:
	case BossAct_SurfaceSlash:
	case BossAct_UpSlash:
	case BossAct_HorizonSlash:
	case BossAct_VerticalSlash:
	case BossAct_Stab:
	case BossAct_ComboAttack1:
	case BossAct_ComboAttack2:
	case BossAct_ComboAttack3:
	case BossAct_TwoHandCombo1:
	case BossAct_TwoHandCombo2:

		// 今のアニメーションは中断できない
		// 炎と剣の軌跡を設置する
		if (Boss.Animation->SlashTrack == true)
		{
			// 剣の軌跡
			if (Boss.Phase == Phase1)
			{
				SetSurface(Boss.Sword->HitCapsule.PreP1, Boss.Sword->HitCapsule.P1,
					Boss.Sword->HitCapsule.PreP2, Boss.Sword->HitCapsule.P2, FIRECOLOR(200));
			}
			else if (Boss.Phase == Phase2)
			{
				SetSurface(Boss.Sword->HitCapsule.PreP1, Boss.Sword->HitCapsule.P1,
					Boss.Sword->HitCapsule.PreP2, Boss.Sword->HitCapsule.P2, BLUEFIRE(200));
			}

			// 炎
			if (Difficulty == Easy)
			{
				// カウント加算
				SetFireCount++;

				// 0,1,3,4はエフェクト設置する、本来の0.8倍の量
				if (SetFireCount % 5 != 2)
				{
					SetSlashFire(Boss.Sword->HitCapsule.P2, Boss.Sword->HitCapsule.PreP2);
					SetFireCount = 0;
				}
			}
			else if (Difficulty == Normal || Difficulty == AppealDifficulty)
			{
				SetSlashFire(Boss.Sword->HitCapsule.P2, Boss.Sword->HitCapsule.PreP2);
			}
		}
		break;

	default:
		break;
	}

	// アニメーション終了、次のアニメーションに変わる
	if ((Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk || Boss.Animation->NextAnimID == BossAct_HitReact)
		&& Boss.Animation->CurrentAnimID != PowerUp)
	{
		AnimChangeable = true;
	}
	else if (Boss.Animation->MotionEnd == true)
	{
		Boss.Animation->MotionEnd = false;
		Boss.GiveDamage = false;
		Boss.TurnRotation = true;
		AnimChangeable = true;
	}

	// ===============================
	// 次のアニメーション
	// ===============================
	// アニメーションを変わるのは可能か
	if (AnimChangeable == true)
	{
		switch (Boss.Animation->NextAnimID)
		{
		case BossAct_Idle:
			ChangeAnimation(Boss.Animation, BossAct_Idle, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_Walk:
			ChangeAnimation(Boss.Animation, BossAct_Walk, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Walk;
			break;
		case BossAct_HitReact:
			ChangeAnimation(Boss.Animation, BossAct_HitReact, 1.0f, true);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_Rolling:
			ChangeAnimation(Boss.Animation, BossAct_Rolling, 1.5f, true);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_BossDeath:
			ChangeAnimation(Boss.Animation, BossAct_BossDeath, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_BossDeath;
			break;
		case BossAct_SlantSlash:
			ChangeAnimation(Boss.Animation, BossAct_SlantSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_LeftRightSlash:
			ChangeAnimation(Boss.Animation, BossAct_LeftRightSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_RollingSlash:
			ChangeAnimation(Boss.Animation, BossAct_RollingSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_SurfaceSlash:
			ChangeAnimation(Boss.Animation, BossAct_SurfaceSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_UpSlash:
			ChangeAnimation(Boss.Animation, BossAct_UpSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_HorizonSlash:
			ChangeAnimation(Boss.Animation, BossAct_HorizonSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_VerticalSlash:
			ChangeAnimation(Boss.Animation, BossAct_VerticalSlash, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_Stab:
			ChangeAnimation(Boss.Animation, BossAct_Stab, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_ComboAttack1:
			ChangeAnimation(Boss.Animation, BossAct_ComboAttack1, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_ComboAttack2:
			ChangeAnimation(Boss.Animation, BossAct_ComboAttack2, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_ComboAttack3:
			ChangeAnimation(Boss.Animation, BossAct_ComboAttack3, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_TwoHandCombo1:
			ChangeAnimation(Boss.Animation, BossAct_TwoHandCombo1, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_TwoHandCombo2:
			ChangeAnimation(Boss.Animation, BossAct_TwoHandCombo2, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		case BossAct_PowerUp:
			ChangeAnimation(Boss.Animation, BossAct_PowerUp, 1.0f, false);
			Boss.Animation->NextAnimID = BossAct_Idle;
			break;
		default:
			break;
		}
	}

	return;
}

//=============================================================================
// ボスの移動を制御する
//=============================================================================
void BossMove(void)
{
	D3DXVECTOR3 PlayerBossVec;
	PLAYER *Player = GetPlayer();
	float DiffRotY = 0.0f;

	// プレイヤーに向く角度を計算する
	if (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk ||
		Boss.TurnRotation == true)
	{
		if (Boss.Animation->CurrentAnimID == BossAct_Walk)
		{
			Boss.Move.x += sinf(Boss.Rot.y) * BossMoveSpeed;
			Boss.Move.z += cosf(Boss.Rot.y) * BossMoveSpeed;
		}

		Boss.TurnRotation = false;

		// ボスの向き角度を計算する
		PlayerBossVec = Player->Pos - Boss.Pos;
		D3DXVec3Normalize(&PlayerBossVec, &PlayerBossVec);
		float Angle = acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &PlayerBossVec));

		// ベクトル平行
		if (isnan(Angle))
		{
			Angle = 3.14f;
		}

		if (Player->Pos.x >= Boss.Pos.x)
		{
			Boss.DestAngle = Angle;
		}
		else
		{
			Boss.DestAngle = -Angle;
		}
	}
	if (Boss.Animation->StartMove == true)
	{
		switch (Boss.Animation->CurrentAnimID)
		{
		case BossAct_Rolling:
		case BossAct_VerticalSlash:
		case BossAct_SlantSlash:
		case BossAct_UpSlash:
		case BossAct_RollingSlash:
		case BossAct_ComboAttack1:
		case BossAct_ComboAttack2:
		case BossAct_ComboAttack3:
		case BossAct_TwoHandCombo1:
		case BossAct_TwoHandCombo2:

			// 回避、攻撃の移動
			Boss.Move.x += sinf(Boss.Rot.y) * 1.0f;
			Boss.Move.z += cosf(Boss.Rot.y) * 1.0f;
			break;

		case BossAct_Stab:

			// 突進
			Boss.Move.x += sinf(Boss.Rot.y) * 4.0f;
			Boss.Move.z += cosf(Boss.Rot.y) * 4.0f;
			break;

		case BossAct_HorizonSlash:

			// 水平斬撃
			Boss.Move.x += sinf(Boss.Rot.y) * 1.2f;
			Boss.Move.z += cosf(Boss.Rot.y) * 1.2f;
			break;

		default:
			break;
		}
	}

	// 移動量に慣性をかける
	Boss.Move.x -= Boss.Move.x * MoveInertia;
	if (Boss.JumpState != StartFalling)
	{
		Boss.Move.y -= Boss.Move.y * MoveInertia;
	}
	else
	{
		Boss.Move.y += Boss.Move.y * MoveInertia;
	}
	Boss.Move.z -= Boss.Move.z * MoveInertia;

	// 座標更新
	Boss.Pos += Boss.Move;
	if (Boss.Pos.y <= 0.0f)
	{
		Boss.Pos.y = 0.0f;
		Boss.Move.y = 0.0f;
	}

	// 左の壁
	if (Boss.Pos.x < -Stage_Size)
	{
		Boss.Pos.x = -Stage_Size;
	}
	// 右の壁
	else if (Boss.Pos.x > Stage_Size)
	{
		Boss.Pos.x = Stage_Size;
	}

	// 前の壁
	if (Boss.Pos.z > Stage_Size)
	{
		Boss.Pos.z = Stage_Size;
	}
	// 後ろの壁
	else if (Boss.Pos.z < -Stage_Size)
	{
		Boss.Pos.z = -Stage_Size;
	}

	// 中心座標更新
	Boss.CenterPos = D3DXVECTOR3(Boss.Pos.x, Boss.Pos.y + 50.0f, Boss.Pos.z);

	// 目的の角度までの差分
	DiffRotY = Boss.DestAngle - Boss.Rot.y;
	if (DiffRotY > D3DX_PI)
	{
		DiffRotY -= D3DX_2PI;
	}
	if (DiffRotY < -D3DX_PI)
	{
		DiffRotY += D3DX_2PI;
	}

	// 目的の角度まで慣性をかける
	Boss.Rot.y += DiffRotY * BossRotInertia * 0.5f;
	if (Boss.Rot.y > D3DX_PI)
	{
		Boss.Rot.y -= D3DX_2PI;
	}
	if (Boss.Rot.y < -D3DX_PI)
	{
		Boss.Rot.y += D3DX_2PI;
	}

	return;
}

//=============================================================================
// ボスのポインタを取得する
//=============================================================================
BOSS *GetBoss(void)
{
	return &Boss;
}

