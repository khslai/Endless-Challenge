//=============================================================================
//
// メインヘッダー [main.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _MAIN_H_
#define _MAIN_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#define D3D_DEBUG_INFO

#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>

// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>

// DirectX 9.0
#define DIRECTINPUT_VERSION (0x0800)	// 警告対策
#include <dinput.h>
#include <dsound.h>
#include <d3dx9.h>

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#if 1	// [ここを"0"にした場合、"構成プロパティ" -> "リンカ" -> "入力" -> "追加の依存ファイル"に対象ライブラリを設定する]
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dsound.lib")

#if _DEBUG
#pragma comment(lib, "Debug/Effekseer.lib" )
#pragma comment(lib, "Debug/EffekseerRendererDX9.lib" )
#pragma comment(lib, "Debug/EffekseerSoundXAudio2.lib" )
#else
#pragma comment(lib, "Release/Effekseer.lib" )
#pragma comment(lib, "Release/EffekseerRendererDX9.lib" )
#pragma comment(lib, "Release/EffekseerSoundXAudio2.lib" )
#endif

#endif

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// ２Ｄポリゴン頂点フォーマット( 頂点座標[2D] / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// ３Ｄポリゴン頂点フォーマット( 頂点座標[3D] / 法線 / 反射光 / テクスチャ座標 )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// ３Ｄ直線頂点フォーマット( 頂点座標[3D] / 反射光 )
#define	FVF_3DLine		(D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define Screen_Width		(1920)					// ウインドウの幅
#define Screen_Height		(1080)					// ウインドウの高さ
#define Screen_Center_X		(Screen_Width / 2)		// ウインドウの中心X座標
#define Screen_Center_Y		(Screen_Height / 2)		// ウインドウの中心Y座標
#define Stage_Size			(1025.0f)				// ステージのサイズ
#define	MoveInertia			(0.20f)					// 移動慣性係数
#define	RotateInertia		(0.20f)					// 回転慣性係数
#define TimePerFrame		(0.0167f)				// 1 / 60秒
#define D3DX_HalfPI			(1.5708f)				// D3DX_PIの半分
#define D3DX_2PI			(6.28318f)				// D3DX_PIの二倍
#define	VERTEX_NUM			(4)						// 一つの平面を描画する時必要な頂点数
#define	POLYGON_NUM			(2)						// 一つの平面を描画する時必要なポリゴン数
#define DXtoEffekVec(Vec)	::Effekseer::Vector3D(Vec.x, Vec.y, Vec.z)

// カメラ関連
#define	VIEW_ANGLE			(D3DXToRadian(60.0f))		// 視野角度
// ビュー平面のアスペクト比
#define	VIEW_ASPECT			((float)Screen_Width / (float)Screen_Height)	
#define	VIEW_NEAR_Z			(10.0f)						// ビュー平面のNearZ値、見える最短距離
#define	VIEW_FAR_Z			(5000.0f)					// ビュー平面のFarZ値、見える最長距離

// 選択肢ループ
#define RepeatCount (60)
#define RepeatSpeed (5)

// 初期座標
#define PositionZero		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
// 初期の大きさ
#define DefaultScale		(D3DXVECTOR3(1.0f, 1.0f, 1.0f))
// 上のベクトル
#define UpVector			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
// 右のベクトル
#define RightVector			(D3DXVECTOR3(1.0f, 0.0f, 0.0f))

// メニューテクスチャ
#define Texture_BlackScreen			_T("data/Texture/BlackScreen.png")
#define Texture_BlackScreen_Width	(1920)
#define Texture_BlackScreen_Height	(1080)
#define Texture_SelectBox			_T("data/Texture/SelectBox.png")
#define Texture_SelectBox_Width		(500)
#define Texture_SelectBox_Height	(150)

// メモリリリース
// new
#define SafeDelete(Object)		{ if (Object) { delete(Object);			(Object)=NULL; } }
#define SafeDeleteArray(Object)	{ if (Object) { delete[](Object);		(Object)=NULL; } }
// malloc,calloc
#define SafeFree(Object)		{ if (Object) { free(Object);			(Object)=NULL; } }
// DirectX
#define SafeRelease(Object)		{ if (Object) { (Object)->Release();	(Object)=NULL; } }

