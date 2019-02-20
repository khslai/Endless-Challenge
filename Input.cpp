//=============================================================================
//
// キー入力処理 [Input.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Input.h"
#include "DebugProcess.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX			(256)

// Game Pad用設定値
#define DEADZONE			(2500)			// 各軸の25%を無効ゾーンとする
#define RANGE_MAX			(1000)			// 有効範囲の最大値
#define RANGE_MIN			(-1000)			// 有効範囲の最小値
#define PadRotateSpeed		(0.00005f)		// カメラ回転速度

// Mouse
#define MouseRotateSpeed	(0.005f)		// カメラ回転速度
#define WheelSpeed			(0.05f)			// カメラ拡大、縮小速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);		// キーボードの初期化
void UninitKeyboard(void);								// キーボードの終了処理
HRESULT UpdateKeyboard(void);							// キーボードの更新処理

HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow); // マウスの初期化
void UninitMouse();										// マウスの終了処理
HRESULT UpdateMouse();									// マウスの更新処理

HRESULT InitializePad(void);							// パッド初期化
void UninitPad(void);									// パッドの終了処理
void UpdatePad(void);									// パッドの更新処理


//*****************************************************************************
// グローバル変数
//*****************************************************************************
//------------------------------- Keyboard
LPDIRECTINPUT8			g_pDInput = NULL;					// IDirectInput8インターフェースへのポインタ
LPDIRECTINPUTDEVICE8	g_pDIDevKeyboard = NULL;			// IDirectInputDevice8インターフェースへのポインタ(キーボード)
BYTE					g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
BYTE					g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE					g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
int						g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ

//--------------------------------- Mouse
static LPDIRECTINPUTDEVICE8 pMouse = NULL;	// mouse
static DIMOUSESTATE2   mouseState;			// マウスのダイレクトな状態
static DIMOUSESTATE2   mouseTrigger;		// 押された瞬間だけON
static DIMOUSESTATE2   mouseRelease;		// 押された瞬間だけON
MOUSE				   Mouse;
//--------------------------------- Game Pad
static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = { NULL };	// パッドデバイス
static DWORD	padState[GAMEPADMAX];							// パッド情報（複数対応）
static DWORD	padTrigger[GAMEPADMAX];
static DWORD	padRelease[GAMEPADMAX];
static DWORD	padRepeat[GAMEPADMAX];
static int		padRepeatCnt[GAMEPADMAX];
static int		padCount = 0;									// 検出したパッドの数
GAMEPAD			GamePad;

//=============================================================================
// 入力処理の初期化
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if (!g_pDInput)
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
			IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// キーボードの初期化
	InitKeyboard(hInst, hWnd);

	// マウスの初期化
	InitializeMouse(hInst, hWnd);

	// パッドの初期化
	InitializePad();

	return S_OK;
}

