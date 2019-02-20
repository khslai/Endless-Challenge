//=============================================================================
//
// �`���[�g���A������ [Tutorial.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Tutorial.h"
#include "Input.h"
#include "Transition.h"
#include "CapsuleMesh.h"
#include "Effect.h"
#include "SummonSword.h"
#include "Player.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define Texture_Tutorial_Key		_T("data/Texture/Tutorial_Keyboard.png")
#define Texture_Tutorial_Pad		_T("data/Texture/Tutorial_GamePad.png")
#define Texture_Tutorial_Width		(1024)
#define Texture_Tutorial_Height		(768)

// �����̉�]���x
#define RotationSpeed				(0.03f)

// �ǂݍ��ރ��f����
#define	Model_Cube					"data/Model/Cube.x"	
// �����̂̑ϋv�� 
#define Durability_Max				(5.0f)

#define RectWidth					(500)
#define RectHeight					(150)
#define TutorialPos_Y				(256)
#define OptionPos_X					(710)
#define Tutorial_ResumePos_Y		(540)
#define Tutorial_ToTitlePos_Y		(690)
#define AskSentencePos_Y			(200)
#define YesNoPos_Y					(700)
#define YesPos_X					(480)
#define NoPos_X						(1440)

#define BossHPTextPos_X				(192)
#define BossHPTextPos_Y				(800)

// �`���[�g���A�������̍��W
#define InformationPos_Y			(600)

