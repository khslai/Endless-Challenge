//=============================================================================
//
// �v���C���[�w�b�_�[ [Player.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "D3DXAnimation.h"
#include "Equipment.h"
#include "CapsuleMesh.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �v���C���[�̃A�j���[�V����ID
enum PlayerAction
{
	Sit,
	Idle,
	Running,
	Walk_Left,
	Walk_Right,
	Walk_Back,
	Rolling,
	FallingBack,
	FlyingBack,
	FallToStand,
	HitReact,
	HPRestore,
	StandUp,
	PlayerDeath,
	Attack1,
	Attack2,
	SitPose1,
	SitPose2,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �v���C���[�\����
typedef struct
{
	D3DXMATRIX			WorldMatrix;		// ���[���h�}�g���b�N�X
	D3DXVECTOR3			Pos;				// ���W
	D3DXVECTOR3			PrePos;				// 1�t���C���O�̍��W
	D3DXVECTOR3			CenterPos;			// �v���C���[�̒��S���W
	D3DXVECTOR3			NextPos;			// ���̍��W
	D3DXVECTOR3			Move;				// �ړ���
	D3DXVECTOR3			Rot;				// ��]
	D3DXVECTOR3			Scale;				// �傫��(�X�P�[��)
	D3DXVECTOR3			FlyingBackDir;		// �����ɂ���Ĕ�ԕ���
	D3DXVECTOR3			Direction;			// ����
	D3DXANIMATION		*Animation;			// �A�j���[�V����
	SWORD				*Sword;				// ��
	CAPSULE				HitCapsule;			// �����蔻��p�J�v�Z��
	float				DestAngle;			// �ڕW�p�x
	float				NextDestAngle;		// ���̃A�j���[�V�����̖ڕW�p�x
	float				PreviousHP;			// �O�̗̑�
	float				HP;					// ���ݑ̗�
	float				HP_Max;				// �ő�̗�
	float				RestoreHP;			// �񕜌�̗̑�
	float				PreviousST;			// �O�̃X�^�~�i
	float				Stamina;			// ���݃X�^�~�i
	float				Stamina_Max;		// �ő�X�^�~�i
	float				ActionSpeed;		// �A�N�V�����̑��x
	float				Damage;				// �_���[�W
	float				DifficultyRate;		// ��Փx�W��
	int					HPPotionNum;		// HP�|�[�V�����̐�
	bool				InHPRestore;		// HP�񕜒��t���O
	bool				GiveDamage;			// �G�Ƀ_���[�W��^������
	bool				BeDamaged;			// �G�ɍU�����ꂽ
	bool				Invincible;			// ���[�����O���G
	bool				HPDecreaseStart;	// HP�����J�n
	bool				STDecreaseStart;	// �X�^�~�i�����J�n
	bool				MoveBack;			// ��ރt���O
}PLAYER;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitPlayer(bool FirstInit);
// �I������
void UninitPlayer(void);
// �X�V����
void UpdatePlayer(void);
// �`�揈��
void DrawPlayer(void);
// �v���C���[�̃|�C���^���擾����
PLAYER *GetPlayer(void);

#endif
