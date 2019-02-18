//=============================================================================
//
// �G�t�F�N�g�w�b�_�[ [Effect.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum EffectType
{
	HPHeal,					// HP��
	HitBlood,				// �{�X�𖽒�����Ƃ��̏o��
	HitFire,				// �{�X�ɖ�������鉊
	HitFire_Phase2,			
	HitLight,				// �`���[�u�𖽒������
	SummonSwordEffect,		// ������������
	ExplodeFire,			// �{�X�̍U������
	ExplodeFire_Phase2,
	FireWall,				// �t�@�C���E�H�[��
	FireWall_Phase2,
	DashSmoke,				// �_�b�V���̉�
	FirePillar,				// ���̒�
	PowerUp,				// �{�X�p���[�A�b�v
	BossDebut,				// �{�X�o��
	BossDeath,				// �{�X���S
	MagicCircle,			// �`���[�u�𕂂��Ԗ��@�w
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	EffekseerRendererDX9::Renderer		*Render;		// �G�t�F�N�g�����_
	Effekseer::Manager					*Manager;		// �G�t�F�N�g�}�l�[�W���[
	Effekseer::Effect					**Effect;		// �G�t�F�N�g�|�C���^
	int									EffectNum;		// �G�t�F�N�g�̐�
}EFFECTCONTROLLER;

typedef struct
{
	D3DXVECTOR3							Pos;			// ���W
	int									ID;				// �G�t�F�N�g�ԍ�
	int									EffectType;		// �G�t�F�N�g���
	bool								Use;			// �g�p�t���O
}EFFECT;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitEffect(bool FirstInit);
// �I������
void UninitEffect(void);
// �X�V����
void UpdateEffect(void);
// �`�揈��
void DrawEffect(void);
// �G�t�F�N�g��ݒu����
int SetEffect(D3DXVECTOR3 Pos, int EffectType);
// �{�X�͓�i�K�ɓ��鎞�̃G�t�F�N�g
void TurnPhaseEffect(void);
// �G�t�F�N�g��ǂݍ���
HRESULT SafeLoadEffect(Effekseer::Manager *Manager, Effekseer::Effect **Effect, const EFK_CHAR* SrcPath, const char* ErrorSrc);
// �G�t�F�N�g�R���g���[���[���擾����
EFFECTCONTROLLER *GetEffectCtrl(void);

#endif
