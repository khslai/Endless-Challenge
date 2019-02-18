//=============================================================================
//
// �V���h�E�}�b�s���O���� [ShadowMapping.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "ShadowMapping.h"
#include "Light.h"
#include "Stage.h"
#include "Player.h"
#include "Boss.h"
#include "Equipment.h"
#include "D3DXAnimation.h"
#include "ThirdPersonCamera.h"
#include "Tutorial.h"
#include "Input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �V�F�[�_�[�̃p�X
#define TexCreatorShader	_T("data/Shaders/TexCreatorShader.fx")
// �f�v�X�e�N�X�`���̃T�C�Y
#define	ShadowMap_Size		(2048)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �`��J�n�錾
void TexCreator_Begin(void);
// �V�F�[�_�[�̃p�����[�^��ݒ肷��
void SetTexCreatorParam(void);
// �`��I���錾
void TexCreator_End(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
TEXCREATOR			TexCreator;
LPDIRECT3DSURFACE9	ShadowMapSurface;		// �V���h�E�}�b�v�o�b�t�@
LPDIRECT3DSURFACE9	ShadowMapDepthSurface;	// �V���h�E�}�b�v�[�x�o�b�t�@
LPDIRECT3DSURFACE9	SceneSurface;			// �`��V�[���o�b�t�@
LPDIRECT3DSURFACE9	SceneDepthSurface;		// �`��V�[���[�x�o�b�t�@
static LPD3DXSPRITE	Sprite = NULL;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitShadowMapping(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	PLAYER *Player = GetPlayer();

	// ���߂ď�����
	if (FirstInit == true)
	{
		HRESULT hr;
		D3DXCreateSprite(Device, &Sprite);

		// ���\�[�X�ɂ���Z�l�v���b�g�V�F�[�_�v���O������ǂݍ���
		if (FAILED(D3DXCreateEffectFromFile(
			Device,
			TexCreatorShader,
			NULL,
			NULL,
			0,
			NULL,
			&TexCreator.Effect,
			NULL)))
		{
			return E_FAIL;
		}

		// �w���Z�l�e�N�X�`���𐶐�
		hr = D3DXCreateTexture(
			Device,
			ShadowMap_Size, ShadowMap_Size,
			1,
			D3DUSAGE_RENDERTARGET,
			D3DFMT_R32F,
			D3DPOOL_DEFAULT,
			&TexCreator.ShadowMapTexture);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// �T�[�t�F�C�X�擾
		TexCreator.ShadowMapTexture->GetSurfaceLevel(0, &ShadowMapSurface);

		// �`��f�o�C�X�ɒ�`����Ă���o�b�t�@�̔\�͂��擾
		IDirect3DSurface9 *pSurf;
		Device->GetDepthStencilSurface(&pSurf);
		D3DSURFACE_DESC Desc;
		pSurf->GetDesc(&Desc);
		pSurf->Release();

		// �Ǝ��[�x�o�b�t�@���쐬
		hr = Device->CreateDepthStencilSurface(
			ShadowMap_Size, ShadowMap_Size,
			Desc.Format,
			Desc.MultiSampleType,
			Desc.MultiSampleQuality,
			FALSE,
			&ShadowMapDepthSurface,
			NULL);
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		// �G�t�F�N�g���̊e��p�����[�^�n���h�����擾
		TexCreator.Para_WVPMatrix = TexCreator.Effect->GetParameterByName(NULL, "WorldViewProj_Light");
		TexCreator.Technique = TexCreator.Effect->GetTechniqueByName("ShadowTexture");

		if (!TexCreator.Para_WVPMatrix || !TexCreator.Technique)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitShadowMapping(void)
{
	// �`��T�[�t�F�C�X�̊J��
	SafeRelease(ShadowMapSurface);
	SafeRelease(ShadowMapDepthSurface);
	SafeRelease(SceneSurface);
	SafeRelease(SceneDepthSurface);
	SafeRelease(Sprite);

	SafeRelease(TexCreator.Effect);				// Z�l�v���b�g�G�t�F�N�g
	SafeRelease(TexCreator.ShadowMapTexture);	// �f�v�X�e�N�X�`��

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateShadowMapping(void)
{

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawShadowTexture(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	STAGE *Stage = GetStage();
	CUBE *Cube = GetCube();
	int GameStage = GetGameStage();

	// Z�l�e�N�X�`���Ƀ��C�g��������`��

	// �`��J�n
	TexCreator_Begin();

	// �v���C���[
	DrawAnimation(Player->Animation, &Player->WorldMatrix, true);

	if (GameStage != Stage_Title)
	{
		// �v���C���[�̌�
		TexCreator.WorldMatrix = Player->Sword->WorldMatrix;
		SetTexCreatorParam();
		for (int i = 0; i < (int)Player->Sword->MaterialNum; i++)
		{
			TexCreator.Effect->BeginPass(0);
			Player->Sword->Mesh->DrawSubset(i);
			TexCreator.Effect->EndPass();
		}

		if (Boss->Exist == true)
		{
			// �{�X
			DrawAnimation(Boss->Animation, &Boss->WorldMatrix, true);

			// �{�X�̌�
			TexCreator.WorldMatrix = Boss->Sword->WorldMatrix;
			SetTexCreatorParam();
			for (int i = 0; i < (int)Boss->Sword->MaterialNum; i++)
			{
				TexCreator.Effect->BeginPass(0);
				Boss->Sword->Mesh->DrawSubset(i);				// ���b�V���`��
				TexCreator.Effect->EndPass();
			}
		}
	}

	if (Cube->Exist == true)
	{
		// ��̗�����
		TexCreator.WorldMatrix = Cube->WorldMatrix;
		SetTexCreatorParam();
		for (int i = 0; i < (int)Cube->MaterialNum; i++)
		{
			TexCreator.Effect->BeginPass(0);
			Cube->Mesh->DrawSubset(i);
			TexCreator.Effect->EndPass();
		}
	}

	// �`��I��
	TexCreator_End();

	// ������������ʂ��g�āA�f�v�X�e�N�X�`����\������
#if 0
	D3DXMATRIX SpriteScaleMat, TransMatrix;
	float Ratio = (float)Screen_Width / 2048.0f;	// ��ʂɑ΂���䗦���v�Z
	D3DXMatrixScaling(&SpriteScaleMat, Ratio / 3, Ratio / 3, 1.0f);
	D3DXMatrixTranslation(&TransMatrix, 0.0f, 400.0f, 0.0f);
	D3DXMatrixMultiply(&SpriteScaleMat, &SpriteScaleMat, &TransMatrix);
	Sprite->SetTransform(&SpriteScaleMat);
	Sprite->Begin(0);
	Sprite->Draw(TexCreator.ShadowMapTexture, NULL, NULL, NULL, WHITE(255));
	Sprite->End();
#endif

	return;
}

//=============================================================================
// �`��J�n�錾
//=============================================================================
void TexCreator_Begin(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �f�o�C�X�������Ă���o�b�t�@���ꎞ�ۑ�
	Device->GetRenderTarget(0, &SceneSurface);
	Device->GetDepthStencilSurface(&SceneDepthSurface);

	// �f�o�C�X��Z�l�e�N�X�`���T�[�t�F�C�X�Ɛ[�x�o�b�t�@��ݒ�
	Device->SetRenderTarget(0, ShadowMapSurface);
	Device->SetDepthStencilSurface(ShadowMapDepthSurface);

	// �e�T�[�t�F�C�X��������
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0);

	// �v���O���}�u���V�F�[�_�̃e�N�j�b�N��ݒ�
	TexCreator.Effect->SetTechnique(TexCreator.Technique);

	// �V�F�[�_�̊J�n��錾
	UINT Tmp;
	TexCreator.Effect->Begin(&Tmp, 0);

	return;
}

//=============================================================================
// �V�F�[�_�[�̃p�����[�^��ݒ肷��
//=============================================================================
void SetTexCreatorParam(void)
{
	D3DXMATRIX WVPMatrix;
	D3DXMATRIX LightViewMatrix = GetLightViewMatrix();
	D3DXMATRIX LightProjMatrix = GetLightProjMatrix();
	WVPMatrix = TexCreator.WorldMatrix * LightViewMatrix * LightProjMatrix;
	TexCreator.Effect->SetMatrix(TexCreator.Para_WVPMatrix, &WVPMatrix);

	return;
}

//=============================================================================
// �`��I���錾
//=============================================================================
void TexCreator_End(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	TexCreator.Effect->End();

	// �f�o�C�X�Ɍ��̃T�[�t�F�C�X��߂�
	Device->SetRenderTarget(0, SceneSurface);
	Device->SetDepthStencilSurface(SceneDepthSurface);

	SceneSurface = NULL;
	SceneDepthSurface = NULL;

	// �Œ�@�\�ɖ߂�
	Device->SetVertexShader(NULL);
	Device->SetPixelShader(NULL);

	// �e�T�[�t�F�C�X��������
	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, WHITE(255), 1.0f, 0);

	return;
}

//=============================================================================
// �f�v�X�e�N�X�`���̃|�C���^���擾����
//=============================================================================
LPDIRECT3DTEXTURE9 *GetShadowMapTexture(void)
{
	return &TexCreator.ShadowMapTexture;
}
