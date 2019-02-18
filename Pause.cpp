//=============================================================================
//
// 一時停止処理 [Pause.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Pause.h"
#include "Player.h"
#include "Input.h"
#include "Transition.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// 文字表示の矩形サイズ
#define RectWidth					(500)
#define RectHeight					(150)

// 文字の座標
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
// プロトタイプ宣言
//*****************************************************************************
// 頂点情報の作成
HRESULT MakePauseVertex(void);
// テクスチャ頂点座標の設定	
void SetPauseTexture(void);
// 頂点座標の設定
void SetPauseVertex(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
// 頂点情報格納ワーク
VERTEX_2D PauseVertexWk[VERTEX_NUM];
// 一時停止選択肢
static SELECT PauseSelect;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPause(bool FirstInit)
{
	PauseSelect.Pos = D3DXVECTOR2(Screen_Center_X, Pause_ResumePos_Y);
	PauseSelect.PrePos = PauseSelect.Pos;
	PauseSelect.Phase = Pause_Resume;
	PauseSelect.InYes = false;

	if (FirstInit == true)
	{
		// 頂点情報の作成
		MakePauseVertex();
	}
	else
	{
		// 頂点座標更新
		SetPauseVertex();
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPause(void)
{

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePause(void)
{
	static int PressCount = 0;
	PLAYER *Player = GetPlayer();
	RECT SelectBox = {
	(LONG)PauseSelect.VertexWk[0].vtx.x, (LONG)PauseSelect.VertexWk[0].vtx.y,
	(LONG)PauseSelect.VertexWk[3].vtx.x, (LONG)PauseSelect.VertexWk[3].vtx.y };

	// 選択肢移動効果音
	if (PauseSelect.Pos.x != PauseSelect.PrePos.x || PauseSelect.Pos.y != PauseSelect.PrePos.y)
	{
		SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
	}
	PauseSelect.PrePos = PauseSelect.Pos;

	// 一時停止解除
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

	// Enterキー
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

		// 選択肢移動効果音を流さないため
		PauseSelect.PrePos = PauseSelect.Pos;
	}

	// 選択肢移動
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

	// 選択肢ループ
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

	// プレスカウント初期化
	if (GetKeyboardRelease(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
		GetKeyboardRelease(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
	{
		PressCount = 0;
	}

	// 確認画面
	if (PauseSelect.Phase == Pause_RestartCheck || PauseSelect.Phase == Pause_ToTitleCheck)
	{
		// 前の状態に戻る
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

	// 頂点座標更新
	SetPauseVertex();

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPause(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 PauseTexture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 PauseSelectTexture = GetUITexture(SelectBox);
	LPD3DXFONT Font_108 = GetFont(FontSize_108);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	RECT TextRect = { 0, 0, Screen_Width, RectHeight };

	// 頂点フォーマットの設定
	Device->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	Device->SetTexture(0, PauseTexture);

	// ポリゴンの描画
	Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, PauseVertexWk, sizeof(VERTEX_2D));

	// テクスチャの設定
	Device->SetTexture(0, PauseSelectTexture);

	// ポリゴンの描画
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
			Font_72->DrawText(NULL, "続く", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "続く", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
			Font_72->DrawText(NULL, "最初から", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "最初から", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
			Font_72->DrawText(NULL, "タイトルへ", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "タイトルへ", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
	}
	else
	{
		TextRect.top = AskSentencePos_Y;
		TextRect.bottom = AskSentencePos_Y + RectHeight;
		if (PauseSelect.Phase == Pause_RestartCheck)
		{
			Font_72->DrawText(NULL, "よろしいですか？", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
		else if (PauseSelect.Phase == Pause_ToTitleCheck)
		{
			Font_72->DrawText(NULL, "タイトルに戻りますか？", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
			Font_72->DrawText(NULL, "はい", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "はい", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
			Font_72->DrawText(NULL, "いいえ", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
		}
		else
		{
			Font_72->DrawText(NULL, "いいえ", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
	}

	return;
}


//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakePauseVertex(void)
{
	// 頂点座標の設定	
	SetPauseTexture();

	// rhwの設定
	PauseVertexWk[0].rhw = 1.0f;
	PauseVertexWk[1].rhw = 1.0f;
	PauseVertexWk[2].rhw = 1.0f;
	PauseVertexWk[3].rhw = 1.0f;

	PauseSelect.VertexWk[0].rhw = 1.0f;
	PauseSelect.VertexWk[1].rhw = 1.0f;
	PauseSelect.VertexWk[2].rhw = 1.0f;
	PauseSelect.VertexWk[3].rhw = 1.0f;

	// 反射光の設定
	PauseVertexWk[0].diffuse = WHITE(200);
	PauseVertexWk[1].diffuse = WHITE(200);
	PauseVertexWk[2].diffuse = WHITE(200);
	PauseVertexWk[3].diffuse = WHITE(200);

	PauseSelect.VertexWk[0].diffuse = WHITE(255);
	PauseSelect.VertexWk[1].diffuse = WHITE(255);
	PauseSelect.VertexWk[2].diffuse = WHITE(255);
	PauseSelect.VertexWk[3].diffuse = WHITE(255);

	// テクスチャ座標の設定
	SetPauseVertex();

	return S_OK;
}

//=============================================================================
// テクスチャ座標の設定
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
// 頂点座標の設定
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
// 選択のポインタを取得する
//=============================================================================
SELECT *GetPauseSelect(void)
{
	return &PauseSelect;
}