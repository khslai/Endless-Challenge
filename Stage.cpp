//=============================================================================
//
// �X�e�[�W���� [Stage.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Stage.h"
#include "Light.h"
#include "ThirdPersonCamera.h"
#include "ShadowMapping.h"
#include "Input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �ǂݍ��ރ��f����
#define	Model_Stage			"data/Model/Stage.x"	
// �ǂݍ��ރV�F�[�_�[��
#define DepthBufferShader	_T("data/Shaders/DepthBufferShader.fx")


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �`��J�n�錾
void DepthBuffer_Begin(void);
// �V�F�[�_�[�̃p�����[�^��ݒ肷��
void SetDepthBufferParam(void);
// �`��I���錾
void DepthBuffer_End(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
STAGE			Stage;			// �X�e�[�W
DEPTHBUFFER		DepthBuffer;	// �[�x�o�b�t�@

//=============================================================================
// ����������
//=============================================================================
HRESULT InitStage(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	Stage.Pos = PositionZero;
	Stage.Rot = PositionZero;
	Stage.Scale = DefaultScale;

	// ���߂ď�����
	if (FirstInit == true)
	{
		Stage.Texture = NULL;
		Stage.MaterialBuffer = NULL;
		Stage.Mesh = NULL;
		Stage.MaterialNum = 0;

		// X�t�@�C���̓ǂݍ���
		if (FAILED(D3DXLoadMeshFromX(Model_Stage,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,						// ���b�V���̍쐬�I�v�V�������w��
			Device,									// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,									// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&Stage.MaterialBuffer,					// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,									// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&Stage.MaterialNum,						// D3DXMATERIAL�\���̂̐�
			&Stage.Mesh)))							// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		// ���f���p�̃e�N�X�`���̃�������Ԃ�z��
		Stage.Texture = (LPDIRECT3DTEXTURE9*)calloc(Stage.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Stage.Texture == NULL)
		{
			MessageBox(0, "Alloc Stage Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// �e�N�X�`����ǂݍ���
		if (FAILED(SafeLoadModelTexture(Stage.Texture, Stage.MaterialBuffer, Stage.MaterialNum, "Stage")))
		{
			return E_FAIL;
		}

		// ���\�[�X�ɂ���[�x�o�b�t�@�V���h�E�V�F�[�_�v���O������ǂݍ���
		if (FAILED(D3DXCreateEffectFromFile(
			Device,
			DepthBufferShader,
			NULL,
			NULL,
			0,
			NULL,
			&DepthBuffer.Effect,
			NULL)))
		{
			return E_FAIL;
		}

		DepthBuffer.Para_WorldMatrix = DepthBuffer.Effect->GetParameterByName(NULL, "WorldMatrix");
		DepthBuffer.Para_WVPMatrix = DepthBuffer.Effect->GetParameterByName(NULL, "WorldViewProj");
		DepthBuffer.Para_WVPMatrix_Light = DepthBuffer.Effect->GetParameterByName(NULL, "WorldViewProj_Light");
		DepthBuffer.Para_CosTheta = DepthBuffer.Effect->GetParameterByName(NULL, "CosTheta");
		DepthBuffer.Para_ShadowMapTexture = DepthBuffer.Effect->GetParameterByName(NULL, "ShadowMapTex");
		DepthBuffer.Technique = DepthBuffer.Effect->GetTechniqueByName("RenderShadow");

		if (!DepthBuffer.Para_WorldMatrix || !DepthBuffer.Para_WVPMatrix || !DepthBuffer.Para_WVPMatrix_Light ||
			!DepthBuffer.Para_CosTheta || !DepthBuffer.Para_ShadowMapTexture || !DepthBuffer.Technique)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitStage(void)
{
	// �e�N�X�`���̊J��
	for (int i = 0; i < (int)Stage.MaterialNum; i++)
	{
		SafeRelease(Stage.Texture[i]);
	}

	// �}�e���A���̊J��
	SafeRelease(Stage.MaterialBuffer);

	// ���b�V���̊J��
	SafeRelease(Stage.Mesh);

	SafeRelease(DepthBuffer.Effect);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateStage(void)
{

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawStage(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;

	//=================================================
	// �X�e�[�W���f��
	//=================================================
	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&Stage.WorldMatrix);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&ScaleMatrix, Stage.Scale.x, Stage.Scale.y, Stage.Scale.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &ScaleMatrix);

	// ��]�𔽉f
	D3DXMatrixRotationYawPitchRoll(&RotMatrix, Stage.Rot.y, Stage.Rot.x, Stage.Rot.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &RotMatrix);

	// �ړ��𔽉f
	D3DXMatrixTranslation(&TransMatrix, Stage.Pos.x, Stage.Pos.y, Stage.Pos.z);
	D3DXMatrixMultiply(&Stage.WorldMatrix, &Stage.WorldMatrix, &TransMatrix);

	// ���[���h�}�g���b�N�X�̐ݒ�
	Device->SetTransform(D3DTS_WORLD, &Stage.WorldMatrix);

	// �}�e���A�����ɑ΂���|�C���^���擾
	pD3DXMat = (D3DXMATERIAL*)Stage.MaterialBuffer->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)Stage.MaterialNum; nCntMat++)
	{
		// �}�e���A���̐ݒ�
		Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, Stage.Texture[nCntMat]);

		// �`��
		Stage.Mesh->DrawSubset(nCntMat);
	}

	//=================================================
	// �n�ʂ̉e
	//=================================================
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);

	// �`��J�n
	DepthBuffer_Begin();

	// �V�F�[�_�[�̃p�����[�^��ݒ肷��
	SetDepthBufferParam();

	// �`��
	for (int nCntMat = 0; nCntMat < (int)Stage.MaterialNum; nCntMat++)
	{
		DepthBuffer.Effect->BeginPass(0);
		Stage.Mesh->DrawSubset(nCntMat);
		DepthBuffer.Effect->EndPass();
	}

	// �`��I��
	DepthBuffer_End();

	// �ʏ�u�����h
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return;
}

//=============================================================================
// �`��J�n�錾
//=============================================================================
void DepthBuffer_Begin(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �v���O���}�u���V�F�[�_�ɐ؂�ւ�
	DepthBuffer.Effect->SetTechnique(DepthBuffer.Technique);

	// �f�v�X�e�N�X�`�����擾����
	DepthBuffer.ShadowMapTexture = GetShadowMapTexture();

	// �J�n�錾
	UINT Pass;
	DepthBuffer.Effect->Begin(&Pass, 0);

	return;
}

//=============================================================================
// �V�F�[�_�[�̃p�����[�^��ݒ肷��
//=============================================================================
void SetDepthBufferParam(void)
{
	D3DXMATRIX WVPMatrix;
	D3DXMATRIX CameraViewMatrix = GetViewMatrix();
	D3DXMATRIX CameraProjMatrix = GetProjMatrix();
	D3DXMATRIX LightViewMatrix = GetLightViewMatrix();
	D3DXMATRIX LightProjMatrix = GetLightProjMatrix();
	D3DXVECTOR3 LightPos = GetLightPos();
	D3DXVECTOR3 LightDir = PositionZero;

	// ���C�g����
	D3DXVec3Normalize(&LightDir, &LightPos);
	LightDir *= -1;
	// 90�x
	DepthBuffer.Effect->SetFloat(DepthBuffer.Para_CosTheta, cosf(D3DX_PI / 2));
	DepthBuffer.Effect->SetFloatArray("LightPos", (float*)&LightPos, 3);
	DepthBuffer.Effect->SetFloatArray("LightDirection", (float*)&LightDir, 3);
	DepthBuffer.Effect->SetMatrix(DepthBuffer.Para_WorldMatrix, &Stage.WorldMatrix);

	// �e�N�X�`��
	DepthBuffer.Effect->SetTexture(DepthBuffer.Para_ShadowMapTexture, *DepthBuffer.ShadowMapTexture);

	// �J����
	WVPMatrix = Stage.WorldMatrix * CameraViewMatrix * CameraProjMatrix;
	DepthBuffer.Effect->SetMatrix(DepthBuffer.Para_WVPMatrix, &WVPMatrix);

	// ���C�g
	WVPMatrix = Stage.WorldMatrix * LightViewMatrix * LightProjMatrix;
	DepthBuffer.Effect->SetMatrix("WorldViewProj_Light", &WVPMatrix);

	return;
}

//=============================================================================
// �`��I���錾
//=============================================================================
void DepthBuffer_End(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �`��I��
	DepthBuffer.Effect->End();

	// �Œ�@�\�ɖ߂�
	Device->SetVertexShader(NULL);
	Device->SetPixelShader(NULL);

	return;
}

//=============================================================================
// �X�e�[�W�̃|�C���^���擾����
//=============================================================================
STAGE *GetStage(void)
{
	return &Stage;
}