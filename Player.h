//=============================================================================
//
// プレイヤーヘッダー [Player.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "D3DXAnimation.h"
#include "Equipment.h"
#include "CapsuleMesh.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// プレイヤーのアニメーションID
enum PlayerAction
{
	Sit,
	Idle,
	Running,
	Walk_Left,
	Walk_Right,
	Walk_Back,
	Rolling,
	FallingBack,
	FlyingBack,
	FallToStand,
	HitReact,
	HPRestore,
	StandUp,
	PlayerDeath,
	Attack1,
	Attack2,
	SitPose1,
	SitPose2,
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// プレイヤー構造体
typedef struct
{
	D3DXMATRIX			WorldMatrix;		// ワールドマトリックス
	D3DXVECTOR3			Pos;				// 座標
	D3DXVECTOR3			PrePos;				// 1フレイム前の座標
	D3DXVECTOR3			CenterPos;			// プレイヤーの中心座標
	D3DXVECTOR3			NextPos;			// 次の座標
	D3DXVECTOR3			Move;				// 移動量
	D3DXVECTOR3			Rot;				// 回転
	D3DXVECTOR3			Scale;				// 大きさ(スケール)
	D3DXVECTOR3			FlyingBackDir;		// 爆発によって飛ぶ方向
	D3DXVECTOR3			Direction;			// 向き
	D3DXANIMATION		*Animation;			// アニメーション
	SWORD				*Sword;				// 剣
	CAPSULE				HitCapsule;			// 当たり判定用カプセル
	float				DestAngle;			// 目標角度
	float				NextDestAngle;		// 次のアニメーションの目標角度
	float				PreviousHP;			// 前の体力
	float				HP;					// 現在体力
	float				HP_Max;				// 最大体力
	float				RestoreHP;			// 回復後の体力
	float				PreviousST;			// 前のスタミナ
	float				Stamina;			// 現在スタミナ
	float				Stamina_Max;		// 最大スタミナ
	float				ActionSpeed;		// アクションの速度
	float				Damage;				// ダメージ
	float				DifficultyRate;		// 難易度係数
	int					HPPotionNum;		// HPポーションの数
	bool				InHPRestore;		// HP回復中フラグ
	bool				GiveDamage;			// 敵にダメージを与えたか
	bool				BeDamaged;			// 敵に攻撃された
	bool				Invincible;			// ローリング無敵
	bool				HPDecreaseStart;	// HP減少開始
	bool				STDecreaseStart;	// スタミナ減少開始
	bool				MoveBack;			// 後退フラグ
}PLAYER;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitPlayer(bool FirstInit);
// 終了処理
void UninitPlayer(void);
// 更新処理
void UpdatePlayer(void);
// 描画処理
void DrawPlayer(void);
// プレイヤーのポインタを取得する
PLAYER *GetPlayer(void);

#endif
