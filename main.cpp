//=============================================================================
//
// メイン処理 [main.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "resource.h"
#include "Input.h"
#include "Light.h"
#include "Stage.h"
#include "DebugProcess.h"
#include "Player.h"
#include "AllocateHierarchy.h"
#include "D3DXAnimation.h"
#include "Title.h"
#include "Tutorial.h"
#include "SkyBox.h"
#include "ThirdPersonCamera.h"
#include "Boss.h"
#include "CapsuleMesh.h"
#include "Equipment.h"
#include "HitCheck.h"
#include "Gauge.h"
#include "Transition.h"
#include "FireBall.h"
#include "SummonSword.h"
#include "FireSword.h"
#include "Effect.h"
#include "AttackRange.h"
#include "ShadowMapping.h"
#include "Pause.h"
#include "GameOver.h"
#include "Potion.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"				// ウインドウのクラス名
#define WINDOW_NAME		"Endless Challenge"		// ウインドウのキャプション名
#define FPS				(60)					// フレイム更新率

enum ProcessStage
{
	Process_Init = 0,
	Process_Update,
	Process_Draw,
	Process_Uninitialize,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);
void ProcessStart(void);
void ProcessEnd(void);
void DrawProcessTime(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
LPDIRECT3D9			g_pD3D = NULL;				// Direct3D オブジェクト
LPDIRECT3DDEVICE9	g_pD3DDevice = NULL;		// Deviceオブジェクト(描画に必要)
LPDIRECT3DTEXTURE9	UITexture[UITexture_Max] = { NULL };	// UIテクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 LineVtxBuffer;			// 直線描画用頂点バッファ
LPD3DXFONT			Font[Font_Max] = { NULL };				// フォントへのポインタ
// FPSカウント
int					FPSCount;						// FPSカウンタ
// ゲーム遷移
int					GameStage = Stage_Title;		// ゲームステージ
// 処理段階
int					ProcessStage = Process_Init;
// 当たり判定描画
#if _DEBUG
// デバッグ表示
bool				DisplayDebugMsg = true;		// デバッグ表示ON/OFF
// カプセル表示
bool				DisplayCapsule = true;		// 当たり判定カプセル表示ON/OFF
bool				DrawMode_WireFrame = true;	// カプセルの描画モードはWireFrame表示ON/OFF
#else
bool				DisplayDebugMsg = false;		// デバッグ表示ON/OFF
bool				DisplayCapsule = false;		// 当たり判定カプセル表示ON/OFF
bool				DrawMode_WireFrame = false;	// カプセルの描画モードはWireFrame表示ON/OFF
#endif
// ゲームカウント
int					GameCount = 0;
// 処理速度計算
LARGE_INTEGER		StartTime;
double				UpdateTime = 0.0;
double				DrawTime = 0.0;

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	DWORD dwExecLastTime;	// 前回画面処理の時間
	DWORD dwFPSLastTime;	// 前回FPG計算の時間
	DWORD dwCurrentTime;	// 現在時間
	DWORD dwFrameCount;		// 一秒間何回処理する = FPS
	SELECT *TitleSelect = GetTitleSelect();

	// タスクバーを除いた領域のサイズを取得
	RECT rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);

	// ゲームアイコン
	HICON MyIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		MyIcon,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		MyIcon
	};
	HWND hWnd;
	MSG msg;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		rt.right,
		rt.bottom,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (hWnd == NULL)
	{
		return false;
	}

#if _DEBUG
	bool bWindow = true;
#else
	bool bWindow = false;
	if (MessageBox(hWnd, _T("ウィンドウモードで実行しますか？"), _T("画面モード"), MB_YESNO) == IDYES)
	{
		bWindow = true;
	}
