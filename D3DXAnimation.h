﻿//=============================================================================
//
// アニメーションヘッダー [D3DXAnimation.h]
// Author：HAL東京　ゲーム学科1年生　頼凱興 
//
//=============================================================================

#ifndef _D3DXANIMATION_H_
#define _D3DXANIMATION_H_

#include "AllocateHierarchy.h"

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// アニメーションセットデータ構造体
typedef struct
{
	LPCSTR				SetName;				// アニメーションセットの名前
	ID3DXAnimationSet	*AnimSet;				// アニメーションセット
	float				ShiftTime;              // シフトするのにかかる時間
	float				CurWeightTime;          // 現在のウェイト時間
	float				Damage;					// 与えるダメージ
}ANIMATIONDATA;

// アニメーション構造体
typedef struct D3DXANIMATION D3DXANIMATION;
struct D3DXANIMATION
{
	// メンバー変数
	LPD3DXANIMATIONCONTROLLER	AnimController;	// アニメーションコントローラー
	UINT						PreviousAnimID;	// 前再生していたアニメーションの番号
	UINT						CurrentAnimID;	// 現在再生しているアニメーションの番号
	UINT						NextAnimID;		// 次再生するアニメーションの番号
	ANIMATIONDATA				*AnimData;		// アニメーションセットのデータ
	AllocateHierarchy			*AllocateHier;	// x fileの各情報を保存する
	LPD3DXFRAME					FrameRoot;		// ルートフレイム	
	int							AnimSetNum;		// アニメーションセットの数
	const char					*Owner;			// 所有者の名前
	bool						MotionEnd;		// 今再生しているアニメーションは最後かどうか
	bool						StartMove;		// キャラの移動を停止かどうか
	bool						SlashTrack;		// 斬撃の軌跡を設置するフラグ
	bool						MotionBlendOver;// モーションブレンド終了
	bool						Cancelable;		// このアニメーションは中断できるかフラグ
};

// アニメーションコールバック構造体
struct AnimCallBackHandler : public ID3DXAnimationCallbackHandler
{
	// メンバー変数
	D3DXANIMATION		*AnimPointer;

	// メンバー関数
	HRESULT CALLBACK HandleCallback(THIS_ UINT Track, LPVOID pCallbackData);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 初期化処理
HRESULT InitAnimation(D3DXANIMATION* Animation, int Set_No);
// 終了処理
void UninitAnimation(D3DXANIMATION* Animation);
// 更新処理
void UpdateAnimation(D3DXANIMATION* Animation, float Time);
// 描画処理
void DrawAnimation(D3DXANIMATION* Animation, LPD3DXMATRIX WorldMatrix, bool DrawShadowMap);
// スキンメッシュシェーダーを読み込む
HRESULT LoadSkinMeshShader(void);
// アニメーションを切り替える
void ChangeAnimation(D3DXANIMATION* Animation, UINT AnimID, float PlaySpeed, bool ResetMotion);
// モデルのx Fileを読み込む
HRESULT Load_xFile(D3DXANIMATION* D3DXAnimation, LPCTSTR filename, const char* ErrorSrc);
// アニメーション中断イベントのKeyframesを設置する
HRESULT InitCallbackKeys_Player(void);
HRESULT InitCallbackKeys_Boss(void);
// 特定なボーンのマトリックスを取得
D3DXMATRIX GetBoneMatrix(D3DXANIMATION* Animation, const char* BoneName);


#endif
