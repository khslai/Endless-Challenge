//=============================================================================
//
// チュートリアル処理 [Tutorial.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
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
// マクロ定義
//*****************************************************************************
#define Texture_Tutorial_Key		_T("data/Texture/Tutorial_Keyboard.png")
#define Texture_Tutorial_Pad		_T("data/Texture/Tutorial_GamePad.png")
#define Texture_Tutorial_Width		(1024)
#define Texture_Tutorial_Height		(768)

// 立方体回転速度
#define RotationSpeed				(0.03f)

// 読み込むモデル名
#define	Model_Cube					"data/Model/Cube.x"	
// 立方体の耐久性 
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

// チュートリアル文字の座標
#define InformationPos_Y			(600)

enum CompletePhase
{
	NoComplete,
	ShowCompleteText,
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeTutorialVertex(void);
void SetTutorialTexture(void);
void SetTutorialVertex(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
// テクスチャへのポインタ
LPDIRECT3DTEXTURE9	Tutorial_Texture_Key = NULL;
LPDIRECT3DTEXTURE9	Tutorial_Texture_Pad = NULL;
// 頂点情報格納ワーク
VERTEX_2D			Tutorial_VertexWk[VERTEX_NUM];
VERTEX_2D			BlackScreenVertexWk[VERTEX_NUM];
// 選択肢
static SELECT		TutorialSelect;
// チュートリアルの状態
int					TutorialState = 0;
// 謎の立方体
CUBE				Cube;
// チュートリアルの指示を達成する
bool				Complete = false;
// チュートリアル文字消失フラグ
bool				TextDisappear = false;
// 魔法陣エフェクトを設置するエフェクト
bool				SetMagicCircle = false;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int GameStage = GetGameStage();

	TutorialState = Tutorial_Attack;
	Complete = false;
	TextDisappear = false;
	SetMagicCircle = false;

	// 位置、選択状態の初期設定
	TutorialSelect.Pos = D3DXVECTOR2(Screen_Center_X, Tutorial_ResumePos_Y);
	TutorialSelect.Phase = Tutorial_Resume;
	TutorialSelect.InYes = false;

	// 位置・回転・スケールの初期設定
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

	// 初めて初期化
	if (FirstInit == true)
	{
		Cube.Texture = NULL;
		Cube.MaterialBuffer = NULL;
		Cube.Mesh = NULL;
		Cube.MaterialNum = 0;

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(Model_Cube,	// 読み込むモデルファイル名(Xファイル)
			D3DXMESH_SYSTEMMEM,						// メッシュの作成オプションを指定
			Device,									// IDirect3DDevice9インターフェイスへのポインタ
			NULL,									// 隣接性データを含むバッファへのポインタ
			&Cube.MaterialBuffer,					// マテリアルデータを含むバッファへのポインタ
			NULL,									// エフェクトインスタンスの配列を含むバッファへのポインタ
			&Cube.MaterialNum,						// D3DXMATERIAL構造体の数
			&Cube.Mesh)))							// ID3DXMeshインターフェイスへのポインタのアドレス
		{
			return E_FAIL;
		}

		// モデル用のテクスチャのメモリ空間を配る
		Cube.Texture = (LPDIRECT3DTEXTURE9*)calloc(Cube.MaterialNum, sizeof(LPDIRECT3DTEXTURE9));
		if (Cube.Texture == NULL)
		{
			MessageBox(0, "Alloc Cube Texture Memory Failed！", "Error", 0);
			return E_FAIL;
		}

		// モデルテクスチャを読み込む
		if (FAILED(SafeLoadModelTexture(Cube.Texture, Cube.MaterialBuffer, Cube.MaterialNum, "Cube")))
		{
			return E_FAIL;
		}

		// 当たり判定カプセルを生成
		if (FAILED(
			CreateCapsule(&Cube.HitCapsule, PositionZero, D3DXVECTOR3(0.0f, 0.0f, 1.0f), 20.0f, 40.0f, true)))
		{
			return E_FAIL;
		}

		// 頂点情報の作成
		if (FAILED(MakeTutorialVertex()))
		{
			return E_FAIL;
		}

		// テクスチャの読み込み
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
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	// テクスチャの開放
	SafeRelease(Tutorial_Texture_Key);
	SafeRelease(Tutorial_Texture_Pad);

	return;
}

