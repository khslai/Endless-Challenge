//=============================================================================
//
// �`���[�g���A���w�b�_�[ [Tutorial.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum TutorialPhase
{
	Tutorial_Attack,			// �U��
	Tutorial_SetSword,			// ���̏���
	Tutorial_WaitRolling,		// �v���C���[�̉����҂��Ă���
	Tutorial_RollingClear,		// ��𐬌�
	Tutorial_HPRestore,			// HP��
	Tutorial_CameraReset,		// �J�������Z�b�g
	Tutorial_CameraLockOn,		// ���b�N�I��
	HelpOver,					// �w���v�I���A�����̂���
	TutorialPause,				// �ꎞ��~
	DisplayHelp,				// ���������\������
	TutorialClear,				// �`���[�g���A���N���A
};

enum TutorialSelectState
{
	Tutorial_Resume,
	Tutorial_ToTitle,
	Tutorial_ToTitleCheck,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3			Pos;				// ���f���̈ʒu
	D3DXVECTOR3			Rot;				// ���f���̌���(��])
	D3DXVECTOR3			Scale;				// ���f���̑傫��(�X�P�[��)
	D3DXMATRIX			WorldMatrix;		// ���[���h�}�g���b�N�X
	LPDIRECT3DTEXTURE9	*Texture;			// �e�N�X�`���ւ̃|�C���^
	LPD3DXMESH			Mesh;				// ���b�V�����ւ̃|�C���^
	LPD3DXBUFFER		MaterialBuffer;		// �}�e���A�����ւ̃|�C���^
	DWORD				MaterialNum;		// �}�e���A�����̐�
	CAPSULE				HitCapsule;			// �����蔻��p�J�v�Z��
	float				Durability;			// �ϋv�� 
	float				MaxDurability;		// �ő�ϋv�� 
	bool				Exist;				// ���݃t���O
	bool				EffectOver;			// �G�t�F�N�g�I���t���O
}CUBE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitTutorial(bool FirstInit);
// �I������
void UninitTutorial(void);
// �X�V����
void UpdateTutorial(void);
// �`�揈��
void DrawTutorial(void);
// �`���[�g���A���̏�Ԃ�ݒu����
void SetTutorialState(int State);
// �`���[�g���A���̏�Ԃ��擾����
int GetTutorialState(void);
// �����̂̃|�C���^���擾����
CUBE *GetCube(void);
// �I�����̏�Ԃ��擾����
SELECT *GetTutorialSelect(void);

#endif
