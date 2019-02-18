//=============================================================================
//
// �Q�[���I�[�o�[�w�b�_�[ [GameOver.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _GAMEOVER_H_
#define _GAMEOVER_H_


//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �Q�[���I�[�o�[�I�������
enum GameOverSelectState
{
	BGMStart,					// BGM�炷�̂��n�܂�
	Display,					// GameOver�����\����
	GameOver_Restart,			// �ŏ�����
	GameOver_RestartCheck,		// �ŏ�����̊m�F
	GameOver_ToTitle,			// �^�C�g����
	GameOver_ToTitleCheck,		// �^�C�g���ւ̊m�F
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitGameOver(bool FirstInit);
// �I������
void UninitGameOver(void);
// �X�V����
void UpdateGameOver(void);
// �`�揈��
void DrawGameOver(void);
// �I�����̏�Ԃ��擾����
SELECT *GetGameOverSelect(void);

#endif