//=============================================================================
// 更新処理
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

	// 操作説明表示
	if (TutorialState == DisplayHelp)
	{
		if (IsMouseLeftTriggered() || GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_B))
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			TutorialState = PreState;
		}
	}
	// 一時停止
	else if (TutorialState == TutorialPause)
	{
		// 選択肢移動効果音
		if (TutorialSelect.Pos.x != TutorialSelect.PrePos.x || TutorialSelect.Pos.y != TutorialSelect.PrePos.y)
		{
			SetSound(NormalSE, SE_SelectMove, E_DS8_FLAG_NONE, true);
		}
		TutorialSelect.PrePos = TutorialSelect.Pos;

		// ゲームに戻る
		if (TutorialSelect.Phase != Tutorial_ToTitleCheck && 
			(GetKeyboardTrigger(DIK_P) || IsMouseRightTriggered() ||
			IsButtonTriggered(0, BUTTON_START) || IsButtonTriggered(0, BUTTON_A)))
		{
			SetSound(NormalSE, SE_Determine_Yes, E_DS8_FLAG_NONE, true);
			TutorialState = PreState;
			while (ShowCursor(false) >= 0);
		}

		// Enterキー
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

			// 選択肢移動効果音を流さないため
			TutorialSelect.PrePos = TutorialSelect.Pos;
		}

		// 選択肢移動
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

		// 選択肢ループ
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

		// プレスカウント初期化
		if (GetKeyboardRelease(DIK_UP) || GetKeyboardTrigger(DIK_W) || IsButtonReleased(0, BUTTON_UP) ||
			GetKeyboardRelease(DIK_DOWN) || GetKeyboardTrigger(DIK_S) || IsButtonReleased(0, BUTTON_DOWN))
		{
			PressCount = 0;
		}

		// 確認画面
		if (TutorialSelect.Phase == Tutorial_ToTitleCheck)
		{
			// 前の状態に戻る
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

		// 頂点座標更新
		SetTutorialVertex();
	}
	// チュートリアル開始
	else
	{
		// 操作説明表示
		if (GetKeyboardTrigger(DIK_H) || IsButtonTriggered(0, BUTTON_SELECT))
		{
			SetSound(NormalSE, SE_ShowTutorial, E_DS8_FLAG_NONE, true);
			PreState = TutorialState;
			TutorialState = DisplayHelp;
		}

		// 一時停止
		if (GetKeyboardTrigger(DIK_P) || IsButtonTriggered(0, BUTTON_START))
		{
			SetSound(NormalSE, SE_Menu, E_DS8_FLAG_NONE, true);
			PreState = TutorialState;
			TutorialState = TutorialPause;
			while (ShowCursor(true) < 0);
		}

		// 魔法陣エフェクトを設置する
		if (SetMagicCircle == false)
		{
			SetMagicCircle = true;
			D3DXVECTOR3 CirclePos = D3DXVECTOR3(Cube.Pos.x, 5.0f, Cube.Pos.z);
			SetEffect(CirclePos, MagicCircle);
		}

		// 立方体回転
		Angle += RotationSpeed;
		Cube.Rot = D3DXVECTOR3(Angle, Angle * 0.8f, Angle * 0.5f);

		// 文字消失しました
		if (TextDisappear == true)
		{
			NextStateCount++;
			// 二秒経過、次の段階に入る
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
					// チュートリアルクリア
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

		// チュートリアル状態
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
// 描画処理
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
	// チュートリアル文字
	//=================
	Font_54->DrawText(NULL, "Boss(?) HP", -1, &BossHP, DT_LEFT | DT_BOTTOM, WHITE(255));

	if (TutorialState != DisplayHelp)
	{
		rect.top = BossHPTextPos_Y;
		rect.bottom = BossHPTextPos_Y + RectHeight;
		if (GamePad->Use == true)
		{
			Font_54->DrawText(NULL, "操作説明(BACK)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
		}
		else
		{
			Font_54->DrawText(NULL, "操作説明(H)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
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
					Font_72->DrawText(NULL, "攻撃(RBボタン)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "攻撃(左クリック)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_WaitRolling:
			case Tutorial_RollingClear:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "攻撃が来る、早く回避(Aボタン)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "攻撃が来る、早く回避(Spaceキー)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_HPRestore:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "少しやけどがある、回復しよう(Xボタン)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "少しやけどがある、回復しよう(Fキー)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_CameraReset:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "カメラリセット(LBボタン)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "カメラリセット(Rキー)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case Tutorial_CameraLockOn:
				if (GamePad->Use == true)
				{
					Font_72->DrawText(NULL, "ターゲットをロックオン(R3ボタン)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				else
				{
					Font_72->DrawText(NULL, "ターゲットをロックオン(Tキー)", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				}
				break;
			case HelpOver:
				Font_72->DrawText(NULL, "封印された立方体を壊そう！", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
				break;
			default:
				break;
			}
		}
		else
		{
			rect.top = InformationPos_Y;
			rect.bottom = InformationPos_Y + RectHeight;
			Font_72->DrawText(NULL, "達 成", -1, &rect, DT_CENTER | DT_VCENTER, GREEN(255));
		}
	}

	//=================
	// 操作説明
	//=================
	if (TutorialState == DisplayHelp)
	{
		// 頂点フォーマットの設定
		Device->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		if (GamePad->Use == true)
		{
			Device->SetTexture(0, Tutorial_Texture_Pad);
		}
		else
		{
			Device->SetTexture(0, Tutorial_Texture_Key);
		}

		// ポリゴンの描画
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, Tutorial_VertexWk, sizeof(VERTEX_2D));
	}
	//=================
	// 一時停止画面
	//=================
	else if (TutorialState == TutorialPause)
	{
		// 頂点フォーマットの設定
		Device->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		Device->SetTexture(0, BlackScreen_Texture);

		// ポリゴンの描画
		Device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, POLYGON_NUM, BlackScreenVertexWk, sizeof(VERTEX_2D));

		// テクスチャの設定
		Device->SetTexture(0, SelectBox_Texture);

		// ポリゴンの描画
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
				Font_72->DrawText(NULL, "続く", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "続く", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
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
				Font_72->DrawText(NULL, "タイトルへ", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "タイトルへ", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
		else
		{
			rect.top = AskSentencePos_Y;
			rect.bottom = AskSentencePos_Y + RectHeight;
			if (TutorialSelect.Phase == Tutorial_ToTitleCheck)
			{
				Font_72->DrawText(NULL, "タイトルに戻りますか？", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
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
				Font_72->DrawText(NULL, "はい", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "はい", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
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
				Font_72->DrawText(NULL, "いいえ", -1, &rect, DT_CENTER | DT_VCENTER, BLUE(255));
			}
			else
			{
				Font_72->DrawText(NULL, "いいえ", -1, &rect, DT_CENTER | DT_VCENTER, WHITE(255));
			}
		}
	}

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorialCube(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX ScaleMatrix, RotMatrix, TransMatrix;
	D3DXMATERIAL *pD3DXMat;

	//=================
	// 立方体
	//=================
	if (Cube.Exist == true)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&Cube.WorldMatrix);

		// スケールを反映
		D3DXMatrixScaling(&ScaleMatrix, Cube.Scale.x, Cube.Scale.y, Cube.Scale.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &ScaleMatrix);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&RotMatrix, Cube.Rot.y, Cube.Rot.x, Cube.Rot.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &RotMatrix);

		// 移動を反映
		D3DXMatrixTranslation(&TransMatrix, Cube.Pos.x, Cube.Pos.y, Cube.Pos.z);
		D3DXMatrixMultiply(&Cube.WorldMatrix, &Cube.WorldMatrix, &TransMatrix);

		// ワールドマトリックスの設定
		Device->SetTransform(D3DTS_WORLD, &Cube.WorldMatrix);

		// マテリアル情報に対するポインタを取得
		pD3DXMat = (D3DXMATERIAL*)Cube.MaterialBuffer->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)Cube.MaterialNum; nCntMat++)
		{
			// マテリアルの設定
			Device->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// テクスチャの設定
			Device->SetTexture(0, Cube.Texture[nCntMat]);

			// 描画
			Cube.Mesh->DrawSubset(nCntMat);
		}

		// 立方体のカプセルを描画する
		DrawCapsule(&Cube.HitCapsule, &Cube.WorldMatrix);
	}

	return;
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeTutorialVertex(void)
{
	// 頂点座標の設定	
	SetTutorialVertex();

	// rhwの設定
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

	// 反射光の設定
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

	// テクスチャ座標の設定
	SetTutorialTexture();

	return S_OK;
}

//=============================================================================
// テクスチャ座標の設定
//=============================================================================
void SetTutorialTexture(void)
{
	// 操作説明
	Tutorial_VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	Tutorial_VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	Tutorial_VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	Tutorial_VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// Pause背景
	BlackScreenVertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	BlackScreenVertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	BlackScreenVertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	BlackScreenVertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 選択肢背景
	TutorialSelect.VertexWk[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	TutorialSelect.VertexWk[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	TutorialSelect.VertexWk[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	TutorialSelect.VertexWk[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	return;
}

//=============================================================================
// 頂点座標の設定
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
// 立方体のポインタを取得する
//=============================================================================
CUBE *GetCube(void)
{
	return &Cube;
}

//=============================================================================
// チュートリアルの状態を設置する
//=============================================================================
void SetTutorialState(int State)
{
	TutorialState = State;
	return;
}

//=============================================================================
// チュートリアルの状態を取得する
//=============================================================================
int GetTutorialState(void)
{
	return TutorialState;
}

//=============================================================================
// 選択のポインタを取得する
//=============================================================================
SELECT *GetTutorialSelect(void)
{
	return &TutorialSelect;
}