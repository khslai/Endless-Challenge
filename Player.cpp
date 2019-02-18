//=============================================================================
//
// �v���C���[���� [Player.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Player.h"
#include "Boss.h"
#include "Input.h"
#include "Effect.h"
#include "Equipment.h"
#include "FireSword.h"
#include "ThirdPersonCamera.h"
#include "D3DXAnimation.h"
#include "CapsuleMesh.h"
#include "DebugProcess.h"
#include "Transition.h"
#include "Title.h"
#include "Potion.h"
#include "Tutorial.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	Model_Player			"data/Model/Player.x"		// �ǂݍ��ރ��f����
#define FallToStandCount		(60)						// �����オ��܂ł����鎞��
#define DecreaseCount			(90)						// DecreaseCount�J�E���g�Ȍ�HP�AST�����邱�Ƃ��n�܂�
#define PlayerHP_Max			(100.0f)					// �v���C���[�ő�HP
#define PlayerStamina_Max		(100.0f)					// �v���C���[�ő�X�^�~�i
#define StaminaRestore			(0.6f)						// �X�^�~�i�񕜑��x
#define HPPotionRestore			(50.0f)						// HP�|�[�V�����񕜐��l
#define SpendStamina_Attack		(17.0f)						// �U����������X�^�~�i
#define SpendStamina_Rolling	(15.0f)						// �����������X�^�~�i
#define PlayerDamage			(3.4f)						// �G�ɗ^����_���[�W
#define	MoveSpeed				(0.50f)						// �ړ����x
#define RollingSpeed			(0.65f)						// �e���[�V�����̈ړ����x
#define AttackStepSpeed			(0.4f)
#define FlyingBackSpeed			(0.3f)
#define StepSpeed				(0.2f)

