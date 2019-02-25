//=============================================================================
//
// �����蔻�菈�� [HitCheck.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
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
// �}�N����`
//*****************************************************************************
#define FireBallDamage (40.0f)		// �t�@�C�A�{�[���̃_���[�W

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �_�Ɠ_�̋�����2��
float PntPntDistance(D3DXVECTOR3 p1, D3DXVECTOR3 p2);
// �_�Ɛ����̋�����2��
float PntSegDistance(D3DXVECTOR3 p, D3DXVECTOR3 p0, D3DXVECTOR3 p1);
// �����Ɛ����̋�����2��
float LinLinDistance(D3DXVECTOR3 p10, D3DXVECTOR3 p11, float& t1, D3DXVECTOR3 p20, D3DXVECTOR3 p21, float& t2);
// �J�v�Z���ƃJ�v�Z���̋�����2��
float CapCapDistance(CAPSULE* Cap1, CAPSULE* Cap2);
// �J�v�Z�����m�̏Փ˔���
bool CapsuleHitCheck(CAPSULE* Cap1, CAPSULE* Cap2);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************


//*****************************************************************************
// �����蔻�菈��
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
			// �v���C���[�Ɠ�̗�����
			// �����̂̒n�ʍ��W
			D3DXVECTOR3 CubeGroundPos = D3DXVECTOR3(Cube->Pos.x, 0.0f, Cube->Pos.z);
			float Distance = PntPntDistance(Player->Pos, CubeGroundPos);
			if (Distance < pow((Player->HitCapsule.Radius + Cube->HitCapsule.Radius + 15.0f), 2))
			{
				DirVec = Player->Pos - CubeGroundPos;
				D3DXVec3Normalize(&DirVec, &DirVec);
				Player->Pos += DirVec * 2.0f;
			}

			// �v���C���[�̌��Ɠ�̗�����
			if (Player->GiveDamage == false && Player->Animation->SlashTrack == true &&
				CapsuleHitCheck(&Player->Sword->HitCapsule, &Cube->HitCapsule))
			{
				// ���ʉ�
				StopSound(SE3D, SE3D_Slash);
				Set3DSound(Cube->Pos, SE3D_HitCube, E_DS8_FLAG_NONE, true);

				// �G�t�F�N�g�ݒu
				SetEffect(Cube->Pos, HitLight);

				// �ϋv�l����
				if (TutorialState == HelpOver)
				{
					Cube->Durability--;
				}

				// �����̂��󂳂ꂽ
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
		// �v���C���[�ƃ{�X
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

		// �v���C���[�̌��ƃ{�X
		if (Boss->Animation->CurrentAnimID != BossAct_PowerUp && Boss->Animation->CurrentAnimID != BossAct_BossDeath)
		{
			if (Player->GiveDamage == false && Player->Animation->SlashTrack == true &&
				(CapsuleHitCheck(&Player->Sword->HitCapsule, &Boss->HitCapsule) ||
					PntPntDistance(Player->Sword->HitCapsule.P1, Boss->CenterPos) < pow(Player->HitCapsule.Radius + Boss->HitCapsule.Radius, 2)))
			{
				// ���ʉ�
				StopSound(SE3D, SE3D_Slash);
				StopSound(SE3D, SE3D_Slash2);
				Set3DSound(Boss->CenterPos, SE3D_HitHuman, E_DS8_FLAG_NONE, true);

				// HP����
				Boss->HP -= Player->Damage;
				if (Boss->HP <= 0.0f)
				{
					Boss->HP = 0.0f;
				}

				// ���݂̃A�j���[�V���������f�ł���
				if (Boss->Animation->Cancelable == true)
				{
					ChangeAnimation(Boss->Animation, BossAct_HitReact, 1.5f, true);
					Boss->Animation->NextAnimID = BossAct_Idle;
				}

				// �G�t�F�N�g�ݒu
				SetEffect(Boss->CenterPos, HitBlood);

				Player->GiveDamage = true;
			}
		}

		// �{�X�̌��ƃv���C���[
		if (Boss->GiveDamage == false && Boss->Animation->SlashTrack == true &&
			Player->Invincible == false && CapsuleHitCheck(&Boss->Sword->HitCapsule, &Player->HitCapsule))
		{
			// ���ʉ�
			Set3DSound(Player->CenterPos, SE3D_HitFire, E_DS8_FLAG_NONE, true);

			// HP����
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

			// �G�t�F�N�g�ݒu
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

		// �{�X�̒e�ƃv���C���[
		if (Player->BeDamaged == false)
		{
			for (int Bullet_No = 0; Bullet_No < BulletMax; Bullet_No++)
			{
				Bullet = GetBullet(Bullet_No);
				if (Bullet->Use == true)
				{
					// ����
					if (Bullet->State == Shot)
					{
						// �e�ƃv���C���[�����v�Z
						Distance = PntSegDistance(Bullet->Pos, Player->HitCapsule.P1, Player->HitCapsule.P2);

						if (Distance < pow((Bullet->Radius + Player->HitCapsule.Radius), 2))
						{
							// �����G�t�F�N�g��ݒu����
							Bullet->State = SetBurnningEffect;

							// ���ʉ�
							Set3DSound(Player->Pos, SE3D_Burnning, E_DS8_FLAG_NONE, true);

							// HP����
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// �|���
								ChangeAnimation(Player->Animation, FallingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;
							}

							Player->BeDamaged = true;
						}
					}
					// ���܂�锚��
					else if (Bullet->State == WaitDisappear)
					{
						Distance = PntPntDistance(Bullet->Pos, Player->Pos);
						if (Distance < pow(BurnningRange, 2))
						{
							// �����G�t�F�N�g��ݒu����
							Bullet->State = BurnningExplode;

							// HP����
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// ������΂����
								ChangeAnimation(Player->Animation, FlyingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;

								// ��ԕ������v�Z����
								Player->FlyingBackDir = Player->Pos - Bullet->Pos;
								D3DXVec3Normalize(&Player->FlyingBackDir, &Player->FlyingBackDir);
								Player->FlyingBackDir.y = 0.0f;
							}

							Player->BeDamaged = true;
						}
					}
					// �����锚��
					else if (Bullet->State == DisappearExplode)
					{
						Distance = PntPntDistance(Bullet->Pos, Player->Pos);
						if (Distance < pow(ExplodeRange, 2))
						{
							// HP����
							Player->HP -= FireBallDamage;
							if (Player->HP <= 0.0f)
							{
								Player->HP = 0.0f;
							}
							else
							{
								// ������΂����
								ChangeAnimation(Player->Animation, FlyingBack, 1.0f, false);
								Player->Animation->NextAnimID = FallToStand;

								// ��ԕ������v�Z����
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
// �J�v�Z�����m�̏Փ˔���
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
// �_�Ɠ_�̋�����2��(Z�����������ȊO��2D�Ɠ���)
//*****************************************************************************
float PntPntDistance(D3DXVECTOR3 P1, D3DXVECTOR3 P2)
{
	D3DXVECTOR3 v = P2 - P1;
	return D3DXVec3LengthSq(&v);
}

//*****************************************************************************
// �_�Ɛ����̋�����2��(Z�����������ȊO��2D�Ɠ���)
//*****************************************************************************
float PntSegDistance(D3DXVECTOR3 Center, D3DXVECTOR3 Seg_P0, D3DXVECTOR3 Seg_P1)
{
	// t�̎��̕��ꂪ0�ɂȂ�(��̒[�_���d�Ȃ��Ă���)�ꍇ���l��
	D3DXVECTOR3 Vec = Seg_P1 - Seg_P0;
	float t0 = D3DXVec3LengthSq(&Vec);
	if (t0 == 0)
	{
		// �ǂ��炩�̒[�_(�d�Ȃ��Ă���̂łǂ���ł�)�Ƃ̋�����Ԃ�
		return PntPntDistance(Center, Seg_P0);
	}

	float t = ((Center.x - Seg_P0.x) * (Seg_P1.x - Seg_P0.x) +
		(Center.y - Seg_P0.y) * (Seg_P1.y - Seg_P0.y) +
		(Center.z - Seg_P0.z) * (Seg_P1.z - Seg_P0.z)) / t0;

	// ��_�������̊O���Ȃ�[�_�Ƃ̋�����Ԃ�
	if (t < 0)
	{
		// t < 0�Ȃ�p0�̊O���Ȃ̂Œ[�_p0�Ƃ̋�����Ԃ�
		return PntPntDistance(Center, Seg_P0);
	}
	else if (t > 1)
	{
		// t > 1�Ȃ�p1�̊O���Ȃ̂�p1�Ƃ̋�����Ԃ�
		return PntPntDistance(Center, Seg_P1);
	}

	// ��_�Ƃ̋�����Ԃ�
	D3DXVECTOR3 pc((Seg_P1.x - Seg_P0.x) * t + Seg_P0.x,
		(Seg_P1.y - Seg_P0.y) * t + Seg_P0.y,
		(Seg_P1.z - Seg_P0.z) * t + Seg_P0.z);

	return PntPntDistance(Center, pc);
}

//*****************************************************************************
// �����ƒ����̋�����2��
// 2�����ɒ�����������̒�����2�����̋����ƂȂ�
// t1�At2�ɂ́A2�����Ԃ̍ŒZ�����̒[�_���e�����ɑ΂��Ăǂ̈ʒu�ɂ��邩�������p�����[�^���Q�Ɠn���ɂ���ĕԂ����
// �Q�lURL�Fhttp://d.hatena.ne.jp/obelisk2/20101228/1293521247
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
// �J�v�Z���ƃJ�v�Z���̋�����2��
//*****************************************************************************
float CapCapDistance(CAPSULE* Cap1, CAPSULE* Cap2)
{
	float d;
	D3DXVECTOR3 seg[2][2];
	D3DXVECTOR3 v;

	// �J�v�Z���̒��S���������߂�
	seg[0][0] = Cap1->P1;
	seg[0][1] = Cap1->P2;

	seg[1][0] = Cap2->P1;
	seg[1][1] = Cap2->P2;

	// �J�v�Z���~�����̒�����0(����)�Ȃ�A���S�_�Ƃ����Е��̒��S�����Ƃ̋����ŏ����ł���
	if (seg[0][0] == seg[0][1])
	{
		return PntSegDistance(seg[0][0], seg[1][0], seg[1][1]);
	}
	if (seg[1][0] == seg[1][1])
	{
		return PntSegDistance(seg[1][0], seg[0][0], seg[0][1]);
	}

	// ���s�łȂ��u�˂���̈ʒu�֌W�v
	// (��������ꍇ�������v�Z�ŋ����u0�v�ƂȂ�)
	D3DXVec3Cross(&v, &Cap1->Direction, &Cap2->Direction);
	if (D3DXVec3Length(&v) != 0)
	{
		// �������܂ޒ������m�̋��������߂�
		float t1, t2;	// 2�����Ԃ̍ŒZ�����̒[�_�������ɑ΂��Ăǂ̈ʒu�ɂ��邩�������p�����[�^
						// (����2�{���B�Q�Ɠn���ɂ���Č��ʂ��󂯎��)
		d = LinLinDistance(seg[0][0], seg[0][1], t1, seg[1][0], seg[1][1], t2);

		// �ŒZ�����̗��[�_�����������ɂ���΍ŒZ�����ƂȂ�
		if (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1)
		{
			return d;
		}
	}

	// �����̒[�_�Ƒ����̐����̋���(�s��4�̒l�����܂�)�̒��ōŒZ�̂��̂�Ԃ�
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



