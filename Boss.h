//=============================================================================
//
// ボスヘッダー [Boss.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _BOSS_H_
#define _BOSS_H_

#include "D3DXAnimation.h"
#include "Equipment.h"
#include "CapsuleMesh.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// ボスのアニメーションID
enum BossAction
{
	BossAct_Idle,
	BossAct_Walk,
	BossAct_HitReact,
	BossAct_Rolling,
	BossAct_BossDeath,
	BossAct_SlantSlash,
	BossAct_LeftRightSlash,
	BossAct_RollingSlash,
	BossAct_SurfaceSlash,
	BossAct_UpSlash,
	BossAct_HorizonSlash,
	BossAct_VerticalSlash,
	BossAct_Stab,
	BossAct_ComboAttack1,
	BossAct_ComboAttack2,
	BossAct_ComboAttack3,
	BossAct_TwoHandCombo1,
	BossAct_TwoHandCombo2,
	BossAct_PowerUp,
};

enum JumpState
{
	Jump_Start,			// ジャンプ開始
	StartFalling,		// 落下開始
	Jump_Stop,			// ジャンプ終了
};

// ボスの段階
enum Phase
{
	Phase1,				// 第一段階
	TurnToPhase2,		// 第二段階に変身
	Phase2,				// 第二段階
	SetDeathEffect,		// 死亡エフェクトを設置する
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXMATRIX			WorldMatrix;	// ワールドマトリックス
	D3DXVECTOR3			Pos;			// 座標
	D3DXVECTOR3			PrePos;			// 1フレイム前の座標
	D3DXVECTOR3			CenterPos;		// 中心座標
	D3DXVECTOR3			Move;			// 移動量
	D3DXVECTOR3			Rot;			// モデルの向き(回転)
	D3DXVECTOR3			Scale;			// モデルの大きさ(スケール)
	D3DXANIMATION		*Animation;		// モデルアニメーション
	SWORD				*Sword;			// 剣
	CAPSULE				HitCapsule;		// 当たり判定用カプセル
	float				DestAngle;		// 目標角度
	float				HP;				// 体力
	float				HP_Max;			// 最大体力
	float				PreviousHP;		// 受けたダメージ
	float				ActionSpeed;	// アクションの速度
	int					JumpState;		// 跳ぶの状態
	int					Phase;			// ボスの段階
	int					DistanceState;	// プレイヤーとの距離状態
	bool				Exist;			// 存在フラグ
	bool				SetFireSword;	// 炎の剣のエフェクト
	bool				HPDecreaseStart;// ダメージゲージを減る
	bool				GiveDamage;		// プレイヤーにダメージを与えたか
	bool				TurnRotation;	// 攻撃中の方向修正
}BOSS;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitBoss(bool FirstInit);
// 終了処理
void UninitBoss(void);
// 更新処理
void UpdateBoss(void);
// 描画処理
void DrawBoss(void);
// ボスのポインタを取得する
BOSS *GetBoss(void);

#endif
