//=============================================================================
//
// �Q�[�W���� [Gauge.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Gauge.h"
#include "Player.h"
#include "Boss.h"
#include "Effect.h"
#include "Tutorial.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define Texture_Gauge				_T("data/Texture/Gauge.png")		// �e�N�X�`���̃p�X
#define Texture_GaugeBox			_T("data/Texture/GaugeBox.png")	
#define Texture_HPPotion			_T("data/Texture/HPPotion.png")	

// HP�AST�����炷���x
#define DecreaseSpeed				(1.0f)

// �����\���̋�`�T�C�Y
#define RectWidth					(500)
#define RectHeight					(150)

// �v���C���[��HP�Q�[�W�̃T�C�Y
#define PlayerHPGauge_Width			(700)
#define PlayerHPGauge_Height		(30)
// �v���C���[��HP�Q�[�W�̍��W
#define PlayerHPGauge_Pos_X			(100)
#define PlayerHPGauge_Pos_Y			(60)

// �v���C���[�̃X�^�~�i�Q�[�W�̃T�C�Y
#define PlayerStaminaGauge_Width	(700)
#define PlayerStaminaGauge_Height	(30)
// �v���C���[�̃X�^�~�i�Q�[�W�̍��W
#define PlayerStaminaGauge_Pos_X	(100)
#define PlayerStaminaGauge_Pos_Y	(100)

// �{�X��HP�Q�[�W�̃T�C�Y
#define BossHPGauge_Width			(1536)
#define BossHPGauge_Height			(30)
// �{�X��HP�Q�[�W�̍��W
#define BossHPGauge_Pos_X			(192)
#define BossHPGauge_Pos_Y			(950)

// HP�|�[�V�����̃T�C�Y
#define HPPotion_Size				(80)	
// HP�|�[�V�����̍��W
#define HPPotion_Pos_X				(100)
#define HPPotion_Pos_Y				(150)

// �Q�[�W�̎��
enum
{
	PlayerHP,
	PlayerHPBox,
	PlayerPreHP,
	PlayerStamina,
	PlayerStaminaBox,
	PreviousStamina,
	BossHP,
	BossHPBox,
	BossPreHP,
};

