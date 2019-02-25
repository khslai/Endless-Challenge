//=============================================================================
//
// �V���h�E�}�b�s���O�w�b�_�[ [ShadowMapping.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _SHADOWMAPPING_H_
#define _SHADOWMAPPING_H_

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �f�v�X�e�N�X�`���\����
typedef struct
{
	LPDIRECT3DTEXTURE9	ShadowMapTexture;		// �V���h�E�}�b�v�e�N�X�`��
	LPD3DXEFFECT		Effect;					// Z�l�v���b�g�G�t�F�N�g
	D3DXMATRIX			WorldMatrix;			// ���[���h�ϊ��s��
	D3DXHANDLE			Para_WVPMatrix;			// ���[���h x �r���[ x �ˉe�ϊ��s��n���h��
	D3DXHANDLE			Technique;				// �e�N�j�b�N�ւ̃n���h��
}TEXCREATOR;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitShadowMapping(bool FirstInit);
// �I������
void UninitShadowMapping(void);
// �X�V����
void UpdateShadowMapping(void);
// �`�揈��
void DrawShadowTexture(void);
// �f�v�X�e�N�X�`���̃|�C���^���擾����
LPDIRECT3DTEXTURE9 *GetShadowMapTexture(void);

#endif
