//=============================================================================
//
// �Q�[���I�[�o�[���� [GameOver.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "GameOver.h"
#include "Player.h"
#include "Boss.h"
#include "Input.h"
#include "Transition.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �����\���̋�`�T�C�Y
#define RectWidth					(500)
#define RectHeight					(150)

// �����̍��W
#define OptionPos_X					(710)
#define GameOverPos_Y				(256)
#define GameOver_RestartPos_Y		(540)
#define GameOver_ToTitlePos_Y		(690)
#define AskSentencePos_Y			(200)
#define YesNoPos_Y					(700)
#define YesPos_X					(480)
#define NoPos_X						(1440)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���_���̍쐬
HRESULT MakeGameOverVertex(void);
// �e�N�X�`�����_���W�̐ݒ�	
void SetGameOverTexture(void);
// ���_���W�̐ݒ�
void SetGameOverVertex(void);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
// ���_���i�[���[�N
static VERTEX_2D BlackScreenVertexWk[VERTEX_NUM];
// "GAME OVER"�̃A���t�@�l
static int Alpha = 0;
// �Q�[���I�[�o�[�I����
static SELECT GameOverSelect;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitGameOver(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	GameOverSelect.Pos = D3DXVECTOR2(Screen_Center_X, GameOver_RestartPos_Y);
	GameOverSelect.PrePos = GameOverSelect.Pos;
	GameOverSelect.Phase = BGMStart;
	GameOverSelect.InYes = false;

	Alpha = 0;

	if (FirstInit == true)
	{
		// ���_���̍쐬
		MakeGameOverVertex();
	}
	else
	{
		// ���_���W�X�V
		SetGameOverVertex();
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGameOver(void)
{

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGameOver(void)
{
	static int PressCount = 0;
	RECT SelectBox = {
		(LONG)GameOverSelect.VertexWk[0].vtx.x, (LONG)GameOverSelect.VertexWk[0].vtx.y,
		(LONG)GameOverSelect.VertexWk[3].vtx.x, (LONG)GameOverSelect.VertexWk[3].vtx.y };

	// Game Over�̕������\��������Ȃ����
	if (GameOverSelect.Phase != Display && GameOverSelect.Phase != BGMStart)
	{
		// �I�����ړ����ʉ�
		if (GameOverSelect.Pos.x != GameOverSelect.PrePos.x || GameOverSelect.Pos.y != GameOverSelect.PrePos.y)
		{
			SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
		}
		GameOverSelect.PrePos = GameOverSelect.Pos;

		// Enter�L�[
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_NUMPADENTER) ||
			IsButtonTriggered(0, BUTTON_B) || (IsMouseLeftTriggered() && CheckMousePos(SelectBox)))
		{
			switch (GameOverSelect.Phase)
			{
			case GameOver_Restart:
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				GameOverSelect.Phase = GameOver_RestartCheck;
				GameOverSelect.Pos.x = NoPos_X;
				GameOverSelect.Pos.y = YesNoPos_Y;
				break;
			case GameOver_RestartCheck:
				if (GameOverSelect.InYes == true)
				{
					SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
					SetTransition(Fadein);
				}
				else
				{
					SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
					GameOverSelect.Phase = GameOver_Restart;
					GameOverSelect.Pos.x = Screen_Center_X;
					GameOverSelect.Pos.y = GameOver_RestartPos_Y;
				}
				break;
			case GameOver_ToTitle:
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				GameOverSelect.Phase = GameOver_ToTitleCheck;
				GameOverSelect.Pos.x = NoPos_X;
				GameOverSelect.Pos.y = YesNoPos_Y;
				break;
			case GameOver_ToTitleCheck:
				if (GameOverSelect.InYes == true)
				{
					SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
					SetTransition(Fadein);
				}
				else
				{
					SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
					GameOverSelect.Phase = GameOver_ToTitle;
					GameOverSelect.Pos.x = Screen_Center_X;
					GameOverSelect.Pos.y = GameOver_ToTitlePos_Y;
				}
				break;
			default:
				break;
			}

			// �I�����ړ����ʉ��𗬂��Ȃ�����
			GameOverSelect.PrePos = GameOverSelect.Pos;
		}

		// �I�����ړ�
		if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP) ||
			GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
		{
			switch (GameOverSelect.Phase)
			{
			case GameOver_Restart:
				GameOverSelect.Phase = GameOver_ToTitle;
				GameOverSelect.Pos.y = GameOver_ToTitlePos_Y;
				break;
			case GameOver_ToTitle:
				GameOverSelect.Phase = GameOver_Restart;
				GameOverSelect.Pos.y = GameOver_RestartPos_Y;
				break;
			default:
				break;
			}
		}

		// �I�������[�v
		if (GetKeyboardRepeat(DIK_UP) || GetKeyboardRepeat(DIK_W) || IsButtonRepeat(0, BUTTON_UP) ||
			GetKeyboardRepeat(DIK_DOWN) || GetKeyboardRepeat(DIK_S) || IsButtonRepeat(0, BUTTON_DOWN))
		{
			PressCount++;
			if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
			{
				if (GameOverSelect.Phase == GameOver_Restart)
				{
					GameOverSelect.Phase = GameOver_ToTitle;
					GameOverSelect.Pos.y = GameOver_ToTitlePos_Y;
				}
				else if (GameOverSelect.Phase == GameOver_ToTitle)
				{
					GameOverSelect.Phase = GameOver_Restart;
					GameOverSelect.Pos.y = GameOver_RestartPos_Y;
				}
			}
		}

		// �v���X�J�E���g������
		if (GetKeyboardRelease(DIK_UP) || GetKeyboardRelease(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
			GetKeyboardRelease(DIK_DOWN) || GetKeyboardRelease(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
		{
			PressCount = 0;
		}

		// �m�F���
		if (GameOverSelect.Phase == GameOver_RestartCheck || GameOverSelect.Phase == GameOver_ToTitleCheck)
		{
			// �O�̏�Ԃɖ߂�
			if (IsMouseRightTriggered() || IsButtonTriggered(0, BUTTON_A))
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);

				GameOverSelect.Pos.x = Screen_Center_X;
				if (GameOverSelect.Phase == GameOver_RestartCheck)
				{
					GameOverSelect.Pos.y = GameOver_RestartPos_Y;
					GameOverSelect.Phase = GameOver_Restart;
				}
				else if (GameOverSelect.Phase == GameOver_ToTitleCheck)
				{
					GameOverSelect.Pos.y = GameOver_ToTitlePos_Y;
					GameOverSelect.Phase = GameOver_ToTitle;
				}
				GameOverSelect.InYes = false;
				GameOverSelect.PrePos = GameOverSelect.Pos;
			}

			if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_LEFT))
			{
				GameOverSelect.Pos.x = YesPos_X;
				GameOverSelect.InYes = true;
			}
			else if (GetKeyboardTrigger(DIK_RIGHT) || GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_RIGHT))
			{
				GameOverSelect.Pos.x = NoPos_X;
				GameOverSelect.InYes = false;
			}
		}

		// ���_���W�X�V
		SetGameOverVertex();
	}
	// BGM��炷
	else if (GameOverSelect.Phase == BGMStart)
	{
		ReInitSound();
		GameOverSelect.Phase = Display;
	}

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGameOver(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 BlackScreenTexture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 GameOverSelectTexture = GetUITexture(SelectBox);
	LPD3DXFONT Font_108 = GetFont(FontSize_108);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	RECT TextRect = { 0, 0, Screen_Width, RectHeight };

	// ���_�t�H�[�}�b�g�̐ݒ�
	Device->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	Device->SetTexture(0, BlackScreenTexture);

	// �|���S���̕`��
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BlackScreenVertexWk, sizeof(VERTEX_2D));

	TextRect.top = GameOverPos_Y;
	TextRect.bottom = GameOverPos_Y + Texture_SelectBox_Height;
	if (GameOverSelect.Phase == Display)
	{
		Alpha += 2;
		if (Alpha >= 255)
		{
			Alpha = 255;
			GameOverSelect.Phase = GameOver_Restart;
		}
		Font_108->DrawText(NULL, "GAME OVER", -1, &TextRect, DT_CENTER | DT_VCENTER, RED(Alpha));
	}
	else if (GameOverSelect.Phase != GameOver_RestartCheck && GameOverSelect.Phase != GameOver_ToTitleCheck)
	{
		Font_108->DrawText(NULL, "GAME OVER", -1, &TextRect, DT_CENTER | DT_VCENTER, RED(Alpha));
	}

	if (GameOverSelect.Phase != Display && GameOverSelect.Phase != BGMStart)
	{
		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, GameOverSelectTexture);
		// �|���S���̕`��
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, GameOverSelect.VertexWk, sizeof(VERTEX_2D));

		if (GameOverSelect.Phase == GameOver_Restart || GameOverSelect.Phase == GameOver_ToTitle)
		{
			TextRect.left = OptionPos_X;
			TextRect.top = GameOver_RestartPos_Y;
			TextRect.right = OptionPos_X + RectWidth;
			TextRect.bottom = GameOver_RestartPos_Y + Texture_SelectBox_Height;
			if (CheckMousePos(TextRect) == true)
			{
				GameOverSelect.Phase = GameOver_Restart;
				GameOverSelect.Pos.y = GameOver_RestartPos_Y;
			}
			if (GameOverSelect.Phase == GameOver_Restart)
			{
				Font_72->DrawText(NULL, "�ŏ�����", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "�ŏ�����", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			TextRect.top = GameOver_ToTitlePos_Y;
			TextRect.bottom = GameOver_ToTitlePos_Y + Texture_SelectBox_Height;
			if (CheckMousePos(TextRect) == true)
			{
				GameOverSelect.Phase = GameOver_ToTitle;
				GameOverSelect.Pos.y = GameOver_ToTitlePos_Y;
			}
			if (GameOverSelect.Phase == GameOver_ToTitle)
			{
				Font_72->DrawText(NULL, "�^�C�g����", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "�^�C�g����", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
		else
		{
			TextRect.top = AskSentencePos_Y;
			TextRect.bottom = AskSentencePos_Y + 100;
			if (GameOverSelect.Phase == GameOver_RestartCheck)
			{
				Font_72->DrawText(NULL, "��낵���ł����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
			else if (GameOverSelect.Phase == GameOver_ToTitleCheck)
			{
				Font_72->DrawText(NULL, "�^�C�g���ɖ߂�܂����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			TextRect.top = YesNoPos_Y;
			TextRect.bottom = YesNoPos_Y + RectHeight;
			TextRect.left = 0;
			TextRect.right = Screen_Center_X;
			if (CheckMousePos(TextRect) == true)
			{
				GameOverSelect.InYes = true;
				GameOverSelect.Pos.x = YesPos_X;
			}
			if (GameOverSelect.InYes == true)
			{
				Font_72->DrawText(NULL, "�͂�", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "�͂�", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			TextRect.left = Screen_Center_X;
			TextRect.right = Screen_Width;
			if (CheckMousePos(TextRect) == true)
			{
				GameOverSelect.InYes = false;
				GameOverSelect.Pos.x = NoPos_X;
			}
			if (GameOverSelect.InYes == false)
			{
				Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
	}

	return;
}


//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeGameOverVertex(void)
{
	// ���_���W�̐ݒ�	
	SetGameOverTexture();

	// rhw�̐ݒ�
	BlackScreenVertexWk[0].rhw = 1.0f;
	BlackScreenVertexWk[1].rhw = 1.0f;
	BlackScreenVertexWk[2].rhw = 1.0f;
	BlackScreenVertexWk[3].rhw = 1.0f;

	GameOverSelect.VertexWk[0].rhw = 1.0f;
	GameOverSelect.VertexWk[1].rhw = 1.0f;
	GameOverSelect.VertexWk[2].rhw = 1.0f;
	GameOverSelect.VertexWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	BlackScreenVertexWk[0].diffuse = WHITE(200);
	BlackScreenVertexWk[1].diffuse = WHITE(200);
	BlackScreenVertexWk[2].diffuse = WHITE(200);
	BlackScreenVertexWk[3].diffuse = WHITE(200);

	GameOverSelect.VertexWk[0].diffuse = WHITE(255);
	GameOverSelect.VertexWk[1].diffuse = WHITE(255);
	GameOverSelect.VertexWk[2].diffuse = WHITE(255);
	GameOverSelect.VertexWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	SetGameOverVertex();

	return S_OK;
}

//=============================================================================
// �e�N�X�`�����W�̐ݒ�
//=============================================================================
void SetGameOverTexture(void)
{
	BlackScreenVertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	BlackScreenVertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	BlackScreenVertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	BlackScreenVertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	GameOverSelect.VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	GameOverSelect.VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	GameOverSelect.VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	GameOverSelect.VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return;
}


//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetGameOverVertex(void)
{
	BlackScreenVertexWk[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	BlackScreenVertexWk[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
	BlackScreenVertexWk[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
	BlackScreenVertexWk[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

	GameOverSelect.VertexWk[0].vtx = D3DXVECTOR3(GameOverSelect.Pos.x - Texture_SelectBox_Width / 2, (float)GameOverSelect.Pos.y, 0.0f);
	GameOverSelect.VertexWk[1].vtx = D3DXVECTOR3(GameOverSelect.Pos.x + Texture_SelectBox_Width / 2, (float)GameOverSelect.Pos.y, 0.0f);
	GameOverSelect.VertexWk[2].vtx = D3DXVECTOR3(GameOverSelect.Pos.x - Texture_SelectBox_Width / 2, (float)GameOverSelect.Pos.y + Texture_SelectBox_Height, 0.0f);
	GameOverSelect.VertexWk[3].vtx = D3DXVECTOR3(GameOverSelect.Pos.x + Texture_SelectBox_Width / 2, (float)GameOverSelect.Pos.y + Texture_SelectBox_Height, 0.0f);

	return;
}

//=============================================================================
// �I�����̏�Ԃ��擾����
//=============================================================================
SELECT *GetGameOverSelect(void)
{
	return &GameOverSelect;
}