// �Q�[�W�̐F
enum
{
	Green,
	Green_AlphaHalf,
	Red,
	Red_AlphaHalf,
	DarkRed,
	DarkRed_AlphaHalf,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ���_���̍쐬
HRESULT MakeGaugeVertex(GAUGE *GaugePtr);
HRESULT MakeHPPotionVertex(void);
// ���_���W�̐ݒ�
void SetGaugeVertex(GAUGE *GaugePtr);
// �Q�[�W�F�̐ݒ�
void SetGaugeDiffuse(GAUGE *GaugePtr, int Color);
// UI�e�L�X�g�`��
void DrawUIText(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
LPDIRECT3DTEXTURE9 GaugeBoxTexture = NULL;
LPDIRECT3DTEXTURE9 GaugeTexture = NULL;
LPDIRECT3DTEXTURE9 HPPotionTexture = NULL;
GAUGE PlayerHPGauge;
GAUGE PlayerHPGaugeBox;
GAUGE PlayerPreHPGauge;
GAUGE PlayerStaminaGauge;
GAUGE PlayerStaminaGaugeBox;
GAUGE PreviousStaminaGauge;
GAUGE BossHPGauge;
GAUGE BossHPGaugeBox;
GAUGE BossPreHPGauge;
VERTEX_2D	HPPotionVtxWk[VERTEX_NUM];		// ���_���i�[���[�N

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGauge(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	PlayerHPGauge.Type = PlayerHP;
	PlayerHPGaugeBox.Type = PlayerHPBox;
	PlayerPreHPGauge.Type = PlayerPreHP;
	PlayerStaminaGauge.Type = PlayerStamina;
	PlayerStaminaGaugeBox.Type = PlayerStaminaBox;
	PreviousStaminaGauge.Type = PreviousStamina;
	BossHPGauge.Type = BossHP;
	BossHPGaugeBox.Type = BossHPBox;
	BossPreHPGauge.Type = BossPreHP;

	// ���߂ď�����
	if (FirstInit == true)
	{
		// ���_���̍쐬
		MakeGaugeVertex(&PlayerHPGauge);
		MakeGaugeVertex(&PlayerHPGaugeBox);
		MakeGaugeVertex(&PlayerPreHPGauge);
		MakeGaugeVertex(&PlayerStaminaGauge);
		MakeGaugeVertex(&PlayerStaminaGaugeBox);
		MakeGaugeVertex(&PreviousStaminaGauge);
		MakeGaugeVertex(&BossHPGauge);
		MakeGaugeVertex(&BossHPGaugeBox);
		MakeGaugeVertex(&BossPreHPGauge);
		MakeHPPotionVertex();

		// �e�N�X�`���̓ǂݍ���
		if (FAILED(SafeLoadTexture(Texture_Gauge, &GaugeTexture, "Gauge")))
		{
			return E_FAIL;
		}
		if (FAILED(SafeLoadTexture(Texture_GaugeBox, &GaugeBoxTexture, "Gauge")))
		{
			return E_FAIL;
		}
		if (FAILED(SafeLoadTexture(Texture_HPPotion, &HPPotionTexture, "HP Potion")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGauge(void)
{
	// �e�N�X�`���̊J��
	SafeRelease(GaugeTexture);
	SafeRelease(GaugeBoxTexture);
	SafeRelease(HPPotionTexture);

	return;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGauge(void)
{
	int GameCount = GetGameCount();
	BOSS *Boss = GetBoss();

	// �v���C���[HP�Q�[�W
	SetGaugeDiffuse(&PlayerHPGauge, Red);
	SetGaugeVertex(&PlayerHPGauge);

	SetGaugeDiffuse(&PlayerPreHPGauge, Red_AlphaHalf);
	SetGaugeVertex(&PlayerPreHPGauge);

	// �v���C���[�X�^�~�i�Q�[�W
	SetGaugeDiffuse(&PlayerStaminaGauge, Green);
	SetGaugeVertex(&PlayerStaminaGauge);

	SetGaugeDiffuse(&PreviousStaminaGauge, Green_AlphaHalf);
	SetGaugeVertex(&PreviousStaminaGauge);

	// �{�XHP�Q�[�W
	SetGaugeDiffuse(&BossHPGauge, DarkRed);
	SetGaugeVertex(&BossHPGauge);

	SetGaugeDiffuse(&BossPreHPGauge, DarkRed_AlphaHalf);
	SetGaugeVertex(&BossPreHPGauge);

	return;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGauge(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	DrawUIText();

	Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	Device->SetFVF(FVF_VERTEX_2D);

	// �v���C���[HP
	Device->SetTexture(0, GaugeBoxTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PlayerHPGaugeBox.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PlayerHPGauge.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PlayerPreHPGauge.VertexWk, sizeof(VERTEX_2D));

	// �v���C���[�X�^�~�i
	Device->SetTexture(0, GaugeBoxTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PlayerStaminaGaugeBox.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PlayerStaminaGauge.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PreviousStaminaGauge.VertexWk, sizeof(VERTEX_2D));

	// �{�XHP
	Device->SetTexture(0, GaugeBoxTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BossHPGaugeBox.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BossHPGauge.VertexWk, sizeof(VERTEX_2D));

	Device->SetTexture(0, GaugeTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BossPreHPGauge.VertexWk, sizeof(VERTEX_2D));

	// HP�|�[�V����
	Device->SetTexture(0, HPPotionTexture);
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, HPPotionVtxWk, sizeof(VERTEX_2D));

	Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	return;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetGaugeVertex(GAUGE *GaugePtr)
{
	int GameStage = GetGameStage();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CUBE *Cube = GetCube();
	float PlayerHPPercent = 0.0f;
	float PlayerPreHPPercent = 0.0f;
	float PlayerStaminaPercent = 0.0f;
	float PreStaminaPercent = 0.0f;
	float BossHPPercent = 0.0f;
	float BossPreHPPercent = 0.0f;

	switch (GaugePtr->Type)
	{
	case PlayerHP:

		// ���݃v���C���[��HP�̊���
		PlayerHPPercent = Player->HP / Player->HP_Max;
		if (PlayerHPPercent <= 0.0f && Player->Animation->CurrentAnimID != PlayerDeath)
		{
			PlayerHPPercent = 0.0f;
			ChangeAnimation(Player->Animation, PlayerDeath, 1.0f, false);
		}

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width * PlayerHPPercent, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width * PlayerHPPercent, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		break;

	case PlayerHPBox:

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		break;

	case PlayerPreHP:

		PlayerHPPercent = Player->HP / Player->HP_Max;
		PlayerPreHPPercent = Player->PreviousHP / Player->HP_Max;

		if (Player->HPDecreaseStart == true)
		{
			if (PlayerPreHPPercent > PlayerHPPercent)
			{
				Player->PreviousHP -= DecreaseSpeed;
			}
			else
			{
				Player->PreviousHP = Player->HP;
				Player->HPDecreaseStart = false;
			}
		}

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width * PlayerPreHPPercent, PlayerHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerHPGauge_Pos_X + PlayerHPGauge_Width * PlayerPreHPPercent, PlayerHPGauge_Pos_Y + PlayerHPGauge_Height, 0);
		break;

	case PlayerStamina:

		// ���݃v���C���[�̃X�^�~�i�̊���
		PlayerStaminaPercent = Player->Stamina / Player->Stamina_Max;
		if (PlayerStaminaPercent <= 0.0f)
		{
			PlayerStaminaPercent = 0.0f;
		}

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width * PlayerStaminaPercent, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width * PlayerStaminaPercent, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		break;

	case PlayerStaminaBox:

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		break;

	case PreviousStamina:

		PlayerStaminaPercent = Player->Stamina / Player->Stamina_Max;
		PreStaminaPercent = Player->PreviousST / Player->Stamina_Max;

		if (Player->STDecreaseStart == true)
		{
			if (PreStaminaPercent > PlayerStaminaPercent)
			{
				Player->PreviousST -= DecreaseSpeed;
			}
			else
			{
				Player->PreviousST = Player->Stamina;
				Player->STDecreaseStart = false;
			}
		}

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width * PreStaminaPercent, PlayerStaminaGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(PlayerStaminaGauge_Pos_X + PlayerStaminaGauge_Width * PreStaminaPercent, PlayerStaminaGauge_Pos_Y + PlayerStaminaGauge_Height, 0);
		break;


	case BossHP:

		// ���݃{�XHP�̊���
		if (GameStage != Stage_Tutorial)
		{
			BossHPPercent = Boss->HP / Boss->HP_Max;
			if (BossHPPercent <= 0.5f && Boss->Phase == Phase1 && Boss->Animation->MotionEnd == true)
			{
				Boss->Phase = TurnToPhase2;
			}
			else if (BossHPPercent <= 0.0f)
			{
				BossHPPercent = 0.0f;
			}
		}
		else
		{
			BossHPPercent = Cube->Durability / Cube->MaxDurability;
			if (BossHPPercent <= 0.0f)
			{
				BossHPPercent = 0.0f;
			}
		}

		GaugePtr->VertexWk[0].vtx = D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx = D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width * BossHPPercent, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx = D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx = D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width * BossHPPercent, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		break;

	case BossHPBox:

		GaugePtr->VertexWk[0].vtx = D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx = D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx = D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx = D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		break;

	case BossPreHP:

		if (GameStage != Stage_Tutorial)
		{
			BossHPPercent = Boss->HP / Boss->HP_Max;
			BossPreHPPercent = Boss->PreviousHP / Boss->HP_Max;

			if (Boss->HPDecreaseStart == true)
			{
				if (BossPreHPPercent > BossHPPercent)
				{
					Boss->PreviousHP -= DecreaseSpeed;
				}
				else
				{
					Boss->PreviousHP = Boss->HP;
					Boss->HPDecreaseStart = false;
				}
			}
		}
		else
		{
			BossPreHPPercent = Cube->Durability / Cube->MaxDurability;
		}

		GaugePtr->VertexWk[0].vtx =
			D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[1].vtx =
			D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width * BossPreHPPercent, BossHPGauge_Pos_Y, 0);
		GaugePtr->VertexWk[2].vtx =
			D3DXVECTOR3(BossHPGauge_Pos_X, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		GaugePtr->VertexWk[3].vtx =
			D3DXVECTOR3(BossHPGauge_Pos_X + BossHPGauge_Width * BossPreHPPercent, BossHPGauge_Pos_Y + BossHPGauge_Height, 0);
		break;

	default:
		break;
	}

	return;
}

//=============================================================================
// �Q�[���e�L�X�g����`�悷��
//=============================================================================
void DrawUIText(void)
{
	LPD3DXFONT Font_54 = GetFont(FontSize_54);
	PLAYER *Player = GetPlayer();
	int GameStage = GetGameStage();
	RECT BossHP = { BossHPGauge_Pos_X, BossHPGauge_Pos_Y - RectHeight , BossHPGauge_Pos_X + RectWidth, BossHPGauge_Pos_Y };
	RECT HPPotionNum = { HPPotion_Pos_X, HPPotion_Pos_Y, HPPotion_Pos_X + 300, HPPotion_Pos_Y + HPPotion_Size };
	char Text[16];

	sprintf_s(Text, 16, "x %d", Player->HPPotionNum);
	Font_54->DrawText(NULL, Text, -1, &HPPotionNum, DT_CENTER | DT_VCENTER, BLACK(255));

	if (GameStage == Stage_Game || GameStage == Stage_Pause || GameStage == Stage_GameOver)
	{
		Font_54->DrawText(NULL, "Boss HP", -1, &BossHP, DT_LEFT | DT_BOTTOM, WHITE(255));
	}

	return;
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeGaugeVertex(GAUGE *GaugePtr)
{
	// ���_���W�̐ݒ�	
	SetGaugeVertex(GaugePtr);

	// rhw�̐ݒ�
	GaugePtr->VertexWk[0].rhw = 1.0f;
	GaugePtr->VertexWk[1].rhw = 1.0f;
	GaugePtr->VertexWk[2].rhw = 1.0f;
	GaugePtr->VertexWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	GaugePtr->VertexWk[0].diffuse = WHITE(255);
	GaugePtr->VertexWk[1].diffuse = WHITE(255);
	GaugePtr->VertexWk[2].diffuse = WHITE(255);
	GaugePtr->VertexWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	GaugePtr->VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	GaugePtr->VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	GaugePtr->VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	GaugePtr->VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeHPPotionVertex(void)
{
	// ���_���W�̐ݒ�	
	HPPotionVtxWk[0].vtx = D3DXVECTOR3(HPPotion_Pos_X, HPPotion_Pos_Y, 0.0f);
	HPPotionVtxWk[1].vtx = D3DXVECTOR3(HPPotion_Pos_X + HPPotion_Size, HPPotion_Pos_Y, 0.0f);
	HPPotionVtxWk[2].vtx = D3DXVECTOR3(HPPotion_Pos_X, HPPotion_Pos_Y + HPPotion_Size, 0.0f);
	HPPotionVtxWk[3].vtx = D3DXVECTOR3(HPPotion_Pos_X + HPPotion_Size, HPPotion_Pos_Y + HPPotion_Size, 0.0f);

	// rhw�̐ݒ�
	HPPotionVtxWk[0].rhw = 1.0f;
	HPPotionVtxWk[1].rhw = 1.0f;
	HPPotionVtxWk[2].rhw = 1.0f;
	HPPotionVtxWk[3].rhw = 1.0f;

	// ���ˌ��̐ݒ�
	HPPotionVtxWk[0].diffuse = WHITE(255);
	HPPotionVtxWk[1].diffuse = WHITE(255);
	HPPotionVtxWk[2].diffuse = WHITE(255);
	HPPotionVtxWk[3].diffuse = WHITE(255);

	// �e�N�X�`�����W�̐ݒ�
	HPPotionVtxWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	HPPotionVtxWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	HPPotionVtxWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	HPPotionVtxWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// �F�A�����x�̐ݒ�
//=============================================================================
void SetGaugeDiffuse(GAUGE *GaugePtr, int Color)
{
	switch (Color)
	{
	case Green:
		GaugePtr->VertexWk[0].diffuse = GREEN(255);
		GaugePtr->VertexWk[1].diffuse = GREEN(255);
		GaugePtr->VertexWk[2].diffuse = GREEN(255);
		GaugePtr->VertexWk[3].diffuse = GREEN(255);
		break;
	case Green_AlphaHalf:
		GaugePtr->VertexWk[0].diffuse = GREEN(128);
		GaugePtr->VertexWk[1].diffuse = GREEN(128);
		GaugePtr->VertexWk[2].diffuse = GREEN(128);
		GaugePtr->VertexWk[3].diffuse = GREEN(128);
		break;
	case Red:
		GaugePtr->VertexWk[0].diffuse = RED(255);
		GaugePtr->VertexWk[1].diffuse = RED(255);
		GaugePtr->VertexWk[2].diffuse = RED(255);
		GaugePtr->VertexWk[3].diffuse = RED(255);
		break;
	case Red_AlphaHalf:
		GaugePtr->VertexWk[0].diffuse = RED(128);
		GaugePtr->VertexWk[1].diffuse = RED(128);
		GaugePtr->VertexWk[2].diffuse = RED(128);
		GaugePtr->VertexWk[3].diffuse = RED(128);
		break;
	case DarkRed:
		GaugePtr->VertexWk[0].diffuse = DARKRED(255);
		GaugePtr->VertexWk[1].diffuse = DARKRED(255);
		GaugePtr->VertexWk[2].diffuse = DARKRED(255);
		GaugePtr->VertexWk[3].diffuse = DARKRED(255);
		break;
	case DarkRed_AlphaHalf:
		GaugePtr->VertexWk[0].diffuse = DARKRED(128);
		GaugePtr->VertexWk[1].diffuse = DARKRED(128);
		GaugePtr->VertexWk[2].diffuse = DARKRED(128);
		GaugePtr->VertexWk[3].diffuse = DARKRED(128);
		break;
	default:
		break;
	}

	return;
}


