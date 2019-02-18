//=============================================================================
//
// エフェクトヘッダー [Effect.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#ifndef _EFFECT_H_
#define _EFFECT_H_

//*****************************************************************************
// マクロ定義
//*****************************************************************************
enum EffectType
{
	HPHeal,					// HP回復
	HitBlood,				// ボスを命中するときの出血
	HitFire,				// ボスに命中される炎
	HitFire_Phase2,			
	HitLight,				// チューブを命中する光
	SummonSwordEffect,		// 剣を召喚する
	ExplodeFire,			// ボスの攻撃爆発
	ExplodeFire_Phase2,
	FireWall,				// ファイルウォール
	FireWall_Phase2,
	DashSmoke,				// ダッシュの煙
	FirePillar,				// 炎の柱
	PowerUp,				// ボスパワーアップ
	BossDebut,				// ボス登場
	BossDeath,				// ボス死亡
	MagicCircle,			// チューブを浮かぶ魔法陣
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	EffekseerRendererDX9::Renderer		*Render;		// エフェクトレンダ
	Effekseer::Manager					*Manager;		// エフェクトマネージャー
	Effekseer::Effect					**Effect;		// エフェクトポインタ
	int									EffectNum;		// エフェクトの数
}EFFECTCONTROLLER;

typedef struct
{
	D3DXVECTOR3							Pos;			// 座標
	int									ID;				// エフェクト番号
	int									EffectType;		// エフェクト種類
	bool								Use;			// 使用フラグ
}EFFECT;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitEffect(bool FirstInit);
// 終了処理
void UninitEffect(void);
// 更新処理
void UpdateEffect(void);
// 描画処理
void DrawEffect(void);
// エフェクトを設置する
int SetEffect(D3DXVECTOR3 Pos, int EffectType);
// ボスは二段階に入る時のエフェクト
void TurnPhaseEffect(void);
// エフェクトを読み込む
HRESULT SafeLoadEffect(Effekseer::Manager *Manager, Effekseer::Effect **Effect, const EFK_CHAR* SrcPath, const char* ErrorSrc);
// エフェクトコントローラーを取得する
EFFECTCONTROLLER *GetEffectCtrl(void);

#endif