#endif

	// 初期化処理(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, bWindow)))
	{
		return -1;
	}

	// フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime =
		dwFPSLastTime = timeGetTime();
	dwCurrentTime =
		dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);

	// メッセージループ
	while (1)
	{
		// ゲーム終了
		if (TitleSelect->Phase == ExitDetermine)
		{
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳とディスパッチ
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			// 1秒ごとに実行
			// 每一秒更新一次幀數
			if ((dwCurrentTime - dwFPSLastTime) >= 1000)
			{
				FPSCount = dwFrameCount;

				// FPSを測定した時刻を保存
				dwFPSLastTime = dwCurrentTime;

				// カウントをクリア
				dwFrameCount = 0;
			}

			// 1/60秒ごとに実行
			if ((dwCurrentTime - dwExecLastTime) >= (1000 / FPS))
			{
#ifdef _DEBUG
				PrintDebugMsg("FPS:%d\n\n", FPSCount);
				//PrintDebugMsg("GameCount:%d\n", GameCount);
#endif
				// 処理した時刻を保存
				dwExecLastTime = dwCurrentTime;

				// 更新処理
				ProcessStage = Process_Update;
				Update();

				// 描画処理
				ProcessStage = Process_Draw;
				Draw();

				// 処理回数のカウントを加算
				dwFrameCount++;

				// ゲームカウントを加算
				GameCount++;
			}
		}
	}

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	timeEndPeriod(1);				// 分解能を戻す

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
#if _DEBUG
			DestroyWindow(hWnd);
