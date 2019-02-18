//=============================================================================
//
// �X�J�C�{�b�N�X�w�b�_�[ [SkyBox.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _SKYBOX_H_
#define _SKYBOX_H_

typedef struct
{
	D3DXVECTOR3				Pos;			// �ʒu
	D3DXVECTOR3				Rot;			// ����(��])
	D3DXVECTOR3				Scale;			// �傫��(�X�P�[��)
}SKYBOX;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitSkyBox(bool FirstInit);
// �I������
void UninitSkyBox(void);
// �X�V����
void UpdateSkyBox(void);
// �`�揈��
void DrawSkyBox(void);

#endif
