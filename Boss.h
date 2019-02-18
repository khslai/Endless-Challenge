//=============================================================================
//
// �{�X�w�b�_�[ [Boss.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _BOSS_H_
#define _BOSS_H_

#include "D3DXAnimation.h"
#include "Equipment.h"
#include "CapsuleMesh.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �{�X�̃A�j���[�V����ID
enum BossAction
{
	BossAct_Idle,
	BossAct_Walk,
	BossAct_HitReact,
	BossAct_Rolling,
	BossAct_BossDeath,
	BossAct_SlantSlash,
	BossAct_LeftRightSlash,
	BossAct_RollingSlash,
	BossAct_SurfaceSlash,
	BossAct_UpSlash,
	BossAct_HorizonSlash,
	BossAct_VerticalSlash,
	BossAct_Stab,
	BossAct_ComboAttack1,
	BossAct_ComboAttack2,
	BossAct_ComboAttack3,
	BossAct_TwoHandCombo1,
	BossAct_TwoHandCombo2,
	BossAct_PowerUp,
};

enum JumpState
{
	Jump_Start,			// �W�����v�J�n
	StartFalling,		// �����J�n
	Jump_Stop,			// �W�����v�I��
};

// �{�X�̒i�K
enum Phase
{
	Phase1,				// ���i�K
	TurnToPhase2,		// ���i�K�ɕϐg
	Phase2,				// ���i�K
	SetDeathEffect,		// ���S�G�t�F�N�g��ݒu����
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXMATRIX			WorldMatrix;	// ���[���h�}�g���b�N�X
	D3DXVECTOR3			Pos;			// ���W
	D3DXVECTOR3			PrePos;			// 1�t���C���O�̍��W
	D3DXVECTOR3			CenterPos;		// ���S���W
	D3DXVECTOR3			Move;			// �ړ���
	D3DXVECTOR3			Rot;			// ���f���̌���(��])
	D3DXVECTOR3			Scale;			// ���f���̑傫��(�X�P�[��)
	D3DXANIMATION		*Animation;		// ���f���A�j���[�V����
	SWORD				*Sword;			// ��
	CAPSULE				HitCapsule;		// �����蔻��p�J�v�Z��
	float				DestAngle;		// �ڕW�p�x
	float				HP;				// �̗�
	float				HP_Max;			// �ő�̗�
	float				PreviousHP;		// �󂯂��_���[�W
	float				ActionSpeed;	// �A�N�V�����̑��x
	int					JumpState;		// ���Ԃ̏��
	int					Phase;			// �{�X�̒i�K
	int					DistanceState;	// �v���C���[�Ƃ̋������
	bool				Exist;			// ���݃t���O
	bool				SetFireSword;	// ���̌��̃G�t�F�N�g
	bool				HPDecreaseStart;// �_���[�W�Q�[�W������
	bool				GiveDamage;		// �v���C���[�Ƀ_���[�W��^������
	bool				TurnRotation;	// �U�����̕����C��
}BOSS;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitBoss(bool FirstInit);
// �I������
void UninitBoss(void);
// �X�V����
void UpdateBoss(void);
// �`�揈��
void DrawBoss(void);
// �{�X�̃|�C���^���擾����
BOSS *GetBoss(void);

#endif
