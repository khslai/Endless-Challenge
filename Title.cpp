//=============================================================================
//
// �^�C�g����ʏ��� [Title.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Title.h"
#include "Input.h"
#include "Transition.h"
#include "Player.h"
#include "D3DXAnimation.h"
#include "Sound.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �ǂݍ��ރe�N�X�`���t�@�C����
#define TextureName_Title			_T("data/Texture/Title.png")	
#define Texture_Title_Width			(1500)	// �e�N�X�`�����������i��)
#define Texture_Title_Height		(200)	// �e�N�X�`�����������i�c)

// �����\���̋�`�T�C�Y
#define RectWidth					(550)
#define RectHeight					(150)

// �����̍��W
#define Title_Pos_Y					(100)
#define OptionPos_X					(1200)	
#define Option_CenterPos_X			(1450)	
#define Difficulty_EasyPos_Y		(600)
#define Difficulty_NormalPos_Y		(750)
#define GameStartPos_Y				(600)
#define AppealModePos_Y				(750)
#define ExitGamePos_Y				(900)
#define AskSentencePos_Y			(200)
#define YesNoPos_Y					(700)
#define YesPos_X					(480)
#define NoPos_X						(1440)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���_���̍쐬
HRESULT MakeTitleVertex(void);
// �e�N�X�`�����_���W�̐ݒ�	
void SetTitleTexture(void);
// ���_���W�̐ݒ�
void SetTitleVertex(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DTEXTURE9	Title_Texture = NULL;
// ���_���i�[���[�N
static VERTEX_2D	BlackScreenVertexWk[VERTEX_NUM];
VERTEX_2D			Title_VertexWk[VERTEX_NUM];
// �I����
SELECT				TitleSelect;
// �Q�[����Փx
int					Difficulty = Normal;
// �X�e�[�W�J�ڒ��t���O
bool				StageChanging = false;		

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int GameStage = GetGameStage();

	TitleSelect.Pos = D3DXVECTOR2(Option_CenterPos_X, GameStartPos_Y);
	TitleSelect.PrePos = TitleSelect.Pos;
	TitleSelect.Phase = GameStart;
	TitleSelect.InYes = false;
	if (GameStage == Stage_Title)
	{
		Difficulty = -1;
	}

	StageChanging = false;

	// ���߂ď�����
	if (FirstInit == true)
	{
		// ���_���̍쐬
		if (FAILED(MakeTitleVertex()))
		{
			return E_FAIL;
		}

		// �e�N�X�`���̓ǂݍ���
		if (FAILED(SafeLoadTexture(TextureName_Title, &Title_Texture, "Title")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
{
	// �e�N�X�`���̊J��
	SafeRelease(Title_Texture);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitle(void)
{
	PLAYER *Player = GetPlayer();
	MOUSE *Mouse = GetMouse();
	static int PressCount = 0;
	RECT SelectBox = {
		(LONG)TitleSelect.VertexWk[0].vtx.x, (LONG)TitleSelect.VertexWk[0].vtx.y,
		(LONG)TitleSelect.VertexWk[3].vtx.x, (LONG)TitleSelect.VertexWk[3].vtx.y };

	// �V�[���J�ڒ�
	if (StageChanging == true)
	{
		return;
	}

	// �I�����ړ����ʉ�
	if (TitleSelect.Pos.x != TitleSelect.PrePos.x || TitleSelect.Pos.y != TitleSelect.PrePos.y)
	{
		SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
	}
	TitleSelect.PrePos = TitleSelect.Pos;

	// ��ʑJ��
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_NUMPADENTER) ||
		IsButtonTriggered(0, BUTTON_B) || (IsMouseLeftTriggered() && CheckMousePos(SelectBox)))
	{
		if (TitleSelect.Phase != ExitCheck && TitleSelect.Phase != SkipTutorialCheck)
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);

			if (TitleSelect.Phase == GameStart)
			{
				Difficulty = Normal;
				TitleSelect.Phase = Difficulty_Normal;
			}
			else if (TitleSelect.Phase == AppealMode)
			{
				StageChanging = true;
				Difficulty = AppealDifficulty;
				ChangeAnimation(Player->Animation, StandUp, 0.8f, false);
				Player->Animation->NextAnimID = Idle;
			}
			else if (TitleSelect.Phase == ExitGame)
			{
				TitleSelect.Phase = ExitCheck;
				TitleSelect.Pos.x = NoPos_X;
				TitleSelect.Pos.y = YesNoPos_Y;
			}
			else if (TitleSelect.Phase == Difficulty_Easy || TitleSelect.Phase == Difficulty_Normal)
			{
				TitleSelect.Phase = SkipTutorialCheck;
				TitleSelect.Pos.x = NoPos_X;
				TitleSelect.Pos.y = YesNoPos_Y;
			}
		}
		// �I���m�F
		else if (TitleSelect.Phase == ExitCheck)
		{
			if (TitleSelect.InYes == true)
			{
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				TitleSelect.Phase = ExitDetermine;
			}
			else
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
				TitleSelect.Phase = ExitGame;
				TitleSelect.Pos.x = Option_CenterPos_X;
				TitleSelect.Pos.y = ExitGamePos_Y;
			}
		}
		// �X�L�b�v�m�F
		else if (TitleSelect.Phase == SkipTutorialCheck)
		{

			if (TitleSelect.InYes == true)
			{
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				TitleSelect.Phase = SkipDetermine;
			}
			else
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
				TitleSelect.Phase = NoSkip;
			}
			StageChanging = true;
			ChangeAnimation(Player->Animation, StandUp, 0.8f, false);
			Player->Animation->NextAnimID = Idle;
		}

		// �I�����ړ����ʉ��𗬂��Ȃ�����
		TitleSelect.PrePos = TitleSelect.Pos;
	}
	else
	{
		if (TitleSelect.Phase != ExitCheck && TitleSelect.Phase != SkipTutorialCheck)
		{
			// �O�̏�Ԃɖ߂�
			if (TitleSelect.Phase == Difficulty_Easy || TitleSelect.Phase == Difficulty_Normal)
			{
				if (IsMouseRightTriggered() || IsButtonTriggered(0, BUTTON_A))
				{
					SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
					TitleSelect.Phase = GameStart;
					TitleSelect.InYes = false;
					TitleSelect.Pos.x = Option_CenterPos_X;
					TitleSelect.Pos.y = GameStartPos_Y;
					TitleSelect.PrePos = TitleSelect.Pos;
				}
			}

			// �I�����ړ�
			if (GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
			{
				if (TitleSelect.Phase == GameStart)
				{
					TitleSelect.Phase = ExitGame;
					TitleSelect.Pos.y = ExitGamePos_Y;
				}
				else if (TitleSelect.Phase == AppealMode)
				{
					TitleSelect.Phase = GameStart;
					TitleSelect.Pos.y = GameStartPos_Y;
				}
				else if (TitleSelect.Phase == ExitGame)
				{
					TitleSelect.Phase = AppealMode;
					TitleSelect.Pos.y = AppealModePos_Y;
				}
				else if (TitleSelect.Phase == Difficulty_Easy)
				{
					Difficulty = Normal;
					TitleSelect.Phase = Difficulty_Normal;
					TitleSelect.Pos.y = Difficulty_NormalPos_Y;
				}
				else if (TitleSelect.Phase == Difficulty_Normal)
				{
					Difficulty = Easy;
					TitleSelect.Phase = Difficulty_Easy;
					TitleSelect.Pos.y = Difficulty_EasyPos_Y;
				}
			}
			else if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN))
			{
				if (TitleSelect.Phase == GameStart)
				{
					TitleSelect.Phase = AppealMode;
					TitleSelect.Pos.y = AppealModePos_Y;
				}
				else if (TitleSelect.Phase == AppealMode)
				{
					TitleSelect.Phase = ExitGame;
					TitleSelect.Pos.y = ExitGamePos_Y;
				}
				else if (TitleSelect.Phase == ExitGame)
				{
					TitleSelect.Phase = GameStart;
					TitleSelect.Pos.y = GameStartPos_Y;
				}
				else if (TitleSelect.Phase == Difficulty_Easy)
				{
					Difficulty = Normal;
					TitleSelect.Phase = Difficulty_Normal;
					TitleSelect.Pos.y = Difficulty_NormalPos_Y;
				}
				else if (TitleSelect.Phase == Difficulty_Normal)
				{
					Difficulty = Easy;
					TitleSelect.Phase = Difficulty_Easy;
					TitleSelect.Pos.y = Difficulty_EasyPos_Y;
				}
			}

			// �I�������[�v
			if (GetKeyboardRepeat(DIK_UP) || GetKeyboardRepeat(DIK_W) || IsButtonRepeat(0, BUTTON_UP))
			{
				PressCount++;
				if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
				{
					if (TitleSelect.Phase == GameStart)
					{
						TitleSelect.Phase = ExitGame;
						TitleSelect.Pos.y = ExitGamePos_Y;
					}
					else if (TitleSelect.Phase == AppealMode)
					{
						TitleSelect.Phase = GameStart;
						TitleSelect.Pos.y = GameStartPos_Y;
					}
					else if (TitleSelect.Phase == ExitGame)
					{
						TitleSelect.Phase = AppealMode;
						TitleSelect.Pos.y = AppealModePos_Y;
					}
					else if (TitleSelect.Phase == Difficulty_Easy)
					{
						Difficulty = Normal;
						TitleSelect.Phase = Difficulty_Normal;
						TitleSelect.Pos.y = Difficulty_NormalPos_Y;
					}
					else if (TitleSelect.Phase == Difficulty_Normal)
					{
						Difficulty = Easy;
						TitleSelect.Phase = Difficulty_Easy;
						TitleSelect.Pos.y = Difficulty_EasyPos_Y;
					}
				}
			}
			else if (GetKeyboardRepeat(DIK_DOWN) || GetKeyboardRepeat(DIK_S) || IsButtonRepeat(0, BUTTON_DOWN))
			{
				PressCount++;
				if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
				{
					if (TitleSelect.Phase == GameStart)
					{
						TitleSelect.Phase = AppealMode;
						TitleSelect.Pos.y = AppealModePos_Y;
					}
					else if (TitleSelect.Phase == AppealMode)
					{
						TitleSelect.Phase = ExitGame;
						TitleSelect.Pos.y = ExitGamePos_Y;
					}
					else if (TitleSelect.Phase == ExitGame)
					{
						TitleSelect.Phase = GameStart;
						TitleSelect.Pos.y = GameStartPos_Y;
					}
					else if (TitleSelect.Phase == Difficulty_Easy)
					{
						TitleSelect.Phase = Difficulty_Normal;
						TitleSelect.Pos.y = Difficulty_NormalPos_Y;
					}
					else if (TitleSelect.Phase == Difficulty_Normal)
					{
						TitleSelect.Phase = Difficulty_Easy;
						TitleSelect.Pos.y = Difficulty_EasyPos_Y;
					}
				}
			}

			// �v���X�J�E���g������
			if (GetKeyboardRelease(DIK_UP) || GetKeyboardRelease(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
				GetKeyboardRelease(DIK_DOWN) || GetKeyboardRelease(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
			{
				PressCount = 0;
			}
		}
		else
		{
			// �O�̏�Ԃɖ߂�
			if (IsMouseRightTriggered() || IsButtonTriggered(0, BUTTON_A))
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);

				TitleSelect.Pos.x = Option_CenterPos_X;
				if (TitleSelect.Phase == SkipTutorialCheck)
				{
					TitleSelect.Pos.y = Difficulty_NormalPos_Y;
					TitleSelect.Phase = Difficulty_Normal;
				}
				else if (TitleSelect.Phase == ExitCheck)
				{
					TitleSelect.Pos.y = ExitGamePos_Y;
					TitleSelect.Phase = ExitGame;
				}
				TitleSelect.InYes = false;
				TitleSelect.PrePos = TitleSelect.Pos;
			}

			// �I�����ړ�
			if (GetKeyboardTrigger(DIK_RIGHT) || GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_RIGHT))
			{
				TitleSelect.InYes = false;
				TitleSelect.Pos.x = NoPos_X;
			}
			else if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_LEFT))
			{
				TitleSelect.InYes = true;
				TitleSelect.Pos.x = YesPos_X;
			}
		}
	}

	// ���_���W�X�V
	SetTitleVertex();

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 BlackScreenTexture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 TitleSelectTexture = GetUITexture(SelectBox);
	LPD3DXFONT Font_96 = GetFont(FontSize_96);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	RECT TextRect = { OptionPos_X, GameStartPos_Y, OptionPos_X + RectWidth, GameStartPos_Y + RectHeight };
	PLAYER *Player = GetPlayer();

	// ���_�t�H�[�}�b�g�̐ݒ�
	Device->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	Device->SetTexture(0, Title_Texture);

	// �^�C�g���̕`��
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, Title_VertexWk, sizeof(VERTEX_2D));

	if (TitleSelect.Phase != ExitCheck && TitleSelect.Phase != SkipTutorialCheck)
	{
		if (StageChanging == true)
		{
			return;
		}

		if (TitleSelect.Phase == GameStart || TitleSelect.Phase == AppealMode || TitleSelect.Phase == ExitGame)
		{
			// ====================
			// Game Start
			// ====================
			// �}�E�X�͑I�����̒����m�F
			if (CheckMousePos(TextRect) == true)
			{
				TitleSelect.Phase = GameStart;
				TitleSelect.Pos.y = GameStartPos_Y;
			}
			if (TitleSelect.Phase == GameStart)
			{
				Font_96->DrawText(NULL, "Game Start", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_96->DrawText(NULL, "Game Start", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(128));
			}

			// ====================
			// Appeal Mode
			// ====================
			TextRect.top = AppealModePos_Y;
			TextRect.bottom = AppealModePos_Y + RectHeight;
			// �}�E�X�͑I�����̒����m�F
			if (CheckMousePos(TextRect) == true)
			{
				TitleSelect.Phase = AppealMode;
				TitleSelect.Pos.y = AppealModePos_Y;
			}
			if (TitleSelect.Phase == AppealMode)
			{
				Font_96->DrawText(NULL, "Appeal Mode", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_96->DrawText(NULL, "Appeal Mode", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(128));
			}

			// ====================
			// Exit Game
			// ====================
			TextRect.top = ExitGamePos_Y;
			TextRect.bottom = ExitGamePos_Y + RectHeight;
			if (CheckMousePos(TextRect) == true)
			{
				TitleSelect.Phase = ExitGame;
				TitleSelect.Pos.y = ExitGamePos_Y;
			}
			if (TitleSelect.Phase == ExitGame)
			{
				Font_96->DrawText(NULL, "Exit Game", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_96->DrawText(NULL, "Exit Game", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(128));
			}
		}
		else if (TitleSelect.Phase == Difficulty_Easy || TitleSelect.Phase == Difficulty_Normal)
		{
			// ====================
			// Difficulty_Easy
			// ====================
			// �}�E�X�͑I�����̒����m�F
			TextRect.top = Difficulty_EasyPos_Y;
			TextRect.bottom = Difficulty_EasyPos_Y + RectHeight;
			if (CheckMousePos(TextRect) == true)
			{
				Difficulty = Easy;
				TitleSelect.Phase = Difficulty_Easy;
				TitleSelect.Pos.y = Difficulty_EasyPos_Y;
			}
			if (TitleSelect.Phase == Difficulty_Easy)
			{
				Font_96->DrawText(NULL, "Easy", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_96->DrawText(NULL, "Easy", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(128));
			}

			// ====================
			// Difficulty_Normal
			// ====================
			TextRect.top = Difficulty_NormalPos_Y;
			TextRect.bottom = Difficulty_NormalPos_Y + RectHeight;
			if (CheckMousePos(TextRect) == true)
			{
				Difficulty = Normal;
				TitleSelect.Phase = Difficulty_Normal;
				TitleSelect.Pos.y = Difficulty_NormalPos_Y;
			}
			if (TitleSelect.Phase == Difficulty_Normal)
			{
				Font_96->DrawText(NULL, "Normal", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_96->DrawText(NULL, "Normal", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(128));
			}
		}
	}
	else if (TitleSelect.Phase == ExitCheck)
	{
		// ������ʕ`��
		Device->SetTexture(0, BlackScreenTexture);
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BlackScreenVertexWk, sizeof(VERTEX_2D));

		// �I��g�`��
		Device->SetTexture(0, TitleSelectTexture);
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, TitleSelect.VertexWk, sizeof(VERTEX_2D));

		// ====================
		// �Q�[���I���m�F
		// ====================
		TextRect.top = AskSentencePos_Y;
		TextRect.left = 0;
		TextRect.bottom = AskSentencePos_Y + RectHeight;
		TextRect.right = Screen_Width;
		Font_72->DrawText(NULL, "�{���ɃQ�[�����I�����܂����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));

		TextRect.top = YesNoPos_Y;
		TextRect.bottom = YesNoPos_Y + RectHeight;
		TextRect.left = 0;
		TextRect.right = Screen_Center_X;
		if (CheckMousePos(TextRect) == true)
		{
			TitleSelect.InYes = true;
			TitleSelect.Pos.x = YesPos_X;
		}
		if (TitleSelect.InYes == true)
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
			TitleSelect.InYes = false;
			TitleSelect.Pos.x = NoPos_X;
		}
		if (TitleSelect.InYes == false)
		{
			Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "������", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
	}
	else if (TitleSelect.Phase == SkipTutorialCheck)
	{
		// ������ʕ`��
		Device->SetTexture(0, BlackScreenTexture);
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BlackScreenVertexWk, sizeof(VERTEX_2D));

		// �I��g�`��
		Device->SetTexture(0, TitleSelectTexture);
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, TitleSelect.VertexWk, sizeof(VERTEX_2D));

		// ====================
		// �`���[�g���A���X�L�b�v�m�F
		// ====================
		TextRect.top = AskSentencePos_Y;
		TextRect.left = 0;
		TextRect.bottom = AskSentencePos_Y + RectHeight;
		TextRect.right = Screen_Width;
		Font_72->DrawText(NULL, "�`���[�g���A�����X�L�b�v���܂����H", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));

		TextRect.top = YesNoPos_Y;
		TextRect.bottom = YesNoPos_Y + RectHeight;
		TextRect.left = 0;
		TextRect.right = Screen_Center_X;
		if (CheckMousePos(TextRect) == true)
		{
			TitleSelect.InYes = true;
			TitleSelect.Pos.x = YesPos_X;
		}
		if (TitleSelect.InYes == true)
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
			TitleSelect.InYes = false;
			TitleSelect.Pos.x = NoPos_X;
		}
		if (TitleSelect.InYes == false)
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
HRESULT MakeTitleVertex(void)
{
	// ���_���W�̐ݒ�	
	SetTitleTexture();

	// rhw�̐ݒ�
	Title_VertexWk[0].rhw = 1.0f;
	Title_VertexWk[1].rhw = 1.0f;
	Title_VertexWk[2].rhw = 1.0f;
	Title_VertexWk[3].rhw = 1.0f;

	BlackScreenVertexWk[0].rhw = 1.0f;
	BlackScreenVertexWk[1].rhw = 1.0f;
	BlackScreenVertexWk[2].rhw = 1.0f;
	BlackScreenVertexWk[3].rhw = 1.0f;

	TitleSelect.VertexWk[0].rhw = 1.0f;
	TitleSelect.VertexWk[1].rhw = 1.0f;
	TitleSelect.VertexWk[2].rhw = 1.0f;
	TitleSelect.VertexWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	Title_VertexWk[0].diffuse = WHITE(255);
	Title_VertexWk[1].diffuse = WHITE(255);
	Title_VertexWk[2].diffuse = WHITE(255);
	Title_VertexWk[3].diffuse = WHITE(255);

	BlackScreenVertexWk[0].diffuse = WHITE(200);
	BlackScreenVertexWk[1].diffuse = WHITE(200);
	BlackScreenVertexWk[2].diffuse = WHITE(200);
	BlackScreenVertexWk[3].diffuse = WHITE(200);

	TitleSelect.VertexWk[0].diffuse = WHITE(255);
	TitleSelect.VertexWk[1].diffuse = WHITE(255);
	TitleSelect.VertexWk[2].diffuse = WHITE(255);
	TitleSelect.VertexWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	SetTitleVertex();

	return S_OK;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetTitleVertex(void)
{
	float Width_Half = Texture_Title_Width / 2;

	Title_VertexWk[0].vtx = D3DXVECTOR3(Screen_Center_X - Width_Half, Title_Pos_Y, 0.0f);
	Title_VertexWk[1].vtx = D3DXVECTOR3(Screen_Center_X + Width_Half, Title_Pos_Y, 0.0f);
	Title_VertexWk[2].vtx = D3DXVECTOR3(Screen_Center_X - Width_Half, Title_Pos_Y + Texture_Title_Height, 0.0f);
	Title_VertexWk[3].vtx = D3DXVECTOR3(Screen_Center_X + Width_Half, Title_Pos_Y + Texture_Title_Height, 0.0f);

	BlackScreenVertexWk[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	BlackScreenVertexWk[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
	BlackScreenVertexWk[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
	BlackScreenVertexWk[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

	TitleSelect.VertexWk[0].vtx = D3DXVECTOR3(TitleSelect.Pos.x - Texture_SelectBox_Width / 2, (float)TitleSelect.Pos.y, 0.0f);
	TitleSelect.VertexWk[1].vtx = D3DXVECTOR3(TitleSelect.Pos.x + Texture_SelectBox_Width / 2, (float)TitleSelect.Pos.y, 0.0f);
	TitleSelect.VertexWk[2].vtx = D3DXVECTOR3(TitleSelect.Pos.x - Texture_SelectBox_Width / 2, (float)TitleSelect.Pos.y + Texture_SelectBox_Height, 0.0f);
	TitleSelect.VertexWk[3].vtx = D3DXVECTOR3(TitleSelect.Pos.x + Texture_SelectBox_Width / 2, (float)TitleSelect.Pos.y + Texture_SelectBox_Height, 0.0f);

	return;
}

//=============================================================================
// �e�N�X�`�����W�̐ݒ�
//=============================================================================
void SetTitleTexture(void)
{
	Title_VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	Title_VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	Title_VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	Title_VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	BlackScreenVertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	BlackScreenVertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	BlackScreenVertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	BlackScreenVertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	TitleSelect.VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	TitleSelect.VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	TitleSelect.VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	TitleSelect.VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return;
}

//=============================================================================
// �I���̃|�C���^���擾����
//=============================================================================
SELECT *GetTitleSelect(void)
{
	return &TitleSelect;
}

//=============================================================================
// �I��������Փx���擾����
//=============================================================================
int GetDifficulty(void)
{
	return Difficulty;
}