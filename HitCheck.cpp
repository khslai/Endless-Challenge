//=============================================================================
//
// 当たり判定処理 [HitCheck.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "HitCheck.h"
#include "Player.h"
#include "Boss.h"
#include "FireBall.h"
#include "Equipment.h"
#include "Effect.h"
#include "DebugProcess.h"
#include "AttackRange.h"
#include "Tutorial.h"
#include "Title.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FireBallDamage (40.0f)		// ファイアボールのダメージ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// 点と点の距離の2乗
float PntPntDistance(D3DXVECTOR3 p1, D3DXVECTOR3 p2);
// 点と線分の距離の2乗
float PntSegDistance(D3DXVECTOR3 p, D3DXVECTOR3 p0, D3DXVECTOR3 p1);
// 線分と線分の距離の2乗
float LinLinDistance(D3DXVECTOR3 p10, D3DXVECTOR3 p11, float& t1, D3DXVECTOR3 p20, D3DXVECTOR3 p21, float& t2);
// カプセルとカプセルの距離の2乗
float CapCapDistance(CAPSULE* Cap1, CAPSULE* Cap2);
// カプセル同士の衝突判定
bool CapsuleHitCheck(CAPSULE* Cap1, CAPSULE* Cap2);


//*****************************************************************************
// グローバル変数
//*****************************************************************************


