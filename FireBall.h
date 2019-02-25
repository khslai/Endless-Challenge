//=============================================================================
//
// �t�@�C�A�{�[���w�b�_�[ [FireBall.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _FIREBALL_H_
#define _FIREBALL_H_

#include "Effect.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define BulletMax			(10)		// �t�@�C�A�{�[���̍ő吔
#define ExplodeRange		(80.0f)		// �����͈̔�
#define BurnningRange		(20.0f)		// �R�Ē��̔����͈�

enum BulletState
{
	Standby,			// �ҋ@
	Shot,				// �e����
	SetBurnningEffect,	// ����
	BurnningExplode,	// ���܂�锚��
	WaitDisappear,		// ������܂ő҂�
	DisappearExplode,	// �����锚��
	WaitExplodeOver,	// �������I���
	OverStageRange,		// �X�e�[�W�͈͂𒴂���
	BulletInit = 99,
};

enum FireBallEffect
{
	BulletFire,			// �t�@�C�A�{�[��
	Burnning,			// �����G�t�F�N�g
	Explode,			// �����̔���
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �t�@�C�A�{�[���\����
typedef struct
{
	D3DXVECTOR3		Pos;				// ���W
	D3DXVECTOR3		PrePos;				// 1�t���C���O�̍��W
	D3DXVECTOR3		BezierPoint[2];		// �x�W�F�Ȑ��v�Z�p���W
	D3DXVECTOR3		StartPos;			// �x�W�F�Ȑ��������W
	D3DXVECTOR3		DestPos;			// �^�[�Q�b�g���W
	D3DXVECTOR3		Direction;			// �����x�N�g��
	float			Radius;				// ���a
	float			Speed;				// �ړ����x
	int				EffectID;			// �G�t�F�N�gID
	int				ShotTime;			// ���ˎ���
	int				Count;				// ���݃J�E���g
	int				State;				// ���
	int				SphereID;			// ������͈͕\���ԍ�
	bool			Use;				// �g�p�t���O
}BULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitFireBall(bool FirstInit);
// �I������
void UninitFireBall(void);
// �X�V����
void UpdateFireBall(void);
// �`�揈��
void DrawFireBall(void);
// �t�@�C�A�{�[����ݒu����
void SetFireBall(D3DXVECTOR3 Pos);
// �t�@�C�A�{�[���̃|�C���^���擾����
BULLET *GetBullet(int Bullet_No);

#endif
