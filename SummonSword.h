//=============================================================================
//
// ���̏����w�b�_�[ [SummonSword.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _SUMMONSWORD_H_
#define _SUMMONSWORD_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum SummonSwordState
{
	SetSummonEffect,		// ���̏����̃G�t�F�N�g��ݒu����
	SetFireEffect,			// ���̉��̃G�t�F�N�g��ݒu����
	WaitRolling,			// �v���C���[�̉����҂��Ă���
	AttackStart,			// �v���C���[���������u�ԍU���J�n
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3				Pos;				// ���W
	D3DXVECTOR3				Rot;				// ����(��])
	D3DXVECTOR3				Scale;				// �傫��(�X�P�[��)
	LPDIRECT3DTEXTURE9		*Texture;			// �e�N�X�`���ւ̃|�C���^
	LPD3DXMESH				Mesh;				// ���b�V�����ւ̃|�C���^
	LPD3DXBUFFER			MaterialBuffer;		// �}�e���A�����ւ̃|�C���^
	DWORD					MaterialNum;		// �}�e���A�����̐�
	D3DXMATRIX				WorldMatrix;		// ���[���h�}�g���b�N�X
	CAPSULE					HitCapsule;			// �����蔻��p�J�v�Z��
	int						EffectID;			// �G�t�F�N�g�ԍ�
	int						Count;				// ���݃J�E���g
	int						State;				// ���
	bool					Use;				// �g�p�t���O
	bool					BurnedFire;			// ���̉���ݒu�������̃t���O
}SUMMONSWORD;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitSummonSword(bool FirstInit);
// �I������
void UninitSummonSword(void);
// �X�V����
void UpdateSummonSword(void);
// �`�揈��
void DrawSummonSword(void);
// �|�C���^���擾����
SUMMONSWORD *GetSummonSword(void);

#endif