//*****************************************************************************
// 当たり判定処理
//*****************************************************************************
void HitCheck(void)
{
	int GameCount = GetGameCount();
	int GameStage = GetGameStage();
	int Difficulty = GetDifficulty();
	int TutorialState = GetTutorialState();
	float Distance = 0.0f;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	BULLET *Bullet = GetBullet(0);
	CUBE *Cube = GetCube();
	D3DXVECTOR3 DirVec;

	if (GameCount == 0)
	{
		return;
	}

	if (GameStage == Stage_Tutorial)
	{
		if (Cube->Exist == true)
		{
			// プレイヤーと謎の立方体
			// 立方体の地面座標
			D3DXVECTOR3 CubeGroundPos = D3DXVECTOR3(Cube->Pos.x, 0.0f, Cube->Pos.z);
			float Distance = PntPntDistance(Player->Pos, CubeGroundPos);
			if (Distance < pow((Player->HitCapsule.Radius + Cube->HitCapsule.Radius + 15.0f), 2))
			{
				DirVec = Player->Pos - CubeGroundPos;
				D3DXVec3Normalize(&DirVec, &DirVec);
				Player->Pos += DirVec * 2.0f;
			}

			// プレイヤーの剣と謎の立方体
			if (Player->GiveDamage == false && Player->Animation->SlashTrack == true &&
				CapsuleHitCheck(&Player->Sword->HitCapsule, &Cube->HitCapsule))
			{
				// 効果音
				StopSound(SE3D, SE3D_Slash);
				Set3DSound(Cube->Pos, SE3D_HitCube, E_DS8_FLAG_NONE, true);

				// エフェクト設置
				SetEffect(Cube->Pos, HitLight);

				// 耐久値減少
				if (TutorialState == HelpOver)
				{
					Cube->Durability--;
				}

				// 立方体が壊された
				if (Cube->Durability <= 0)
				{
					Set3DSound(Cube->Pos, SE3D_ExplosionWind, E_DS8_FLAG_NONE, true);
					Set3DSound(Cube->Pos, SE3D_DestroyCube, E_DS8_FLAG_NONE, true);
					SetEffect(Cube->Pos, BossDebut);
					Cube->Durability = 0;
					Cube->Exist = false;
				}
				Player->GiveDamage = true;
			}
		}
	}
	else if (GameStage == Stage_Game)
	{
		// プレイヤーとボス
		if (Boss->Exist == true)
		{
			if (PntPntDistance(Player->Pos, Boss->Pos) < pow((Player->HitCapsule.Radius + Boss->HitCapsule.Radius + 20.0f), 2))
			{
				DirVec = Player->Pos - Boss->Pos;
				D3DXVec3Normalize(&DirVec, &DirVec);
				Player->Pos += DirVec * 2.0f;
				Player->Pos.y = 0.0f;
			}
		}

		// プレイヤーの剣とボス
		if (Boss->Animation->CurrentAnimID != BossAct_PowerUp && Boss->Animation->CurrentAnimID != BossAct_BossDeath)
		{
			if (Player->GiveDamage == false && Player->Animation->SlashTrack == true &&
				(CapsuleHitCheck(&Player->Sword->HitCapsule, &Boss->HitCapsule) ||
					PntPntDistance(Player->Sword->HitCapsule.P1, Boss->CenterPos) < pow(Player->HitCapsule.Radius + Boss->HitCapsule.Radius, 2)))
			{
				// 効果音
				StopSound(SE3D, SE3D_Slash);
				StopSound(SE3D, SE3D_Slash2);
				Set3DSound(Boss->CenterPos, SE3D_HitHuman, E_DS8_FLAG_NONE, true);

				// HP減少
				Boss->HP -= Player->Damage;
				if (Boss->HP <= 0.0f)
				{
					Boss->HP = 0.0f;
				}

				// 現在のアニメーションが中断できる
				if (Boss->Animation->Cancelable == true)
				{
					ChangeAnimation(Boss->Animation, BossAct_HitReact, 1.5f, true);
					Boss->Animation->NextAnimID = BossAct_Idle;
				}

				// エフェクト設置
				SetEffect(Boss->CenterPos, HitBlood);

				Player->GiveDamage = true;
			}
		}

		// ボスの剣とプレイヤー
		if (Boss->GiveDamage == false && Boss->Animation->SlashTrack == true &&
			Player->Invincible == false && CapsuleHitCheck(&Boss->Sword->HitCapsule, &Player->HitCapsule))
		{
			// 効果音
			Set3DSound(Player->CenterPos, SE3D_HitFire, E_DS8_FLAG_NONE, true);

			// HP減少
			if (Difficulty == Easy)
			{
				Player->HP -= Boss->Animation->AnimData[Boss->Animation->CurrentAnimID].Damage * 0.7f;
			}
			else
			{
				Player->HP -= Boss->Animation->AnimData[Boss->Animation->CurrentAnimID].Damage;
			}
			if (Player->HP <= 0.0f)
			{
				Player->HP = 0.0f;
			}

			if (Player->Animation->CurrentAnimID != Rolling && Player->Animation->CurrentAnimID != PlayerDeath)
			{
				ChangeAnimation(Player->Animation, HitReact, 1.0f, true);
				Player->Animation->NextAnimID = Idle;
			}

			// エフェクト設置
			if (Boss->Phase == Phase1)
			{
				SetEffect(Player->Pos, HitFire);
			}
			else if (Boss->Phase == Phase2)
			{
				SetEffect(Player->CenterPos, HitFire_Phase2);
			}

			Boss->GiveDamage = true;
			Player->BeDamaged = true;
		}

		// ボスの弾とプレイヤー
		if (Player->BeDamaged == false)
		{
			for (int Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
			{
				Bullet = GetBullet(Bullet_No);
				if (Bullet->Use == true)
				{
					// 直撃
					if (Bullet->State == Shot)
					{
						// 弾とプレイヤー距離計算
						Distance = PntSegDistance(Bullet->Pos, Player->HitCapsule.P1, Player->HitCapsule.P2);

						if (Distance < pow((Bullet->Radius + Player->HitCapsule.Radius), 2))
						{
							// 命中エフェクトを設置する
							Bullet->State = SetBurnningEffect;

							// 効果音
							Set3DSound(Player->Pos, SE3D_Burnning, E_DS8_FLAG_NONE, true);

							// HP減少
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// 倒れる
								ChangeAnimation(Player->Animation, FallingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;
							}

							Player->BeDamaged = true;
						}
					}
					// 踏まれる爆発
					else if (Bullet->State == WaitDisappear)
					{
						Distance = PntPntDistance(Bullet->Pos, Player->Pos);
						if (Distance < pow(BurnningRange, 2))
						{
							// 爆発エフェクトを設置する
							Bullet->State = BurnningExplode;

							// HP減少
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// 吹っ飛ばされる
								ChangeAnimation(Player->Animation, FlyingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;

								// 飛ぶ方向を計算する
								Player->FlyingBackDir = Player->Pos - Bullet->Pos;
								D3DXVec3Normalize(&Player->FlyingBackDir, &Player->FlyingBackDir);
								Player->FlyingBackDir.y = 0.0f;
							}

							Player->BeDamaged = true;
						}
					}
					// 消える爆発
					else if (Bullet->State == DisappearExplode)
					{
						Distance = PntPntDistance(Bullet->Pos, Player->Pos);
						if (Distance < pow(ExplodeRange, 2))
						{
							// HP減少
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// 吹っ飛ばされる
								ChangeAnimation(Player->Animation, FlyingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;

								// 飛ぶ方向を計算する
								Player->FlyingBackDir = Player->Pos - Bullet->Pos;
								D3DXVec3Normalize(&Player->FlyingBackDir, &Player->FlyingBackDir);
								Player->FlyingBackDir.y = 0.0f;
							}

							Player->BeDamaged = true;
						}
					}
				}
			}
		}
	}

	return;
}

//*****************************************************************************
// カプセル同士の衝突判定
//*****************************************************************************
bool CapsuleHitCheck(CAPSULE* Cap1, CAPSULE* Cap2)
{
	float Distance = CapCapDistance(Cap1, Cap2);
	if (Distance < pow((Cap1->Radius + Cap2->Radius), 2))
	{
		return true;
	}

	return false;
}


//*****************************************************************************
// 点と点の距離の2乗(Z成分が加わる以外は2Dと同じ)
//*****************************************************************************
float PntPntDistance(D3DXVECTOR3 P1, D3DXVECTOR3 P2)
{
	D3DXVECTOR3 v = P2 - P1;
	return D3DXVec3LengthSq(&v);
}

//*****************************************************************************
// 点と線分の距離の2乗(Z成分が加わる以外は2Dと同じ)
//*****************************************************************************
float PntSegDistance(D3DXVECTOR3 Center, D3DXVECTOR3 Seg_P0, D3DXVECTOR3 Seg_P1)
{
	// tの式の分母が0になる(二つの端点が重なっている)場合を考慮
	D3DXVECTOR3 Vec = Seg_P1 - Seg_P0;
	float t0 = D3DXVec3LengthSq(&Vec);
	if (t0 == 0)
	{
		// どちらかの端点(重なっているのでどちらでも)との距離を返す
		return PntPntDistance(Center, Seg_P0);
	}

	float t = ((Center.x - Seg_P0.x) * (Seg_P1.x - Seg_P0.x) +
		(Center.y - Seg_P0.y) * (Seg_P1.y - Seg_P0.y) +
		(Center.z - Seg_P0.z) * (Seg_P1.z - Seg_P0.z)) / t0;

	// 交点が線分の外側なら端点との距離を返す
	if (t < 0)
	{
		// t < 0ならp0の外側なので端点p0との距離を返す
		return PntPntDistance(Center, Seg_P0);
	}
	else if (t > 1)
	{
		// t > 1ならp1の外側なのでp1との距離を返す
		return PntPntDistance(Center, Seg_P1);
	}

	// 交点との距離を返す
	D3DXVECTOR3 pc((Seg_P1.x - Seg_P0.x) * t + Seg_P0.x,
		(Seg_P1.y - Seg_P0.y) * t + Seg_P0.y,
		(Seg_P1.z - Seg_P0.z) * t + Seg_P0.z);

	return PntPntDistance(Center, pc);
}

//*****************************************************************************
// 直線と直線の距離の2乗
// 2直線に直交する線分の長さ＝2直線の距離となる
// t1、t2には、2直線間の最短線分の端点が各線分に対してどの位置にあるかを示すパラメータが参照渡しによって返される
// 参考URL：http://d.hatena.ne.jp/obelisk2/20101228/1293521247
//*****************************************************************************
float LinLinDistance(D3DXVECTOR3 p10, D3DXVECTOR3 p11, float& t1, D3DXVECTOR3 p20, D3DXVECTOR3 p21, float& t2)
{
	D3DXVECTOR3 v1 = p11 - p10, v2 = p21 - p20;
	D3DXVECTOR3 v1n, v2n;
	D3DXVec3Normalize(&v1n, &v1);
	D3DXVec3Normalize(&v2n, &v2);

	D3DXVECTOR3 v = p20 - p10;
	float dot1 = D3DXVec3Dot(&v, &v1n);
	float dot2 = D3DXVec3Dot(&v, &v2n);
	float dot3 = D3DXVec3Dot(&v1n, &v2n);
	t1 = (dot1 - dot2 * dot3) / (1 - dot3 * dot3);
	t2 = -(dot2 - dot1 * dot3) / (1 - dot3 * dot3);
	v += v2n * t2 - v1n * t1;
	t1 /= D3DXVec3Length(&v1);
	t2 /= D3DXVec3Length(&v2);

	return PntPntDistance(D3DXVECTOR3(0, 0, 0), v);
}

//*****************************************************************************
// カプセルとカプセルの距離の2乗
//*****************************************************************************
float CapCapDistance(CAPSULE* Cap1, CAPSULE* Cap2)
{
	float d;
	D3DXVECTOR3 seg[2][2];
	D3DXVECTOR3 v;

	// カプセルの中心線分を求める
	seg[0][0] = Cap1->P1;
	seg[0][1] = Cap1->P2;

	seg[1][0] = Cap2->P1;
	seg[1][1] = Cap2->P2;

	// カプセル円筒部の長さが0(球体)なら、中心点ともう片方の中心線分との距離で処理できる
	if (seg[0][0] == seg[0][1])
	{
		return PntSegDistance(seg[0][0], seg[1][0], seg[1][1]);
	}
	if (seg[1][0] == seg[1][1])
	{
		return PntSegDistance(seg[1][0], seg[0][0], seg[0][1]);
	}

	// 平行でない「ねじれの位置関係」
	// (交差する場合も同じ計算で距離「0」となる)
	D3DXVec3Cross(&v, &Cap1->Direction, &Cap2->Direction);
	if (D3DXVec3Length(&v) != 0)
	{
		// 線分を含む直線同士の距離を求める
		float t1, t2;	// 2直線間の最短線分の端点が線分に対してどの位置にあるかを示すパラメータ
						// (直線2本分。参照渡しによって結果を受け取る)
		d = LinLinDistance(seg[0][0], seg[0][1], t1, seg[1][0], seg[1][1], t2);

		// 最短線分の両端点が両線分内にあれば最短距離となる
		if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
		{
			return d;
		}
	}

	// 線分の端点と他方の線分の距離(都合4つの値が求まる)の中で最短のものを返す
	float min = INFINITY;
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			d = PntSegDistance(seg[i][j], seg[1 - i][0], seg[1 - i][1]);
			if (d < min)
			{
				min = d;
			}
		}
	}

	return min;
}