//=============================================================================
// 入力処理の再初期化
//=============================================================================
void ReinitInput(void)
{
	// マウス初期化
	Mouse.Pos.x = 0;
	Mouse.Pos.y = 0;
	Mouse.PreviousPos.x = 0;
	Mouse.PreviousPos.y = 0;
	Mouse.RotateRight = 0.0f;
	Mouse.RotateUp = 0.0f;
	Mouse.WheelRot = 0.0f;

	// ゲームパッド初期化
	GamePad.RotateRight = 0.0f;
	GamePad.RotateUp = 0.0f;

	return;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void UninitInput(void)
{
	// キーボードの終了処理
	UninitKeyboard();

	// マウスの終了処理
	UninitMouse();

	// パッドの終了処理
	UninitPad();

	// 入力終了処理
	SafeRelease(g_pDInput);

	return;
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void UpdateInput(void)
{
	// キーボードの更新
	UpdateKeyboard();

	// マウスの更新
	UpdateMouse();

	// パッドの更新
	UpdatePad();

	return;
}

//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if (FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	SafeRelease(g_pDIDevKeyboard);
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];
	int GameStage = GetGameStage();

	// 前回のデータを保存
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if (SUCCEEDED(hr))
	{
		for (int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			g_keyStateTrigger[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & g_keyState[cnt];
			g_keyStateRelease[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & ~g_keyState[cnt];
			g_keyStateRepeat[cnt] = g_keyStateTrigger[cnt];

			if (g_keyState[cnt])
			{
				if (cnt != 148)
				{
					GamePad.Use = false;
					switch (GameStage)
					{
					case Stage_Title:
					case Stage_Pause:
					case Stage_GameOver:
						while (ShowCursor(true) < 0);
					default:
						break;
					}
				}
				g_keyStateRepeatCnt[cnt]++;
				if (g_keyStateRepeatCnt[cnt] >= 20)
				{
					g_keyStateRepeat[cnt] = g_keyState[cnt];
				}
			}
			else
			{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		// キーボードへのアクセス権を取得
		g_pDIDevKeyboard->Acquire();
	}

	return S_OK;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyboardPress(int key)
{
	return (g_keyState[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return (g_keyStateTrigger[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return (g_keyStateRepeat[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return (g_keyStateRelease[key] & 0x80) ? true : false;
}





//=============================================================================
//								マウス関係の処理
//=============================================================================

//=============================================================================
// マウスの初期化
//=============================================================================
HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow)
{
	HRESULT result;
	// デバイス作成
	result = g_pDInput->CreateDevice(GUID_SysMouse, &pMouse, NULL);
	if (FAILED(result) || pMouse == NULL)
	{
		MessageBox(hWindow, "No mouse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}
	// データフォーマット設定
	result = pMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(result))
	{
		MessageBox(hWindow, "Can't setup mouse", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}
	// 他のアプリと協調モードに設定
	result = pMouse->SetCooperativeLevel(hWindow, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if (FAILED(result))
	{
		MessageBox(hWindow, "Mouse mode error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	// デバイスの設定
	DIPROPDWORD prop;

	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = DIPROPAXISMODE_REL;		// マウスの移動値　相対値

	result = pMouse->SetProperty(DIPROP_AXISMODE, &prop.diph);
	if (FAILED(result))
	{
		MessageBox(hWindow, "Mouse property error", "Warning", MB_OK | MB_ICONWARNING);
		return result;
	}

	// アクセス権を得る
	pMouse->Acquire();

	return result;
}

//=============================================================================
// マウスの終了処理
//=============================================================================
void UninitMouse()
{
	if (pMouse)
	{
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = NULL;
	}
}

//=============================================================================
// マウスの更新処理
//=============================================================================
HRESULT UpdateMouse()
{
	HRESULT result;
	// 前回の値保存
	DIMOUSESTATE2 lastMouseState = mouseState;
	int GameStage = GetGameStage();

	// データ取得
	result = pMouse->GetDeviceState(sizeof(mouseState), &mouseState);

	if (SUCCEEDED(result))
	{
		// 前フレイムの座標を記録する
		Mouse.PreviousPos = Mouse.Pos;

		Mouse.Pos.x += mouseState.lX;
		Mouse.Pos.y += mouseState.lY;
		mouseTrigger.lX = mouseState.lX;
		mouseTrigger.lY = mouseState.lY;
		mouseTrigger.lZ = mouseState.lZ;
		mouseRelease.lX = mouseState.lX;
		mouseRelease.lY = mouseState.lY;
		mouseRelease.lZ = mouseState.lZ;

		// マウスのボタン状態
		for (int i = 0; i < 8; i++)
		{
			mouseTrigger.rgbButtons[i] = ((lastMouseState.rgbButtons[i] ^ mouseState.rgbButtons[i]) & mouseState.rgbButtons[i]);
			mouseRelease.rgbButtons[i] = ((lastMouseState.rgbButtons[i] ^ mouseState.rgbButtons[i]) & ~mouseState.rgbButtons[i]);
		}

		// X方向回転量
		float Xdif = (float)Mouse.Pos.x - (float)Mouse.PreviousPos.x;
		// Y方向回転量
		float Ydif = (float)Mouse.Pos.y - (float)Mouse.PreviousPos.y;

		Mouse.RotateRight = Xdif * MouseRotateSpeed;
		Mouse.RotateUp = Ydif * MouseRotateSpeed;

		// ホイールで画面を拡大、縮小
		Mouse.WheelRot = WheelSpeed * -mouseState.lZ;
	}
	else	// 取得失敗
	{
		// アクセス権を得てみる
		result = pMouse->Acquire();
	}

	return result;
}

//=============================================================================
// マウスのプレス、トリガー状態を取得
//=============================================================================
BOOL IsMouseLeftPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[0] & 0x80);	// 押されたときに立つビットを検査
}
BOOL IsMouseLeftTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[0] & 0x80);
}
BOOL IsMouseLeftReleased(void)
{
	return (BOOL)(mouseRelease.rgbButtons[0] & 0x80);
}
BOOL IsMouseRightPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[1] & 0x80);
}
BOOL IsMouseRightTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[1] & 0x80);
}
BOOL IsMouseRightReleased(void)
{
	return (BOOL)(mouseRelease.rgbButtons[1] & 0x80);
}
BOOL IsMouseCenterPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[2] & 0x80);
}
BOOL IsMouseCenterTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[2] & 0x80);
}
BOOL IsMouseCenterReleased(void)
{
	return (BOOL)(mouseRelease.rgbButtons[2] & 0x80);
}

//=============================================================================
// マウスの座標を取得
//=============================================================================
long GetMouseX(void)
{
	return mouseState.lX;
}
long GetMouseY(void)
{
	return mouseState.lY;
}
long GetMouseZ(void)
{
	return mouseState.lZ;
}

//=============================================================================
// マウスのポインタを取得
//=============================================================================
MOUSE *GetMouse(void)
{
	return &Mouse;
}

//=============================================================================
// マウスは矩形の中か確認
//=============================================================================
bool CheckMousePos(RECT Rect)
{
	bool InRect = false;

	// マウス座標取得
	GetCursorPos(&Mouse.Pos);

	// 矩形の中か確認
	if (Mouse.Pos.x >= Rect.left && Mouse.Pos.x <= Rect.right &&
		Mouse.Pos.y >= Rect.top && Mouse.Pos.y <= Rect.bottom)
	{
		InRect = true;
	}

	return InRect;
}


//=============================================================================
//									Game Pad
//=============================================================================

//=============================================================================
// コールバック関数
//=============================================================================
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID)
{
	HRESULT result;

	result = g_pDInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], NULL);
	return DIENUM_CONTINUE;	// 次のデバイスを列挙

}

