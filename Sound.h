//=============================================================================
//
// サウンドヘッダー [Sound.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// サウンドの種類
enum SoundType
{
	BGM,			// BGM
	NormalSE,		// 2DのSE
	SE3D,			// 3DのSE
};

// 2Dサウンド番号
enum Sound_No
{
	BGM_Battle_Phase1_Intro,
	BGM_Battle_Phase1_Loop,
	BGM_Battle_Phase2_Intro,
	BGM_Battle_Phase2_Loop,
	BGM_Title,
	BGM_GameOver,
	BGM_Tutorial_Intro,
	BGM_Tutorial_Loop,
	SE_Menu,
	SE_Determine_Yes,
	SE_Determine_No,
	SE_SelectMove,
	SE_ShowTutorial,
	Sound_Max,
};

// 3Dサウンド番号
enum Sound3D_No
{
	SE3D_PlayerFootStep,
	SE3D_Rolling,
	SE3D_Slash,
	SE3D_Slash2,
	SE3D_HPRestore,
	SE3D_BossFootStep,
	SE3D_BossSlash,
	SE3D_SlashFire,
	SE3D_ExplodeFire,
	SE3D_BossDeathWind,
	SE3D_PowerUp,
	SE3D_ExplosionWind,
	SE3D_SetFireBall,
	SE3D_Burnning,
	SE3D_FireBallExplode,
	SE3D_HitCube,
	SE3D_DestroyCube,
	SE3D_HitHuman,
	SE3D_HitFire,
	SE3D_FallingDown,
	SE3D_FallingDown2,
	Sound3D_Max,
};

// 再生用フラグ
enum
{
	E_DS8_FLAG_NONE,	// 一回しかプレイしない
	E_DS8_FLAG_LOOP,	// ループ
	E_DS8_FLAG_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT	InitSound(HWND hWnd);
// 再初期化処理
void ReInitSound(void);
// 終了処理
void UninitSound(void);
// 更新処理
void UpdateSound(void);
// BGM、2Dサウンドを設置する
void SetSound(int SoundType, int Sound_No, int Flag, bool FromStart);
// 3Dサウンドを設置する
void Set3DSound(D3DXVECTOR3 Pos, int Sound_No, int Flag, bool FromStart);
// サウンドを停止する
void StopSound(int SoundType, int Sound_No);
// 今鳴らしているBGMの番号を取得する
int GetPlayingBGM_No(void);
// サウンドのフェイドアウト
void SoundFadeOut(int Sound_No);

#endif