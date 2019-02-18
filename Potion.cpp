//=============================================================================
//
// �|�[�V�������f������ [Potion.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Potion.h"
#include "Player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	Model_Potion	"data/Model/Potion.x"	// �ǂݍ��ރ��f����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
POTION Potion;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPotion(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	Potion.Pos = PositionZero;
	Potion.Rot = D3DXVECTOR3(D3DXToRadian(-90.0f), 0.0f, 0.0f);
	Potion.Scale = DefaultScale;

	// ���߂ď�����
	if (FirstInit == true)
	{
		Potion.Texture = NULL;
		Potion.MaterialBuffer = NULL;
		Potion.Mesh = NULL;
		Potion.MaterialNum = 0;

		// X�t�@�C���̓ǂݍ���
		if (FAILED(D3DXLoadMeshFromX(Model_Potion,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,						// ���b�V���̍쐬�I�v�V�������w��
			Device,									// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,									// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&Potion.MaterialBuffer,					// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,									// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&Potion.MaterialNum,					// D3DXMATERIAL�\���̂̐�
			&Potion.Mesh)))							// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		// ���f���p�̃e�N�X�`���̃�������Ԃ�z��
		Potion.Texture = (LPDIRECT3DTEXTURE9*)calloc(Potion.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Potion.Texture == NULL)
		{
			MessageBox(0, "Alloc Potion Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// �e�N�X�`����ǂݍ���
		if (FAILED(SafeLoadModelTexture(Potion.Texture, Potion.MaterialBuffer, Potion.MaterialNum, "Potion")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPotion(void)
{
	// �e�N�X�`���̊J��
	for (int i = 0; i < (int)Potion.MaterialNum; i++)
	{
		SafeRelease(Potion.Texture[i]);
	}

	// �}�e���A���̊J��
	SafeRelease(Potion.MaterialBuffer);

	// ���b�V���̊J��
	SafeRelease(Potion.Mesh);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePotion(void)
{

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPotion(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 DefaultMat;
	D3DXMATRIX PotionMatrix;
	PLAYER *Player = GetPlayer();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&Potion.WorldMatrix);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&ScaleMatrix, Potion.Scale.x, Potion.Scale.y, Potion.Scale.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &ScaleMatrix);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Potion.Rot.y, Potion.Rot.x, Potion.Rot.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &RotMatrix);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&TransMatrix, Potion.Pos.x, Potion.Pos.y, Potion.Pos.z);
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &TransMatrix);

	// ����̃{�[���̃}�g���N�X��T��
	PotionMatrix = GetBoneMatrix(Player->Animation, "PotionBone");
	D3DXMatrixMultiply(&Potion.WorldMatrix, &Potion.WorldMatrix, &PotionMatrix);

	// ���[���h�}�g���b�N�X�̐ݒ�
	Device->SetTransform(D3DTS_WORLD, &Potion.WorldMatrix);

	// ���݂̃}�e���A�����擾
	Device->GetMaterial(&DefaultMat);

	// �}�e���A�����ɑ΂���|�C���^���擾
	pD3DXMat = (D3DXMATERIAL*)Potion.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)Potion.MaterialNum; nCntMat++)
	{
		// �}�e���A���̐ݒ�
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, Potion.Texture[nCntMat]);

		// �`��
		Potion.Mesh->DrawSubset(nCntMat);
	}

	// �}�e���A�����f�t�H���g�ɖ߂�
	Device->SetMaterial(&DefaultMat);

	return;
}
