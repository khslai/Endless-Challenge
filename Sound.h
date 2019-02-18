//=============================================================================
//
// �T�E���h�w�b�_�[ [Sound.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �T�E���h�̎��
enum SoundType
{
	BGM,			// BGM
	NormalSE,		// 2D��SE
	SE3D,			// 3D��SE
};

// 2D�T�E���h�ԍ�
enum Sound_No
{
	BGM_Battle_Phase1_Intro,
	BGM_Battle_Phase1_Loop,
	BGM_Battle_Phase2_Intro,
	BGM_Battle_Phase2_Loop,
	BGM_Title,
	BGM_GameOver,
	BGM_Tutorial_Intro,
	BGM_Tutorial_Loop,
	SE_Menu,
	SE_Determine_Yes,
	SE_Determine_No,
	SE_SelectMove,
	SE_ShowTutorial,
	Sound_Max,
};

// 3D�T�E���h�ԍ�
enum Sound3D_No
{
	SE3D_PlayerFootStep,
	SE3D_Rolling,
	SE3D_Slash,
	SE3D_Slash2,
	SE3D_HPRestore,
	SE3D_BossFootStep,
	SE3D_BossSlash,
	SE3D_SlashFire,
	SE3D_ExplodeFire,
	SE3D_BossDeathWind,
	SE3D_PowerUp,
	SE3D_ExplosionWind,
	SE3D_SetFireBall,
	SE3D_Burnning,
	SE3D_FireBallExplode,
	SE3D_HitCube,
	SE3D_DestroyCube,
	SE3D_HitHuman,
	SE3D_HitFire,
	SE3D_FallingDown,
	SE3D_FallingDown2,
	Sound3D_Max,
};

// �Đ��p�t���O
enum
{
	E_DS8_FLAG_NONE,	// ��񂵂��v���C���Ȃ�
	E_DS8_FLAG_LOOP,	// ���[�v
	E_DS8_FLAG_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT	InitSound(HWND hWnd);
// �ď���������
void ReInitSound(void);
// �I������
void UninitSound(void);
// �X�V����
void UpdateSound(void);
// BGM�A2D�T�E���h��ݒu����
void SetSound(int SoundType, int Sound_No, int Flag, bool FromStart);
// 3D�T�E���h��ݒu����
void Set3DSound(D3DXVECTOR3 Pos, int Sound_No, int Flag, bool FromStart);
// �T�E���h���~����
void StopSound(int SoundType, int Sound_No);
// ���炵�Ă���BGM�̔ԍ����擾����
int GetPlayingBGM_No(void);
// �T�E���h�̃t�F�C�h�A�E�g
void SoundFadeOut(int Sound_No);

#endif