//=============================================================================
// ゲームパッドの初期化
//=============================================================================
HRESULT InitializePad(void)
{
	HRESULT		result;
	int			i;

	padCount = 0;
	// ジョイパッドを探す
	g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);
	// セットしたコールバック関数が、パッドを発見した数だけ呼ばれる。

	for (i = 0; i < padCount; i++)
	{
		// ジョイスティック用のデータ・フォーマットを設定
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(result))
			return false; // データフォーマットの設定に失敗

		// モードを設定（フォアグラウンド＆非排他モード）
//		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//		if ( FAILED(result) )
//			return false; // モードの設定に失敗

		// 軸の値の範囲を設定
		// X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
		// (max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
		// 差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドの性能による)
		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;
		// X軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		diprg.diph.dwObj = DIJOFS_RX;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		diprg.diph.dwObj = DIJOFS_RY;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// 各軸ごとに、無効のゾーン値を設定する。
		// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
		// 指定する値は、10000に対する相対値(2000なら20パーセント)。
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		//X軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		dipdw.diph.dwObj = DIJOFS_RX;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		dipdw.diph.dwObj = DIJOFS_RY;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		//ジョイスティック入力制御開始
		pGamePad[i]->Acquire();
	}

	// ゲームパッドが設置される
	if (padCount > 0)
	{
		while (ShowCursor(false) >= 0);
	}
	// ゲームパッドが設置されない
	else
	{
		// マウスは真ん中に設置する
		RECT rt;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
		SetCursorPos(rt.right / 2, rt.bottom / 2);
		while (ShowCursor(true) < 0);
	}

	return true;

}

