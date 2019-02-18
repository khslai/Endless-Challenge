//=============================================================================
//
// �X�e�[�W�J�ڏ��� [Transition.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Transition.h"
#include "D3DXAnimation.h"
#include "Player.h"
#include "Boss.h"
#include "Tutorial.h"
#include "Pause.h"
#include "GameOver.h"
#include "Title.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FadeRate		(3)		// �t�F�[�h�W��

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���_���̐ݒ�
HRESULT MakeVertexTransition(void);
// �F��ݒ�
void SetColor(int Alpha);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 TransitionVtxBuffer = NULL;		// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
VERTEX_2D				VertexWk[VERTEX_NUM];			// ���_���i�[���[�N
int						Transition = TransitionOver;	// �X�e�[�W�J�ڏ��

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTransition(bool FirstInit)
{
	// ���߂ď�����
	if (FirstInit == true)
	{
		// ���_���̐ݒ�
		if (FAILED(MakeVertexTransition()))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTransition(void)
{
	// ���_�o�b�t�@�̊J��
	SafeRelease(TransitionVtxBuffer);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTransition(void)
{
	int GameStage = GetGameStage();
	int TutorialState = GetTutorialState();
	int PlayingBGM_No = GetPlayingBGM_No();
	static int Alpha = 0;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	SELECT *TitleSelect = GetTitleSelect();
	SELECT *TutoiralSelect = GetTutorialSelect();
	SELECT *PauseSelect = GetPauseSelect();
	SELECT *GameOverSelect = GetGameOverSelect();

	if (Transition != TransitionOver)
	{
		// �t�F�[�h������
		if (Transition == Fadeout)
		{
			// �t�F�[�h�A�E�g����
			Alpha -= FadeRate;		// ���l�����Z���ĉ�ʂ������Ă���

			if (Alpha <= 0)
			{
				// �t�F�[�h�����I��
				Alpha = 0;
				SetTransition(TransitionOver);
			}
		}
		else if (Transition == Fadein)
		{
			// �t�F�[�h�C������
			Alpha += FadeRate;		// ���l�����Z���ĉ�ʂ𕂂��オ�点��

			// �T�E���h�t�F�C�h�A�E�g
			SoundFadeOut(PlayingBGM_No);

			if (Alpha >= 255)
			{
				Alpha = 255;

				// �A�j���[�V�����X�V
				UpdateAnimation(Player->Animation, TimePerFrame);
				UpdateAnimation(Boss->Animation, TimePerFrame);

				// �t�F�[�h�C�������ɐ؂�ւ�
				switch (GameStage)
				{
				case Stage_Title:

					ChangeAnimation(Player->Animation, Idle, 1.0f, false);
					ChangeAnimation(Boss->Animation, Idle, 1.0f, false);
					if (Player->Animation->MotionBlendOver == true &&
						Boss->Animation->MotionBlendOver == true)
					{
						SetTransition(Fadeout);
						// �`���[�g���A���J�n
						if (TitleSelect->Phase == NoSkip)
						{
							SetGameStage(Stage_Tutorial);
						}
						// �Q�[���J�n
						else if (TitleSelect->Phase == SkipDetermine || TitleSelect->Phase == AppealMode)
						{
							SetGameStage(Stage_Game);
						}
						ReInitialize();
					}
					break;

				case Stage_Tutorial:

					// �Q�[���J�n
					if (TutorialState == TutorialClear)
					{
						ChangeAnimation(Player->Animation, Idle, 1.0f, false);
						ChangeAnimation(Boss->Animation, Idle, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true && Boss->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Game);
							ReInitialize();
						}
					}
					// �^�C�g���ɖ߂�
					if (TutoiralSelect->Phase == Tutorial_ToTitleCheck && TutoiralSelect->InYes == true)
					{
						ChangeAnimation(Player->Animation, Sit, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Title);
							ReInitialize();
						}
					}
					break;

				case Stage_Game:

					// �^�C�g���ɖ߂�
					ChangeAnimation(Player->Animation, Sit, 1.0f, false);
					if (Player->Animation->MotionBlendOver == true)
					{
						SetTransition(Fadeout);
						SetGameStage(Stage_Title);
						ReInitialize();
					}
					break;

				case Stage_Pause:

					// �ŏ�����
					if (PauseSelect->Phase == Pause_RestartCheck && PauseSelect->InYes == true)
					{
						ChangeAnimation(Player->Animation, Idle, 1.0f, false);
						ChangeAnimation(Boss->Animation, Idle, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true && Boss->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Game);
							ReInitialize();
						}
					}
					// �^�C�g���ɖ߂�
					else if (PauseSelect->Phase == Pause_ToTitleCheck && PauseSelect->InYes == true)
					{
						ChangeAnimation(Player->Animation, Sit, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Title);
							ReInitialize();
						}
					}
					break;

				case Stage_GameOver:

					// �ŏ�����
					if (GameOverSelect->Phase == GameOver_RestartCheck && GameOverSelect->InYes == true)
					{
						ChangeAnimation(Player->Animation, Idle, 1.0f, false);
						ChangeAnimation(Boss->Animation, Idle, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true && Boss->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Game);
							ReInitialize();
						}
					}
					// �^�C�g���ɖ߂�
					else if (GameOverSelect->Phase == GameOver_ToTitleCheck && GameOverSelect->InYes == true)
					{
						ChangeAnimation(Player->Animation, Sit, 1.0f, false);
						if (Player->Animation->MotionBlendOver == true)
						{
							SetTransition(Fadeout);
							SetGameStage(Stage_Title);
							ReInitialize();
						}
					}
					break;

				default:
					break;
				}
			}
		}

		// �F��ݒ�
		SetColor(Alpha);
	}

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTransition(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
	Device->SetStreamSource(0, TransitionVtxBuffer, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	Device->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	Device->SetTexture(0, NULL);

	// �|���S���̕`��
	Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, POLYGON_NUM);

	return;
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexTransition(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(Device->CreateVertexBuffer(sizeof(VERTEX_2D) * VERTEX_NUM,		// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,				// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_2D,					// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,				// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&TransitionVtxBuffer,				// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))							// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_2D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		TransitionVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

		// ���_���W�̐ݒ�
		pVtx[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

		// �e�N�X�`���̃p�[�X�y�N�e�B�u�R���N�g�p
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// ���ˌ��̐ݒ�
		pVtx[0].diffuse = WHITE(0);
		pVtx[1].diffuse = WHITE(0);
		pVtx[2].diffuse = WHITE(0);
		pVtx[3].diffuse = WHITE(0);

		// �e�N�X�`�����W�̐ݒ�
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// ���_�f�[�^���A�����b�N����
		TransitionVtxBuffer->Unlock();
	}

	return S_OK;
}

//=============================================================================
// �F��ݒ�
//=============================================================================
void SetColor(int Alpha)
{
	VERTEX_2D *pVtx;

	// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
	TransitionVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = WHITE(Alpha);
	pVtx[1].diffuse = WHITE(Alpha);
	pVtx[2].diffuse = WHITE(Alpha);
	pVtx[3].diffuse = WHITE(Alpha);

	// ���_�f�[�^���A�����b�N����
	TransitionVtxBuffer->Unlock();

}

//=============================================================================
// �t�F�[�h�̏�Ԑݒ�
//=============================================================================
void SetTransition(int State)
{
	Transition = State;
}

//=============================================================================
// �t�F�[�h�̏�Ԏ擾
//=============================================================================
int GetTransition(void)
{
	return Transition;
}