// �ړ����
enum MoveState
{
	Move_Running,		// ����
	Move_LockOn,		// �^�[�Q�b�g�����b�N�I�����Ă���
	Move_Rolling,		// ���[�����O
	Move_Attack,		// �U��
	Move_FlyingBack,	// ������΂����
	Move_ChangeRot,		// �����ύX
	Move_Step,			// ����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���ׂẴA�j���[�V�����𐧌䂷��
void AnimationManager(void);
// �v���C���[�̈ړ����v�Z����
void MoveCalculate(int MoveState);
// �v���C���[�ړ�
void PlayerMove(void);
// HP�A�X�^�~�i�̏���A�񕜂��v�Z����
void HP_STCalculate(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
PLAYER					Player;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(bool FirstInit)
{
	int GameStage = GetGameStage();
	int Difficulty = GetDifficulty();
	int Set_No = 0;
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �p�����[�^�̏����ݒ�
	D3DXMatrixIdentity(&Player.WorldMatrix);
	Player.Pos = PositionZero;
	Player.PrePos = PositionZero;
	Player.CenterPos = PositionZero;
	Player.NextPos = PositionZero;
	Player.Move = PositionZero;
	Player.Rot = PositionZero;
	Player.FlyingBackDir = PositionZero;
	Player.Direction = PositionZero;
	Player.Scale = DefaultScale;
	Player.DestAngle = 0.0f;
	Player.NextDestAngle = 0.0f;
	Player.ActionSpeed = 1.0f;
	Player.GiveDamage = false;
	Player.InHPRestore = false;
	Player.BeDamaged = false;
	Player.Invincible = false;
	Player.HPDecreaseStart = false;
	Player.STDecreaseStart = false;
	Player.MoveBack = false;

	// ��Փx�ɂ���āAHP�A�|�[�V�����̐��AST�񕜑��x�Ȃǂ��ς��
	if (Difficulty == Easy)
	{
		Player.DifficultyRate = 1.2f;
		Player.HP_Max = PlayerHP_Max * Player.DifficultyRate;
		Player.HP = Player.HP_Max;
		Player.RestoreHP = Player.HP_Max;
		Player.PreviousHP = Player.HP_Max;
		Player.Stamina_Max = PlayerStamina_Max * Player.DifficultyRate;
		Player.Stamina = Player.Stamina_Max;
		Player.PreviousST = Player.Stamina_Max;
		Player.Damage = PlayerDamage * Player.DifficultyRate;
		Player.HPPotionNum = 15;
	}
	else if (Difficulty == Normal || Difficulty == AppealDifficulty)
	{
		Player.DifficultyRate = 1.0f;
		Player.HP_Max = PlayerHP_Max * Player.DifficultyRate;
		Player.HP = Player.HP_Max;
		Player.RestoreHP = Player.HP_Max;
		Player.PreviousHP = Player.HP_Max;
		Player.Stamina_Max = PlayerStamina_Max * Player.DifficultyRate;
		Player.Stamina = Player.Stamina_Max;
		Player.PreviousST = Player.Stamina_Max;
		Player.Damage = PlayerDamage * Player.DifficultyRate;
		Player.HPPotionNum = 10;
	}

	// ���߂ď�����
	if (FirstInit == true)
	{
		// �����擾����
		Player.Sword = GetSword("Player");
		if (Player.Sword == NULL)
		{
			return E_FAIL;
		}

		// �A�j���[�V�����I�u�W�F�N�g�����
		Player.Animation = (D3DXANIMATION*)calloc(1, sizeof(D3DXANIMATION));
		Player.Animation->Owner = "Player";

		// xfile��ǂݍ���
		if (FAILED(Load_xFile(Player.Animation, Model_Player, "Player")))
		{
			return E_FAIL;
		}

		// �A�j���[�V�����Z�b�g�̖��O�Ɛݒu����R�[���o�b�N�̐��Ȃǂ̎���
		// ���Ԃ�x file�ƈꏏ�ɂ��Ȃ��Ƃ����Ȃ�
		for (Set_No = 0; Set_No < Player.Animation->AnimSetNum; Set_No++)
		{
			switch (Set_No)
			{
				//									   SetName		AnimSet ShiftTime CurWeightTime
			case Idle:
				Player.Animation->AnimData[Set_No] = { "Idle",			NULL, 0.3f,	0.0f };
				break;
			case Running:
				Player.Animation->AnimData[Set_No] = { "Running",		NULL, 0.1f,	0.0f };
				break;
			case Walk_Left:
				Player.Animation->AnimData[Set_No] = { "Walk_Left",		NULL, 0.1f,	0.0f };
				break;
			case Walk_Right:
				Player.Animation->AnimData[Set_No] = { "Walk_Right",	NULL, 0.1f,	0.0f };
				break;
			case Walk_Back:
				Player.Animation->AnimData[Set_No] = { "Walk_Back",		NULL, 0.1f,	0.0f };
				break;
			case Rolling:
				Player.Animation->AnimData[Set_No] = { "Rolling",		NULL, 0.1f,	0.0f };
				break;
			case FallingBack:
				Player.Animation->AnimData[Set_No] = { "FallingBack",	NULL, 0.1f,	0.0f };
				break;
			case FlyingBack:
				Player.Animation->AnimData[Set_No] = { "FlyingBack",	NULL, 0.1f,	0.0f };
				break;
			case FallToStand:
				Player.Animation->AnimData[Set_No] = { "FallToStand",	NULL, 0.3f,	0.0f };
				break;
			case HitReact:
				Player.Animation->AnimData[Set_No] = { "HitReact",		NULL, 0.1f,	0.0f };
				break;
			case HPRestore:
				Player.Animation->AnimData[Set_No] = { "HPRestore",		NULL, 0.1f,	0.0f };
				break;
			case Sit:
				Player.Animation->AnimData[Set_No] = { "Sit",			NULL, 1.0f,	0.0f };
				break;
			case StandUp:
				Player.Animation->AnimData[Set_No] = { "StandUp",		NULL, 0.5f, 0.0f };
				break;
			case PlayerDeath:
				Player.Animation->AnimData[Set_No] = { "PlayerDeath",	NULL, 0.2f,	0.0f };
				break;
			case Attack1:
				Player.Animation->AnimData[Set_No] = { "Attack1",		NULL, 0.1f,	0.0f };
				break;
			case Attack2:
				Player.Animation->AnimData[Set_No] = { "Attack2",		NULL, 0.3f, 0.0f };
				break;
			case SitPose1:
				Player.Animation->AnimData[Set_No] = { "SitPose1",		NULL, 1.0f, 0.0f };
				break;
			case SitPose2:
				Player.Animation->AnimData[Set_No] = { "SitPose2",		NULL, 1.0f, 0.0f };
				break;
			default:
				break;
			}
		}

		// AnimationCallback��ݒu����
		if (FAILED(InitCallbackKeys_Player()))
		{
			return E_FAIL;
		}

		for (Set_No = 0; Set_No < Player.Animation->AnimSetNum; Set_No++)
		{
			// AnimationSet������������
			if (FAILED(InitAnimation(Player.Animation, Set_No)))
			{
				return E_FAIL;
			}
		}

		// �����A�j���[�V����
		if (GameStage == Stage_Title)
		{
			Player.Animation->CurrentAnimID = -1;
			ChangeAnimation(Player.Animation, Sit, 0.5f, false);
		}
		else if (GameStage == Stage_Game || GameStage == Stage_Tutorial)
		{
			Player.Animation->CurrentAnimID = -1;
			ChangeAnimation(Player.Animation, Idle, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
		}

		// �����蔻��J�v�Z���𐶐�
		if (FAILED(
			CreateCapsule(&Player.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 60.0f, 15.0f, true)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	// �A�j���[�V�����������[�X
	UninitAnimation(Player.Animation);

	// �J�v�Z���������[�X
	UninitCapsule(&Player.HitCapsule);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	int GameCount = GetGameCount();
	int GameStage = GetGameStage();
	int Transition = GetTransition();
	static int Count = 0;
	BOSS *Boss = GetBoss();
	int TutorialState = GetTutorialState();

	switch (GameStage)
	{
	case Stage_Game:

		// �܂��X�e�[�W�J�ڒ�
		if (Transition != TransitionOver)
		{
			// ���S���W�X�V
			Player.CenterPos = D3DXVECTOR3(Player.Pos.x, Player.Pos.y + 50.0f, Player.Pos.z);
			return;
		}

		// �ꎞ��~
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
		{
			SetSound(NormalSE, SE_Menu, E_DS8_FLAG_NONE, true);
			SetGameStage(Stage_Pause);
			while (ShowCursor(true) < 0);
		}

		// 1�t���C���O�̍��W���L�^����
		Player.PrePos = Player.Pos;

		// HP�A�X�^�~�i�̌v�Z���s��
		HP_STCalculate();

		// �A�j���[�V�����𐧌䂷��
		if (Boss->Phase != SetDeathEffect)
		{
			AnimationManager();
		}
		else
		{
			// �{�X���S���S�܂ł̊ԁA�ҋ@��Ԃɖ߂�
			if (Player.Animation->CurrentAnimID != Idle && Player.Animation->MotionEnd == true)
			{
				ChangeAnimation(Player.Animation, Idle, 1.0f, false);
			}
		}

		// �A�j���[�V�������X�V
		UpdateAnimation(Player.Animation, TimePerFrame * Player.ActionSpeed);

		// �v���C���[�ړ�
		PlayerMove();
		break;

	case Stage_Title:

		if (Player.Animation->MotionEnd == true)
		{
			// ����A�j���[�V�����I���A���̃A�j���[�V�����ɕς��
			if (Player.Animation->CurrentAnimID == Sit)
			{
				ChangeAnimation(Player.Animation, SitPose1, 1.0f, false);
			}
			// �����オ��A�j���[�V�����I���A�Đ����x�̓[���ɂȂ�
			else if (Player.Animation->CurrentAnimID == StandUp)
			{
				Player.ActionSpeed = 0.0f;
			}
		}
		if (Player.Animation->CurrentAnimID == SitPose1 || Player.Animation->CurrentAnimID == SitPose2)
		{
			Count++;
			if (Count % 600 == 0)
			{
				if (Player.Animation->CurrentAnimID == SitPose1)
				{
					ChangeAnimation(Player.Animation, SitPose2, 1.0f, false);
				}
				else
				{
					ChangeAnimation(Player.Animation, Sit, 0.5f, false);
				}
			}
		}

		// �A�j���[�V�������X�V
		UpdateAnimation(Player.Animation, TimePerFrame * Player.ActionSpeed);
		break;

	case Stage_Tutorial:

		if (Transition != TransitionOver || TutorialState == Tutorial_WaitRolling)
		{
			// ���S���W�X�V
			Player.CenterPos = D3DXVECTOR3(Player.Pos.x, Player.Pos.y + 50.0f, Player.Pos.z);
			return;
		}

		// 1�t���C���O�̍��W���L�^����
		Player.PrePos = Player.Pos;

		// HP�A�X�^�~�i�̌v�Z���s��
		HP_STCalculate();

		// �A�j���[�V�����𐧌䂷��
		AnimationManager();

		// �A�j���[�V�������X�V
		UpdateAnimation(Player.Animation, TimePerFrame * Player.ActionSpeed);

		// �v���C���[�ړ�
		PlayerMove();
		break;

	default:
		break;
	}

	// �f�o�b�O�\��
#if _DEBUG
	PrintDebugMsg("PlayerPos�F(%f : %f : %f)\n", Player.Pos.x, Player.Pos.y, Player.Pos.z);
	PrintDebugMsg("PlayerNextPos�F(%f : %f : %f)\n", Player.NextPos.x, Player.NextPos.y, Player.NextPos.z);
	PrintDebugMsg("Player.DestAngle�F%f\n", Player.DestAngle);
	PrintDebugMsg("Player Animation�F%s\n", Player.Animation->AnimData[Player.Animation->CurrentAnimID].SetName);
	double Time = Player.Animation->AnimController->GetTime();
	double Period = Player.Animation->AnimData[Player.Animation->CurrentAnimID].AnimSet->GetPeriod();
	int Frame = (int)(fmod(Time, Period) * 60);
	PrintDebugMsg(" Animation Total Frame�F%d\n", (int)(Period * 60));
	PrintDebugMsg(" Animation Frame�F%d\n", Frame);
	PrintDebugMsg(" Next Animation�F%s\n", Player.Animation->AnimData[Player.Animation->NextAnimID].SetName);
#endif

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	CAMERA_3RD *Camera = GetCamera_3rd();
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix, CapsuleMatrix;
	D3DMATERIAL9 matDef;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&Player.WorldMatrix);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&ScaleMatrix, Player.Scale.x, Player.Scale.y, Player.Scale.z);
	D3DXMatrixMultiply(&Player.WorldMatrix, &Player.WorldMatrix, &ScaleMatrix);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Player.Rot.y, Player.Rot.x, Player.Rot.z);
	D3DXMatrixMultiply(&Player.WorldMatrix, &Player.WorldMatrix, &RotMatrix);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&TransMatrix, Player.Pos.x, Player.Pos.y, Player.Pos.z);
	D3DXMatrixMultiply(&Player.WorldMatrix, &Player.WorldMatrix, &TransMatrix);

	// ���[���h�}�g���b�N�X�̐ݒ�
	Device->SetTransform(D3DTS_WORLD, &Player.WorldMatrix);

	// ���݂̃}�e���A�����擾
	Device->GetMaterial(&matDef);

	// �A�j���[�V������`�悷��
	DrawAnimation(Player.Animation, &Player.WorldMatrix, false);

	// �}�e���A�����f�t�H���g�ɖ߂�
	Device->SetMaterial(&matDef);

	// �v���C���[�̃J�v�Z����`�悷��
	if (Player.Invincible != true)
	{
		// �{�[���̃}�g���N�X	��T��
		CapsuleMatrix = GetBoneMatrix(Player.Animation, "Hips");
		// �`��
		DrawCapsule(&Player.HitCapsule, &CapsuleMatrix);
	}

	// HP�񕜒��A�|�[�V������`�悷��
	if (Player.Animation->CurrentAnimID == HPRestore)
	{
		DrawPotion();
	}

	return;
}


//=============================================================================
// ���ׂẴA�j���[�V�����𐧌䂷��
//=============================================================================
void AnimationManager(void)
{
	CAMERA_3RD *Camera_3rd = GetCamera_3rd();
	bool AnimChangeable = false;
	static bool MoveBack = false;
	static bool CountDown = false;
	static int Count = 0;

	// �v���C���[�ړ�
	if (Player.BeDamaged == false)
	{
		if (Camera_3rd->InLockOn == false)
		{
			if (GetKeyboardPress(DIK_W) || GetKeyboardPress(DIK_A) ||
				GetKeyboardPress(DIK_S) || GetKeyboardPress(DIK_D) ||
				IsButtonPressed(0, BUTTON_UP) || IsButtonPressed(0, BUTTON_LEFT) ||
				IsButtonPressed(0, BUTTON_DOWN) || IsButtonPressed(0, BUTTON_RIGHT))
			{
				if (Player.Animation->CurrentAnimID == Idle || Player.Animation->CurrentAnimID == Running ||
					(Player.Animation->CurrentAnimID == Rolling && Player.Animation->MotionEnd == true))
				{
					Player.Animation->NextAnimID = Running;
				}
			}
		}
		else
		{
			switch (Player.Animation->CurrentAnimID)
			{
			case Rolling:
				if (Player.Animation->MotionEnd != true)
				{
					break;
				}
			case Idle:
			case Running:
			case Walk_Left:
			case Walk_Right:
			case Walk_Back:
				if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, BUTTON_UP))
				{
					Player.Animation->NextAnimID = Running;
				}
				else if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))
				{
					Player.Animation->NextAnimID = Walk_Back;
				}
				if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT))
				{
					Player.Animation->NextAnimID = Walk_Left;
				}
				else if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT))
				{
					Player.Animation->NextAnimID = Walk_Right;
				}
				break;
			default:
				break;
			}
		}
	}

	// �U��
	if ((IsMouseLeftTriggered() || IsButtonTriggered(0, BUTTON_RB)) && Player.Stamina > 0.0f)
	{
		switch (Player.Animation->CurrentAnimID)
		{
		case Idle:
		case Running:
		case Walk_Left:
		case Walk_Right:
		case Walk_Back:
		case Rolling:
		case Attack2:
			MoveCalculate(Move_ChangeRot);
			Player.Animation->NextAnimID = Attack1;
			break;
		case Attack1:
			MoveCalculate(Move_ChangeRot);
			Player.Animation->NextAnimID = Attack2;
		default:
			break;
		}
	}

	// ���[�����O
	if ((GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A)) && Player.Stamina > 0.0f)
	{
		switch (Player.Animation->CurrentAnimID)
		{
		case Idle:
		case Running:
		case Walk_Left:
		case Walk_Right:
		case Walk_Back:
		case Rolling:
		case Attack1:
		case Attack2:
			MoveCalculate(Move_ChangeRot);
			Player.Animation->NextAnimID = Rolling;
			break;
		default:
			break;
		}
	}

	// ��
	if ((GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_X)) && Player.HPPotionNum > 0)
	{
		switch (Player.Animation->CurrentAnimID)
		{
		case Idle:
		case Running:
		case Walk_Left:
		case Walk_Right:
		case Walk_Back:
		case Rolling:
		case Attack1:
		case Attack2:
			Player.Animation->NextAnimID = HPRestore;
			break;
		default:
			break;
		}
	}

	// ===============================
	// ���݂̃A�j���[�V����
	// ===============================
	switch (Player.Animation->CurrentAnimID)
	{
	case Idle:

		if (Player.Animation->PreviousAnimID == Attack1 && Player.Animation->StartMove == true)
		{
			MoveCalculate(Move_Step);
		}
		else
		{
			Player.Animation->PreviousAnimID = Idle;
			MoveCalculate(Move_Running);
		}
		break;

	case Running:

		MoveCalculate(Move_Running);
		break;

	case Walk_Left:
	case Walk_Right:
	case Walk_Back:

		MoveCalculate(Move_LockOn);
		break;

	case Rolling:

		MoveCalculate(Move_Rolling);
		break;

	case Attack1:
	case Attack2:

		MoveCalculate(Move_Attack);
		if (Player.Animation->SlashTrack == true)
		{
			SetSurface(Player.Sword->HitCapsule.PreP1, Player.Sword->HitCapsule.P1,
				Player.Sword->HitCapsule.PreP2, Player.Sword->HitCapsule.P2, WHITE(200));
		}
		break;

	case FlyingBack:
		// ������΂����
		MoveCalculate(Move_FlyingBack);
	case FallingBack:

		// �����オ��J�E���g
		if (CountDown == true)
		{
			Count++;
			if (Count == FallToStandCount)
			{
				Count = 0;
				CountDown = false;
				Player.GiveDamage = false;
				Player.BeDamaged = false;
				Player.Animation->AnimController->ResetTime();
				AnimChangeable = true;
			}
		}
		break;

	case FallToStand:
	case HitReact:
	case HPRestore:

		Player.NextDestAngle = Player.DestAngle;
		break;

	case PlayerDeath:

		Player.Animation->NextAnimID = Idle;
		// �v���C���[���S
		if (Player.Animation->MotionEnd == true)
		{
			SetGameStage(Stage_GameOver);
			while (ShowCursor(true) < 0);
		}
		break;

	default:
		break;
	}

	// �ҋ@�A�����ԂȂ炢�ł��A�j���[�V�������ς���
	if (Player.Animation->CurrentAnimID == Idle || Player.Animation->CurrentAnimID == Running ||
		Player.Animation->CurrentAnimID == Walk_Left || Player.Animation->CurrentAnimID == Walk_Right ||
		Player.Animation->CurrentAnimID == Walk_Back || Player.Animation->CurrentAnimID == Sit)
	{
		Player.Stamina += StaminaRestore * Player.DifficultyRate;
		if (Player.Stamina >= Player.Stamina_Max)
		{
			Player.Stamina = Player.Stamina_Max;
		}
		AnimChangeable = true;
	}
	else
	{
		// �A�j���[�V�����I���A���̃A�j���[�V�����ɕς��
		if (Player.Animation->MotionEnd == true)
		{
			Player.Animation->MotionEnd = false;
			if (Player.Animation->CurrentAnimID == FallingBack || Player.Animation->CurrentAnimID == FlyingBack)
			{
				// �J�E���g�_�E���J�n
				CountDown = true;
				// �A�j���[�V�������~����
				Player.Animation->AnimController->SetTrackSpeed(0, 0.0f);
			}
			else
			{
				Player.GiveDamage = false;
				Player.BeDamaged = false;
				Player.DestAngle = Player.NextDestAngle;
				AnimChangeable = true;
			}
		}
	}

	// ===============================
	// ���̃A�j���[�V����
	// ===============================
	// �A�j���[�V������ς��͉̂\��
	if (AnimChangeable == true)
	{
		switch (Player.Animation->NextAnimID)
		{
		case Idle:
			ChangeAnimation(Player.Animation, Idle, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Running:
			ChangeAnimation(Player.Animation, Running, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Walk_Left:
			ChangeAnimation(Player.Animation, Walk_Left, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Walk_Right:
			ChangeAnimation(Player.Animation, Walk_Right, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Walk_Back:
			ChangeAnimation(Player.Animation, Walk_Back, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Rolling:
			Player.Stamina -= SpendStamina_Rolling;
			ChangeAnimation(Player.Animation, Rolling, 1.5f, true);
			Player.Animation->NextAnimID = Idle;
			break;
		case FallingBack:
			ChangeAnimation(Player.Animation, FallingBack, 1.0f, false);
			Player.Animation->NextAnimID = FallToStand;
			break;
		case FlyingBack:
			ChangeAnimation(Player.Animation, FlyingBack, 1.0f, false);
			Player.Animation->NextAnimID = FallToStand;
			break;
		case FallToStand:
			ChangeAnimation(Player.Animation, FallToStand, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case HitReact:
			ChangeAnimation(Player.Animation, HitReact, 1.0f, true);
			Player.Animation->NextAnimID = Idle;
			break;
		case HPRestore:
			Player.HPPotionNum--;
			Player.RestoreHP = Player.HP + HPPotionRestore;
			Player.InHPRestore = true;
			if (Player.RestoreHP >= Player.HP_Max)
			{
				Player.RestoreHP = Player.HP_Max;
			}
			Set3DSound(Player.Pos, SE3D_HPRestore, E_DS8_FLAG_NONE, true);
			SetEffect(Player.Pos, HPHeal);
			ChangeAnimation(Player.Animation, HPRestore, 2.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Sit:
			ChangeAnimation(Player.Animation, Sit, 1.0f, false);
			Player.Animation->NextAnimID = Sit;
			break;
		case StandUp:
			ChangeAnimation(Player.Animation, StandUp, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case PlayerDeath:
			ChangeAnimation(Player.Animation, PlayerDeath, 1.0f, false);
			break;
		case Attack1:
			Player.Stamina -= SpendStamina_Attack;
			ChangeAnimation(Player.Animation, Attack1, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		case Attack2:
			Player.Stamina -= SpendStamina_Attack;
			ChangeAnimation(Player.Animation, Attack2, 1.0f, false);
			Player.Animation->NextAnimID = Idle;
			break;
		default:
			break;
		}
	}

	return;
}

//=============================================================================
// �v���C���[�̈ړ��𐧌䂷��
//=============================================================================
void MoveCalculate(int MoveState)
{
	CAMERA_3RD *Camera_3rd = GetCamera_3rd();
	// �����ύX�����t���O
	static bool RotChanged = false;
	float DiffRotY = 0.0f;
	float PreDestAngle = 0.0f;

	if (MoveState == Move_Running || MoveState == Move_ChangeRot || MoveState == Move_LockOn)
	{
		PreDestAngle = Player.DestAngle;

		if (GetKeyboardPress(DIK_A) || IsButtonPressed(0, BUTTON_LEFT))
		{
			if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, BUTTON_UP))
			{
				// ���O�ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot - D3DX_PI * 0.25f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot - D3DX_PI * 0.25f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot - D3DX_PI * 0.25f;
			}
			else if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))
			{
				// ����ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot - D3DX_PI * 0.75f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot - D3DX_PI * 0.75f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot - D3DX_PI * 0.75f;
			}
			else
			{
				// ���ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot - D3DX_PI * 0.50f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot - D3DX_PI * 0.50f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot - D3DX_PI * 0.50f;
			}
		}
		else if (GetKeyboardPress(DIK_D) || IsButtonPressed(0, BUTTON_RIGHT))
		{
			if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, BUTTON_UP))
			{
				// �E�O�ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot + D3DX_PI * 0.25f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot + D3DX_PI * 0.25f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot + D3DX_PI * 0.25f;
			}
			else if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))
			{
				// �E��ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot + D3DX_PI * 0.75f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot + D3DX_PI * 0.75f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot + D3DX_PI * 0.75f;
			}
			else
			{
				// �E�ړ�
				if (MoveState == Move_Running || MoveState == Move_LockOn)
				{
					Player.Move.x += sinf(Camera_3rd->Rot + D3DX_PI * 0.50f) * MoveSpeed;
					Player.Move.z += cosf(Camera_3rd->Rot + D3DX_PI * 0.50f) * MoveSpeed;
				}

				Player.DestAngle = Camera_3rd->Rot + D3DX_PI * 0.50f;
			}
		}
		else if (GetKeyboardPress(DIK_W) || IsButtonPressed(0, BUTTON_UP))
		{
			// �O�ړ�
			if (MoveState == Move_Running || MoveState == Move_LockOn)
			{
				Player.Move.x += sinf(Camera_3rd->Rot) * MoveSpeed;
				Player.Move.z += cosf(Camera_3rd->Rot) * MoveSpeed;
			}

			Player.DestAngle = Camera_3rd->Rot;
		}
		else if (GetKeyboardPress(DIK_S) || IsButtonPressed(0, BUTTON_DOWN))
		{
			// ��ړ�
			if (MoveState == Move_Running || MoveState == Move_LockOn)
			{
				Player.Move.x += sinf(D3DX_PI + Camera_3rd->Rot) * MoveSpeed;
				Player.Move.z += cosf(D3DX_PI + Camera_3rd->Rot) * MoveSpeed;
			}

			Player.DestAngle = D3DX_PI + Camera_3rd->Rot;
		}
		else if (Camera_3rd->InLockOn == true)
		{
			Player.DestAngle = Camera_3rd->Rot;
		}

		if (MoveState == Move_ChangeRot)
		{
			// ���̃��[�V�����̖ڕW�p�x��ۑ�����
			Player.NextDestAngle = Player.DestAngle;
			// �{���̊p�x�ɖ߂�A�������Ȃ��ƁA��u�ŕ�����ς��̂���
			Player.DestAngle = PreDestAngle;
			RotChanged = true;
		}
		else if (MoveState == Move_LockOn && RotChanged == false)
		{
			Player.DestAngle = Camera_3rd->Rot;
		}
	}
	else
	{
		if (Player.Animation->StartMove == true)
		{
			RotChanged = false;
			switch (MoveState)
			{
			case Move_Rolling:
				// ���[�����O����̈ړ�
				Player.Move.x += sinf(Player.DestAngle) * RollingSpeed;
				Player.Move.z += cosf(Player.DestAngle) * RollingSpeed;
				break;
			case Move_Attack:
				// �U���̈ړ�
				Player.Move.x += sinf(Player.DestAngle) * AttackStepSpeed;
				Player.Move.z += cosf(Player.DestAngle) * AttackStepSpeed;
				break;
			case Move_FlyingBack:
				Player.Move += Player.FlyingBackDir * FlyingBackSpeed;
				break;
			case Move_Step:
				Player.Move.x += sinf(Player.DestAngle) * StepSpeed;
				Player.Move.z += cosf(Player.DestAngle) * StepSpeed;
				break;
			default:
				break;
			}
		}
		else if (Player.MoveBack == true)
		{
			Player.Move.x -= sinf(Player.DestAngle) * StepSpeed;
			Player.Move.z -= cosf(Player.DestAngle) * StepSpeed;
		}
		else if (Player.Animation->CurrentAnimID == Attack1)
		{
			// Attack1�͈ړ����Ȃ��̂ŁA���b�N�I������Ƃ��ɁA�����ύX�t���O��false�ɂ��Ȃ���
			// �L�����̕����͕ςɂȂ�
			RotChanged = false;
		}
	}

	if (MoveState != Move_ChangeRot)
	{
		if (Player.DestAngle > D3DX_PI)
		{
			Player.DestAngle -= D3DX_2PI;
		}
		else if (Player.DestAngle < -D3DX_PI)
		{
			Player.DestAngle += D3DX_2PI;
		}

		// �ړI�̊p�x�܂ł̍���
		DiffRotY = Player.DestAngle - Player.Rot.y;
		if (DiffRotY > D3DX_PI)
		{
			DiffRotY -= D3DX_2PI;
		}
		if (DiffRotY < -D3DX_PI)
		{
			DiffRotY += D3DX_2PI;
		}

		// �ړI�̊p�x�܂Ŋ�����������
		Player.Rot.y += DiffRotY * RotateInertia;
		if (Player.Rot.y > D3DX_PI)
		{
			Player.Rot.y -= D3DX_2PI;
		}
		if (Player.Rot.y < -D3DX_PI)
		{
			Player.Rot.y += D3DX_2PI;
		}
	}

	return;
}


