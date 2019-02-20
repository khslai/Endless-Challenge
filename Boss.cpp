//=============================================================================
//
// �{�X���� [Boss.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
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
// �}�N����`
//*****************************************************************************
#define	Model_Boss			"data/Model/Boss.x"			// �ǂݍ��ރ��f����
#define BossMoveSpeed		(0.2f)						// �ړ����x
#define DefaultHP			(100.0f)					// �ő�HP
#define	BossRotInertia		(0.2f)						// ��]�����W��

// �v���C���[�Ƃ̋����W��
#define VeryCloseBorderline	(100.0f)
#define CloseBorderline		(200.0f)
#define MediumBorderline	(350.0f)
#define FarBorderline		(550.0f)
enum DistanceStandard
{
	VeryClose,			// VeryCloseBorderline�ȓ�
	Close,				// VeryCloseBorderline ~ CloseBorderline
	Medium,				// CloseBorderline ~ MediumBorderline
	Far,				// MediumBorderline ~ FarBorderline
	VeryFar,			// FarBorderline�ȏ�
};

typedef struct
{
	int		AnimID;		// ���[�V����ID
	int		Priority;	// �D��x
}ATTACKINFO;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �{�X�̃A�j���[�V�����𐧌䂷��
static void AnimationManager(void);
// �{�X�̈ړ��𐧌䂷��
void BossMove(void);
// �{�X�̍s���𐧌䂷��
void BossAI(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
BOSS				Boss;

// ���[�V����ID , �D��x(�S��100%)
// �ƂĂ��ߋ���
ATTACKINFO VeryCloseAttack[] =
{
	{BossAct_LeftRightSlash, 50},
	{BossAct_SurfaceSlash, 50},
};

// �ߋ���
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

// ������
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

// ������
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
// ����������
//=============================================================================
HRESULT InitBoss(bool FirstInit)
{
	int GameStage = GetGameStage();
	int Difficulty = GetDifficulty();
	int Set_No = 0;
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �p�����[�^�̏����ݒ�
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

	// ��Փx�ɂ���āAHP�A�s�����x���ς��
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

	// ���݃t���O
	if (GameStage == Stage_Game)
	{
		Boss.Exist = true;
	}
	else
	{
		Boss.Exist = false;
	}

	// �{�X�̒i�K
	if (Difficulty != AppealDifficulty)
	{
		Boss.Phase = Phase1;
	}
	else
	{
		Boss.Phase = TurnToPhase2;
	}

	// ���߂ď�����
	if (FirstInit == true)
	{
		// �����擾����
		Boss.Sword = GetSword("Boss");
		if (Boss.Sword == NULL)
		{
			return E_FAIL;
		}

		// �A�j���[�V�����I�u�W�F�N�g�����
		Boss.Animation = (D3DXANIMATION*)calloc(1, sizeof(D3DXANIMATION));
		Boss.Animation->Owner = "Boss";
		Boss.Animation->CurrentAnimID = -1;
		Boss.Animation->NextAnimID = BossAct_Idle;

		// xfile��ǂݍ���
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

		// AnimationCallback��ݒu����
		if (FAILED(InitCallbackKeys_Boss()))
		{
			return E_FAIL;
		}

		// AnimationSet������������
		for (Set_No = 0; Set_No < Boss.Animation->AnimSetNum; Set_No++)
		{
			if (FAILED(InitAnimation(Boss.Animation, Set_No)))
			{
				return E_FAIL;
			}
		}

		// �ŏ��͑ҋ@���
		ChangeAnimation(Boss.Animation, BossAct_Idle, 1.0f, false);

		// �����蔻��J�v�Z���𐶐�
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
// �I������
//=============================================================================
void UninitBoss(void)
{
	// �A�j���[�V�����������[�X
	UninitAnimation(Boss.Animation);

	// �J�v�Z���������[�X
	UninitCapsule(&Boss.HitCapsule);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBoss(void)
{
	EFFECTCONTROLLER *FireSwordCtrl = GetFireSwordCtrl();
	int GameCount = GetGameCount();
	int Transition = GetTransition();
	// �_���[�W���󂯂���J�E���g
	static int HPDecreaseCount = 0;
	static int Count = 0;

	if (Boss.Exist == false || Transition != TransitionOver)
	{
		// ���S���W�X�V
		Boss.CenterPos = D3DXVECTOR3(Boss.Pos.x, Boss.Pos.y + 50.0f, Boss.Pos.z);
		return;
	}

	// �O�t���C���̍��W���L�^����
	Boss.PrePos = Boss.Pos;

	// ���̌��̃G�t�F�N�g
	if (Boss.SetFireSword == true)
	{
		// �G�t�F�N�g�̍��W�͌��̒[�_��ݒu����
		FireSwordCtrl->Manager->SetLocation(Boss.Sword->EffectID, DXtoEffekVec(Boss.Sword->HitCapsule.P1));
		// �G�t�F�N�g�̉�]��
		D3DXVECTOR3 RotAxis;
		// �ړI����
		D3DXVECTOR3 Direction = Boss.Sword->HitCapsule.Direction;
		D3DXVec3Normalize(&Direction, &Direction);
		// ��]���W�A���v�Z
		float RotRadian = acosf(D3DXVec3Dot(&RightVector, &Direction));
		// �G�t�F�N�g�̃f�t�H���g�����͉E
		D3DXVec3Cross(&RotAxis, &RightVector, &Direction);
		D3DXVec3Normalize(&RotAxis, &RotAxis);
		// �G�t�F�N�g��]
		FireSwordCtrl->Manager->SetRotation(Boss.Sword->EffectID, DXtoEffekVec(RotAxis), RotRadian - D3DX_PI);
		// �i�K�ϊ����A�G�t�F�N�g��~
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
			// ���̌��̃G�t�F�N�g��ݒu����
			Boss.Sword->EffectID = SetFireSword(Boss.Sword->HitCapsule.P1, Boss.Sword->HitCapsule.Direction);
			Boss.SetFireSword = true;
		}
	}

	// ���i�K�ɓ���
	if (Boss.Phase == TurnToPhase2 && Boss.Animation->CurrentAnimID != BossAct_PowerUp &&
		(Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk))
	{
		ChangeAnimation(Boss.Animation, BossAct_PowerUp, 1.0f, true);
		// ���ʉ�
		Set3DSound(Boss.Pos, SE3D_PowerUp, E_DS8_FLAG_NONE, true);
		// �p���[�A�b�v�G�t�F�N�g�ݒu
		SetEffect(Boss.CenterPos, PowerUp);
	}

	// �{�X���|���ꂽ
	if (Boss.HP <= 0.0f)
	{
		// ���S���[�V�����Đ�
		ChangeAnimation(Boss.Animation, BossAct_BossDeath, 0.5f, false);
		if (Boss.Animation->MotionEnd == true && Boss.Phase != SetDeathEffect)
		{
			Boss.Phase = SetDeathEffect;
			Boss.Animation->MotionEnd = false;

			// ���ʉ�
			Set3DSound(Boss.Pos, SE3D_BossDeathWind, E_DS8_FLAG_NONE, true);
			// ���̌��̃G�t�F�N�g��~
			FireSwordCtrl->Manager->StopEffect(Boss.Sword->EffectID);
			// ���S�G�t�F�N�g�ݒu
			SetEffect(Boss.CenterPos, BossDeath);
		}
	}

	// �_���[�W�v�Z
	if (Boss.PreviousHP > Boss.HP)
	{
		HPDecreaseCount++;
		// HP�����J�n
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

	// �{�X�̍s���𐧌䂷��
	if (Boss.Animation->CurrentAnimID != BossAct_PowerUp && Boss.Animation->NextAnimID != BossAct_HitReact &&
		Boss.Phase != TurnToPhase2)
	{
		BossAI();
	}

	// �A�j���[�V�������X�V����
	if (Boss.HP > 0.0f)
	{
		AnimationManager();
	}

	// �A�j���[�V�����X�V
	if (Boss.Phase != SetDeathEffect)
	{
		UpdateAnimation(Boss.Animation, TimePerFrame * Boss.ActionSpeed);
	}

	// �{�X�ړ�
	BossMove();

#if _DEBUG
	PrintDebugMsg("BossPos�F(%f : %f : %f)\n", Boss.Pos.x, Boss.Pos.y, Boss.Pos.z);
	PrintDebugMsg("Boss Animation�F%s\n", Boss.Animation->AnimData[Boss.Animation->CurrentAnimID].SetName);
	double Time = Boss.Animation->AnimController->GetTime();
	double Period = Boss.Animation->AnimData[Boss.Animation->CurrentAnimID].AnimSet->GetPeriod();
	int Frame = (int)(fmod(Time, Period) * 60);
	PrintDebugMsg(" Animation Total Frame�F%d\n", (int)(Period * 60));
	PrintDebugMsg(" Animation Frame�F%d\n", Frame);
	PrintDebugMsg(" Next Animation�F%s\n", Boss.Animation->AnimData[Boss.Animation->NextAnimID].SetName);
#endif

	return;
}

//=============================================================================
// �`�揈��
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

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&Boss.WorldMatrix);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&ScaleMatrix, Boss.Scale.x, Boss.Scale.y, Boss.Scale.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &ScaleMatrix);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Boss.Rot.y, Boss.Rot.x, Boss.Rot.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &RotMatrix);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&TransMatrix, Boss.Pos.x, Boss.Pos.y, Boss.Pos.z);
	D3DXMatrixMultiply(&Boss.WorldMatrix, &Boss.WorldMatrix, &TransMatrix);

	// ���[���h�}�g���b�N�X�̐ݒ�
	Device->SetTransform(D3DTS_WORLD, &Boss.WorldMatrix);

	// ���݂̃}�e���A�����擾
	Device->GetMaterial(&matDef);

	DrawAnimation(Boss.Animation, &Boss.WorldMatrix, false);

	// �}�e���A�����f�t�H���g�ɖ߂�
	Device->SetMaterial(&matDef);

	// �v���C���[�̃J�v�Z����`�悷��
	CapsuleMatrix = GetBoneMatrix(Boss.Animation, "Hips");
	DrawCapsule(&Boss.HitCapsule, &CapsuleMatrix);

	return;
}

//=============================================================================
// �{�X�̍s���𐧌䂷��
//=============================================================================
void BossAI(void)
{
	PLAYER *Player = GetPlayer();
	ATTACKINFO *AttackInfo = NULL;
	// �v���C���[�Ƃ̋���
	float Distance = D3DXVec3LengthSq(&D3DXVECTOR3(Player->Pos - Boss.Pos));
	int AttackPercent = 0;			// �U������̊m��
	int RollingPercent = 0;			// �������̊m��
	int AttackRand = 0;				// �ǂ�ȍU���̊m��
	int TotalPriority = 0;			// ���D��x
	int i = 0;
	int AttackInfo_Num = 0;			// �U���O���[�v�̐�
	bool AttackDetermine = false;	// �U���m��̃t���O
	static int MotionInterval = 0;	// �U���̊Ԋu
	static int Count = 0;			// ���Z�J�E���g

	// �ҋ@��Ԃ������Z
	if (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk)
	{
		Count++;
	}

	// �U���Ԋu��90~120�J�E���g�̊�
	if (MotionInterval == 0)
	{
		MotionInterval = rand() % (30 + 1) + 90;
	}

	// �s���̔���
	if (Count != 0 && Count % MotionInterval == 0 && (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk) ||
		(Boss.Animation->CurrentAnimID == BossAct_Rolling || Boss.Animation->CurrentAnimID == BossAct_HitReact) && Boss.Animation->MotionEnd == true)
	{
		Count = 0;
		MotionInterval = 0;

		// ���͍U������Ă���
		if (Boss.Animation->CurrentAnimID == BossAct_HitReact)
		{
			// �������m�����擾
			RollingPercent = rand() % (100 + 1);
		}
		// �U������m�����擾
		AttackPercent = rand() % (100 + 1);
		// �O��̍s���͉���A���̍s���͍U���̊m�����オ��
		if (Boss.Animation->PreviousAnimID == BossAct_HitReact)
		{
			AttackPercent += 20;
		}

		// �����ɂ���āA�U���̗~�]�ƋZ���ς��
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
			// ���̍s���͍U��
			AttackDetermine = true;
		}

	}

	// ������ǂ����m�F
	if (RollingPercent >= 30)
	{
		// �U����������D��
		AttackDetermine = false;
		ChangeAnimation(Boss.Animation, BossAct_Rolling, 1.5f, false);
		// ����̕���������
		int RollingDir = rand() % (2 + 1);
		if (RollingDir == 0)
		{
			// ���
			Boss.DestAngle = Boss.Rot.y - D3DX_PI;
		}
		else if (RollingDir == 1)
		{
			// �E
			Boss.DestAngle = Boss.Rot.y + D3DX_HalfPI;
		}
		else if (RollingDir == 2)
		{
			// ��
			Boss.DestAngle = Boss.Rot.y - D3DX_HalfPI;
		}
	}

	// �U���J�n
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

		// �S���U���̗D��x�����Z����
		for (i = 0; i < AttackInfo_Num; i++)
		{
			TotalPriority += AttackInfo[i].Priority;
		}

		// �ǂ�ȍU���������_���ɂ���
		AttackRand = rand() % (TotalPriority + 1);

		// �D��x�ɂ���Ăǂ�ȍU��������
		for (i = 0, TotalPriority = 0; i < AttackInfo_Num; i++)
		{
			TotalPriority += AttackInfo[i].Priority;
			if (AttackRand <= TotalPriority)
			{
				// �U���O�͕�������
				Boss.TurnRotation = true;
				// �U�����[�V�����Ɉڂ�
				ChangeAnimation(Boss.Animation, AttackInfo[i].AnimID, 1.0f, false);
				Boss.Animation->NextAnimID = BossAct_Walk;
				break;
			}
		}
	}
	// �v���C���[�Ɍ������Ĉړ�
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
// �{�X�̃A�j���[�V�����𐧌䂷��
//=============================================================================
void AnimationManager(void)
{
	int Difficulty = GetDifficulty();
	static int SetFireCount = 0;
	bool AnimChangeable = false;

	// ===============================
	// ���݂̃A�j���[�V����
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

		// ���̃A�j���[�V�����͒��f�ł��Ȃ�
		// ���ƌ��̋O�Ղ�ݒu����
		if (Boss.Animation->SlashTrack == true)
		{
			// ���̋O��
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

			// ��
			if (Difficulty == Easy)
			{
				// �J�E���g���Z
				SetFireCount++;

				// 0,1,3,4�̓G�t�F�N�g�ݒu����A�{����0.8�{�̗�
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

	// �A�j���[�V�����I���A���̃A�j���[�V�����ɕς��
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
	// ���̃A�j���[�V����
	// ===============================
	// �A�j���[�V������ς��͉̂\��
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
// �{�X�̈ړ��𐧌䂷��
//=============================================================================
void BossMove(void)
{
	D3DXVECTOR3 PlayerBossVec;
	PLAYER *Player = GetPlayer();
	float DiffRotY = 0.0f;

	// �v���C���[�Ɍ����p�x���v�Z����
	if (Boss.Animation->CurrentAnimID == BossAct_Idle || Boss.Animation->CurrentAnimID == BossAct_Walk ||
		Boss.TurnRotation == true)
	{
		if (Boss.Animation->CurrentAnimID == BossAct_Walk)
		{
			Boss.Move.x += sinf(Boss.Rot.y) * BossMoveSpeed;
			Boss.Move.z += cosf(Boss.Rot.y) * BossMoveSpeed;
		}

		Boss.TurnRotation = false;

		// �{�X�̌����p�x���v�Z����
		PlayerBossVec = Player->Pos - Boss.Pos;
		D3DXVec3Normalize(&PlayerBossVec, &PlayerBossVec);
		float Angle = acosf(D3DXVec3Dot(&D3DXVECTOR3(0.0f, 0.0f, 1.0f), &PlayerBossVec));

		// �x�N�g�����s
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

			// ����A�U���̈ړ�
			Boss.Move.x += sinf(Boss.Rot.y) * 1.0f;
			Boss.Move.z += cosf(Boss.Rot.y) * 1.0f;
			break;

		case BossAct_Stab:

			// �ːi
			Boss.Move.x += sinf(Boss.Rot.y) * 4.0f;
			Boss.Move.z += cosf(Boss.Rot.y) * 4.0f;
			break;

		case BossAct_HorizonSlash:

			// �����a��
			Boss.Move.x += sinf(Boss.Rot.y) * 1.2f;
			Boss.Move.z += cosf(Boss.Rot.y) * 1.2f;
			break;

		default:
			break;
		}
	}

	// �ړ��ʂɊ�����������
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

	// ���W�X�V
	Boss.Pos += Boss.Move;
	if (Boss.Pos.y <= 0.0f)
	{
		Boss.Pos.y = 0.0f;
		Boss.Move.y = 0.0f;
	}

	// ���̕�
	if (Boss.Pos.x < -Stage_Size)
	{
		Boss.Pos.x = -Stage_Size;
	}
	// �E�̕�
	else if (Boss.Pos.x > Stage_Size)
	{
		Boss.Pos.x = Stage_Size;
	}

	// �O�̕�
	if (Boss.Pos.z > Stage_Size)
	{
		Boss.Pos.z = Stage_Size;
	}
	// ���̕�
	else if (Boss.Pos.z < -Stage_Size)
	{
		Boss.Pos.z = -Stage_Size;
	}

	// ���S���W�X�V
	Boss.CenterPos = D3DXVECTOR3(Boss.Pos.x, Boss.Pos.y + 50.0f, Boss.Pos.z);

	// �ړI�̊p�x�܂ł̍���
	DiffRotY = Boss.DestAngle - Boss.Rot.y;
	if (DiffRotY > D3DX_PI)
	{
		DiffRotY -= D3DX_2PI;
	}
	if (DiffRotY < -D3DX_PI)
	{
		DiffRotY += D3DX_2PI;
	}

	// �ړI�̊p�x�܂Ŋ�����������
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
// �{�X�̃|�C���^���擾����
//=============================================================================
BOSS *GetBoss(void)
{
	return &Boss;
}