#else
			ShowCursor(true);
			if (MessageBox(hWnd, _T("本当にゲームを終了しますか？"), _T("ゲーム終了"), MB_YESNO) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			ShowCursor(false);
#endif
			break;
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	// ランダムの初期化
	srand((unsigned)time(NULL));

	// Direct3Dオブジェクトの作成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));				// ワークをゼロクリア
	d3dpp.BackBufferCount = 1;						// バックバッファの数
	d3dpp.BackBufferWidth = Screen_Width;			// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = Screen_Height;			// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;			// カラーモードの指定
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;		// 映像信号に同期してフリップする
	d3dpp.Windowed = bWindow;						// ウィンドウモード
	d3dpp.EnableAutoDepthStencil = TRUE;			// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	//d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		// デプスバッファとして16bitを使う
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;		// デプスバッファとして16bitを使う

	if (bWindow)
	{
		// ウィンドウモード
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = 0;							// リフレッシュレート
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;		// インターバル
	}
	else
	{
		// フルスクリーンモード
		d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;			// バックバッファ
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル
	}

	// デバイスの生成
	// ディスプレイアダプタを表すためのデバイスを作成
	// 描画と頂点処理をハードウェアで行なう
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &g_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// 描画をハードウェアで行い、頂点処理はCPUで行なう
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// 描画と頂点処理をCPUで行なう
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp, &g_pD3DDevice)))
			{
				// 初期化失敗
				return E_FAIL;
			}
		}
	}

	// レンダリングステートパラメータの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// 裏面をカリング
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);						// Zバッファを使用
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				// αブレンドを行う
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定

	// サンプラーステートパラメータの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(U値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);	// テクスチャアドレッシング方法(V値)を設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャ縮小フィルタモードを設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);	// テクスチャ拡大フィルタモードを設定

	// テクスチャステージ加算合成処理
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

	g_pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	g_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// 情報表示用フォントの設定
	if (FAILED(D3DXCreateFont(g_pD3DDevice, 108, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Consolas"), &Font[FontSize_108])))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateFont(g_pD3DDevice, 96, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Consolas"), &Font[FontSize_96])))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateFont(g_pD3DDevice, 72, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Consolas"), &Font[FontSize_72])))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateFont(g_pD3DDevice, 54, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Consolas"), &Font[FontSize_54])))
	{
		return E_FAIL;
	}
	if (FAILED(D3DXCreateFont(g_pD3DDevice, 40, 0, 0, 0, FALSE, SHIFTJIS_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Consolas"), &Font[FontSize_40])))
	{
		return E_FAIL;
	}

	// 直線描画用頂点バッファを生成
	g_pD3DDevice->CreateVertexBuffer(
		sizeof(VERTEX_3DLINE) * 2,		// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,				// 頂点バッファの使用法　
		FVF_3DLine,						// 使用する頂点フォーマット
		D3DPOOL_MANAGED,				// リソースのバッファを保持するメモリクラスを指定
		&LineVtxBuffer,					// 頂点バッファインターフェースへのポインタ
		NULL);							// NULLに設定

	// テクスチャの読み込み
	if (FAILED(SafeLoadTexture(Texture_BlackScreen, &UITexture[BlackScreen], "Other") == false))
	{
		return E_FAIL;
	}
	if (FAILED(SafeLoadTexture(Texture_SelectBox, &UITexture[SelectBox], "Other") == false))
	{
		return E_FAIL;
	}

	// スキンメッシュシェーダーを読み込む
	if (FAILED(LoadSkinMeshShader()))
	{
		MessageBox(0, "SkinMeshShader Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 入力処理の初期化
	if (FAILED(InitInput(hInstance, hWnd)))
	{
		MessageBox(0, "DirectInput Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// デバッグ表示処理の初期化
	if (FAILED(InitDebugProcess(true)))
	{
		MessageBox(0, "DebugProcess Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 当たり範囲表示の初期化
	if (FAILED(InitAttackRange(true)))
	{
		MessageBox(0, "AttackRange Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// サウンドの初期化
	if (FAILED(InitSound(hWnd)))
	{
		MessageBox(0, "Sound Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// ライトの初期化
	if (FAILED(InitLight(true)))
	{
		MessageBox(0, "Light Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// カメラの初期化
	if (FAILED(InitCamera_3rd(true)))
	{
		MessageBox(0, "Camera Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// シーン遷移画面の初期化
	if (FAILED(InitTransition(true)))
	{
		MessageBox(0, "Transition Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 一時停止画面の初期化
	if (FAILED(InitPause(true)))
	{
		MessageBox(0, "Pause Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// ゲームオーバー画面の初期化
	if (FAILED(InitGameOver(true)))
	{
		MessageBox(0, "GameOver Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 地面の初期化
	if (FAILED(InitStage(true)))
	{
		MessageBox(0, "Stage Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// プレイヤーの初期化
	if (FAILED(InitPlayer(true)))
	{
		MessageBox(0, "Player Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// ポーションの初期化
	if (FAILED(InitPotion(true)))
	{
		MessageBox(0, "Potion Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// ファイアボールの初期化
	if (FAILED(InitFireBall(true)))
	{
		MessageBox(0, "FireBall Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 剣の召喚の初期化
	if (FAILED(InitSummonSword(true)))
	{
		MessageBox(0, "SummonSword Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 炎の剣の初期化
	if (FAILED(InitFireSword(true)))
	{
		MessageBox(0, "FireSword Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// ボスの初期化
	if (FAILED(InitBoss(true)))
	{
		MessageBox(0, "Boss Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// エフェクトの初期化
	if (FAILED(InitEffect(true)))
	{
		MessageBox(0, "Effect Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// タイトルの初期化
	if (FAILED(InitTitle(true)))
	{
		MessageBox(0, "Title Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// チュートリアルの初期化
	if (FAILED(InitTutorial(true)))
	{
		MessageBox(0, "Tutorial Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 空の初期化
	if (FAILED(InitSkyBox(true)))
	{
		MessageBox(0, "SkyBox Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// 装備の初期化
	if (FAILED(InitEquipment(true)))
	{
		MessageBox(0, "Equipment Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// UIゲージの初期化
	if (FAILED(InitGauge(true)))
	{
		MessageBox(0, "Gauge Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	// シャッドマッピングの初期化
	if (FAILED(InitShadowMapping(true)))
	{
		MessageBox(0, "ShadowMapping Initialize Failed！", "Error", 0);
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	int i = 0;

	// 入力処理の終了処理
	UninitInput();

	// デバッグ表示処理の終了処理
	UninitDebugProcess();

	// 当たり範囲表示の終了処理
	UninitAttackRange();

	// サウンドの終了処理
	UninitSound();

	// シーン遷移画面の終了処理
	UninitTransition();

	// 一時停止画面の終了処理
	UninitPause();

	// ゲームオーバー画面の終了処理
	UninitGameOver();

	// 地面の終了処理
	UninitStage();

	// プレイヤーの終了処理
	UninitPlayer();

	// ポーションの終了処理
	UninitPotion();

	// ボスの終了処理
	UninitBoss();

	// ファイアボールの終了処理
	UninitFireBall();

	// 剣の召喚の終了処理
	UninitSummonSword();

	// 炎の剣の終了処理
	UninitFireSword();

	// エフェクトの終了処理
	UninitEffect();

	// タイトルの終了処理
	UninitTitle();

	// チュートリアルの終了処理
	UninitTutorial();

	// 空の終了処理
	UninitSkyBox();

	// 装備の終了処理
	UninitEquipment();

	// UIゲージの終了処理
	UninitGauge();

	// シャッドマッピングの終了処理
	UninitShadowMapping();

	// UIテクスチャの開放
	for (i = 0; i < UITexture_Max; i++)
	{
		SafeRelease(UITexture[i]);
	}

	// UI表示フォントの開放
	for (i = 0; i < Font_Max; i++)
	{
		SafeRelease(Font[i]);
	}

	// 頂点バッファの開放
	SafeRelease(LineVtxBuffer);

	// デバイスの開放
	SafeRelease(g_pD3DDevice);

	// Direct3Dオブジェクトの開放
	SafeRelease(g_pD3D);
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	int TutorialState = GetTutorialState();

	// 処理開始の時間を記録
	ProcessStart();

#if _DEBUG
	// デバッグ表示ON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		DisplayDebugMsg = DisplayDebugMsg ? false : true;
	}

	// カプセル表示ON/OFF
	if (GetKeyboardTrigger(DIK_F2))
	{
		DisplayCapsule = DisplayCapsule ? false : true;
	}

	// カプセル描画モード変更
	if (GetKeyboardTrigger(DIK_F3))
	{
		DrawMode_WireFrame = DrawMode_WireFrame ? false : true;
	}
#endif

	// 入力の更新処理
	UpdateInput();

	// サウンドの更新処理
	UpdateSound();

	switch (GameStage)
	{
	case Stage_Title:

		// ライトの更新処理
		UpdateLight();

		// 空処理の更新処理
		UpdateSkyBox();

		// プレイヤーの更新処理
		UpdatePlayer();

		// タイトルの更新処理
		UpdateTitle();

		break;

	case Stage_Tutorial:

		// チュートリアル非表示の時更新
		if (TutorialState != DisplayHelp && TutorialState != TutorialPause)
		{
			// ライトの更新処理
			UpdateLight();

			// カメラの更新処理
			UpdateCamera_3rd();

			// 空処理の更新処理
			UpdateSkyBox();

			// プレイヤーの更新処理
			UpdatePlayer();

			// 剣の召喚の更新処理
			UpdateSummonSword();

			// 炎の剣の更新処理
			UpdateFireSword();

			// 装備の更新処理
			UpdateEquipment();

			// 当たり判定処理
			HitCheck();

			// エフェクトの更新処理
			UpdateEffect();

			// UIゲージの更新処理
			UpdateGauge();
		}

		// チュートリアル
		UpdateTutorial();

		break;

	case Stage_Game:

		// ライトの更新処理
		UpdateLight();

		// カメラの更新処理
		UpdateCamera_3rd();

		// 空処理の更新処理
		UpdateSkyBox();

		// プレイヤーの更新処理
		UpdatePlayer();

		// ボスの更新処理
		UpdateBoss();

		// ファイアボールの更新処理
		UpdateFireBall();

		// 剣の召喚の更新処理
		UpdateSummonSword();

		// 炎の剣の更新処理
		UpdateFireSword();

		// 装備の更新処理
		UpdateEquipment();

		// 当たり判定処理
		HitCheck();

		// エフェクトの更新処理
		UpdateEffect();

		// UIゲージの更新処理
		UpdateGauge();

		break;

	case Stage_Pause:

		// プレイヤーの更新処理
		UpdatePlayer();

		// 一時停止画面の更新処理
		UpdatePause();

		break;

	case Stage_GameOver:

		// プレイヤーの更新処理
		UpdatePlayer();

		// ゲームオーバー画面の更新処理
		UpdateGameOver();

		break;

	default:
		break;
	}

	// シーン遷移画面の更新処理
	UpdateTransition();

	// 処理終了の時間を記録
	ProcessEnd();

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// 処理開始の時間を記録
	ProcessStart();

	// バックバッファ＆Ｚバッファのクリア
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// Direct3Dによる描画の開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		switch (GameStage)
		{
		case Stage_Title:

			// カメラの設定
			SetCamera_3rd();

			// シャッドテクスチャの描画
			DrawShadowTexture();

			// 地面の描画
			DrawStage();

			// 空の描画
			DrawSkyBox();

			// プレイヤーの描画
			DrawPlayer();

			// タイトルの描画
			DrawTitle();

			break;
		case Stage_Tutorial:

			// カメラの設定
			SetCamera_3rd();

			// シャッドテクスチャの描画
			DrawShadowTexture();

			// 地面の描画
			DrawStage();

			// 空の描画
			DrawSkyBox();

			// 装備の描画
			DrawEquipment();

			// プレイヤーの描画
			DrawPlayer();

			// エフェクトの描画
			DrawEffect();

			// 剣の召喚の描画
			DrawSummonSword();

			// 炎の剣の描画
			DrawFireSword();

			// UIゲージの描画
			DrawGauge();

			// チュートリアルの描画
			DrawTutorial();

			break;
		case Stage_Game:

			// カメラの設定
			SetCamera_3rd();

			// シャッドテクスチャの描画
			DrawShadowTexture();

			// 地面の描画
			DrawStage();

			// 空の描画
			DrawSkyBox();

			// 装備の描画
			DrawEquipment();

			// プレイヤーの描画
			DrawPlayer();

			// ボスの描画
			DrawBoss();

			// 当たり範囲の描画
			DrawAttackRange();

			// エフェクトの描画
			DrawEffect();

			// ファイアボールの描画
			DrawFireBall();

			// 剣の召喚の描画
			DrawSummonSword();

			// 炎の剣の描画
			DrawFireSword();

			// UIゲージの描画
			DrawGauge();

			break;

		case Stage_Pause:

			// カメラの設定
			SetCamera_3rd();

			// シャッドテクスチャの描画
			DrawShadowTexture();

			// 地面の描画
			DrawStage();

			// 空の描画
			DrawSkyBox();

			// 装備の描画
			DrawEquipment();

			// プレイヤーの描画
			DrawPlayer();

			// ボスの描画
			DrawBoss();

			// エフェクトの描画
			DrawEffect();

			// ファイアボールの描画
			DrawFireBall();

			// 剣の召喚の描画
			DrawSummonSword();

			// 炎の剣の描画
			DrawFireSword();

			// UIゲージの描画
			DrawGauge();

			// 一時停止画面の描画
			DrawPause();

			break;

		case Stage_GameOver:

			// カメラの設定
			SetCamera_3rd();

			// シャッドテクスチャの描画
			DrawShadowTexture();

			// 地面の描画
			DrawStage();

			// 空の描画
			DrawSkyBox();

			// 装備の描画
			DrawEquipment();

			// プレイヤーの描画
			DrawPlayer();

			// ボスの描画
			DrawBoss();

			// エフェクトの描画
			DrawEffect();

			// ファイアボールの描画
			DrawFireBall();

			// 剣の召喚の描画
			DrawSummonSword();

			// 炎の剣の描画
			DrawFireSword();

			// UIゲージの描画
			DrawGauge();

			// ゲームオーバー画面の描画
			DrawGameOver();

			break;

		default:
			break;
		}

#if _DEBUG
		// デバッグ表示の描画
		if (DisplayDebugMsg)
		{
			DrawDebugProcess();
		}
#endif
		// シーン遷移画面の描画
		DrawTransition();

		// Direct3Dによる描画の終了
		g_pD3DDevice->EndScene();
	}

	// 処理終了の時間を記録
	ProcessEnd();

#if _DEBUG
	// 処理時間を表示
	DrawProcessTime();
#endif

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);

	return;
}

//=============================================================================
// デバイスの取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//=============================================================================
// GameCountを取得する
//=============================================================================
int GetGameCount(void)
{
	return GameCount;
}

//=============================================================================
// カプセルの描画設定を取得する
//=============================================================================
bool GetDrawWireFrame(void)
{
	return DrawMode_WireFrame;
}

//=============================================================================
// カプセルの表示設定を取得する
//=============================================================================
bool GetDisplayCapsule(void)
{
	return DisplayCapsule;
}

//=============================================================================
// テクスチャ読み込む関数
//=============================================================================
HRESULT SafeLoadTexture(LPCSTR SrcFile, LPDIRECT3DTEXTURE9* TexturePtr, const char* ErrorSrc)
{
	char Message[64];

	D3DXCreateTextureFromFile(g_pD3DDevice, SrcFile, TexturePtr);

	if (*TexturePtr == NULL)
	{
		sprintf(Message, "Load %s Texture Failed！", ErrorSrc);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// モデルのテクスチャを読み込む関数
//=============================================================================
HRESULT SafeLoadModelTexture(LPDIRECT3DTEXTURE9	*Texture, LPD3DXBUFFER MaterialBufferconst, DWORD MaterialNum, const char* ErrorSrc)
{
	char Message[256];
	char TextureName[256];

	D3DXMATERIAL *pMtrls = (D3DXMATERIAL*)MaterialBufferconst->GetBufferPointer();

	for (int i = 0; i < (int)MaterialNum; i++)
	{
		Texture[i] = NULL;

		ZeroMemory(TextureName, sizeof(TextureName));
		sprintf_s(TextureName, "data/Model/%s", pMtrls[i].pTextureFilename);

		// テクスチャの読み込み
		D3DXCreateTextureFromFile(g_pD3DDevice,		// デバイスへのポインタ
			TextureName,							// ファイルの名前
			&Texture[i]);							// 読み込むメモリー

		if (Texture[i] == NULL)
		{
			sprintf(Message, "Load %s Texture Failed！", ErrorSrc);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// ゲームステージを取得する
//=============================================================================
int GetGameStage(void)
{
	return GameStage;
}

//=============================================================================
// ゲームステージ設定関数（画面遷移）
//=============================================================================
void SetGameStage(int Stage)
{
	GameStage = Stage;

	return;
}

//=============================================================================
// 再初期化する
//=============================================================================
void ReInitialize(void)
{
	// 入力処理の再初期化
	ReinitInput();

	// サウンドの再初期化処理
	ReInitSound();

	// デバッグ表示処理の再初期化
	InitDebugProcess(false);

	// タイトルの再初期化
	InitTitle(false);

	// チュートリアルの再初期化
	InitTutorial(false);

	// シーン遷移画面の再初期化
	InitTransition(false);

	// 一時停止画面の再初期化
	InitPause(false);

	// ゲームオーバー画面の再初期化
	InitGameOver(false);

	// ライトの再初期化
	InitLight(false);

	// カメラの再初期化
	InitCamera_3rd(false);

	// 空の再初期化
	InitSkyBox(false);

	// プレイヤーの再初期化
	InitPlayer(false);

	// ボスの再初期化
	InitBoss(false);

	// ファイアボールの再初期化
	InitFireBall(false);

	// 剣の召喚の再初期化
	InitSummonSword(false);

	// 炎の剣の再初期化
	InitFireSword(false);

	// 装備の再初期化
	InitEquipment(false);

	// UIゲージの再初期化
	InitGauge(false);

	// エフェクトの再初期化
	InitEffect(false);

	// マウス表示の設定
	if (GameStage == Stage_Title || GameStage == Stage_Pause || GameStage == Stage_GameOver)
	{
		while (ShowCursor(true) < 0);
	}
	else
	{
		while (ShowCursor(false) >= 0);
	}

	return;
}

//=============================================================================
// 情報表示用フォントを取得する
//=============================================================================
LPD3DXFONT GetFont(int FontType)
{
	return Font[FontType];
}

//=============================================================================
// UI用テクスチャを取得する
//=============================================================================
LPDIRECT3DTEXTURE9 GetUITexture(int UITextureType)
{
	return UITexture[UITextureType];
}


//=============================================================================
// 処理開始の時間を記録
//=============================================================================
void ProcessStart()
{
	QueryPerformanceCounter(&StartTime);
	switch (ProcessStage)
	{
	case Process_Update:
		UpdateTime = 0.0;
		break;
	case Process_Draw:
		DrawTime = 0.0;
		break;
	default:
		break;
	}
}

//=============================================================================
// 処理終了の時間を記録
//=============================================================================
void ProcessEnd()
{
	LARGE_INTEGER e, f;
	QueryPerformanceCounter(&e);
	QueryPerformanceFrequency(&f);
	switch (ProcessStage)
	{
	case Process_Update:
		UpdateTime = (double)(e.QuadPart - StartTime.QuadPart) / f.QuadPart;
		break;
	case Process_Draw:
		DrawTime = (double)(e.QuadPart - StartTime.QuadPart) / f.QuadPart;
		break;
	default:
		break;
	}
}

//=============================================================================
// 処理時間を表示
//=============================================================================
void DrawProcessTime()
{
	RECT rect = { 0, 0, Screen_Width, Screen_Height };
	char ProcessTime[512];

	sprintf_s(ProcessTime, 512, "UpdateTime = %f\nDrawTime = %f\n", UpdateTime, DrawTime);
	Font[FontSize_40]->DrawText(NULL, ProcessTime, -1, &rect, DT_RIGHT, RED(255));
}

//=============================================================================
// 3D直線描画
//=============================================================================
void DrawLine(D3DXVECTOR3 P1, D3DXVECTOR3 P2, D3DCOLOR Color)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX WorldMatrix;

	VERTEX_3DLINE *pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	LineVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].Point = P1;
	pVtx[1].Point = P2;

	// 色の設定
	pVtx[0].Color = Color;
	pVtx[1].Color = Color;

	// 頂点データをアンロックする
	LineVtxBuffer->Unlock();

	// ラインティングを無効にする
	Device->SetRenderState(D3DRS_LIGHTING, false);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&WorldMatrix);

	// ワールドマトリックスの設定
	Device->SetTransform(D3DTS_WORLD, &WorldMatrix);

	// 頂点バッファをレンダリングパイプラインに設定
	Device->SetStreamSource(0, LineVtxBuffer, 0, sizeof(VERTEX_3DLINE));

	// 頂点フォーマットの設定
	Device->SetFVF(FVF_3DLine);

	// テクスチャの設定
	Device->SetTexture(0, NULL);

	// ポリゴンの描画
	Device->DrawPrimitive(D3DPT_LINELIST, 0, 1);

	// ラインティングを有効にする
	Device->SetRenderState(D3DRS_LIGHTING, true);

	return;
}