//=============================================================================
// ゲームパッドの終了処理
//=============================================================================
void UninitPad(void)
{
	for (int i = 0; i < GAMEPADMAX; i++)
	{
		if (pGamePad[i])
		{
			pGamePad[i]->Unacquire();
			pGamePad[i]->Release();
		}
	}

}
//=============================================================================
// ゲームパッドの更新処理
//=============================================================================
void UpdatePad(void)
{
	HRESULT			result;
	DIJOYSTATE2		dijs;
	int				i;

	for (i = 0; i < padCount; i++)
	{
		DWORD lastPadState;
		lastPadState = padState[i];
		padState[i] = 0x00000000l;	// 初期化

		result = pGamePad[i]->Poll();	// ジョイスティックにポールをかける
		if (FAILED(result))
		{
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
			{
				result = pGamePad[i]->Acquire();
			}
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	// デバイス状態を読み取る
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
		{
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
			{
				result = pGamePad[i]->Acquire();
			}
		}

		// ３２の各ビットに意味を持たせ、ボタン押下に応じてビットをオンにする
		// 左スティック(上)
		if (dijs.lY < 0)					padState[i] |= BUTTON_UP;
		// 左スティック(下)
		if (dijs.lY > 0)					padState[i] |= BUTTON_DOWN;
		// 左スティック(左)
		if (dijs.lX < 0)					padState[i] |= BUTTON_LEFT;
		// 左スティック(右)
		if (dijs.lX > 0)					padState[i] |= BUTTON_RIGHT;

		// 右スティック(上)
		if (dijs.lRy < 0)					padState[i] |= BUTTON_RSUP;
		// 右スティック(下)
		if (dijs.lRy > 0)					padState[i] |= BUTTON_RSDOWN;
		// 右スティック(左)
		if (dijs.lRx < 0)					padState[i] |= BUTTON_RSLEFT;
		// 右スティック(右)
		if (dijs.lRx > 0)					padState[i] |= BUTTON_RSRIGHT;

		// 十字キー(上)
		if (dijs.rgdwPOV[0] == 0)			padState[i] |= BUTTON_UP;
		// 十字キー(右)
		if (dijs.rgdwPOV[0] == 9000)		padState[i] |= BUTTON_RIGHT;
		// 十字キー(下)
		if (dijs.rgdwPOV[0] == 18000)		padState[i] |= BUTTON_DOWN;
		// 十字キー(左)
		if (dijs.rgdwPOV[0] == 27000)		padState[i] |= BUTTON_LEFT;
		// 十字キー(右上)
		if (dijs.rgdwPOV[0] == 4500)		padState[i] |= (BUTTON_UP | BUTTON_RIGHT);
		// 十字キー(右下)
		if (dijs.rgdwPOV[0] == 13500)		padState[i] |= (BUTTON_DOWN | BUTTON_RIGHT);
		// 十字キー(左下)
		if (dijs.rgdwPOV[0] == 22500)		padState[i] |= (BUTTON_DOWN | BUTTON_LEFT);
		// 十字キー(左上)
		if (dijs.rgdwPOV[0] == 31500)		padState[i] |= (BUTTON_UP | BUTTON_LEFT);

		// Ａボタン
		if (dijs.rgbButtons[0] & 0x80)		padState[i] |= BUTTON_A;
		// Ｂボタン
		if (dijs.rgbButtons[1] & 0x80)		padState[i] |= BUTTON_B;
		// Ｘボタン
		if (dijs.rgbButtons[2] & 0x80)		padState[i] |= BUTTON_X;
		// Ｙボタン
		if (dijs.rgbButtons[3] & 0x80)		padState[i] |= BUTTON_Y;
		// ＬＢボタン
		if (dijs.rgbButtons[4] & 0x80)		padState[i] |= BUTTON_LB;
		// ＲＢボタン
		if (dijs.rgbButtons[5] & 0x80)		padState[i] |= BUTTON_RB;
		// ＳＥＬＥＣＴボタン
		if (dijs.rgbButtons[6] & 0x80)		padState[i] |= BUTTON_SELECT;
		// ＳＴＡＲＴボタン
		if (dijs.rgbButtons[7] & 0x80)		padState[i] |= BUTTON_START;
		// Ｌ３ボタン
		if (dijs.rgbButtons[8] & 0x80)		padState[i] |= BUTTON_L3;
		// Ｒ３ボタン
		if (dijs.rgbButtons[9] & 0x80)		padState[i] |= BUTTON_R3;

		// ＬＴ、ＲＴボタンを使いたいなら、lZを調整する

		// Trigger、Release設定
		// 前回と違っていて、しかも今ONのやつ
		padTrigger[i] = ((lastPadState ^ padState[i]) & padState[i]);
		padRelease[i] = ((lastPadState ^ padState[i]) & ~padState[i]);

		GamePad.RotateRight = 0.0f;
		GamePad.RotateUp = 0.0f;

		if (padState[i])
		{
			GamePad.Use = true;
			while (ShowCursor(false) >= 0);

			// リピートカウント加算
			padRepeatCnt[i]++;

			// リピート状態
			if (padRepeatCnt[i] >= 20)
			{
				padRepeat[i] = padState[i];
			}

			GamePad.RotateRight = dijs.lRx * PadRotateSpeed;
			GamePad.RotateUp = dijs.lRy * PadRotateSpeed;
		}
		else
		{
			padRepeatCnt[i] = 0;
			padRepeat[i] = 0;
		}


#if _DEBUG
		for (int i = 0; i < 20; i++)
		{
			if (dijs.rgbButtons[i] != 0)
			{
				PrintDebugMsg("Button No. :%d, %d\n", i,dijs.rgbButtons[i]);
			}
		}
		PrintDebugMsg("dijs.lRx : %d\n", dijs.lRx);
		PrintDebugMsg("dijs.lRy : %d\n", dijs.lRy);
#endif

	}

	return;
}

//=============================================================================
// ゲームパッドのプレス状態を取得
//=============================================================================
BOOL IsButtonPressed(int Pad_No, DWORD Button)
{
	return (Button & padState[Pad_No]);
}

//=============================================================================
// ゲームパッドのトリガー状態を取得
//=============================================================================
BOOL IsButtonTriggered(int Pad_No, DWORD Button)
{
	return (Button & padTrigger[Pad_No]);
}

//=============================================================================
// ゲームパッドのリリ－ス状態を取得
//=============================================================================
BOOL IsButtonReleased(int Pad_No, DWORD Button)
{
	return (Button & padRelease[Pad_No]);
}

//=============================================================================
// ゲームパッドのリピート状態を取得
//=============================================================================
BOOL IsButtonRepeat(int Pad_No, DWORD Button)
{
	return (Button & padRepeat[Pad_No]);
}

//=============================================================================
// ゲームパッドのポインタを取得
//=============================================================================
GAMEPAD *GetGamePad(void)
{
	return &GamePad;
}
