//=============================================================================
//
// アニメーション処理 [D3DXAnimation.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "D3DXAnimation.h"
#include "Player.h"
#include "Boss.h"
#include "Equipment.h"
#include "ShadowMapping.h"
#include "ThirdPersonCamera.h"
#include "Light.h"
#include "Effect.h"
#include "Transition.h"
#include "Title.h"
#include "FireBall.h"
#include "Sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define Keydata_Max (50)
#define SkinMeshShader	_T("data/Shaders/SkinnedMesh.fx")

enum CallbackKeyType
{
	NoEvent = 0,
	StartMove,				// 攻撃中の移動開始
	StopMove,				// 攻撃中の移動終了
	MoveBack,				// 攻撃中の後退開始
	StopMoveBack,			// 攻撃中の後退終了
	SetSlashTrack,			// 剣の軌跡を設置開始
	StopSlashTrack,			// 剣の軌跡を設置終了
	SlowSlowSpeed,			// モーションスピード：超遅い
	SlowSpeed,				// モーションスピード：遅い
	NormalSpeed,			// モーションスピード：普通
	Invincible,				// 回避の無敵状態中
	NotInvincible,			// 回避の無敵状態終了
	StartJump,				// ジャンプ開始
	FallingDown,			// 落下開始
	StopJump,				// ジャンプ終了
	SetFireWallEffect,		// ファイアウォールエフェクト設置
	SetExplodeFire,			// 攻撃爆発エフェクト設置
	SetRushSmoke,			// ダッシュの煙エフェクト設置
	SetFireBallEffect,		// ファイアボール設置
	SetBossPhase,			// ボスは第二段階に設置する
	SetFootStepSE,			// 足音を設置する
	SetRollingSE,			// ローリング効果音
	SetFallingSE,			// 倒れる効果音
	SetFallingSE_2,			
	TurnRotation,			// 攻撃中の方向更新
	NotCancelable,			// モーションが中断できない状態になる
	SetFadein,				// ゲームステージ遷移開始
	ResetTime,				// アニメーションのローカル時間をリセット 
	MotionEnd,				// モーション終了
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	float	Time;			// Keyframeの時間
	int		KeyType;		// Keyframeの種類
}KEYDATA;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// メッシュを描画する
void DrawMeshContainer(D3DXANIMATION* Animation, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, bool DrawShadowMap);
// フレイム(ボーン)を描画する
void DrawFrame(D3DXANIMATION* Animation, LPD3DXFRAME pFrame, bool DrawShadowMap);
// ボーンマトリクス間の関係を設置する
HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot);
// フレイムのマトリクスを更新する
void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix);
// 特定なボーンのマトリックスを取得
D3DXFRAME_DERIVED* SearchBoneFrame(D3DXANIMATION* Animation, const char* BoneName, D3DXFRAME* Frame);
// アニメーション中断イベントのキーを設置する
HRESULT SetupCallbackKeys(KEYDATA *Keydata, LPCSTR SetName, int CallbacksNum, const char *Owner);
// キーデータを設置する
void AddKeydata(float Time, int KeyType);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
// 描画モード
static int		SkinningMethod = HLSL_Mode;
int				Keydata_No = 0;
KEYDATA			Keydata[Keydata_Max];
LPD3DXEFFECT    SkinMeshEffect = NULL;


