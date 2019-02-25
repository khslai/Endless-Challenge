//=============================================================================
//
// ファイアボールヘッダー [FireBall.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================
#ifndef _FIREBALL_H_
#define _FIREBALL_H_

#include "Effect.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BulletMax			(10)		// ファイアボールの最大数
#define ExplodeRange		(80.0f)		// 爆発の範囲
#define BurnningRange		(20.0f)		// 燃焼中の爆発範囲

enum BulletState
{
	Standby,			// 待機
	Shot,				// 弾発射
	SetBurnningEffect,	// 直撃
	BurnningExplode,	// 踏まれる爆発
	WaitDisappear,		// 消えるまで待つ
	DisappearExplode,	// 消える爆発
	WaitExplodeOver,	// 爆発が終わる
	OverStageRange,		// ステージ範囲を超える
	BulletInit = 99,
};

enum FireBallEffect
{
	BulletFire,			// ファイアボール
	Burnning,			// 命中エフェクト
	Explode,			// 消失の爆発
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// ファイアボール構造体
typedef struct
{
	D3DXVECTOR3		Pos;				// 座標
	D3DXVECTOR3		PrePos;				// 1フレイム前の座標
	D3DXVECTOR3		BezierPoint[2];		// ベジェ曲線計算用座標
	D3DXVECTOR3		StartPos;			// ベジェ曲線初期座標
	D3DXVECTOR3		DestPos;			// ターゲット座標
	D3DXVECTOR3		Direction;			// 方向ベクトル
	float			Radius;				// 半径
	float			Speed;				// 移動速度
	int				EffectID;			// エフェクトID
	int				ShotTime;			// 発射時間
	int				Count;				// 存在カウント
	int				State;				// 状態
	int				SphereID;			// 当たり範囲表示番号
	bool			Use;				// 使用フラグ
}BULLET;


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitFireBall(bool FirstInit);
// 終了処理
void UninitFireBall(void);
// 更新処理
void UpdateFireBall(void);
// 描画処理
void DrawFireBall(void);
// ファイアボールを設置する
void SetFireBall(D3DXVECTOR3 Pos);
// ファイアボールのポインタを取得する
BULLET *GetBullet(int Bullet_No);

#endif