// 色のRGBA
#define WHITE(Alpha)			D3DCOLOR_RGBA(255, 255, 255, Alpha)
#define BLACK(Alpha)			D3DCOLOR_RGBA(0, 0, 0, Alpha)
#define GREEN(Alpha)			D3DCOLOR_RGBA(0, 255, 0, Alpha)
#define BLUE(Alpha)				D3DCOLOR_RGBA(0, 0, 255, Alpha)
#define YELLOW(Alpha)			D3DCOLOR_RGBA(255, 255, 0, Alpha)
#define RED(Alpha)				D3DCOLOR_RGBA(255, 0, 0, Alpha)
#define FIRECOLOR(Alpha)		D3DCOLOR_RGBA(253, 165, 15, Alpha)
#define BLUEFIRE(Alpha)			D3DCOLOR_RGBA(10, 10, 255, Alpha)
#define SKYBLUE(Alpha)			D3DCOLOR_RGBA(135, 206, 235, Alpha)
#define ORANGE(Alpha)			D3DCOLOR_RGBA(255, 165, 0, Alpha)
#define PURPLE(Alpha)			D3DCOLOR_RGBA(128, 0, 128, Alpha)
#define DARKRED(Alpha)			D3DCOLOR_RGBA(139, 0, 0, Alpha)

// ゲームステージ
enum StagePhase
{
	Stage_Title,
	Stage_Tutorial,
	Stage_Game,
	Stage_Pause,
	Stage_GameOver,
};

// ゲーム難易度
enum GameDifficulty
{
	Easy,
	Normal,
	AppealDifficulty,
};

// 画面遷移の状態
enum TransitionState
{
	TransitionOver = 0,	// 何もない状態
	Fadein,				// フェードイン処理
	Fadeout,			// フェードアウト処理
};

// UIテクスチャの種類
enum UITextureType
{
	BlackScreen,
	SelectBox,
	UITexture_Max,
};

// UIフォントの種類
enum FontType
{
	FontSize_108,
	FontSize_96,
	FontSize_72,
	FontSize_54,
	FontSize_40,
	Font_Max,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// 上記２Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 vtx;		// 頂点座標
	float rhw;				// テクスチャのパースペクティブコレクト用
	D3DCOLOR diffuse;		// 反射光
	D3DXVECTOR2 tex;		// テクスチャ座標
} VERTEX_2D;

// 上記３Ｄポリゴン頂点フォーマットに合わせた構造体を定義
typedef struct
{
	D3DXVECTOR3 vtx;		// 頂点座標
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR diffuse;		// 反射光
	D3DXVECTOR2 tex;		// テクスチャ座標
} VERTEX_3D;

// 3D空間で直線描画用構造体を定義
typedef struct
{
	D3DXVECTOR3 Point;
	D3DCOLOR Color;
} VERTEX_3DLINE;

// 選択肢構造体
typedef struct
{
	VERTEX_2D		VertexWk[VERTEX_NUM];	// 頂点情報格納ワーク
	D3DXVECTOR2		PrePos;					// 前フレイムの座標
	D3DXVECTOR2		Pos;					// 座標
	int				Phase;					// 今指す選択肢
	bool			InYes;					// 答えは"YES"か"No"か
}SELECT;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// デバイスを取得する
LPDIRECT3DDEVICE9 GetDevice(void);
// UI用テクスチャを取得する
LPDIRECT3DTEXTURE9 GetUITexture(int UITextureType);
// ゲームカウントを取得する
int GetGameCount(void);
// カプセルの描画設定を取得する
bool GetDrawWireFrame(void);
// カプセルの表示設定を取得する
bool GetDisplayCapsule(void);
// ゲームステージを取得する
int GetGameStage(void);
// ゲームステージ設定関数（画面遷移）
void SetGameStage(int Stage);
// 一般テクスチャ読み込む関数
HRESULT SafeLoadTexture(LPCSTR SrcFile, LPDIRECT3DTEXTURE9* TexturePtr, const char* ErrorSrc);
// モデルのテクスチャを読み込む
HRESULT SafeLoadModelTexture(LPDIRECT3DTEXTURE9	*Texture, LPD3DXBUFFER MaterialBufferconst, DWORD MaterialNum, const char* ErrorSrc);
// 情報表示用フォントを取得する
LPD3DXFONT GetFont(int FontType);
// 再初期化
void ReInitialize(void);
// 3D空間で直線描画
void DrawLine(D3DXVECTOR3 P1, D3DXVECTOR3 P2, D3DCOLOR Color);

#endif