//=============================================================================
// スキンメッシュシェーダーを読み込む
//=============================================================================
HRESULT LoadSkinMeshShader(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	if (FAILED(D3DXCreateEffectFromFile(Device, SkinMeshShader, NULL, NULL, 0,
		NULL, &SkinMeshEffect, NULL)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// ボーンマトリクス間の関係を設置する
//=============================================================================
HRESULT SetupBoneMatrixPointers(LPD3DXFRAME pFrameBase, LPD3DXFRAME pFrameRoot)
{
	if (pFrameBase->pMeshContainer != NULL)
	{
		D3DXFRAME_DERIVED* pFrame = NULL;
		D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrameBase->pMeshContainer;

		// if there is a skinmesh, then setup the bone matrices
		if (pMeshContainer->pSkinInfo != NULL)
		{
			UINT cBones = pMeshContainer->pSkinInfo->GetNumBones();
			pMeshContainer->ppBoneMatrix = new D3DXMATRIX*[cBones];
			for (UINT iBone = 0; iBone < cBones; iBone++)
			{
				pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
				if (pFrame == NULL) return E_FAIL;

				pMeshContainer->ppBoneMatrix[iBone] = &pFrame->CombinedTransformationMatrix;
			}
		}
	}

	if (pFrameBase->pFrameSibling != NULL)
	{
		if (FAILED(SetupBoneMatrixPointers(pFrameBase->pFrameSibling, pFrameRoot)))
			return E_FAIL;
	}

	if (pFrameBase->pFrameFirstChild != NULL)
	{
		if (FAILED(SetupBoneMatrixPointers(pFrameBase->pFrameFirstChild, pFrameRoot)))
			return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// フレイム(ボーン)を描画する
//=============================================================================
void DrawFrame(D3DXANIMATION* Animation, LPD3DXFRAME pFrame, bool DrawShadowMap)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	LPD3DXMESHCONTAINER pMeshContainer;

	if (pFrame == NULL)
	{
		return;
	}

	// メッシュを取得する
	pMeshContainer = pFrame->pMeshContainer;

	while (pMeshContainer != NULL)
	{
		// メッシュを描画する
		DrawMeshContainer(Animation, pMeshContainer, pFrame, DrawShadowMap);
		// 次のメッシュを探す
		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	// 兄弟フレイムを描画する
	DrawFrame(Animation, pFrame->pFrameSibling, DrawShadowMap);
	// 子フレイムを描画する
	DrawFrame(Animation, pFrame->pFrameFirstChild, DrawShadowMap);
}

//=============================================================================
// メッシュを描画する
//=============================================================================
void DrawMeshContainer(D3DXANIMATION* Animation, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, bool DrawShadowMap)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	LPDIRECT3DDEVICE9 Device = GetDevice();
	UINT iMaterial;
	UINT NumBlend;
	UINT iAttrib;
	DWORD AttribIdPrev;
	LPD3DXBONECOMBINATION pBoneComb;
	UINT iPaletteEntry;
	D3DXMATRIX CameraViewMatrix = GetViewMatrix();

	UINT iMatrixIndex;
	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	Device->GetDeviceCaps(&d3dCaps);

	// first check for skinning
	if (SkinningMethod == NormalMode)
	{
		if (pMeshContainer->pSkinInfo != NULL)
		{
			AttribIdPrev = UNUSED32;
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());

			// Draw using default vtx processing of the device (typically HW)
			for (iAttrib = 0; iAttrib < pMeshContainer->BoneNum; iAttrib++)
			{
				NumBlend = 0;
				for (DWORD i = 0; i < pMeshContainer->BoneWeightNum; ++i)
				{
					if (pBoneComb[iAttrib].BoneId[i] != UINT_MAX)
					{
						NumBlend = i;
					}
				}

				if (d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1)
				{
					// first calculate the world matrices for the current set of blend weights and get the accurate count of the number of blends
					for (DWORD i = 0; i < pMeshContainer->BoneWeightNum; ++i)
					{
						iMatrixIndex = pBoneComb[iAttrib].BoneId[i];
						if (iMatrixIndex != UINT_MAX)
						{
							D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
								pMeshContainer->ppBoneMatrix[iMatrixIndex]);
							Device->SetTransform(D3DTS_WORLDMATRIX(i), &matTemp);
						}
					}

					Device->SetRenderState(D3DRS_VERTEXBLEND, NumBlend);

					// lookup the material used for this subset of faces
					if ((AttribIdPrev != pBoneComb[iAttrib].AttribId) || (AttribIdPrev == UNUSED32))
					{
						Device->SetMaterial(&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D);
						Device->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);
						AttribIdPrev = pBoneComb[iAttrib].AttribId;
					}

					// draw the subset now that the correct material and matrices are loaded
					pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
				}
			}
			Device->SetRenderState(D3DRS_VERTEXBLEND, 0);
		}
		else  // standard mesh, just draw it after setting material properties
		{
			Device->SetTransform(D3DTS_WORLD, &pFrame->CombinedTransformationMatrix);

			for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
			{
				Device->SetMaterial(&pMeshContainer->pMaterials[iMaterial].MatD3D);
				Device->SetTexture(0, pMeshContainer->ppTextures[iMaterial]);
				pMeshContainer->MeshData.pMesh->DrawSubset(iMaterial);
			}
		}
	}
	else if (SkinningMethod == HLSL_Mode)
	{
		if (DrawShadowMap == true)
		{
			SkinMeshEffect->SetTechnique("RenderShadow");

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < pMeshContainer->BoneNum; iAttrib++)
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							pMeshContainer->ppBoneMatrix[iMatrixIndex]);
						pMeshContainer->g_pBoneMatrices[iPaletteEntry] = matTemp;
					}
				}
				SkinMeshEffect->SetMatrixArray("mWorldMatrixArray", pMeshContainer->g_pBoneMatrices,
					pMeshContainer->NumPaletteEntries);

				// Set CurNumBones to select the correct vertex shader for the number of bones
				SkinMeshEffect->SetInt("CurNumBones", pMeshContainer->BoneWeightNum - 1);

				D3DXMATRIX ViewMatrix = GetLightViewMatrix();
				D3DXMATRIX ProjMatrix = GetLightProjMatrix();
				SkinMeshEffect->SetMatrix("matView", &ViewMatrix);
				SkinMeshEffect->SetMatrix("matProj", &ProjMatrix);
				SkinMeshEffect->CommitChanges();
				// Start the effect now all parameters have been updated
				UINT numPasses;
				//SkinMeshEffect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
				SkinMeshEffect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
				for (UINT iPass = 0; iPass < numPasses; iPass++)
				{
					SkinMeshEffect->BeginPass(iPass);

					// draw the subset with the current world matrix palette and material state
					pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);

					SkinMeshEffect->EndPass();
				}

				SkinMeshEffect->End();

				Device->SetVertexShader(NULL);
			}
		}
		else
		{
			SkinMeshEffect->SetTechnique("RenderSkinMesh");

			if (pMeshContainer->UseSoftwareVP)
			{
				// If hw or pure hw vertex processing is forced, we can't render the
				// mesh, so just exit out.  Typical applications should create
				// a device with appropriate vertex processing capability for this
				// skinning method.

				//if (g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
				//	return;

				Device->SetSoftwareVertexProcessing(TRUE);
			}

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (iAttrib = 0; iAttrib < pMeshContainer->BoneNum; iAttrib++)
			{
				// first calculate all the world matrices
				for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							pMeshContainer->ppBoneMatrix[iMatrixIndex]);
						D3DXMatrixMultiply(&pMeshContainer->g_pBoneMatrices[iPaletteEntry], &matTemp, &CameraViewMatrix);
					}
				}
				SkinMeshEffect->SetMatrixArray("mWorldMatrixArray", pMeshContainer->g_pBoneMatrices,
					pMeshContainer->NumPaletteEntries);

				// Sum of all ambient and emissive contribution
				D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
				D3DXCOLOR color2(.25, .25, .25, 1.0);
				D3DXCOLOR ambEmm;
				D3DXColorModulate(&ambEmm, &color1, &color2);
				ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

				// set material color properties 
				SkinMeshEffect->SetVector("MaterialDiffuse",
					(D3DXVECTOR4*)&(
						pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
				SkinMeshEffect->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

				// setup the material of the mesh subset - REMEMBER to use the original pre-skinning attribute id to get the correct material id
				Device->SetTexture(0, pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId]);

				// Set CurNumBones to select the correct vertex shader for the number of bones
				SkinMeshEffect->SetInt("CurNumBones", pMeshContainer->BoneWeightNum - 1);

				// Start the effect now all parameters have been updated
				UINT numPasses;
				SkinMeshEffect->Begin(&numPasses, D3DXFX_DONOTSAVESTATE);
				for (UINT iPass = 0; iPass < numPasses; iPass++)
				{
					SkinMeshEffect->BeginPass(iPass);

					// draw the subset with the current world matrix palette and material state
					pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);

					SkinMeshEffect->EndPass();
				}

				SkinMeshEffect->End();

				Device->SetVertexShader(NULL);
			}

			// remember to reset back to hw vertex processing if software was required
			if (pMeshContainer->UseSoftwareVP)
			{
				Device->SetSoftwareVertexProcessing(FALSE);
			}
		}
	}
}

