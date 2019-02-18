//=============================================================================
//
// チュートリアルヘッダー [Tutorial.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum TutorialPhase
{
	Tutorial_Attack,			// 攻撃
	Tutorial_SetSword,			// 剣の召喚
	Tutorial_WaitRolling,		// プレイヤーの回避を待っている
	Tutorial_RollingClear,		// 回避成功
	Tutorial_HPRestore,			// HP回復
	Tutorial_CameraReset,		// カメラリセット
	Tutorial_CameraLockOn,		// ロックオン
	HelpOver,					// ヘルプ終了、立方体を壊す
	TutorialPause,				// 一時停止
	DisplayHelp,				// 操作説明を表示する
	TutorialClear,				// チュートリアルクリア
};

enum TutorialSelectState
{
	Tutorial_Resume,
	Tutorial_ToTitle,
	Tutorial_ToTitleCheck,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3			Pos;				// モデルの位置
	D3DXVECTOR3			Rot;				// モデルの向き(回転)
	D3DXVECTOR3			Scale;				// モデルの大きさ(スケール)
	D3DXMATRIX			WorldMatrix;		// ワールドマトリックス
	LPDIRECT3DTEXTURE9	*Texture;			// テクスチャへのポインタ
	LPD3DXMESH			Mesh;				// メッシュ情報へのポインタ
	LPD3DXBUFFER		MaterialBuffer;		// マテリアル情報へのポインタ
	DWORD				MaterialNum;		// マテリアル情報の数
	CAPSULE				HitCapsule;			// 当たり判定用カプセル
	float				Durability;			// 耐久性 
	float				MaxDurability;		// 最大耐久性 
	bool				Exist;				// 存在フラグ
	bool				EffectOver;			// エフェクト終了フラグ
}CUBE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitTutorial(bool FirstInit);
// 終了処理
void UninitTutorial(void);
// 更新処理
void UpdateTutorial(void);
// 描画処理
void DrawTutorial(void);
// チュートリアルの状態を設置する
void SetTutorialState(int State);
// チュートリアルの状態を取得する
int GetTutorialState(void);
// 立方体のポインタを取得する
CUBE *GetCube(void);
// 選択肢の状態を取得する
SELECT *GetTutorialSelect(void);

#endif
