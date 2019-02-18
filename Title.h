//=============================================================================
//
// �^�C�g���w�b�_�[ [Title.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _TITLE_H_
#define _TITLE_H_


//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �^�C�g���̑I�������
enum TitleSelectState
{
	GameStart,				// �Q�[���X�^�[�g
	Difficulty_Easy,		// ��ՓxEasy
	Difficulty_Normal,		// ��ՓxNormal
	SkipTutorialCheck,		// �`���[�g���A���X�L�b�v�m�F
	NoSkip,					// �X�L�b�v���Ȃ�
	SkipDetermine,			// �X�L�b�v����
	AppealMode,				// �A�s�[�����[�h
	ExitGame,				// �Q�[���I��
	ExitCheck,				// �Q�[���I���m�F
	ExitDetermine,			// �Q�[���I������
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitTitle(bool FirstInit);
// �I������
void UninitTitle(void);
// �X�V����
void UpdateTitle(void);
// �`�揈��
void DrawTitle(void);
// �I�����̏�Ԃ��擾����
SELECT *GetTitleSelect(void);
// �I��������Փx���擾����
int GetDifficulty(void);

#endif