//=============================================================================
// �v���C���[�ړ�
//=============================================================================
void PlayerMove(void)
{
	// ���W�X�V
	Player.Pos += Player.Move;

	// �ǂƂ̓����蔻��
	// ���̕�
	if (Player.Pos.x < -Stage_Size)
	{
		Player.Pos.x = -Stage_Size;
	}
	// �E�̕�
	else if (Player.Pos.x > Stage_Size)
	{
		Player.Pos.x = Stage_Size;
	}
	// �O�̕�
	if (Player.Pos.z > Stage_Size)
	{
		Player.Pos.z = Stage_Size;
	}
	// ���̕�
	else if (Player.Pos.z < -Stage_Size)
	{
		Player.Pos.z = -Stage_Size;
	}

	// �v���C���[���̍��W
	Player.NextPos.x = Player.Pos.x + sinf(Player.DestAngle) * 1.0f;
	Player.NextPos.z = Player.Pos.z + cosf(Player.DestAngle) * 1.0f;
	// �v���C���[�̌����̃x�N�g��
	Player.Direction = Player.NextPos - Player.Pos;
	D3DXVec3Normalize(&Player.Direction, &Player.Direction);

	// ���S���W�X�V
	Player.CenterPos = D3DXVECTOR3(Player.Pos.x, Player.Pos.y + 50.0f, Player.Pos.z);

	// �ړ��ʂɊ�����������
	Player.Move.x -= Player.Move.x * MoveInertia;
	Player.Move.y -= Player.Move.y * MoveInertia;
	Player.Move.z -= Player.Move.z * MoveInertia;

	return;
}

//=============================================================================
// HP�A�X�^�~�i�̏���A�񕜂��v�Z����
//=============================================================================
void HP_STCalculate(void)
{
	static int HPDecreaseCount = 0;
	static int STDecreaseCount = 0;

	// HP��
	if (Player.InHPRestore == true)
	{
		if (Player.HP < Player.RestoreHP)
		{
			Player.HP++;
		}
		else
		{
			Player.InHPRestore = false;
			Player.HP = Player.RestoreHP;
		}
	}

	// HP������
	if (Player.PreviousHP > Player.HP)
	{
		HPDecreaseCount++;
		if (HPDecreaseCount >= DecreaseCount)
		{
			Player.HPDecreaseStart = true;
		}
	}
	else
	{
		Player.PreviousHP = Player.HP;
		HPDecreaseCount = 0;
	}

	// �X�^�~�i����
	if (Player.PreviousST > Player.Stamina)
	{
		STDecreaseCount++;
		if (STDecreaseCount >= DecreaseCount)
		{
			Player.STDecreaseStart = true;
		}
	}
	else
	{
		Player.PreviousST = Player.Stamina;
		STDecreaseCount = 0;
	}

	return;
}

//=============================================================================
// �v���C���[�̃|�C���^���擾����
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &Player;
}