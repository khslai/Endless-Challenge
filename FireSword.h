//=============================================================================
//
// ���̌��w�b�_�[ [FireSword.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _FIRESWORD_H_
#define _FIRESWORD_H_

#include "Effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �G�t�F�N�g�̎��
enum FireSwordEffect
{
	FireSword,
	FireSword_Phase2,
	SlashFire,
	SlashFire_Phase2,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// ���̋O�Ս\����
typedef struct
{
	D3DXVECTOR3			Vtx_LeftUp;			// ���_(����)
	D3DXVECTOR3			Vtx_RightUp;		// ���_(�E��)
	D3DXVECTOR3			Vtx_LeftDown;		// ���_(����)
	D3DXVECTOR3			Vtx_RightDown;		// ���_(�E��)
	D3DXCOLOR			Color;				// �O�Ղ̐F
	bool				IsBack;				// ���ʂ��̃t���O
	bool				Use;				// �g�p�t���O
}SURFACE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitFireSword(bool FirstInit);
// �I������
void UninitFireSword(void);
// �X�V����
void UpdateFireSword(void);
// �`�揈��
void DrawFireSword(void);
// ����Z������ݒu����
int SetFireSword(D3DXVECTOR3 Pos, D3DXVECTOR3 Direction);
// �a���̉���ݒu����
int SetSlashFire(D3DXVECTOR3 Pos, D3DXVECTOR3 PrePos);
// �a���̋O�Ղ�ݒu����
void SetSurface(D3DXVECTOR3 Vtx_LeftUp, D3DXVECTOR3 Vtx_RightUp, D3DXVECTOR3 Vtx_LeftDown, D3DXVECTOR3 Vtx_RightDown, D3DXCOLOR Color);
// �G�t�F�N�g�R���g���[���[���擾����
EFFECTCONTROLLER *GetFireSwordCtrl(void);

#endif
