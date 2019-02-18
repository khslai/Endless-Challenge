//=============================================================================
//
// ステージ遷移処理 [Transition.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
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
// マクロ定義
//*****************************************************************************
#define	FadeRate		(3)		// フェード係数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 頂点情報の設定
HRESULT MakeVertexTransition(void);
// 色を設定
void SetColor(int Alpha);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 TransitionVtxBuffer = NULL;		// 頂点バッファインターフェースへのポインタ
VERTEX_2D				VertexWk[VERTEX_NUM];			// 頂点情報格納ワーク
int						Transition = TransitionOver;	// ステージ遷移状態

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTransition(bool FirstInit)
{
	// 初めて初期化
	if (FirstInit == true)
	{
		// 頂点情報の設定
		if (FAILED(MakeVertexTransition()))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTransition(void)
{
	// 頂点バッファの開放
	SafeRelease(TransitionVtxBuffer);

	return;
}

//=============================================================================
// 更新処理
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
		// フェード処理中
		if (Transition == Fadeout)
		{
			// フェードアウト処理
			Alpha -= FadeRate;		// α値を加算して画面を消していく

			if (Alpha <= 0)
			{
				// フェード処理終了
				Alpha = 0;
				SetTransition(TransitionOver);
			}
		}
		else if (Transition == Fadein)
		{
			// フェードイン処理
			Alpha += FadeRate;		// α値を減算して画面を浮き上がらせる

			// サウンドフェイドアウト
			SoundFadeOut(PlayingBGM_No);

			if (Alpha >= 255)
			{
				Alpha = 255;

				// アニメーション更新
				UpdateAnimation(Player->Animation, TimePerFrame);
				UpdateAnimation(Boss->Animation, TimePerFrame);

				// フェードイン処理に切り替え
				switch (GameStage)
				{
				case Stage_Title:

					ChangeAnimation(Player->Animation, Idle, 1.0f, false);
					ChangeAnimation(Boss->Animation, Idle, 1.0f, false);
					if (Player->Animation->MotionBlendOver == true &&
						Boss->Animation->MotionBlendOver == true)
					{
						SetTransition(Fadeout);
						// チュートリアル開始
						if (TitleSelect->Phase == NoSkip)
						{
							SetGameStage(Stage_Tutorial);
						}
						// ゲーム開始
						else if (TitleSelect->Phase == SkipDetermine || TitleSelect->Phase == AppealMode)
						{
							SetGameStage(Stage_Game);
						}
						ReInitialize();
					}
					break;

				case Stage_Tutorial:

					// ゲーム開始
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
					// タイトルに戻る
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

					// タイトルに戻る
					ChangeAnimation(Player->Animation, Sit, 1.0f, false);
					if (Player->Animation->MotionBlendOver == true)
					{
						SetTransition(Fadeout);
						SetGameStage(Stage_Title);
						ReInitialize();
					}
					break;

				case Stage_Pause:

					// 最初から
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
					// タイトルに戻る
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

					// 最初から
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
					// タイトルに戻る
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

		// 色を設定
		SetColor(Alpha);
	}

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTransition(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
	Device->SetStreamSource(0, TransitionVtxBuffer, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	Device->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	Device->SetTexture(0, NULL);

	// ポリゴンの描画
	Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, POLYGON_NUM);

	return;
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexTransition(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// オブジェクトの頂点バッファを生成
	if (FAILED(Device->CreateVertexBuffer(sizeof(VERTEX_2D) * VERTEX_NUM,		// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,				// 頂点バッファの使用法　
		FVF_VERTEX_2D,					// 使用する頂点フォーマット
		D3DPOOL_MANAGED,				// リソースのバッファを保持するメモリクラスを指定
		&TransitionVtxBuffer,				// 頂点バッファインターフェースへのポインタ
		NULL)))							// NULLに設定
	{
		return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		TransitionVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].vtx = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].vtx = D3DXVECTOR3(Screen_Width, 0.0f, 0.0f);
		pVtx[2].vtx = D3DXVECTOR3(0.0f, Screen_Height, 0.0f);
		pVtx[3].vtx = D3DXVECTOR3(Screen_Width, Screen_Height, 0.0f);

		// テクスチャのパースペクティブコレクト用
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 反射光の設定
		pVtx[0].diffuse = WHITE(0);
		pVtx[1].diffuse = WHITE(0);
		pVtx[2].diffuse = WHITE(0);
		pVtx[3].diffuse = WHITE(0);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		TransitionVtxBuffer->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 色を設定
//=============================================================================
void SetColor(int Alpha)
{
	VERTEX_2D *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	TransitionVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 反射光の設定
	pVtx[0].diffuse = WHITE(Alpha);
	pVtx[1].diffuse = WHITE(Alpha);
	pVtx[2].diffuse = WHITE(Alpha);
	pVtx[3].diffuse = WHITE(Alpha);

	// 頂点データをアンロックする
	TransitionVtxBuffer->Unlock();

}

//=============================================================================
// フェードの状態設定
//=============================================================================
void SetTransition(int State)
{
	Transition = State;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
int GetTransition(void)
{
	return Transition;
}

