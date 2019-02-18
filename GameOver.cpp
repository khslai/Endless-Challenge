//=============================================================================
//
// ゲームオーバー処理 [GameOver.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
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
// マクロ定義
//*****************************************************************************
// 文字表示の矩形サイズ
#define RectWidth					(500)
#define RectHeight					(150)

// 文字の座標
#define OptionPos_X					(710)
#define GameOverPos_Y				(256)
#define GameOver_RestartPos_Y		(540)
#define GameOver_ToTitlePos_Y		(690)
#define AskSentencePos_Y			(200)
#define YesNoPos_Y					(700)
#define YesPos_X					(480)
#define NoPos_X						(1440)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 頂点情報の作成
HRESULT MakeGameOverVertex(void);
// テクスチャ頂点座標の設定	
void SetGameOverTexture(void);
// 頂点座標の設定
void SetGameOverVertex(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
// 頂点情報格納ワーク
static VERTEX_2D BlackScreenVertexWk[VERTEX_NUM];
// "GAME OVER"のアルファ値
static int Alpha = 0;
// ゲームオーバー選択肢
static SELECT GameOverSelect;


//=============================================================================
// 初期化処理
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
		// 頂点情報の作成
		MakeGameOverVertex();
	}
	else
	{
		// 頂点座標更新
		SetGameOverVertex();
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameOver(void)
{

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameOver(void)
{
	static int PressCount = 0;
	RECT SelectBox = {
		(LONG)GameOverSelect.VertexWk[0].vtx.x, (LONG)GameOverSelect.VertexWk[0].vtx.y,
		(LONG)GameOverSelect.VertexWk[3].vtx.x, (LONG)GameOverSelect.VertexWk[3].vtx.y };

	// Game Overの文字が表示中じゃなければ
	if (GameOverSelect.Phase != Display && GameOverSelect.Phase != BGMStart)
	{
		// 選択肢移動効果音
		if (GameOverSelect.Pos.x != GameOverSelect.PrePos.x || GameOverSelect.Pos.y != GameOverSelect.PrePos.y)
		{
			SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
		}
		GameOverSelect.PrePos = GameOverSelect.Pos;

		// Enterキー
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

			// 選択肢移動効果音を流さないため
			GameOverSelect.PrePos = GameOverSelect.Pos;
		}

		// 選択肢移動
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

		// 選択肢ループ
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

		// プレスカウント初期化
		if (GetKeyboardRelease(DIK_UP) || GetKeyboardRelease(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
			GetKeyboardRelease(DIK_DOWN) || GetKeyboardRelease(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
		{
			PressCount = 0;
		}

		// 確認画面
		if (GameOverSelect.Phase == GameOver_RestartCheck || GameOverSelect.Phase == GameOver_ToTitleCheck)
		{
			// 前の状態に戻る
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

		// 頂点座標更新
		SetGameOverVertex();
	}
	// BGMを鳴らす
	else if (GameOverSelect.Phase == BGMStart)
	{
		ReInitSound();
		GameOverSelect.Phase = Display;
	}

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameOver(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPDIRECT3DTEXTURE9 BlackScreenTexture = GetUITexture(BlackScreen);
	LPDIRECT3DTEXTURE9 GameOverSelectTexture = GetUITexture(SelectBox);
	LPD3DXFONT Font_108 = GetFont(FontSize_108);
	LPD3DXFONT Font_72 = GetFont(FontSize_72);
	RECT TextRect = { 0, 0, Screen_Width, RectHeight };

	// 頂点フォーマットの設定
	Device->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	Device->SetTexture(0, BlackScreenTexture);

	// ポリゴンの描画
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
		// テクスチャの設定
		Device->SetTexture(0, GameOverSelectTexture);
		// ポリゴンの描画
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
				Font_72->DrawText(NULL, "最初から", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "最初から", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
			TextRect.bottom = AskSentencePos_Y + 100;
			if (GameOverSelect.Phase == GameOver_RestartCheck)
			{
				Font_72->DrawText(NULL, "よろしいですか？", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
			else if (GameOverSelect.Phase == GameOver_ToTitleCheck)
			{
				Font_72->DrawText(NULL, "タイトルに戻りますか？", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
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
				GameOverSelect.InYes = false;
				GameOverSelect.Pos.x = NoPos_X;
			}
			if (GameOverSelect.InYes == false)
			{
				Font_72->DrawText(NULL, "いいえ", -1, &TextRect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "いいえ", -1, &TextRect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
	}

	return;
}


//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeGameOverVertex(void)
{
	// 頂点座標の設定	
	SetGameOverTexture();

	// rhwの設定
	BlackScreenVertexWk[0].rhw = 1.0f;
	BlackScreenVertexWk[1].rhw = 1.0f;
	BlackScreenVertexWk[2].rhw = 1.0f;
	BlackScreenVertexWk[3].rhw = 1.0f;

	GameOverSelect.VertexWk[0].rhw = 1.0f;
	GameOverSelect.VertexWk[1].rhw = 1.0f;
	GameOverSelect.VertexWk[2].rhw = 1.0f;
	GameOverSelect.VertexWk[3].rhw = 1.0f;

	// 反射光の設定
	BlackScreenVertexWk[0].diffuse = WHITE(200);
	BlackScreenVertexWk[1].diffuse = WHITE(200);
	BlackScreenVertexWk[2].diffuse = WHITE(200);
	BlackScreenVertexWk[3].diffuse = WHITE(200);

	GameOverSelect.VertexWk[0].diffuse = WHITE(255);
	GameOverSelect.VertexWk[1].diffuse = WHITE(255);
	GameOverSelect.VertexWk[2].diffuse = WHITE(255);
	GameOverSelect.VertexWk[3].diffuse = WHITE(255);

	// テクスチャ座標の設定
	SetGameOverVertex();

	return S_OK;
}

//=============================================================================
// テクスチャ座標の設定
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
// 頂点座標の設定
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
// 選択肢の状態を取得する
//=============================================================================
SELECT *GetGameOverSelect(void)
{
	return &GameOverSelect;
}