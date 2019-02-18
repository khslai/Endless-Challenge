//=============================================================================
//
// �ꎞ��~���� [Pause.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Pause.h"
#include "Player.h"
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
#define PausePos_Y					(256)
#define OptionPos_X					(710)
#define Pause_ResumePos_Y			(540)
#define Pause_RestartPos_Y			(690)
#define Pause_ToTitlePos_Y			(840)
#define AskSentencePos_Y			(200)
#define YesNoPos_Y					(700)
#define YesPos_X					(480)
#define NoPos_X						(1440)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���_���̍쐬
HRESULT MakePauseVertex(void);
// �e�N�X�`�����_���W�̐ݒ�	
void SetPauseTexture(void);
// ���_���W�̐ݒ�
void SetPauseVertex(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
// ���_���i�[���[�N
VERTEX_2D PauseVertexWk[VERTEX_NUM];
// �ꎞ��~�I����
static SELECT PauseSelect;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitPause(bool FirstInit)
{
	PauseSelect.Pos = D3DXVECTOR2(Screen_Center_X, Pause_ResumePos_Y);
	PauseSelect.PrePos = PauseSelect.Pos;
	PauseSelect.Phase = Pause_Resume;
	PauseSelect.InYes = false;

	if (FirstInit == true)
	{
		// ���_���̍쐬
		MakePauseVertex();
	}
	else
	{
		// ���_���W�X�V
		SetPauseVertex();
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPause(void)
{

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePause(void)
{
	static int PressCount = 0;
	PLAYER *Player = GetPlayer();
	RECT SelectBox = {
	(LONG)PauseSelect.VertexWk[0].vtx.x, (LONG)PauseSelect.VertexWk[0].vtx.y,
	(LONG)PauseSelect.VertexWk[3].vtx.x, (LONG)PauseSelect.VertexWk[3].vtx.y };

	// �I�����ړ����ʉ�
	if (PauseSelect.Pos.x != PauseSelect.PrePos.x || PauseSelect.Pos.y != PauseSelect.PrePos.y)
	{
		SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
	}
	PauseSelect.PrePos = PauseSelect.Pos;

	// �ꎞ��~����
	if (PauseSelect.Phase != Pause_RestartCheck && PauseSelect.Phase != Pause_ToTitleCheck)
	{
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A))
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			SetGameStage(Stage_Game);
			while (ShowCursor(false) >= 0);
			return;
		}
	}

	// Enter�L�[
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_NUMPADENTER) ||
		IsButtonTriggered(0, BUTTON_B) || (IsMouseLeftTriggered() && CheckMousePos(SelectBox)))
	{
		switch (PauseSelect.Phase)
		{
		case Pause_Resume:
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			SetGameStage(Stage_Game);
			while (ShowCursor(false) >= 0);
			return;
			break;
		case Pause_Restart:
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			PauseSelect.Phase = Pause_RestartCheck;
			PauseSelect.Pos.x = NoPos_X;
			PauseSelect.Pos.y = YesNoPos_Y;
			break;
		case Pause_RestartCheck:
			if (PauseSelect.InYes == true)
			{
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				SetTransition(Fadein);
			}
			else
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
				PauseSelect.Phase = Pause_Restart;
				PauseSelect.Pos.x = Screen_Center_X;
				PauseSelect.Pos.y = Pause_RestartPos_Y;
			}
			break;
		case Pause_ToTitle:
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			PauseSelect.Phase = Pause_ToTitleCheck;
			PauseSelect.Pos.x = NoPos_X;
			PauseSelect.Pos.y = YesNoPos_Y;
			break;
		case Pause_ToTitleCheck:
			if (PauseSelect.InYes == true)
			{
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				SetTransition(Fadein);
			}
			else
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
				PauseSelect.Phase = Pause_ToTitle;
				PauseSelect.Pos.x = Screen_Center_X;
				PauseSelect.Pos.y = Pause_ToTitlePos_Y;
			}
			break;
		default:
			break;
		}

		// �I�����ړ����ʉ��𗬂��Ȃ�����
		PauseSelect.PrePos = PauseSelect.Pos;
	}

	// �I�����ړ�
	if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
	{
		switch (PauseSelect.Phase)
		{
		case Pause_Resume:
			PauseSelect.Phase = Pause_Restart;
			PauseSelect.Pos.y = Pause_RestartPos_Y;
			break;
		case Pause_Restart:
			PauseSelect.Phase = Pause_ToTitle;
			PauseSelect.Pos.y = Pause_ToTitlePos_Y;
			break;
		case Pause_ToTitle:
			PauseSelect.Phase = Pause_Resume;
			PauseSelect.Pos.y = Pause_ResumePos_Y;
			break;
		default:
			break;
		}
	}
	else if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
	{
		switch (PauseSelect.Phase)
		{
		case Pause_Resume:
			PauseSelect.Phase = Pause_ToTitle;
			PauseSelect.Pos.y = Pause_ToTitlePos_Y;
			break;
		case Pause_Restart:
			PauseSelect.Phase = Pause_Resume;
			PauseSelect.Pos.y = Pause_ResumePos_Y;
			break;
		case Pause_ToTitle:
			PauseSelect.Phase = Pause_Restart;
			PauseSelect.Pos.y = Pause_RestartPos_Y;
			break;
		default:
			break;
		}
	}

	// �I�������[�v
	if (GetKeyboardRepeat(DIK_DOWN) || GetKeyboardRepeat(DIK_S) || IsButtonRepeat(0, BUTTON_DOWN))
	{
		PressCount++;
		if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
		{
			if (PauseSelect.Phase == Pause_Resume)
			{
				PauseSelect.Phase = Pause_Restart;
				PauseSelect.Pos.y = Pause_RestartPos_Y;
			}
			else if (PauseSelect.Phase == Pause_Restart)
			{
				PauseSelect.Phase = Pause_ToTitle;
				PauseSelect.Pos.y = Pause_ToTitlePos_Y;
			}
			else if (PauseSelect.Phase == Pause_ToTitle)
			{
				PauseSelect.Phase = Pause_Resume;
				PauseSelect.Pos.y = Pause_ResumePos_Y;
			}
		}
	}
	else if (GetKeyboardRepeat(DIK_UP) || GetKeyboardRepeat(DIK_W) || IsButtonRepeat(0, BUTTON_UP))
	{
		PressCount++;
		if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
		{
			if (PauseSelect.Phase == Pause_Resume)
			{
				PauseSelect.Phase = Pause_ToTitle;
				PauseSelect.Pos.y = Pause_ToTitlePos_Y;
			}
			else if (PauseSelect.Phase == Pause_Restart)
			{
				PauseSelect.Phase = Pause_Resume;
				PauseSelect.Pos.y = Pause_ResumePos_Y;
			}
			else if (PauseSelect.Phase == Pause_ToTitle)
			{
				PauseSelect.Phase = Pause_Restart;
				PauseSelect.Pos.y = Pause_RestartPos_Y;
			}
		}
	}

	// �v���X�J�E���g������
	if (GetKeyboardRelease(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
		GetKeyboardRelease(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
	{
		PressCount = 0;
	}

	// �m�F���
	if (PauseSelect.Phase == Pause_RestartCheck || PauseSelect.Phase == Pause_ToTitleCheck)
	{
		// �O�̏�Ԃɖ߂�
		if (IsMouseRightTriggered() || IsButtonTriggered(0, BUTTON_A))
		{
			SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);

			PauseSelect.Pos.x = Screen_Center_X;
			if (PauseSelect.Phase == Pause_RestartCheck)
			{
				PauseSelect.Pos.y = Pause_RestartPos_Y;
				PauseSelect.Phase = Pause_Restart;
			}
			else if (PauseSelect.Phase == Pause_ToTitleCheck)
			{
				PauseSelect.Pos.y = Pause_ToTitlePos_Y;
				PauseSelect.Phase = Pause_ToTitle;
			}
			PauseSelect.InYes = false;
			PauseSelect.PrePos = PauseSelect.Pos;
		}

		if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_LEFT))
		{
			PauseSelect.Pos.x = YesPos_X;
			PauseSelect.InYes = true;
		}
		else if (GetKeyboardTrigger(DIK_RIGHT) || GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_RIGHT))
		{
			PauseSelect.Pos.x = NoPos_X;
			PauseSelect.InYes = false;
		}
	}

	// ���_���W�X�V
	SetPauseVertex();

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPause(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 PauseTexture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 PauseSelectTexture = GetUITexture(SelectBox);
	LPD3DXFONT Font_108 = GetFont(FontSize_108);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	RECT TextRect = { 0, 0, Screen_Width, RectHeight };

	// ���_�t�H�[�}�b�g�̐ݒ�
	Device->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	Device->SetTexture(0, PauseTexture);

	// �|���S���̕`��
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PauseVertexWk, sizeof(VERTEX_2D));

	// �e�N�X�`���̐ݒ�
	Device->SetTexture(0, PauseSelectTexture);

	// �|���S���̕`��
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PauseSelect.VertexWk, sizeof(VERTEX_2D));

	if (PauseSelect.Phase != Pause_RestartCheck && PauseSelect.Phase != Pause_ToTitleCheck)
	{
		TextRect.top = PausePos_Y;
		TextRect.bottom = PausePos_Y + RectHeight;
		Font_108->DrawText(NULL, "P A U S E", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));

		TextRect.left = OptionPos_X;
		TextRect.top = Pause_ResumePos_Y;
		TextRect.right = OptionPos_X + RectWidth;
		TextRect.bottom = Pause_ResumePos_Y + RectHeight;
		if (CheckMousePos(TextRect) == true)
		{
			PauseSelect.Phase = Pause_Resume;
			PauseSelect.Pos.y = Pause_ResumePos_Y;
		}
		if (PauseSelect.Phase == Pause_Resume)
		{
			Font_72->DrawText(NULL, "����", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "����", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}

		TextRect.top = Pause_RestartPos_Y;
		TextRect.bottom = Pause_RestartPos_Y + RectHeight;
		if (CheckMousePos(TextRect) == true)
		{
			PauseSelect.Phase = Pause_Restart;
			PauseSelect.Pos.y = Pause_RestartPos_Y;
		}
		if (PauseSelect.Phase == Pause_Restart)
		{
			Font_72->DrawText(NULL, "�ŏ�����", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "�ŏ�����", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}

		TextRect.top = Pause_ToTitlePos_Y;
		TextRect.bottom = Pause_ToTitlePos_Y + RectHeight;
		if (CheckMousePos(TextRect) == true)
		{
			PauseSelect.Phase = Pause_ToTitle;
			PauseSelect.Pos.y = Pause_ToTitlePos_Y;
		}
		if (PauseSelect.Phase == Pause_ToTitle)
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
		TextRect.bottom = AskSentencePos_Y + RectHeight;
		if (PauseSelect.Phase == Pause_RestartCheck)
		{
			Font_72->DrawText(NULL, "��낵���ł����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
		else if (PauseSelect.Phase == Pause_ToTitleCheck)
		{
			Font_72->DrawText(NULL, "�^�C�g���ɖ߂�܂����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}

		TextRect.top = YesNoPos_Y;
		TextRect.bottom = YesNoPos_Y + RectHeight;
		TextRect.left = 0;
		TextRect.right = Screen_Center_X;
		if (CheckMousePos(TextRect) == true)
		{
			PauseSelect.InYes = true;
			PauseSelect.Pos.x = YesPos_X;
		}
		if (PauseSelect.InYes == true)
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
			PauseSelect.InYes = false;
			PauseSelect.Pos.x = NoPos_X;
		}
		if (PauseSelect.InYes == false)
		{
			Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
	}

	return;
}


//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakePauseVertex(void)
{
	// ���_���W�̐ݒ�	
	SetPauseTexture();

	// rhw�̐ݒ�
	PauseVertexWk[0].rhw = 1.0f;
	PauseVertexWk[1].rhw = 1.0f;
	PauseVertexWk[2].rhw = 1.0f;
	PauseVertexWk[3].rhw = 1.0f;

	PauseSelect.VertexWk[0].rhw = 1.0f;
	PauseSelect.VertexWk[1].rhw = 1.0f;
	PauseSelect.VertexWk[2].rhw = 1.0f;
	PauseSelect.VertexWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	PauseVertexWk[0].diffuse = WHITE(200);
	PauseVertexWk[1].diffuse = WHITE(200);
	PauseVertexWk[2].diffuse = WHITE(200);
	PauseVertexWk[3].diffuse = WHITE(200);

	PauseSelect.VertexWk[0].diffuse = WHITE(255);
	PauseSelect.VertexWk[1].diffuse = WHITE(255);
	PauseSelect.VertexWk[2].diffuse = WHITE(255);
	PauseSelect.VertexWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	SetPauseVertex();

	return S_OK;
}

//=============================================================================
// �e�N�X�`�����W�̐ݒ�
//=============================================================================
void SetPauseTexture(void)
{
	PauseVertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	PauseVertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	PauseVertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	PauseVertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	PauseSelect.VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	PauseSelect.VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	PauseSelect.VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	PauseSelect.VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return;
}


//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetPauseVertex(void)
{
	PauseVertexWk[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	PauseVertexWk[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
	PauseVertexWk[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
	PauseVertexWk[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

	PauseSelect.VertexWk[0].vtx = D3DXVECTOR3(PauseSelect.Pos.x - Texture_SelectBox_Width / 2, (float)PauseSelect.Pos.y, 0.0f);
	PauseSelect.VertexWk[1].vtx = D3DXVECTOR3(PauseSelect.Pos.x + Texture_SelectBox_Width / 2, (float)PauseSelect.Pos.y, 0.0f);
	PauseSelect.VertexWk[2].vtx = D3DXVECTOR3(PauseSelect.Pos.x - Texture_SelectBox_Width / 2, (float)PauseSelect.Pos.y + Texture_SelectBox_Height, 0.0f);
	PauseSelect.VertexWk[3].vtx = D3DXVECTOR3(PauseSelect.Pos.x + Texture_SelectBox_Width / 2, (float)PauseSelect.Pos.y + Texture_SelectBox_Height, 0.0f);

	return;
}

//=============================================================================
// �I���̃|�C���^���擾����
//=============================================================================
SELECT *GetPauseSelect(void)
{
	return &PauseSelect;
}