enum CompletePhase
{
	NoComplete,
	ShowCompleteText,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeTutorialVertex(void);
void SetTutorialTexture(void);
void SetTutorialVertex(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
// �e�N�X�`���ւ̃|�C���^
LPDIRECT3DTEXTURE9	Tutorial_Texture_Key = NULL;
LPDIRECT3DTEXTURE9	Tutorial_Texture_Pad = NULL;
// ���_���i�[���[�N
VERTEX_2D			Tutorial_VertexWk[VERTEX_NUM];
VERTEX_2D			BlackScreenVertexWk[VERTEX_NUM];
// �I����
static SELECT		TutorialSelect;
// �`���[�g���A���̏��
int					TutorialState = 0;
// ��̗�����
CUBE				Cube;
// �`���[�g���A���̎w����B������
bool				Complete = false;
// �`���[�g���A�����������t���O
bool				TextDisappear = false;
// ���@�w�G�t�F�N�g��ݒu����G�t�F�N�g
bool				SetMagicCircle = false;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorial(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int GameStage = GetGameStage();

	TutorialState = Tutorial_Attack;
	Complete = false;
	TextDisappear = false;
	SetMagicCircle = false;

	// �ʒu�A�I����Ԃ̏����ݒ�
	TutorialSelect.Pos = D3DXVECTOR2(Screen_Center_X, Tutorial_ResumePos_Y);
	TutorialSelect.Phase = Tutorial_Resume;
	TutorialSelect.InYes = false;

	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	Cube.Pos = D3DXVECTOR3(0.0f, 75.0f, 150.0f);
	Cube.Rot = PositionZero;
	Cube.Scale = DefaultScale;
	if (GameStage == Stage_Tutorial)
	{
		Cube.Exist = true;
	}
	else
	{
		Cube.Exist = false;
	}
	Cube.EffectOver = false;
	Cube.MaxDurability = Durability_Max;
	Cube.Durability = Cube.MaxDurability;

	// ���߂ď�����
	if (FirstInit == true)
	{
		Cube.Texture = NULL;
		Cube.MaterialBuffer = NULL;
		Cube.Mesh = NULL;
		Cube.MaterialNum = 0;

		// X�t�@�C���̓ǂݍ���
		if (FAILED(D3DXLoadMeshFromX(Model_Cube,	// �ǂݍ��ރ��f���t�@�C����(X�t�@�C��)
			D3DXMESH_SYSTEMMEM,						// ���b�V���̍쐬�I�v�V�������w��
			Device,									// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
			NULL,									// �אڐ��f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			&Cube.MaterialBuffer,					// �}�e���A���f�[�^���܂ރo�b�t�@�ւ̃|�C���^
			NULL,									// �G�t�F�N�g�C���X�^���X�̔z����܂ރo�b�t�@�ւ̃|�C���^
			&Cube.MaterialNum,						// D3DXMATERIAL�\���̂̐�
			&Cube.Mesh)))							// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^�̃A�h���X
		{
			return E_FAIL;
		}

		// ���f���p�̃e�N�X�`���̃�������Ԃ�z��
		Cube.Texture = (LPDIRECT3DTEXTURE9*)calloc(Cube.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Cube.Texture == NULL)
		{
			MessageBox(0, "Alloc Cube Texture Memory Failed�I", "Error", 0);
			return E_FAIL;
		}

		// ���f���e�N�X�`����ǂݍ���
		if (FAILED(SafeLoadModelTexture(Cube.Texture, Cube.MaterialBuffer, Cube.MaterialNum, "Cube")))
		{
			return E_FAIL;
		}

		// �����蔻��J�v�Z���𐶐�
		if (FAILED(
			CreateCapsule(&Cube.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 20.0f, 40.0f, true)))
		{
			return E_FAIL;
		}

		// ���_���̍쐬
		if (FAILED(MakeTutorialVertex()))
		{
			return E_FAIL;
		}

		// �e�N�X�`���̓ǂݍ���
		if (FAILED(SafeLoadTexture(Texture_Tutorial_Key, &Tutorial_Texture_Key, "Tutorial") == false))
		{
			return E_FAIL;
		}

		if (FAILED(SafeLoadTexture(Texture_Tutorial_Pad, &Tutorial_Texture_Pad, "Tutorial") == false))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorial(void)
{
	// �e�N�X�`���̊J��
	SafeRelease(Tutorial_Texture_Key);
	SafeRelease(Tutorial_Texture_Pad);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTutorial(void)
{
	int Transition = GetTransition();
	static float Angle = 0.0f;
	static int PressCount = 0;
	static int NextStateCount = 0;
	static int PreState = 0;
	PLAYER *Player = GetPlayer();
	SUMMONSWORD *SummonSword = GetSummonSword();
	RECT SelectBox = {
	(LONG)TutorialSelect.VertexWk[0].vtx.x, (LONG)TutorialSelect.VertexWk[0].vtx.y,
	(LONG)TutorialSelect.VertexWk[3].vtx.x, (LONG)TutorialSelect.VertexWk[3].vtx.y };

	// ��������\��
	if (TutorialState == DisplayHelp)
	{
		if (IsMouseLeftTriggered() || GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			TutorialState = PreState;
		}
	}
	// �ꎞ��~
	else if (TutorialState == TutorialPause)
	{
		// �I�����ړ����ʉ�
		if (TutorialSelect.Pos.x != TutorialSelect.PrePos.x || TutorialSelect.Pos.y != TutorialSelect.PrePos.y)
		{
			SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
		}
		TutorialSelect.PrePos = TutorialSelect.Pos;

		// �Q�[���ɖ߂�
		if (TutorialSelect.Phase != Tutorial_ToTitleCheck && 
			(GetKeyboardTrigger(DIK_P) || IsMouseRightTriggered() ||
			IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A)))
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			TutorialState = PreState;
			while (ShowCursor(false) >= 0);
		}

		// Enter�L�[
		if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_NUMPADENTER) ||
			IsButtonTriggered(0, BUTTON_B) || (IsMouseLeftTriggered() && CheckMousePos(SelectBox)))
		{
			switch (TutorialSelect.Phase)
			{
			case Tutorial_Resume:

				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				TutorialState = PreState;
				while (ShowCursor(false) >= 0);
				return;
				break;

			case Tutorial_ToTitle:
				SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
				TutorialSelect.Phase = Tutorial_ToTitleCheck;
				TutorialSelect.Pos.x = NoPos_X;
				TutorialSelect.Pos.y = YesNoPos_Y;
				break;
			case Tutorial_ToTitleCheck:
				if (TutorialSelect.InYes == true)
				{
					SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
					SetTransition(Fadein);
				}
				else
				{
					SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);
					TutorialSelect.Phase = Tutorial_ToTitle;
					TutorialSelect.Pos.x = Screen_Center_X;
					TutorialSelect.Pos.y = Tutorial_ToTitlePos_Y;
				}
				break;
			default:
				break;
			}

			// �I�����ړ����ʉ��𗬂��Ȃ�����
			TutorialSelect.PrePos = TutorialSelect.Pos;
		}

		// �I�����ړ�
		if (GetKeyboardTrigger(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonTriggered(0, BUTTON_DOWN) ||
			GetKeyboardTrigger(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonTriggered(0, BUTTON_UP))
		{
			switch (TutorialSelect.Phase)
			{
			case Tutorial_Resume:
				TutorialSelect.Phase = Tutorial_ToTitle;
				TutorialSelect.Pos.y = Tutorial_ToTitlePos_Y;
				break;
			case Tutorial_ToTitle:
				TutorialSelect.Phase = Tutorial_Resume;
				TutorialSelect.Pos.y = Tutorial_ResumePos_Y;
				break;
			default:
				break;
			}
		}

		// �I�������[�v
		if (GetKeyboardRepeat(DIK_DOWN) || GetKeyboardRepeat(DIK_S) || IsButtonRepeat(0, BUTTON_DOWN) ||
			GetKeyboardRepeat(DIK_UP) || GetKeyboardRepeat(DIK_W) || IsButtonRepeat(0, BUTTON_UP))
		{
			PressCount++;
			if (PressCount >= RepeatCount && PressCount % RepeatSpeed == 0)
			{
				if (TutorialSelect.Phase == Tutorial_Resume)
				{
					TutorialSelect.Phase = Tutorial_ToTitle;
					TutorialSelect.Pos.y = Tutorial_ToTitlePos_Y;
				}
				else if (TutorialSelect.Phase == Tutorial_ToTitle)
				{
					TutorialSelect.Phase = Tutorial_Resume;
					TutorialSelect.Pos.y = Tutorial_ResumePos_Y;
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
		if (TutorialSelect.Phase == Tutorial_ToTitleCheck)
		{
			// �O�̏�Ԃɖ߂�
			if (IsMouseRightTriggered() || IsButtonTriggered(0, BUTTON_A))
			{
				SetSound(NormalSE, SE_Determine_No, E_DS8_FLAG_NONE, true);

				TutorialSelect.Pos.x = Screen_Center_X;
				TutorialSelect.Pos.y = Tutorial_ToTitlePos_Y;
				TutorialSelect.Phase = Tutorial_ToTitle;
				TutorialSelect.InYes = false;
				TutorialSelect.PrePos = TutorialSelect.Pos;
			}

			if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_A) || IsButtonTriggered(0, BUTTON_LEFT))
			{
				TutorialSelect.Pos.x = YesPos_X;
				TutorialSelect.InYes = true;
			}
			else if (GetKeyboardTrigger(DIK_RIGHT) || GetKeyboardTrigger(DIK_D) || IsButtonTriggered(0, BUTTON_RIGHT))
			{
				TutorialSelect.Pos.x = NoPos_X;
				TutorialSelect.InYes = false;
			}
		}

		// ���_���W�X�V
		SetTutorialVertex();
	}
	// �`���[�g���A���J�n
	else
	{
		// ��������\��
		if (GetKeyboardTrigger(DIK_H) || IsButtonTriggered(0, BUTTON_SELECT))
		{
			SetSound(NormalSE, SE_ShowTutorial, E_DS8_FLAG_NONE, true);
			PreState = TutorialState;
			TutorialState = DisplayHelp;
		}

		// �ꎞ��~
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
		{
			SetSound(NormalSE, SE_Menu, E_DS8_FLAG_NONE, true);
			PreState = TutorialState;
			TutorialState = TutorialPause;
			while (ShowCursor(true) < 0);
		}

		// ���@�w�G�t�F�N�g��ݒu����
		if (SetMagicCircle == false)
		{
			SetMagicCircle = true;
			D3DXVECTOR3 CirclePos = D3DXVECTOR3(Cube.Pos.x, 5.0f, Cube.Pos.z);
			SetEffect(CirclePos, MagicCircle);
		}

		// �����̉�]
		Angle += RotationSpeed;
		Cube.Rot = D3DXVECTOR3(Angle, Angle * 0.8f, Angle * 0.5f);

		// �����������܂���
		if (TextDisappear == true)
		{
			NextStateCount++;
			// ��b�o�߁A���̒i�K�ɓ���
			if (NextStateCount >= 120)
			{
				if (TutorialState != HelpOver)
				{
					Complete = false;
					TextDisappear = false;
					NextStateCount = 0;
					TutorialState++;
				}
				else
				{
					// �`���[�g���A���N���A
					if (Cube.EffectOver == true)
					{
						Complete = false;
						TextDisappear = false;
						NextStateCount = 0;
						TutorialState = TutorialClear;
						SetTransition(Fadein);
					}
				}
			}
		}

		// �`���[�g���A�����
		switch (TutorialState)
		{
		case Tutorial_Attack:
			if (IsMouseLeftTriggered() || IsButtonTriggered(0, BUTTON_RB))
			{
				Complete = true;
			}
			break;
		case Tutorial_WaitRolling:
			if (SummonSword->State == AttackStart)
			{
				Complete = true;
				TutorialState = Tutorial_RollingClear;
				Player->HP -= 30.0f;
			}
			break;
		case Tutorial_HPRestore:
			if (GetKeyboardTrigger(DIK_F) || IsButtonTriggered(0, BUTTON_X))
			{
				Complete = true;
			}
			break;
		case Tutorial_CameraReset:
			if (GetKeyboardTrigger(DIK_R) || IsButtonTriggered(0, BUTTON_LB))
			{
				Complete = true;
			}
			break;
		case Tutorial_CameraLockOn:
			if (GetKeyboardTrigger(DIK_T) || IsButtonTriggered(0, BUTTON_R3))
			{
				Complete = true;
			}
			break;
		case HelpOver:
			if (Cube.Durability <= 0)
			{
				Complete = true;
			}
			break;
		default:
			break;
		}
	}

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorialText(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 BlackScreen_Texture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 SelectBox_Texture = GetUITexture(SelectBox);
	LPD3DXFONT Font_108 = GetFont(FontSize_108);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	LPD3DXFONT Font_54 = GetFont(FontSize_54);
	static int DisappearWidth = 0;
	RECT rect = { 0, 0, Screen_Width, RectHeight };
	RECT BossHP = { BossHPTextPos_X, BossHPTextPos_Y, BossHPTextPos_X + RectWidth, BossHPTextPos_Y + RectHeight };
	GAMEPAD *GamePad = GetGamePad();

	//=================
	// �`���[�g���A������
	//=================
	Font_54->DrawText(NULL, "Boss(?) HP", -1, &BossHP, DT_LEFT | DT_BOTTOM, WHITE(255));

	if (TutorialState != DisplayHelp)
	{
		rect.top = BossHPTextPos_Y;
		rect.bottom = BossHPTextPos_Y + RectHeight;
		if (GamePad->Use == true)
		{
			Font_54->DrawText(NULL, "�������(BACK)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
		else
		{
			Font_54->DrawText(NULL, "�������(H)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
		}

		if (TextDisappear == false)
		{
			if (Complete == true)
			{
				DisappearWidth += 10;
			}
			else
			{
				DisappearWidth = 0;
			}
			rect.left = DisappearWidth;
			rect.top = InformationPos_Y;
			rect.right = Screen_Width - DisappearWidth;
			rect.bottom = InformationPos_Y + RectHeight;
			if (rect.left >= Screen_Center_X && rect.right <= Screen_Center_X)
			{
				TextDisappear = true;
			}
			switch (TutorialState)
			{
			case Tutorial_Attack:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "�U��(RB�{�^��)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "�U��(���N���b�N)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_WaitRolling:
			case Tutorial_RollingClear:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "�U��������A�������(A�{�^��)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "�U��������A�������(Space�L�[)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_HPRestore:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "�����₯�ǂ�����A�񕜂��悤(X�{�^��)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "�����₯�ǂ�����A�񕜂��悤(F�L�[)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_CameraReset:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "�J�������Z�b�g(LB�{�^��)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "�J�������Z�b�g(R�L�[)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_CameraLockOn:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "�^�[�Q�b�g�����b�N�I��(R3�{�^��)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "�^�[�Q�b�g�����b�N�I��(T�L�[)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case HelpOver:
				Font_72->DrawText(NULL, "���󂳂ꂽ�����̂��󂻂��I", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				break;
			default:
				break;
			}
		}
		else
		{
			rect.top = InformationPos_Y;
			rect.bottom = InformationPos_Y + RectHeight;
			Font_72->DrawText(NULL, "�B ��", -1, &rect, DT_CENTER | DT_VCENTER, GREEN(255));
		}
	}

	//=================
	// �������
	//=================
	if (TutorialState == DisplayHelp)
	{
		// ���_�t�H�[�}�b�g�̐ݒ�
		Device->SetFVF(FVF_VERTEX_2D);

		// �e�N�X�`���̐ݒ�
		if (GamePad->Use == true)
		{
			Device->SetTexture(0, Tutorial_Texture_Pad);
		}
		else
		{
			Device->SetTexture(0, Tutorial_Texture_Key);
		}

		// �|���S���̕`��
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, Tutorial_VertexWk, sizeof(VERTEX_2D));
	}
	//=================
	// �ꎞ��~���
	//=================
	else if (TutorialState == TutorialPause)
	{
		// ���_�t�H�[�}�b�g�̐ݒ�
		Device->SetFVF(FVF_VERTEX_2D);

		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, BlackScreen_Texture);

		// �|���S���̕`��
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BlackScreenVertexWk, sizeof(VERTEX_2D));

		// �e�N�X�`���̐ݒ�
		Device->SetTexture(0, SelectBox_Texture);

		// �|���S���̕`��
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, TutorialSelect.VertexWk, sizeof(VERTEX_2D));

		if (TutorialSelect.Phase != Tutorial_ToTitleCheck)
		{
			rect.top = TutorialPos_Y;
			rect.bottom = TutorialPos_Y + RectHeight;
			Font_108->DrawText(NULL, "P A U S E", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));

			rect.left = OptionPos_X;
			rect.top = Tutorial_ResumePos_Y;
			rect.right = OptionPos_X + RectWidth;
			rect.bottom = Tutorial_ResumePos_Y + RectHeight;
			if (CheckMousePos(rect) == true)
			{
				TutorialSelect.Phase = Tutorial_Resume;
				TutorialSelect.Pos.y = Tutorial_ResumePos_Y;
			}
			if (TutorialSelect.Phase == Tutorial_Resume)
			{
				Font_72->DrawText(NULL, "����", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "����", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			rect.top = Tutorial_ToTitlePos_Y;
			rect.bottom = Tutorial_ToTitlePos_Y + RectHeight;
			if (CheckMousePos(rect) == true)
			{
				TutorialSelect.Phase = Tutorial_ToTitle;
				TutorialSelect.Pos.y = Tutorial_ToTitlePos_Y;
			}
			if (TutorialSelect.Phase == Tutorial_ToTitle)
			{
				Font_72->DrawText(NULL, "�^�C�g����", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "�^�C�g����", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
		else
		{
			rect.top = AskSentencePos_Y;
			rect.bottom = AskSentencePos_Y + RectHeight;
			if (TutorialSelect.Phase == Tutorial_ToTitleCheck)
			{
				Font_72->DrawText(NULL, "�^�C�g���ɖ߂�܂����H", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			rect.top = YesNoPos_Y;
			rect.bottom = YesNoPos_Y + RectHeight;
			rect.left = 0;
			rect.right = Screen_Center_X;
			if (CheckMousePos(rect) == true)
			{
				TutorialSelect.InYes = true;
				TutorialSelect.Pos.x = YesPos_X;
			}
			if (TutorialSelect.InYes == true)
			{
				Font_72->DrawText(NULL, "�͂�", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "�͂�", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}

			rect.left = Screen_Center_X;
			rect.right = Screen_Width;
			if (CheckMousePos(rect) == true)
			{
				TutorialSelect.InYes = false;
				TutorialSelect.Pos.x = NoPos_X;
			}
			if (TutorialSelect.InYes == false)
			{
				Font_72->DrawText(NULL, "������", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "������", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
	}

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorialCube(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;

	//=================
	// ������
	//=================
	if (Cube.Exist == true)
	{
		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&Cube.WorldMatrix);

		// �X�P�[���𔽉f
		D3DXMatrixScaling(&ScaleMatrix, Cube.Scale.x, Cube.Scale.y, Cube.Scale.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &ScaleMatrix);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, Cube.Rot.y, Cube.Rot.x, Cube.Rot.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &RotMatrix);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&TransMatrix, Cube.Pos.x, Cube.Pos.y, Cube.Pos.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &TransMatrix);

		// ���[���h�}�g���b�N�X�̐ݒ�
		Device->SetTransform(D3DTS_WORLD, &Cube.WorldMatrix);

		// �}�e���A�����ɑ΂���|�C���^���擾
		pD3DXMat = (D3DXMATERIAL*)Cube.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)Cube.MaterialNum; nCntMat++)
		{
			// �}�e���A���̐ݒ�
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// �e�N�X�`���̐ݒ�
			Device->SetTexture(0, Cube.Texture[nCntMat]);

			// �`��
			Cube.Mesh->DrawSubset(nCntMat);
		}

		// �����̂̃J�v�Z����`�悷��
		DrawCapsule(&Cube.HitCapsule, &Cube.WorldMatrix);
	}

	return;
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeTutorialVertex(void)
{
	// ���_���W�̐ݒ�	
	SetTutorialVertex();

	// rhw�̐ݒ�
	Tutorial_VertexWk[0].rhw = 1.0f;
	Tutorial_VertexWk[1].rhw = 1.0f;
	Tutorial_VertexWk[2].rhw = 1.0f;
	Tutorial_VertexWk[3].rhw = 1.0f;

	BlackScreenVertexWk[0].rhw = 1.0f;
	BlackScreenVertexWk[1].rhw = 1.0f;
	BlackScreenVertexWk[2].rhw = 1.0f;
	BlackScreenVertexWk[3].rhw = 1.0f;

	TutorialSelect.VertexWk[0].rhw = 1.0f;
	TutorialSelect.VertexWk[1].rhw = 1.0f;
	TutorialSelect.VertexWk[2].rhw = 1.0f;
	TutorialSelect.VertexWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	Tutorial_VertexWk[0].diffuse = WHITE(255);
	Tutorial_VertexWk[1].diffuse = WHITE(255);
	Tutorial_VertexWk[2].diffuse = WHITE(255);
	Tutorial_VertexWk[3].diffuse = WHITE(255);

	BlackScreenVertexWk[0].diffuse = WHITE(200);
	BlackScreenVertexWk[1].diffuse = WHITE(200);
	BlackScreenVertexWk[2].diffuse = WHITE(200);
	BlackScreenVertexWk[3].diffuse = WHITE(200);

	TutorialSelect.VertexWk[0].diffuse = WHITE(255);
	TutorialSelect.VertexWk[1].diffuse = WHITE(255);
	TutorialSelect.VertexWk[2].diffuse = WHITE(255);
	TutorialSelect.VertexWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	SetTutorialTexture();

	return S_OK;
}

//=============================================================================
// �e�N�X�`�����W�̐ݒ�
//=============================================================================
void SetTutorialTexture(void)
{
	// �������
	Tutorial_VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	Tutorial_VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	Tutorial_VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	Tutorial_VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// Pause�w�i
	BlackScreenVertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	BlackScreenVertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	BlackScreenVertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	BlackScreenVertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// �I�����w�i
	TutorialSelect.VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	TutorialSelect.VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	TutorialSelect.VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	TutorialSelect.VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetTutorialVertex(void)
{
	float Width_Half = Texture_Tutorial_Width / 2;
	float Height_Half = Texture_Tutorial_Height / 2;

	Tutorial_VertexWk[0].vtx = D3DXVECTOR3(Screen_Center_X - Width_Half, Screen_Center_Y - Height_Half, 0.0f);
	Tutorial_VertexWk[1].vtx = D3DXVECTOR3(Screen_Center_X + Width_Half, Screen_Center_Y - Height_Half, 0.0f);
	Tutorial_VertexWk[2].vtx = D3DXVECTOR3(Screen_Center_X - Width_Half, Screen_Center_Y + Height_Half, 0.0f);
	Tutorial_VertexWk[3].vtx = D3DXVECTOR3(Screen_Center_X + Width_Half, Screen_Center_Y + Height_Half, 0.0f);

	BlackScreenVertexWk[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	BlackScreenVertexWk[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
	BlackScreenVertexWk[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
	BlackScreenVertexWk[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

	TutorialSelect.VertexWk[0].vtx = D3DXVECTOR3(TutorialSelect.Pos.x - Texture_SelectBox_Width / 2, (float)TutorialSelect.Pos.y, 0.0f);
	TutorialSelect.VertexWk[1].vtx = D3DXVECTOR3(TutorialSelect.Pos.x + Texture_SelectBox_Width / 2, (float)TutorialSelect.Pos.y, 0.0f);
	TutorialSelect.VertexWk[2].vtx = D3DXVECTOR3(TutorialSelect.Pos.x - Texture_SelectBox_Width / 2, (float)TutorialSelect.Pos.y + Texture_SelectBox_Height, 0.0f);
	TutorialSelect.VertexWk[3].vtx = D3DXVECTOR3(TutorialSelect.Pos.x + Texture_SelectBox_Width / 2, (float)TutorialSelect.Pos.y + Texture_SelectBox_Height, 0.0f);

	return;
}

//=============================================================================
// �����̂̃|�C���^���擾����
//=============================================================================
CUBE *GetCube(void)
{
	return &Cube;
}

//=============================================================================
// �`���[�g���A���̏�Ԃ�ݒu����
//=============================================================================
void SetTutorialState(int State)
{
	TutorialState = State;
	return;
}

//=============================================================================
// �`���[�g���A���̏�Ԃ��擾����
//=============================================================================
int GetTutorialState(void)
{
	return TutorialState;
}

//=============================================================================
// �I���̃|�C���^���擾����
//=============================================================================
SELECT *GetTutorialSelect(void)
{
	return &TutorialSelect;
}