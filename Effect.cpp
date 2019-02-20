//=============================================================================
//
// �G�t�F�N�g���� [Effect.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
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
// �}�N����`
//*****************************************************************************
#define EffectMax (100)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �{�X�ɍU�����ꂽ�Ƃ��A���̃G�t�F�N�g�̉�]���v�Z����
void CalculateFireRot(int EffectID);


//*****************************************************************************
// �O���[�o���ϐ�
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
// ����������
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

	// ���߂ď�����
	if (FirstInit == true)
	{
		EffectCtrl.Manager = NULL;
		EffectCtrl.Render = NULL;
		EffectCtrl.Effect = NULL;
		EffectCtrl.EffectNum = sizeof(EffectFileName) / sizeof(const EFK_CHAR*);

		// �`��p�C���X�^���X�̐���
		EffectCtrl.Render = ::EffekseerRendererDX9::Renderer::Create(Device, 30000);

		// �G�t�F�N�g�Ǘ��p�C���X�^���X�̐���
		EffectCtrl.Manager = ::Effekseer::Manager::Create(30000);

		// �`��p�C���X�^���X����`��@�\��ݒ�
		EffectCtrl.Manager->SetSpriteRenderer(EffectCtrl.Render->CreateSpriteRenderer());
		EffectCtrl.Manager->SetRibbonRenderer(EffectCtrl.Render->CreateRibbonRenderer());
		EffectCtrl.Manager->SetRingRenderer(EffectCtrl.Render->CreateRingRenderer());
		EffectCtrl.Manager->SetTrackRenderer(EffectCtrl.Render->CreateTrackRenderer());
		EffectCtrl.Manager->SetModelRenderer(EffectCtrl.Render->CreateModelRenderer());

		// �`��p�C���X�^���X����e�N�X�`���̓Ǎ��@�\��ݒ�
		// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
		EffectCtrl.Manager->SetTextureLoader(EffectCtrl.Render->CreateTextureLoader());
		EffectCtrl.Manager->SetModelLoader(EffectCtrl.Render->CreateModelLoader());

		// ���������m��
		EffectCtrl.Effect = (Effekseer::Effect**)calloc(EffectCtrl.EffectNum, sizeof(Effekseer::Effect*));
		if (EffectCtrl.Effect == NULL)
		{
			MessageBox(0, "Alloc Effect Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// �G�t�F�N�g�̓Ǎ�
		for (int i = 0; i < EffectCtrl.EffectNum; i++)
		{
			if (FAILED(SafeLoadEffect(EffectCtrl.Manager, &EffectCtrl.Effect[i], EffectFileName[i], "Other")))
			{
				return E_FAIL;
			}
		}
	}

	// �S�G�t�F�N�g��~
	EffectCtrl.Manager->StopAllEffects();
	EffectCtrl.Manager->Update();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
{
	// �G�t�F�N�g�̒�~
	EffectCtrl.Manager->StopAllEffects();

	// �G�t�F�N�g�̔j��
	SafeFree(EffectCtrl.Effect);

	// ��ɃG�t�F�N�g�Ǘ��p�C���X�^���X��j��
	EffectCtrl.Manager->Destroy();

	// ���ɕ`��p�C���X�^���X��j��
	EffectCtrl.Render->Destroy();

	return;
}

//=============================================================================
// �X�V����
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

	// �G�t�F�N�g�̍X�V�������s��
	EffectCtrl.Manager->Update();

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffect(void)
{
	CAMERA_3RD *Camera = GetCamera_3rd();

	// ���e�s���ݒ�
	EffectCtrl.Render->SetProjectionMatrix(
		::Effekseer::Matrix44().PerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z));

	// �J�����s���ݒ�
	EffectCtrl.Render->SetCameraMatrix(
		::Effekseer::Matrix44().LookAtLH(DXtoEffekVec(Camera->Pos), DXtoEffekVec(Camera->At), DXtoEffekVec(Camera->Up)));

	// �G�t�F�N�g�̕`��J�n�������s���B
	EffectCtrl.Render->BeginRendering();

	// �G�t�F�N�g�̕`����s���B
	EffectCtrl.Manager->Draw();

	// �G�t�F�N�g�̕`��I���������s���B
	EffectCtrl.Render->EndRendering();

	return;
}

//=============================================================================
// �G�t�F�N�g��ݒu����
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
// �{�X�͓�i�K�ɓ��鎞�̃G�t�F�N�g
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
// �{�X�ɍU�����ꂽ�Ƃ��A���̃G�t�F�N�g�̉�]���v�Z����
//=============================================================================
void CalculateFireRot(int EffectID)
{
	PLAYER *Player = GetPlayer();
	// �G�t�F�N�g��]����
	D3DXVECTOR3 Direction;
	// ��]��
	D3DXVECTOR3 RotAxis;
	// �{�[�����W
	D3DXVECTOR3 BonePos;
	// ��]���W�A��
	float RotRadian = 0.0f;
	// �{�[���}�g���N�X
	D3DMATRIX HipMatrix;

	// �{�[���̃}�g���N�X���擾
	HipMatrix = GetBoneMatrix(Player->Animation, "Hips");
	// �{�[���̍��W
	BonePos.x = HipMatrix._41;
	BonePos.y = HipMatrix._42;
	BonePos.z = HipMatrix._43;
	// �{�[���̌���
	Direction.x = HipMatrix._31;
	Direction.y = HipMatrix._32;
	Direction.z = HipMatrix._33;
	D3DXVec3Normalize(&Direction, &Direction);
	// ��]���W�A���v�Z
	RotRadian = acosf(D3DXVec3Dot(&UpVector, &Direction));
	// �G�t�F�N�g�̃f�t�H���g�����͏�
	D3DXVec3Cross(&RotAxis, &UpVector, &Direction);
	D3DXVec3Normalize(&RotAxis, &RotAxis);
	// �G�t�F�N�g���W
	EffectCtrl.Manager->SetLocation(EffectID, DXtoEffekVec(BonePos));
	// �G�t�F�N�g��]
	EffectCtrl.Manager->SetRotation(EffectID, DXtoEffekVec(RotAxis), RotRadian + D3DX_PI);

	return;
}

//=============================================================================
// �G�t�F�N�g�R���g���[���[���擾����
//=============================================================================
EFFECTCONTROLLER *GetEffectCtrl(void)
{
	return &EffectCtrl;
}

//=============================================================================
// �G�t�F�N�g��ǂݍ���
//=============================================================================
HRESULT SafeLoadEffect(Effekseer::Manager *Manager, Effekseer::Effect **Effect, const EFK_CHAR* SrcPath, const char* ErrorSrc)
{
	char Message[256];
	// �G�t�F�N�g�t�@�C���̒��̃e�N�X�`���̐�
	int TextureNum = 0;

	// �G�t�F�N�g�t�@�C����ǂݍ���
	(*Effect) = Effekseer::Effect::Create(Manager, SrcPath);
	// ���s������
	if ((*Effect) == NULL)
	{
		sprintf(Message, "Load %s Effect Failed�I", ErrorSrc);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	// �G�t�F�N�g�t�@�C���̒��̃e�N�X�`���̐����擾����
	TextureNum = (*Effect)->GetColorImageCount();
	for (int i = 0; i < TextureNum; i++)
	{
		// �e�N�X�`���̃|�C���^���擾����
		Effekseer::TextureData *Texture = (*Effect)->GetColorImage(i);
		// �e�N�X�`����ǂݍ��ނ����s������
		if (Texture == NULL)
		{
			sprintf(Message, "Load %s Effect Texture Failed�I", ErrorSrc);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	return S_OK;
}


