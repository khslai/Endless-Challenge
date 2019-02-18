//=============================================================================
//
// �������f������ [Equipment.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Equipment.h"
#include "Player.h"
#include "Boss.h"
#include "D3DXAnimation.h"
#include "CapsuleMesh.h"
#include "ThirdPersonCamera.h"
#include "Effect.h"
#include "FireSword.h"
#include "AttackRange.h"
#include "Input.h"
#include "DebugProcess.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �ǂݍ��ރ��f����
#define	Model_PlayerSword	"data/Model/PlayerSword.x"
#define	Model_BossSword		"data/Model/BossSword.x"


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
SWORD		PlayerSword;
SWORD		BossSword;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEquipment(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	PlayerSword.Pos = D3DXVECTOR3(-3.0f, 0.0f, 6.0f);
	PlayerSword.Rot = D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(90.0f));
	PlayerSword.Scale = DefaultScale;

	BossSword.Pos = D3DXVECTOR3(0.0f, 0.0f, 20.0f);
	BossSword.Rot = D3DXVECTOR3(D3DXToRadian(45.0f), D3DXToRadian(90.0f), D3DXToRadian(-90.0f));
	BossSword.Scale = D3DXVECTOR3(1.5f, 1.5f, 1.5f);

	// ���߂ď�����
	if (FirstInit == true)
	{
		PlayerSword.Texture = NULL;
		PlayerSword.MaterialBuffer = NULL;
		PlayerSword.Mesh = NULL;
		PlayerSword.MaterialNum = 0;

		BossSword.Texture = NULL;
		BossSword.MaterialBuffer = NULL;
		BossSword.Mesh = NULL;
		BossSword.MaterialNum = 0;

		// X�t�@�C���̓ǂݍ���
		if (FAILED(D3DXLoadMeshFromX(Model_PlayerSword,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,							// ���b�V���̍쐬�I�v�V�������w��
			Device,										// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,										// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&PlayerSword.MaterialBuffer,				// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,										// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&PlayerSword.MaterialNum,					// D3DXMATERIAL�\���̂̐�
			&PlayerSword.Mesh)))						// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		if (FAILED(D3DXLoadMeshFromX(Model_BossSword,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,							// ���b�V���̍쐬�I�v�V�������w��
			Device,										// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,										// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&BossSword.MaterialBuffer,					// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,										// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&BossSword.MaterialNum,						// D3DXMATERIAL�\���̂̐�
			&BossSword.Mesh)))							// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		// ���f���p�̃e�N�X�`���̃�������Ԃ�z��
		PlayerSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(PlayerSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (PlayerSword.Texture == NULL)
		{
			MessageBox(0, "Alloc PlayerSword Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		BossSword.Texture = (LPDIRECT3DTEXTURE9*)calloc(BossSword.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (BossSword.Texture == NULL)
		{
			MessageBox(0, "Alloc BossSword Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// �e�N�X�`����ǂݍ���
		if (FAILED(SafeLoadModelTexture(PlayerSword.Texture, PlayerSword.MaterialBuffer, PlayerSword.MaterialNum, "PlayerSword")))
		{
			return E_FAIL;
		}

		if (FAILED(SafeLoadModelTexture(BossSword.Texture, BossSword.MaterialBuffer, BossSword.MaterialNum, "BossSword")))
		{
			return E_FAIL;
		}

		// �����蔻��J�v�Z���𐶐�
		if (FAILED(
			CreateCapsule(&PlayerSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 55.0f, 1.5f, false)))
		{
			return E_FAIL;
		}

		if (FAILED(
			CreateCapsule(&BossSword.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 120.0f, 2.0f, false)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEquipment(void)
{
	int i = 0;

	// �e�N�X�`���̊J��
	for (i = 0; i < (int)PlayerSword.MaterialNum; i++)
	{
		SafeRelease(PlayerSword.Texture[i]);
	}
	for (i = 0; i < (int)BossSword.MaterialNum; i++)
	{
		SafeRelease(BossSword.Texture[i]);
	}

	// �}�e���A���̊J��
	SafeRelease(PlayerSword.MaterialBuffer);
	SafeRelease(BossSword.MaterialBuffer);

	// ���b�V���̊J��
	SafeRelease(PlayerSword.Mesh);
	SafeRelease(BossSword.Mesh);

	// �J�v�Z���̊J��
	UninitCapsule(&PlayerSword.HitCapsule);
	UninitCapsule(&BossSword.HitCapsule);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEquipment(void)
{

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEquipment(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 DefaultMat;
	D3DXMATRIX PlayerSwordMatrix;
	D3DXMATRIX BossSwordMatrix;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CAMERA_3RD *Camera = GetCamera_3rd();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&PlayerSword.WorldMatrix);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&ScaleMatrix, PlayerSword.Scale.x, PlayerSword.Scale.y, PlayerSword.Scale.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &ScaleMatrix);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, PlayerSword.Rot.y, PlayerSword.Rot.x, PlayerSword.Rot.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &RotMatrix);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&TransMatrix, PlayerSword.Pos.x, PlayerSword.Pos.y, PlayerSword.Pos.z);
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &TransMatrix);

	// ��������{�[���̃}�g���N�X��T��
	PlayerSwordMatrix = GetBoneMatrix(Player->Animation, "SwordPosBone");
	D3DXMatrixMultiply(&PlayerSword.WorldMatrix, &PlayerSword.WorldMatrix, &PlayerSwordMatrix);

	// ���[���h�}�g���b�N�X�̐ݒ�
	Device->SetTransform(D3DTS_WORLD, &PlayerSword.WorldMatrix);

	// ���݂̃}�e���A�����擾
	Device->GetMaterial(&DefaultMat);

	// �}�e���A�����ɑ΂���|�C���^���擾
	pD3DXMat = (D3DXMATERIAL*)PlayerSword.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)PlayerSword.MaterialNum; nCntMat++)
	{
		// �}�e���A���̐ݒ�
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, PlayerSword.Texture[nCntMat]);

		// �`��
		PlayerSword.Mesh->DrawSubset(nCntMat);
	}

	// �}�e���A�����f�t�H���g�ɖ߂�
	Device->SetMaterial(&DefaultMat);

	// ���̃J�v�Z����`�悷��
	DrawCapsule(&PlayerSword.HitCapsule, &PlayerSwordMatrix);

	//========================================================================
	//========================================================================
	//========================================================================

	if (Boss->Exist == true)
	{
		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&BossSword.WorldMatrix);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&ScaleMatrix, BossSword.Scale.x, BossSword.Scale.y, BossSword.Scale.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &ScaleMatrix);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, BossSword.Rot.y, BossSword.Rot.x, BossSword.Rot.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &RotMatrix);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&TransMatrix, BossSword.Pos.x, BossSword.Pos.y, BossSword.Pos.z);
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &TransMatrix);

		// ��������{�[���̃}�g���N�X��T��
		BossSwordMatrix = GetBoneMatrix(Boss->Animation, "GreatSword");
		D3DXMatrixMultiply(&BossSword.WorldMatrix, &BossSword.WorldMatrix, &BossSwordMatrix);

		// ���[���h�}�g���b�N�X�̐ݒ�
		Device->SetTransform(D3DTS_WORLD, &BossSword.WorldMatrix);

		// ���݂̃}�e���A�����擾
		Device->GetMaterial(&DefaultMat);

		// �}�e���A�����ɑ΂���|�C���^���擾
		pD3DXMat = (D3DXMATERIAL*)BossSword.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)BossSword.MaterialNum; nCntMat++)
		{
			// �}�e���A���̐ݒ�
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// �e�N�X�`���̐ݒ�
			Device->SetTexture(0, BossSword.Texture[nCntMat]);

			// �`��
			BossSword.Mesh->DrawSubset(nCntMat);
		}

		// �}�e���A�����f�t�H���g�ɖ߂�
		Device->SetMaterial(&DefaultMat);

		// ���̃J�v�Z����`�悷��
		DrawCapsule(&BossSword.HitCapsule, &BossSwordMatrix);
	}

	return;
}

//=============================================================================
// �|�C���^���擾����
//=============================================================================
SWORD *GetSword(const char* Owner)
{
	if (strcmp(Owner, "Player") == 0)
	{
		return &PlayerSword;
	}
	else if (strcmp(Owner, "Boss") == 0)
	{
		return &BossSword;
	}
	else
	{
		MessageBox(0, "Can't Find Sword Owner", "Error", 0);
		return NULL;
	}
}
