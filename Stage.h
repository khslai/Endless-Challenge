//=============================================================================
//
// �X�e�[�W�w�b�_�[ [Stage.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _STAGE_H_
#define _STAGE_H_

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �X�e�[�W�\����
typedef struct
{
	D3DXVECTOR3			Pos;					// ���f���̈ʒu
	D3DXVECTOR3			Rot;					// ���f���̌���(��])
	D3DXVECTOR3			Scale;					// ���f���̑傫��(�X�P�[��)
	D3DXMATRIX			WorldMatrix;			// ���[���h�}�g���b�N�X
	LPDIRECT3DTEXTURE9	*Texture;				// �e�N�X�`���ւ̃|�C���^
	LPD3DXMESH			Mesh;					// ���b�V�����ւ̃|�C���^
	LPD3DXBUFFER		MaterialBuffer;			// �}�e���A�����ւ̃|�C���^
	DWORD				MaterialNum;			// �}�e���A�����̐�
}STAGE;

// �f�v�X�o�b�t�@�\����
typedef struct
{
	LPDIRECT3DTEXTURE9	*ShadowMapTexture;		// �V���h�E�}�b�v�e�N�X�`��
	LPD3DXEFFECT		Effect;					// �[�x�o�b�t�@�V���h�E�G�t�F�N�g
	D3DXHANDLE			Para_WorldMatrix;		// ���[���h�ϊ��s��n���h��
	D3DXHANDLE			Para_WVPMatrix;			// World x View x Proj�s��n���h��
	D3DXHANDLE			Para_WVPMatrix_Light;	// ���C�g��World x View x Proj�s��n���h��
	D3DXHANDLE			Para_CosTheta;			// �V���h�E�p�x�n���h��
	D3DXHANDLE			Para_ShadowMapTexture;	// �V���h�E�}�b�v�e�N�X�`���n���h��
	D3DXHANDLE			Technique;				// �e�N�j�b�N�ւ̃n���h��
}DEPTHBUFFER;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitStage(bool FirstInit);
// �I������
void UninitStage(void);
// �X�V����
void UpdateStage(void);
// �`�揈��
void DrawStage(void);
// �X�e�[�W�̃|�C���^���擾����
STAGE *GetStage(void);

#endif
