//=============================================================================
//
// ���C�g���� [Light.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Light.h"
#include "DebugProcess.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
D3DXMATRIX LightView, LightProj;	// ���C�g�r���[�ϊ��E�ˉe�ϊ�
D3DXVECTOR3 Pos = PositionZero;		// ���C�g���W
float Distance = 0.0f;				// ���W�ƒ����_�̋���
float Angle = 0.0f;					// ���C�g�̊p�x

//=============================================================================
// ���C�g�̏���������
//=============================================================================
HRESULT InitLight(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXVECTOR3 NormalizePos;
	int GameStage = GetGameStage();

	// �ʒu
	if (GameStage == Stage_Title)
	{
		Pos = D3DXVECTOR3(-560.0f, 480.0f, 560.0f);
	}
	else
	{
		Pos = D3DXVECTOR3(-1250.0f, 800.0f, 1250.0f);
	}

	// �p�x
	D3DXVec3Normalize(&NormalizePos, &Pos);
	Angle = acosf(D3DXVec3Dot(&NormalizePos, &D3DXVECTOR3(1.0f, 0.0f, 0.0f)));

	// ����
	Distance = D3DXVec3Length(&D3DXVECTOR3(Pos - PositionZero));

	// �r���[�s��
	D3DXMatrixLookAtLH(&LightView, &Pos, &PositionZero, &UpVector);

	// �ˉe�ϊ��s��
	D3DXMatrixPerspectiveFovLH(&LightProj, D3DXToRadian(90.0f), 1.0f, 10.0f, 10000.0f);

	return S_OK;
}

//=============================================================================
// ���C�g�̍X�V����
//=============================================================================
void UpdateLight(void)
{
	D3DXVECTOR3 Move = PositionZero;

	// ���C�g�̉�]
	Angle -= 0.0001f;
	if (Angle >= D3DX_2PI)
	{
		Angle -= D3DX_2PI;
	}
	Pos.x = cosf(Angle) * Distance;
	Pos.z = sinf(Angle) * Distance;

	D3DXMatrixLookAtLH(&LightView, &Pos, &PositionZero, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

#if _DEBUG
	PrintDebugMsg("LightPos : %f , %f , %f\n", Pos.x, Pos.y, Pos.z);
#endif

	return;
}

//=============================================================================
// ���C�g�̃r���[�s����擾����
//=============================================================================
D3DMATRIX GetLightViewMatrix(void)
{
	return LightView;
}

//=============================================================================
// ���C�g�̎ˉe�ϊ��s����擾����
//=============================================================================
D3DMATRIX GetLightProjMatrix(void)
{
	return LightProj;
}

//=============================================================================
// ���C�g�̍��W���擾����
//=============================================================================
D3DXVECTOR3 GetLightPos(void)
{
	return Pos;
}
