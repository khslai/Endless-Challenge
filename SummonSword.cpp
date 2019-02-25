//=============================================================================
//
// ���̏������� [SummonSword.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
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
// �}�N����`
//*****************************************************************************
#define	Model_BossSword		"data/Model/BossSword.x"	// �ǂݍ��ރ��f����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����ݒu����
void SetSummonSword(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
SUMMONSWORD			SummonSword;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitSummonSword(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sword_No = 0;

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	SummonSword.Pos = PositionZero;
	SummonSword.Rot = PositionZero;
	SummonSword.Scale = DefaultScale;
	SummonSword.EffectID = -1;
	SummonSword.State = -1;
	SummonSword.Count = 0;
	SummonSword.Use = false;
	SummonSword.BurnedFire = false;

	// ���߂ď�����
	if (FirstInit == true)
	{
		SummonSword.Texture = NULL;
		SummonSword.MaterialBuffer = NULL;
		SummonSword.Mesh = NULL;
		SummonSword.MaterialNum = 0;

		// X�t�@�C���̓ǂݍ���
		if (FAILED(D3DXLoadMeshFromX(Model_BossSword,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,							// ���b�V���̍쐬�I�v�V�������w��
			Device,										// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,										// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&SummonSword.MaterialBuffer,		// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,										// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&SummonSword.MaterialNum,			// D3DXMATERIAL�\���̂̐�
			&SummonSword.Mesh)))				// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		// ���f���p�̃e�N�X�`���̃�������Ԃ�z��
		SummonSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(SummonSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (SummonSword.Texture == NULL)
		{
			MessageBox(0, "Alloc BossSword Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// �e�N�X�`����ǂݍ���
		if (FAILED(SafeLoadModelTexture(SummonSword.Texture, SummonSword.MaterialBuffer, SummonSword.MaterialNum, "BossSword")))
		{
			return E_FAIL;
		}

		// �����蔻��J�v�Z���𐶐�
		if (FAILED(
			CreateCapsule(&SummonSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 90.0f, 2.0f, false)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSummonSword(void)
{
	// �e�N�X�`���̊J��
	for (int i = 0; i < (int)SummonSword.MaterialNum; i++)
	{
		SafeRelease(SummonSword.Texture[i]);
	}

	// �}�e���A���̊J��
	SafeRelease(SummonSword.MaterialBuffer);

	// ���b�V���̊J��
	SafeRelease(SummonSword.Mesh);

	return;
}

//=============================================================================
// �X�V����
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

			// �G�t�F�N�g��ݒu����
			SummonSword.EffectID = SetEffect(SummonSword.HitCapsule.P2, SummonSwordEffect);

			// �G�t�F�N�g�̉�]���v�Z����
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

		// ���̌��̃G�t�F�N�g��]���v�Z����
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
// �`�揈��
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
		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&SummonSword.WorldMatrix);
		D3DXMatrixIdentity(&CapsuleMatrix);
		CapsuleMatrix._31 = 0.0f;
		CapsuleMatrix._32 = -1.0f;
		CapsuleMatrix._33 = 0.0f;

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&ScaleMatrix, SummonSword.Scale.x, SummonSword.Scale.y, SummonSword.Scale.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &ScaleMatrix);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, SummonSword.Rot.y, SummonSword.Rot.x, SummonSword.Rot.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &RotMatrix);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&TransMatrix, SummonSword.Pos.x, SummonSword.Pos.y, SummonSword.Pos.z);
		D3DXMatrixMultiply(&SummonSword.WorldMatrix, &SummonSword.WorldMatrix, &TransMatrix);

		// ���[���h�}�g���b�N�X�̐ݒ�
		Device->SetTransform(D3DTS_WORLD, &SummonSword.WorldMatrix);

		// ���݂̃}�e���A�����擾
		Device->GetMaterial(&DefaultMat);

		// �}�e���A�����ɑ΂���|�C���^���擾
		pD3DXMat = (D3DXMATERIAL*)SummonSword.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)SummonSword.MaterialNum; nCntMat++)
		{
			// �}�e���A���̐ݒ�
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// �e�N�X�`���̐ݒ�
			Device->SetTexture(0, SummonSword.Texture[nCntMat]);

			// �`��
			SummonSword.Mesh->DrawSubset(nCntMat);
		}

		// �}�e���A�����f�t�H���g�ɖ߂�
		Device->SetMaterial(&DefaultMat);

		// �����̂̃J�v�Z����`�悷��
		D3DXMatrixMultiply(&CapsuleMatrix, &CapsuleMatrix, &SummonSword.WorldMatrix);
		DrawCapsule(&SummonSword.HitCapsule, &CapsuleMatrix);
	}

	return;
}

//=============================================================================
// ����ݒu����
//=============================================================================
void SetSummonSword(void)
{
	PLAYER *Player = GetPlayer();

	// �ŏ�����n�܂�
	if (SummonSword.Use == false)
	{
		SummonSword.Use = true;
		SummonSword.BurnedFire = false;
		SummonSword.Count = 0;
		SummonSword.State = SetSummonEffect;
		SummonSword.Pos.x = Player->CenterPos.x + sinf(Player->DestAngle + D3DX_HalfPI) * 50.0f;
		SummonSword.Pos.y = Player->CenterPos.y;
		SummonSword.Pos.z = Player->CenterPos.z + cosf(Player->DestAngle + D3DX_HalfPI) * 50.0f;

		// ���̃��f����]
		SummonSword.Rot.y = Player->DestAngle;
	}

	return;
}

//=============================================================================
// �|�C���^���擾����
//=============================================================================
SUMMONSWORD *GetSummonSword()
{
	return &SummonSword;
}