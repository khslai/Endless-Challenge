//=============================================================================
//
// ������͈͕\������ [AttackRange.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "AttackRange.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define Sphere_Max	(100)	// �\���ł���ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
RANGESPHERE			RangeSphere[Sphere_Max];
D3DMATERIAL9		Material;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitAttackRange(bool FirstInit)
{

#if _DEBUG
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sphere_No = 0;

	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		RangeSphere[Sphere_No].Mesh = NULL;
		RangeSphere[Sphere_No].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		RangeSphere[Sphere_No].Rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		RangeSphere[Sphere_No].Use = false;
	}

	if (FirstInit == true)
	{
		Material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.7f, 1.0f);
		Material.Diffuse = D3DXCOLOR(0.6f, 0.6f, 0.6f, 1.0f);
		Material.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);
		Material.Emissive = D3DXCOLOR(0.3f, 0.0f, 0.1f, 1.0f);
	}
#endif

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAttackRange(void)
{
	for (int Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		SafeRelease(RangeSphere[Sphere_No].Mesh);
	}

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAttackRange(void)
{

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAttackRange(void)
{

#if _DEBUG
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DMATERIAL9 DefaultMat;
	int Sphere_No = 0;
	bool DrawWireFrame = GetDrawWireFrame();

	// �`�揈��
	if (DrawWireFrame == true)
	{
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		if (RangeSphere[Sphere_No].Use == true)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&RangeSphere[Sphere_No].WorldMatrix);

			// ��]�𔽉f
			D3DXMatrixRotationYawPitchRoll(&RotMatrix, RangeSphere[Sphere_No].Rot.y, RangeSphere[Sphere_No].Rot.x, RangeSphere[Sphere_No].Rot.z);
			D3DXMatrixMultiply(&RangeSphere[Sphere_No].WorldMatrix, &RangeSphere[Sphere_No].WorldMatrix, &RotMatrix);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&TransMatrix, RangeSphere[Sphere_No].Pos.x, RangeSphere[Sphere_No].Pos.y, RangeSphere[Sphere_No].Pos.z);
			D3DXMatrixMultiply(&RangeSphere[Sphere_No].WorldMatrix, &RangeSphere[Sphere_No].WorldMatrix, &TransMatrix);

			// ���[���h�}�g���b�N�X�̐ݒ�
			Device->SetTransform(D3DTS_WORLD, &RangeSphere[Sphere_No].WorldMatrix);

			// ���݂̃}�e���A�����擾
			Device->GetMaterial(&DefaultMat);

			Device->SetMaterial(&Material);

			// �`��
			RangeSphere[Sphere_No].Mesh->DrawSubset(0);

			// �}�e���A�����f�t�H���g�ɖ߂�
			Device->SetMaterial(&DefaultMat);
		}
	}

	// �I������
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	return;
}

//=============================================================================
// ������͈͂�ݒu����
//=============================================================================
int SetRangeSphere(D3DXVECTOR3 Pos, float Radius)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int Sphere_No = -1;

#if _DEBUG
	for (Sphere_No = 0; Sphere_No < Sphere_Max; Sphere_No++)
	{
		if (RangeSphere[Sphere_No].Use == false)
		{
			// ���̃��b�V�����쐬����
			if (RangeSphere[Sphere_No].Mesh == NULL)
			{
				D3DXCreateSphere(Device, Radius, 2, 16, &RangeSphere[Sphere_No].Mesh, NULL);
				RangeSphere[Sphere_No].Rot.z = D3DXToRadian(90.0f);
			}
			RangeSphere[Sphere_No].Pos = Pos;
			RangeSphere[Sphere_No].Pos.y = 1.0f;
			RangeSphere[Sphere_No].Use = true;
			break;
		}
	}
#endif

	return Sphere_No;
}

//=============================================================================
// ������͈͂��폜����
//=============================================================================
void DeleteRangeSphere(int Sphere_No)
{

#if _DEBUG
	if (Sphere_No >= 0 && Sphere_No < Sphere_Max)
	{
		RangeSphere[Sphere_No].Use = false;
		SafeRelease(RangeSphere[Sphere_No].Mesh);
	}
#endif

	return;
}
