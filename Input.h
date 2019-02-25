//=============================================================================
//
// キー入力ヘッダー [Input.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_


//*****************************************************************************
// マクロ定義
//*****************************************************************************

// プログラム分けするときに使う
#define	USE_KEYBOARD				// 宣言するとキーボードで操作可能になる
#define	USE_MOUSE					// 宣言するとマウスで操作可能になる
#define	USE_PAD						// 宣言するとパッドで操作可能になる


/* Game Pad情報 */
#define BUTTON_UP		0x00000001l	// 左スティック、方向キー上(.lY < 0)
#define BUTTON_DOWN		0x00000002l	// 左スティック、方向キー下(.lY > 0)
#define BUTTON_LEFT		0x00000004l	// 左スティック、方向キー左(.lX < 0)
#define BUTTON_RIGHT	0x00000008l	// 左スティック、方向キー右(.lX > 0)
#define BUTTON_RSUP		0x00010000l	// 右スティック上	(.lRy < 0)
#define BUTTON_RSDOWN	0x00020000l	// 右スティック下	(.lRy > 0)
#define BUTTON_RSLEFT	0x00040000l	// 右スティック左	(.lRx < 0)
#define BUTTON_RSRIGHT	0x00080000l	// 右スティック右	(.lRy > 0)
#define BUTTON_A		0x00000010l	// Ａボタン			(.rgbButtons[0] & 0x80)
#define BUTTON_B		0x00000020l	// Ｂボタン			(.rgbButtons[1] & 0x80)
#define BUTTON_X		0x00000040l	// Ｘボタン			(.rgbButtons[2] & 0x80)
#define BUTTON_Y		0x00000080l	// Ｙボタン			(.rgbButtons[3] & 0x80)
#define BUTTON_LB		0x00000100l	// ＬＢボタン		(.rgbButtons[4] & 0x80)
#define BUTTON_RB		0x00000200l	// ＲＢボタン		(.rgbButtons[5] & 0x80)
#define BUTTON_SELECT	0x00000400l	// ＳＥＬＥＣＴボタン	(.rgbButtons[6] & 0x80)
#define BUTTON_START	0x00000800l	// ＳＴＡＲＴボタン	(.rgbButtons[7] & 0x80)
#define BUTTON_L3		0x00001000l	// Ｌ３ボタン		(.rgbButtons[8] & 0x80)
#define BUTTON_R3		0x00002000l	// Ｒ３ボタン		(.rgbButtons[9] & 0x80)
#define GAMEPADMAX		1			// 同時に接続するジョイパッドの最大数をセット


//*****************************************************************************
// 構造体定義
//*****************************************************************************
// マウス構造体
typedef struct
{
	POINT	Pos;					// マウス座標
	POINT	PreviousPos;			// 1フレイム前のマウス座標
	float	RotateRight;            // 水平方向回転量(右はプラス)
	float	RotateUp;               // 垂直方向回転量(上はプラス)
	float	WheelRot;				// ホイールの移動量		 
}MOUSE;

// ゲームパッド構造体
typedef struct
{
	float	RotateRight;            // 水平方向回転量(右はプラス)
	float	RotateUp;               // 垂直方向回転量(上はプラス)
	bool	Use;					// 使用フラグ
}GAMEPAD;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitInput(HINSTANCE hInst, HWND hWnd);
// 終了処理
void UninitInput(void);
// 更新処理
void UpdateInput(void);
// 再初期化処理
void ReinitInput(void);

//---------------------------- keyboard
bool GetKeyboardPress(int nKey);	// キーを押した状態
bool GetKeyboardTrigger(int nKey);	// キーを押した瞬間
bool GetKeyboardRepeat(int nKey);	// キーを押したままの状態
bool GetKeyboardRelease(int nKey);	// キーをリリースした状態

//---------------------------- mouse
BOOL IsMouseLeftPressed(void);      // 左クリックした状態
BOOL IsMouseLeftTriggered(void);    // 左クリックした瞬間
BOOL IsMouseLeftReleased(void);		// 左リリースした瞬間
BOOL IsMouseRightPressed(void);     // 右クリックした状態
BOOL IsMouseRightTriggered(void);   // 右クリックした瞬間
BOOL IsMouseRightReleased(void);	// 右リリースした瞬間
BOOL IsMouseCenterPressed(void);    // 中クリックした状態
BOOL IsMouseCenterTriggered(void);  // 中クリックした瞬間
BOOL IsMouseCenterReleased(void);	// 中リリースした瞬間
long GetMouseX(void);               // マウスがX方向に動いた相対値
long GetMouseY(void);               // マウスがY方向に動いた相対値
long GetMouseZ(void);               // マウスホイールが動いた相対値
MOUSE *GetMouse(void);				// マウスのポインタを取得
bool CheckMousePos(RECT Rect);		// マウスは矩形の中か確認

//---------------------------- game pad
BOOL IsButtonPressed(int Pad_No, DWORD Button);		// ボタンを押した状態
BOOL IsButtonTriggered(int Pad_No, DWORD Button);	// ボタンを押した瞬間
BOOL IsButtonReleased(int Pad_No, DWORD Button);	// ボタンをリリースした状態
BOOL IsButtonRepeat(int Pad_No, DWORD Button);		// ボタンを押したままの状態
GAMEPAD *GetGamePad(void);							// ゲームパッドのポインタを取得


#endif