//=============================================================================
// フレイムのマトリクスを更新する
//=============================================================================
void UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	LPDIRECT3DDEVICE9 Device = GetDevice();

	if (pFrameBase == NULL || pParentMatrix == NULL)
	{
		return;
	}

	// 今のフレイム（ボーン）のマトリクスと親フレイム（ボーン）のマトリクスを乗算する
	D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);

	UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);                              // 兄弟フレイムを更新する
	UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);   // 子フレイムを更新する
}

//=============================================================================
// x Fileを読み込む
//=============================================================================
HRESULT Load_xFile(D3DXANIMATION* D3DXAnimation, LPCTSTR filename, const char* ErrorSrc)
{
	char Message[256];
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXAnimation->AllocateHier = new AllocateHierarchy();

	if (
		FAILED(D3DXLoadMeshHierarchyFromX(filename,
			D3DXMESH_MANAGED,
			Device,
			D3DXAnimation->AllocateHier,
			NULL,
			&D3DXAnimation->FrameRoot,
			&D3DXAnimation->AnimController))
		)
	{
		sprintf(Message, "Load %s Model Failed！", ErrorSrc);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	if (FAILED(SetupBoneMatrixPointers(D3DXAnimation->FrameRoot, D3DXAnimation->FrameRoot)))
	{
		return E_FAIL;
	}

	D3DXAnimation->MotionEnd = false;
	D3DXAnimation->AnimSetNum = D3DXAnimation->AnimController->GetMaxNumAnimationSets();
	// メモリを配る
	D3DXAnimation->AnimData = (ANIMATIONDATA*)calloc(D3DXAnimation->AnimSetNum, sizeof(ANIMATIONDATA));
	if (D3DXAnimation->AnimData == NULL)
	{
		sprintf(Message, "Alloc %s AnimationData Memory Failed！", ErrorSrc);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// 特定なボーンのマトリックスを取得
//=============================================================================
D3DXMATRIX GetBoneMatrix(D3DXANIMATION* Animation, const char* BoneName)
{
	char Message[64];
	D3DXFRAME_DERIVED* pFrame = SearchBoneFrame(Animation, BoneName, Animation->FrameRoot);

	// ボーンが見つかれば
	if (pFrame != NULL && pFrame->Name != NULL && strcmp(pFrame->Name, BoneName) == 0)
	{
		return pFrame->CombinedTransformationMatrix;
	}
	// ボーンが見つからなければ
	else
	{
		sprintf(Message, "Find %s Bone Failed！", BoneName);
		MessageBox(0, Message, "Error", 0);
		D3DXMATRIX TmpMatrix;
		D3DXMatrixIdentity(&TmpMatrix);
		return TmpMatrix;
	}
}

//=============================================================================
// 特定なボーンを探す
//=============================================================================
D3DXFRAME_DERIVED* SearchBoneFrame(D3DXANIMATION* Animation, const char* BoneName, D3DXFRAME* Frame)
{
	D3DXFRAME_DERIVED* pFrame = NULL;

	if (Frame == NULL)
	{
		return NULL;
	}

	if (Frame->Name != NULL && strcmp(Frame->Name, BoneName) == 0)
	{
		pFrame = (D3DXFRAME_DERIVED*)Frame;
		return pFrame;
	}

	if (Frame->pFrameSibling != NULL)
	{
		pFrame = SearchBoneFrame(Animation, BoneName, Frame->pFrameSibling);
		if (pFrame != NULL && strcmp(pFrame->Name, BoneName) == 0)
		{
			return pFrame;
		}
	}

	if (Frame->pFrameFirstChild != NULL)
	{
		pFrame = SearchBoneFrame(Animation, BoneName, Frame->pFrameFirstChild);
		if (pFrame != NULL && strcmp(pFrame->Name, BoneName) == 0)
		{
			return pFrame;
		}
	}

	return NULL;
}

//=============================================================================
// CallbackKeysを初期化(プレイヤー)
//=============================================================================
HRESULT InitCallbackKeys_Player(void)
{
	// 設置するキーの数
	PLAYER *Player = GetPlayer();
	int SetNum = Player->Animation->AnimSetNum;
	float AnimLoopTime = 0.0f;

	for (int Set_No = 0; Set_No < SetNum; Set_No++)
	{
		// キーデータをクリアする
		memset(&Keydata[0], 0, sizeof(KEYDATA) * Keydata_Max);
		Keydata_No = 0;

		switch (Set_No)
		{
		case Idle:

			AnimLoopTime = 109.0f;
			break;

		case Running:

			AnimLoopTime = 41.0f;
			AddKeydata((16.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((36.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((41.0f / AnimLoopTime), ResetTime);
			break;

		case Walk_Left:

			AnimLoopTime = 72.0f;
			AddKeydata((20.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((60.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((72.0f / AnimLoopTime), ResetTime);
			break;

		case Walk_Right:

			AnimLoopTime = 75.0f;
			AddKeydata((21.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((56.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((75.0f / AnimLoopTime), ResetTime);
			break;

		case Walk_Back:

			AnimLoopTime = 80.0f;
			AddKeydata((20.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((60.0f / AnimLoopTime), SetFootStepSE);
			AddKeydata((80.0f / AnimLoopTime), ResetTime);
			break;

		case Rolling:

			AnimLoopTime = 122.0f;
			AddKeydata((0.0f / AnimLoopTime), Invincible);
			AddKeydata((1.0f / AnimLoopTime), StartMove);
			AddKeydata((35.0f / AnimLoopTime), SetRollingSE);
			AddKeydata((55.0f / AnimLoopTime), NotInvincible);
			AddKeydata((80.0f / AnimLoopTime), StopMove);
			AddKeydata((90.0f / AnimLoopTime), MotionEnd);
			break;

		case FallingBack:

			AddKeydata(0.95f, MotionEnd);
			break;

		case FlyingBack:

			AnimLoopTime = 180.0f;
			AddKeydata((1.0f / 180.0f), StartMove);
			AddKeydata((50.0f / 180.0f), SetFallingSE_2);
			AddKeydata((60.0f / 180.0f), StopMove);
			AddKeydata(0.95f, MotionEnd);
			break;

		case FallToStand:

			AddKeydata(0.95f, MotionEnd);
			break;

		case HitReact:

			AddKeydata(0.95f, MotionEnd);
			break;

		case HPRestore:

			AddKeydata(0.95f, MotionEnd);
			break;

		case Attack1:

			AnimLoopTime = 53.0f;
			AddKeydata((5.0f / AnimLoopTime), StartMove);
			AddKeydata((10.0f / AnimLoopTime), StopMove);
			AddKeydata((30.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((45.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case Attack2:

			AnimLoopTime = 65.0f;
			AddKeydata((1.0f / AnimLoopTime), StartMove);
			AddKeydata((10.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((20.0f / AnimLoopTime), StopMove);
			AddKeydata((25.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((26.0f / AnimLoopTime), MoveBack);
			AddKeydata((39.0f / AnimLoopTime), StopMoveBack);
			AddKeydata((40.0f / AnimLoopTime), MotionEnd);
			break;

		case PlayerDeath:

			AnimLoopTime = 233.0f;
			AddKeydata((100.0f / AnimLoopTime), SetFallingSE);
			AddKeydata((190.0f / AnimLoopTime), SetFallingSE_2);
			AddKeydata(0.95f, MotionEnd);
			break;

		case Sit:

			AnimLoopTime = 649.0f;
			AddKeydata(0.95f, MotionEnd);
			break;

		case StandUp:

			AnimLoopTime = 168.0f;
			AddKeydata((120.0f / AnimLoopTime), SetFadein);
			AddKeydata(0.95f, MotionEnd);
			break;

		default:

			continue;

			break;
		}

		if (FAILED(
			SetupCallbackKeys(&Keydata[0], Player->Animation->AnimData[Set_No].SetName,
				Keydata_No, "Player")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// CallbackKeysを初期化(ボス)
//=============================================================================
HRESULT InitCallbackKeys_Boss(void)
{
	BOSS *Boss = GetBoss();
	int SetNum = Boss->Animation->AnimSetNum;
	float AnimLoopTime = 0.0f;
	char Message[256] = { NULL };

	for (int Set_No = 0; Set_No < SetNum; Set_No++)
	{
		// キーデータをクリアする
		memset(&Keydata[0], 0, sizeof(KEYDATA) * Keydata_Max);
		Keydata_No = 0;

		switch (Set_No)
		{
		case BossAct_HitReact:

			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_Rolling:

			AnimLoopTime = 141.0f;
			AddKeydata((0.0f / AnimLoopTime), NotCancelable);
			AddKeydata((1.0f / AnimLoopTime), StartMove);
			AddKeydata((45.0f / AnimLoopTime), SetRollingSE);
			AddKeydata((90.0f / AnimLoopTime), StopMove);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_BossDeath:

			AnimLoopTime = 143.0f;
			AddKeydata((0.0f / AnimLoopTime), NotCancelable);
			AddKeydata((85.0f / AnimLoopTime), SetFallingSE);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_SlantSlash:

			AnimLoopTime = 98.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((48.0f / AnimLoopTime), SetFireBallEffect);
			AddKeydata((50.0f / AnimLoopTime), SetFireBallEffect);
			AddKeydata((52.0f / AnimLoopTime), SetFireBallEffect);
			AddKeydata((54.0f / AnimLoopTime), SetFireBallEffect);
			AddKeydata((56.0f / AnimLoopTime), SetFireBallEffect);
			AddKeydata((65.0f / AnimLoopTime), MotionEnd);
			break;

		case BossAct_LeftRightSlash:

			AnimLoopTime = 163.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((55.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((70.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((90.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((110.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_RollingSlash:

			AnimLoopTime = 118.0f;
			AddKeydata((5.0f / AnimLoopTime), StartMove);
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((45.0f / AnimLoopTime), StopMove);
			AddKeydata((50.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((60.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((65.0f / AnimLoopTime), SetExplodeFire);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_SurfaceSlash:

			AnimLoopTime = 118.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((50.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((65.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((70.0f / AnimLoopTime), MotionEnd);
			break;

		case BossAct_UpSlash:

			AnimLoopTime = 167.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((45.0f / AnimLoopTime), StartMove);
			AddKeydata((55.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((60.0f / AnimLoopTime), SetFireWallEffect);
			AddKeydata((69.0f / AnimLoopTime), StopMove);
			AddKeydata((70.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_HorizonSlash:

			AnimLoopTime = 76.0f;
			AddKeydata((0.0f / AnimLoopTime), SlowSpeed);
			AddKeydata((9.0f / AnimLoopTime), NotCancelable);
			AddKeydata((10.0f / AnimLoopTime), StartMove);
			AddKeydata((14.0f / AnimLoopTime), NormalSpeed);
			AddKeydata((15.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((45.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((50.0f / AnimLoopTime), StopMove);
			AddKeydata((70.0f / AnimLoopTime), MotionEnd);
			break;

		case BossAct_VerticalSlash:

			AnimLoopTime = 145.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((40.0f / AnimLoopTime), StartJump);
			AddKeydata((41.0f / AnimLoopTime), StartMove);
			AddKeydata((60.0f / AnimLoopTime), FallingDown);
			AddKeydata((75.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((80.0f / AnimLoopTime), StopJump);
			AddKeydata((81.0f / AnimLoopTime), StopMove);
			AddKeydata((85.0f / AnimLoopTime), SetExplodeFire);
			AddKeydata((90.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_Stab:

			AnimLoopTime = 79.0f;
			AddKeydata((9.0f / AnimLoopTime), NotCancelable);
			AddKeydata((10.0f / AnimLoopTime), SlowSpeed);
			AddKeydata((20.0f / AnimLoopTime), NormalSpeed);
			AddKeydata((24.0f / AnimLoopTime), SetRushSmoke);
			AddKeydata((25.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((26.0f / AnimLoopTime), StartMove);
			AddKeydata((49.0f / AnimLoopTime), StopMove);
			AddKeydata((50.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_ComboAttack1:

			AnimLoopTime = 227.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((55.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((70.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((80.0f / AnimLoopTime), TurnRotation);
			AddKeydata((90.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((110.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((125.0f / AnimLoopTime), TurnRotation);
			AddKeydata((140.0f / AnimLoopTime), StartMove);
			AddKeydata((150.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((155.0f / AnimLoopTime), StopMove);
			AddKeydata((160.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_ComboAttack2:

			AnimLoopTime = 258.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((100.0f / AnimLoopTime), StartMove);
			AddKeydata((105.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((115.0f / AnimLoopTime), StopMove);
			AddKeydata((130.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((150.0f / AnimLoopTime), TurnRotation);
			AddKeydata((169.0f / AnimLoopTime), StartMove);
			AddKeydata((170.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((185.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((190.0f / AnimLoopTime), StopMove);
			AddKeydata((210.0f / AnimLoopTime), MotionEnd);
			break;

		case BossAct_ComboAttack3:

			AnimLoopTime = 210.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((35.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((50.0f / AnimLoopTime), StartMove);
			AddKeydata((55.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((56.0f / AnimLoopTime), TurnRotation);
			AddKeydata((69.0f / AnimLoopTime), StopMove);
			AddKeydata((70.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((90.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((91.0f / AnimLoopTime), TurnRotation);
			AddKeydata((109.0f / AnimLoopTime), StartJump);
			AddKeydata((110.0f / AnimLoopTime), StartMove);
			AddKeydata((120.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((124.0f / AnimLoopTime), FallingDown);
			AddKeydata((139.0f / AnimLoopTime), StopMove);
			AddKeydata((140.0f / AnimLoopTime), StopJump);
			AddKeydata((150.0f / AnimLoopTime), SetExplodeFire);
			AddKeydata((155.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_TwoHandCombo1:

			AnimLoopTime = 211.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((50.0f / AnimLoopTime), StartMove);
			AddKeydata((74.0f / AnimLoopTime), StopMove);
			AddKeydata((75.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((90.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((110.0f / AnimLoopTime), StartMove);
			AddKeydata((129.0f / AnimLoopTime), StopMove);
			AddKeydata((130.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((145.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((150.0f / AnimLoopTime), SlowSpeed);
			AddKeydata((155.0f / AnimLoopTime), TurnRotation);
			AddKeydata((159.0f / AnimLoopTime), StartMove);
			AddKeydata((160.0f / AnimLoopTime), NormalSpeed);
			AddKeydata((161.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((175.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((180.0f / AnimLoopTime), StopMove);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_TwoHandCombo2:

			AnimLoopTime = 211.0f;
			AddKeydata((10.0f / AnimLoopTime), NotCancelable);
			AddKeydata((15.0f / AnimLoopTime), StartMove);
			AddKeydata((25.0f / AnimLoopTime), StopMove);
			AddKeydata((45.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((60.0f / AnimLoopTime), StartMove);
			AddKeydata((65.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((80.0f / AnimLoopTime), StopMove);
			AddKeydata((85.0f / AnimLoopTime), TurnRotation);
			AddKeydata((95.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((110.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((115.0f / AnimLoopTime), StartMove);
			AddKeydata((149.0f / AnimLoopTime), StopMove);
			AddKeydata((145.0f / AnimLoopTime), TurnRotation);
			AddKeydata((150.0f / AnimLoopTime), SetSlashTrack);
			AddKeydata((165.0f / AnimLoopTime), StopSlashTrack);
			AddKeydata((175.0f / AnimLoopTime), StartMove);
			AddKeydata((195.0f / AnimLoopTime), StopMove);
			AddKeydata(0.95f, MotionEnd);
			break;

		case BossAct_PowerUp:

			AnimLoopTime = 140.0f;
			AddKeydata((0.0f / AnimLoopTime), NotCancelable);
			AddKeydata((1.0f / AnimLoopTime), SlowSlowSpeed);
			AddKeydata((20.0f / AnimLoopTime), NormalSpeed);
			AddKeydata((21.0f / AnimLoopTime), SetBossPhase);
			AddKeydata(0.95f, MotionEnd);
			break;

		default:

			continue;

			break;
		}

		if (FAILED(
			SetupCallbackKeys(&Keydata[0], Boss->Animation->AnimData[Set_No].SetName,
				Keydata_No, "Boss")))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//=============================================================================
// キーデータを設置する
//=============================================================================
void AddKeydata(float Time, int KeyType)
{
	if (Keydata_No > Keydata_Max)
	{
		MessageBox(0, "Keydata_Max is too samll", "Error", 0);
		return;
	}
	else
	{
		Keydata[Keydata_No] = { Time,KeyType };
		Keydata_No++;
	}

	return;
}


//=============================================================================
// アニメーション中断イベントのキーを設置する
//=============================================================================
HRESULT SetupCallbackKeys(KEYDATA *Keydata, LPCSTR SetName, int CallbacksNum, const char *Owner)
{
	D3DXANIMATION *Animation = NULL;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	// Keyframeを設置する予定のAnimationSet
	ID3DXKeyframedAnimationSet* AnimSetTemp = NULL;
	// Keyframeを設置した新しいAnimationSet
	ID3DXCompressedAnimationSet* CompressedAnimSet = NULL;
	// 圧縮された資料
	ID3DXBuffer* CompressedInfo = NULL;
	// エラーメッセージ	
	char Message[256] = { NULL };
	double Ticks = 0.0;
	double Period = 0.0;
	// 設置するCallback Keys
	D3DXKEY_CALLBACK *Keys = NULL;

	if (strcmp(Owner, "Player") == 0)
	{
		Animation = Player->Animation;
		if (FAILED(Animation->AnimController->GetAnimationSetByName(SetName, (ID3DXAnimationSet**)&AnimSetTemp)))
		{
			sprintf(Message, "Setup Callbacks in %s AnimationSet Failed！", SetName);
			goto FunctionExit;

		}
	}
	else if (strcmp(Owner, "Boss") == 0)
	{
		Animation = Boss->Animation;
		if (FAILED(Animation->AnimController->GetAnimationSetByName(SetName, (ID3DXAnimationSet**)&AnimSetTemp)))
		{
			sprintf(Message, "Setup Callbacks in %s AnimationSet Failed！", SetName);
			goto FunctionExit;
		}
	}
	else
	{
		sprintf(Message, "Can't find this AnimationSet Owner");
		goto FunctionExit;
	}

	// 圧縮資料を渡す
	AnimSetTemp->Compress(D3DXCOMPRESS_DEFAULT, 0.5f, 0, &CompressedInfo);

	// 1秒のキーフレイムの数を返す(1秒 = 4800)
	Ticks = AnimSetTemp->GetSourceTicksPerSecond();
	// アニメーションのループ時間
	Period = AnimSetTemp->GetPeriod();

	// メモリを配る
	Keys = (D3DXKEY_CALLBACK*)calloc(CallbacksNum, sizeof(D3DXKEY_CALLBACK));
	if (Keys == NULL)
	{
		sprintf(Message, "Alloc Keys in %s AnimationSet Failed！", SetName);
		goto FunctionExit;
	}

	// Callback Keysを設置する
	for (int i = 0; i < CallbacksNum; i++)
	{
		if (Keydata[i].KeyType == NoEvent)
		{
			sprintf(Message, "There is NoEvent Keydata in %s AnimationSet！", SetName);
			goto FunctionExit;
		}
		Keys[i].Time = (float)(Period * Ticks * Keydata[i].Time);
		Keys[i].pCallbackData = (void*)Keydata[i].KeyType;
	}

	// 圧縮するアニメーションセットを作成する
	D3DXCreateCompressedAnimationSet(AnimSetTemp->GetName(),
		AnimSetTemp->GetSourceTicksPerSecond(),
		AnimSetTemp->GetPlaybackType(), CompressedInfo, CallbacksNum, Keys, &CompressedAnimSet);

	// 古いアニメーションセット(圧縮されない)を削除する
	Animation->AnimController->UnregisterAnimationSet(AnimSetTemp);

	// 新しいアニメーションセット(圧縮された)を追加する
	Animation->AnimController->RegisterAnimationSet(CompressedAnimSet);

	// 終了処理
FunctionExit:

	SafeRelease(CompressedInfo);
	SafeRelease(AnimSetTemp);
	SafeRelease(CompressedAnimSet);
	SafeFree(Keys);

	if (Message[0] == NULL)
	{
		return S_OK;
	}
	else
	{
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}
}


//=============================================================================
// アニメ再生の中断イベント
//=============================================================================
HRESULT AnimCallBackHandler::HandleCallback(UINT Track, LPVOID pCallbackData)
{
	static float PreActionSpeed = 0.0f;
	int KeyType = (int)pCallbackData;
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();

	switch (KeyType)
	{
	case StartMove:
		AnimPointer->StartMove = true;
		break;
	case StopMove:
		AnimPointer->StartMove = false;
		break;
	case MoveBack:
		Player->MoveBack = true;
		break;
	case StopMoveBack:
		Player->MoveBack = false;
		break;
	case MotionEnd:
		AnimPointer->MotionEnd = true;
		break;
	case SetSlashTrack:
		AnimPointer->SlashTrack = true;
		if (AnimPointer->Owner == "Player")
		{
			if (AnimPointer->CurrentAnimID == Attack1)
			{
				Set3DSound(Player->Pos, SE3D_Slash, E_DS8_FLAG_NONE, true);
			}
			else if (AnimPointer->CurrentAnimID == Attack2)
			{
				Set3DSound(Player->Pos, SE3D_Slash2, E_DS8_FLAG_NONE, true);
			}
		}
		else if (AnimPointer->Owner == "Boss")
		{
			Set3DSound(Boss->Pos, SE3D_BossSlash, E_DS8_FLAG_NONE, true);
			Set3DSound(Boss->Pos, SE3D_SlashFire, E_DS8_FLAG_NONE, true);
		}
		break;
	case StopSlashTrack:
		Boss->GiveDamage = false;
		AnimPointer->SlashTrack = false;
		break;
	case SlowSlowSpeed:
		PreActionSpeed = Boss->ActionSpeed;
		Boss->ActionSpeed = 0.1f;
		break;
	case SlowSpeed:
		PreActionSpeed = Boss->ActionSpeed;
		Boss->ActionSpeed = 0.33f;
		break;
	case NormalSpeed:
		Boss->ActionSpeed = PreActionSpeed;
		break;
	case Invincible:
		Player->Invincible = true;
		break;
	case NotInvincible:
		Player->Invincible = false;
		break;
	case SetFireWallEffect:
		if (Boss->Phase == Phase1)
		{
			SetEffect(Boss->Sword->HitCapsule.P2, FireWall);
		}
		else if (Boss->Phase == Phase2)
		{
			SetEffect(Boss->Sword->HitCapsule.P2, FireWall_Phase2);
		}
		break;
	case SetExplodeFire:
		Set3DSound(Boss->Sword->HitCapsule.P2, SE3D_ExplodeFire, E_DS8_FLAG_NONE, true);
		if (Boss->Phase == Phase1)
		{
			SetEffect(Boss->Sword->HitCapsule.P2, ExplodeFire);
		}
		else if (Boss->Phase == Phase2)
		{
			SetEffect(Boss->Sword->HitCapsule.P2, ExplodeFire_Phase2);
			SetEffect(Boss->Sword->HitCapsule.P2, FirePillar);
		}
		break;
	case SetRushSmoke:
		SetEffect(Boss->Pos, DashSmoke);
		break;
	case SetFireBallEffect:
		SetFireBall(Boss->Sword->HitCapsule.P2);
		break;
	case StartJump:
		Boss->JumpState = Jump_Start;
		Boss->Move.y = 15.0f;
		break;
	case FallingDown:
		Boss->JumpState = StartFalling;
		Boss->Move.y *= -1;
		break;
	case StopJump:
		Boss->JumpState = Jump_Stop;
		Boss->Move.y = 0.0f;
		Boss->Pos.y = 0.0f;
		break;
	case SetBossPhase:
		Boss->Phase = Phase2;
		TurnPhaseEffect();
		break;
	case SetFootStepSE:
		if (AnimPointer->Owner == "Player")
		{
			Set3DSound(Player->Pos, SE3D_PlayerFootStep, E_DS8_FLAG_NONE, true);
		}
		else if (AnimPointer->Owner == "Boss")
		{
			Set3DSound(Boss->Pos, SE3D_BossFootStep, E_DS8_FLAG_NONE, true);
		}
		break;
	case SetRollingSE:
		Set3DSound(Player->Pos, SE3D_Rolling, E_DS8_FLAG_NONE, true);
		break;
	case SetFallingSE:
		Set3DSound(Player->Pos, SE3D_FallingDown, E_DS8_FLAG_NONE, true);
		break;
	case SetFallingSE_2:
		Set3DSound(Player->Pos, SE3D_FallingDown2, E_DS8_FLAG_NONE, true);
		break;
	case NotCancelable:
		AnimPointer->Cancelable = false;
		break;
	case TurnRotation:
		Boss->TurnRotation = true;
		break;
	case ResetTime:
		AnimPointer->AnimController->ResetTime();
		AnimPointer->AnimController->SetTrackPosition(0, 0.0f);
		AnimPointer->MotionEnd = true;
		break;
	case SetFadein:
		SetTransition(Fadein);
		break;
	default:
		break;
	}

	return D3D_OK;
}


//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAnimation(D3DXANIMATION* Animation, int Set_No)
{
	char Message[64];

	if (FAILED(Animation->AnimController->GetAnimationSetByName(
		Animation->AnimData[Set_No].SetName, &Animation->AnimData[Set_No].AnimSet)))
	{
		sprintf(Message, "Can't find %s AnimationSet！", Animation->AnimData[Set_No].SetName);
		MessageBox(0, Message, "Error", 0);
		return E_FAIL;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAnimation(D3DXANIMATION* Animation)
{
	SafeRelease(SkinMeshEffect);

	D3DXFrameDestroy(Animation->FrameRoot, Animation->AllocateHier);
	SafeFree(Animation->AnimData);
	SafeRelease(Animation->AnimController);
	SafeDelete(Animation->AllocateHier);
	SafeFree(Animation);

	return;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAnimation(D3DXANIMATION* Animation, float Time)
{
	AnimCallBackHandler CallBackHandler;

	CallBackHandler.AnimPointer = Animation;

	// 合成中か否かを判定
	Animation->AnimData[Animation->CurrentAnimID].CurWeightTime += Time;

	if (Animation->AnimData[Animation->CurrentAnimID].CurWeightTime <= Animation->AnimData[Animation->CurrentAnimID].ShiftTime)
	{
		// 合成中。ウェイトを算出
		float Weight = Animation->AnimData[Animation->CurrentAnimID].CurWeightTime / Animation->AnimData[Animation->CurrentAnimID].ShiftTime;
		// ウェイトを登録
		Animation->AnimController->SetTrackWeight(0, Weight);       // 現在のアニメーション
		Animation->AnimController->SetTrackWeight(1, 1 - Weight);   // 前のアニメーション
	}
	else
	{
		// 合成終了中。通常アニメーションをするTrack0のウェイトを最大値に
		Animation->AnimController->SetTrackWeight(0, 1.0f);			// 現在のアニメーション
		Animation->AnimController->SetTrackEnable(1, false);		// 前のアニメーションを無効にする
		Animation->MotionBlendOver = true;
	}

	// 時間を更新
	Animation->AnimController->AdvanceTime(Time, &CallBackHandler);

	return;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAnimation(D3DXANIMATION* Animation, LPD3DXMATRIX WorldMatrix, bool DrawShadowMap)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	D3DXMATRIX CameraProjMatrix = GetProjMatrix();

	if (DrawShadowMap == false)
	{
		UpdateFrameMatrices(Animation->FrameRoot, WorldMatrix);
	}

	if (SkinningMethod == HLSL_Mode)
	{
		SkinMeshEffect->SetMatrix("mViewProj", &CameraProjMatrix);

		DrawFrame(Animation, Animation->FrameRoot, DrawShadowMap);
	}
	else
	{
		DrawFrame(Animation, Animation->FrameRoot, DrawShadowMap);
	}

	return;
}

//=============================================================================
// アニメーションを切り替え
//=============================================================================
void ChangeAnimation(D3DXANIMATION* Animation, UINT AnimID, float PlaySpeed, bool ResetMotion)
{
	// トラックの能力
	D3DXTRACK_DESC TD;

	// 指定のアニメーションIDの存在をチェック
	if (AnimID > (UINT)Animation->AnimSetNum)
	{
		MessageBox(0, "AnimationSet Don't Exist", "Error", 0);
		return;
	}

	// 異なるアニメーションであるかをチェック
	// 更新する必要がない
	if (Animation->CurrentAnimID == AnimID)
	{
		if (ResetMotion == true)
		{
			Animation->AnimController->ResetTime();
			Animation->AnimController->SetTrackPosition(0, 0.0f);
		}
		return;
	}

	Animation->MotionEnd = false;
	Animation->StartMove = false;
	Animation->MotionBlendOver = false;
	Animation->Cancelable = true;

	// 現在のアニメーションセットの設定値を取得
	Animation->AnimController->GetTrackDesc(0, &TD);

	// 今のアニメーションをトラック1に移行し
	// トラックの設定値も移行
	Animation->AnimController->SetTrackAnimationSet(1, Animation->AnimData[Animation->CurrentAnimID].AnimSet);
	Animation->AnimController->SetTrackDesc(1, &TD);
	Animation->AnimController->SetTrackSpeed(1, 0.0f);

	// 新しいアニメーションセットをトラック0に設定
	Animation->AnimController->SetTrackAnimationSet(0, Animation->AnimData[AnimID].AnimSet);

	// トラック0のスピードの設定
	Animation->AnimController->SetTrackSpeed(0, PlaySpeed);

	// トラック0の位置は最初からに設定
	Animation->AnimController->SetTrackPosition(0, 0.0f);

	// トラックの合成を許可
	Animation->AnimController->SetTrackEnable(0, true);
	Animation->AnimController->SetTrackEnable(1, true);

	// ウェイト時間を初期化
	Animation->AnimData[AnimID].CurWeightTime = 0.0f;
	Animation->AnimController->ResetTime();

	// 現在のアニメーション番号を切り替え
	Animation->PreviousAnimID = Animation->CurrentAnimID;
	Animation->CurrentAnimID = AnimID;

	return;
}

