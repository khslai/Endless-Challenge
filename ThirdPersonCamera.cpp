//=============================================================================
//
// カメラ処理 [ThirdPersonCamera.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "ThirdPersonCamera.h"
#include "Input.h"
#include "DebugProcess.h"
#include "Player.h"
#include "Boss.h"
#include "Tutorial.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CameraDistance_Max	(300.0f)					// カメラと注視点の距離の範囲
#define CameraDistance_Min	(100.0f)
#define CameraAngle_Max		(D3DXToRadian(150.0f))		// カメラ上下の移動角度範囲					
#define CameraAngle_Min		(D3DXToRadian(30.0f))
#define DefaultAngle		(D3DXToRadian(70.0f))		// カメラの初期角度
#define DefaultDistance		(250.0f)					// カメラと注視点の初期距離
#define ResetSpeed			(0.3f)						// リセット速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// X軸を基準にして回転
void RotateX(float Radian);
// Y軸を基準にして回転
void RotateY(float Radian);
// カメラの位置と注視点の距離の変更
void Approach(float dist);
// カメラのリセット
void ResetCamera(void);
// 目標をロックオン
void LockOnTarget(D3DXVECTOR3 TargetPos);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DXMATRIX				ViewMatrix;				// ビューマトリックス
D3DXMATRIX				ProjMatrix;				// プロジェクションマトリックス
CAMERA_3RD				Camera_3rd;

//=============================================================================
// カメラの初期化
//=============================================================================
HRESULT InitCamera_3rd(bool FirstInit)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();
	int GameStage = GetGameStage();

	if (GameStage == Stage_Title)
	{
		Camera_3rd.Pos = D3DXVECTOR3(100.0f, 20.0f, -50.0f);
		Camera_3rd.At = D3DXVECTOR3(50.0f, 25.0f, 0.0f);
		Camera_3rd.Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Camera_3rd.Direction = PositionZero;
		Camera_3rd.Distance = DefaultDistance;
		Camera_3rd.Angle_Xaxis = DefaultAngle;
		Camera_3rd.Angle_Yaxis = 0.0f;
		Camera_3rd.Rot = 0.0f;
		Camera_3rd.InReset = false;
		Camera_3rd.InLockOn = false;
	}
	else
	{
		Camera_3rd.Pos = PositionZero;
		Camera_3rd.At = PositionZero;
		Camera_3rd.Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Camera_3rd.Direction = PositionZero;
		Camera_3rd.Distance = DefaultDistance;
		Camera_3rd.Angle_Xaxis = DefaultAngle;
		Camera_3rd.Angle_Yaxis = 0.0f;
		Camera_3rd.Rot = 0.0f;
		Camera_3rd.InReset = false;
		Camera_3rd.InLockOn = false;
	}

	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera_3rd(void)
{

	return;
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera_3rd(void)
{
	int GameStage = GetGameStage();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CUBE *Cube = GetCube();
	MOUSE *Mouse = GetMouse();
	GAMEPAD *GamePad = GetGamePad();
	static bool CameraRotStart = false;

	// カメラ回転
	if (Camera_3rd.InReset == false && Camera_3rd.InLockOn == false)
	{
		if (GamePad->Use == true)
		{
			RotateX(GamePad->RotateUp);
			RotateY(-GamePad->RotateRight);
		}
		else
		{
			RotateX(Mouse->RotateUp);
			RotateY(-Mouse->RotateRight);
		}
	}
	else if (Camera_3rd.InReset == true)
	{
		ResetCamera();
	}

	// カメラリセット
	if (GetKeyboardTrigger(DIK_R) || IsButtonTriggered(0, BUTTON_LB))
	{
		Camera_3rd.InReset = true;
	}

	// 目標をロックする
	if (GetKeyboardTrigger(DIK_T) || IsButtonTriggered(0, BUTTON_R3))
	{
		Camera_3rd.InLockOn = Camera_3rd.InLockOn ? false : true;
		Camera_3rd.Distance = 300.0f;
		Camera_3rd.Angle_Xaxis = D3DXToRadian(45.0f);
	}

	if (Camera_3rd.InLockOn == false)
	{
		// 注視点はプレイヤーの中心座標
		Camera_3rd.At = Player->CenterPos;
	}
	else
	{
		// ロック状態、注視点はボス
		if (GameStage == Stage_Game)
		{
			Camera_3rd.At = Boss->CenterPos;
			LockOnTarget(Boss->Pos);
		}
		else if (GameStage == Stage_Tutorial)
		{
			Camera_3rd.At = Cube->Pos;
			LockOnTarget(Cube->Pos);
		}
	}

	// カメラ拡大、縮小
	if (Camera_3rd.InLockOn == false)
	{
		Approach(Mouse->WheelRot);
	}

	// 球面座標系
	if (Camera_3rd.InLockOn == false)
	{
		Camera_3rd.Pos.x = Camera_3rd.At.x - Camera_3rd.Distance * sinf(Camera_3rd.Angle_Xaxis) * sinf(Camera_3rd.Angle_Yaxis);
		Camera_3rd.Pos.y = Camera_3rd.At.y + Camera_3rd.Distance * cosf(Camera_3rd.Angle_Xaxis);
		Camera_3rd.Pos.z = Camera_3rd.At.z - Camera_3rd.Distance * sinf(Camera_3rd.Angle_Xaxis) * cosf(Camera_3rd.Angle_Yaxis);
	}
	else
	{
		Camera_3rd.Pos.x = Player->CenterPos.x - Camera_3rd.Distance * sinf(Camera_3rd.Angle_Xaxis) * sinf(Camera_3rd.Angle_Yaxis);
		Camera_3rd.Pos.y = Player->CenterPos.y + Camera_3rd.Distance * cosf(Camera_3rd.Angle_Xaxis);
		Camera_3rd.Pos.z = Player->CenterPos.z - Camera_3rd.Distance * sinf(Camera_3rd.Angle_Xaxis) * cosf(Camera_3rd.Angle_Yaxis);
	}

	Camera_3rd.Rot = Camera_3rd.Angle_Yaxis;

	// 注視点と座標間のベクトル
	Camera_3rd.Direction = Camera_3rd.At - Camera_3rd.Pos;
	D3DXVec3Normalize(&Camera_3rd.Direction, &Camera_3rd.Direction);

	// 地面に当たる処理
	if (Camera_3rd.Pos.y < 5.0f)
	{
		Camera_3rd.Pos.y = 5.0f;
	}

#if _DEBUG
	PrintDebugMsg("CameraPos : x = %f , y = %f , z = %f\n", Camera_3rd.Pos.x, Camera_3rd.Pos.y, Camera_3rd.Pos.z);
	//PrintDebugMsg("Camera_3rd.Angle_Xaxis：%f\n", Camera_3rd.Angle_Xaxis);
	PrintDebugMsg("Camera_3rd.Angle_Yaxis：%f\n", Camera_3rd.Angle_Yaxis);
	//PrintDebugMsg("Camera_3rd.Rot：%f\n", Camera_3rd.Rot);
#endif

}

//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera_3rd(void)
{
	LPDIRECT3DDEVICE9 Device = GetDevice();

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&ViewMatrix);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&ViewMatrix,
		&Camera_3rd.Pos,		// カメラの視点
		&Camera_3rd.At,			// カメラの注視点
		&Camera_3rd.Up);		// カメラの上方向

	// ビューマトリックスの設定
	Device->SetTransform(D3DTS_VIEW, &ViewMatrix);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&ProjMatrix);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&ProjMatrix,
		VIEW_ANGLE,			// 視野角
		VIEW_ASPECT,		// アスペクト比
		VIEW_NEAR_Z,		// ビュー平面のNearZ値
		VIEW_FAR_Z);		// ビュー平面のFarZ値

	// プロジェクションマトリックスの設定
	Device->SetTransform(D3DTS_PROJECTION, &ProjMatrix);

	return;
}

//=============================================================================
// X軸を基準にして回転
//=============================================================================
void RotateX(float Radian)
{
	Camera_3rd.Angle_Xaxis -= Radian;

	if (Camera_3rd.Angle_Xaxis < CameraAngle_Min)
	{
		Camera_3rd.Angle_Xaxis = CameraAngle_Min;
	}
	else if (Camera_3rd.Angle_Xaxis > CameraAngle_Max)
	{
		Camera_3rd.Angle_Xaxis = CameraAngle_Max;
	}

}

//=============================================================================
// Y軸を基準にして回転
//=============================================================================
void RotateY(float Radian)
{
	Camera_3rd.Angle_Yaxis -= Radian;

	// Angle_YaxisをD3DX_2PIで割るの余り
	Camera_3rd.Angle_Yaxis = fmodf(Camera_3rd.Angle_Yaxis, D3DX_2PI);

	if (Camera_3rd.Angle_Yaxis > D3DX_PI)
	{
		Camera_3rd.Angle_Yaxis -= D3DX_2PI;
	}
	else if (Camera_3rd.Angle_Yaxis < -D3DX_PI)
	{
		Camera_3rd.Angle_Yaxis += D3DX_2PI;
	}

}

//=============================================================================
// カメラの位置と注視点の距離の変更
//=============================================================================
void Approach(float dist)
{
	Camera_3rd.Distance += dist;

	// 距離がCameRadianistance_Min～CameRadianistance_Maxの間に限る
	if (Camera_3rd.Distance < CameraDistance_Min)
	{
		Camera_3rd.Distance = CameraDistance_Min;
	}
	else if (Camera_3rd.Distance > CameraDistance_Max)
	{
		Camera_3rd.Distance = CameraDistance_Max;
	}
}

//=============================================================================
// カメラのリセット
//=============================================================================
void ResetCamera(void)
{
	PLAYER *Player = GetPlayer();
	// カメラ角度と目標角度の差分
	float AngleDifference = 0.0f;
	static float DestAngle = 0.0f;
	// 差分を記録したフラグ
	static bool DestAngleSaved = false;

	// Angle_Yaxisのリセット
	if (DestAngleSaved == false)
	{
		DestAngle = Player->DestAngle;
		DestAngleSaved = true;
	}

	// カメラ角度と目標角度の差分
	AngleDifference = Camera_3rd.Angle_Yaxis - DestAngle;

	if (AngleDifference > D3DX_PI)
	{
		AngleDifference -= D3DX_2PI;
	}
	else if (AngleDifference < -D3DX_PI)
	{
		AngleDifference += D3DX_2PI;
	}

	// 目標角度までどんどん移動する
	if (AngleDifference > 0)
	{
		Camera_3rd.Angle_Yaxis -= ResetSpeed;
	}
	else if (AngleDifference < 0)
	{
		Camera_3rd.Angle_Yaxis += ResetSpeed;
	}

	// カメラリセット完了
	if (fabs(AngleDifference) < ResetSpeed)
	{
		Camera_3rd.Angle_Yaxis = DestAngle;
		DestAngle = 0.0f;
		DestAngleSaved = false;
		Camera_3rd.InReset = false;
	}

	// 目的の角度までの差分
	float DiffRotY = Player->DestAngle - Player->Rot.y;
	if (DiffRotY > D3DX_PI)
	{
		DiffRotY -= D3DX_2PI;
	}
	if (DiffRotY < -D3DX_PI)
	{
		DiffRotY += D3DX_2PI;
	}

	// 目的の角度まで慣性をかける
	Player->Rot.y += DiffRotY * ResetSpeed;
	if (Player->Rot.y > D3DX_PI)
	{
		Player->Rot.y -= D3DX_2PI;
	}
	if (Player->Rot.y < -D3DX_PI)
	{
		Player->Rot.y += D3DX_2PI;
	}

	return;
}

//=============================================================================
// 目標をロックオン
//=============================================================================
void LockOnTarget(D3DXVECTOR3 TargetPos)
{
	PLAYER *Player = GetPlayer();
	float CameraDotVector = 0.0f;
	float DotVector = 0.0f;
	// カメラの回転角度
	float AngleOfVector = 0.0f;
	D3DXVECTOR2 PlayerTargetVec = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR2 TargetCameraVec = D3DXVECTOR2(0.0f, 0.0f);
	D3DXVECTOR2 VerticalVec = D3DXVECTOR2(0.0f, 0.0f);

	// プレイヤーとターゲットのベクトル
	PlayerTargetVec = D3DXVECTOR2(Player->Pos.x - TargetPos.x, Player->Pos.z - TargetPos.z);
	// ターゲットとカメラのベクトル
	TargetCameraVec = D3DXVECTOR2(Camera_3rd.Pos.x - TargetPos.x, Camera_3rd.Pos.z - TargetPos.z);
	// プレイヤーとターゲットのベクトルは-90度回転させる
	VerticalVec.x = PlayerTargetVec.x * cosf(D3DXToRadian(-90.0f)) - PlayerTargetVec.y * sinf(D3DXToRadian(-90.0f));
	VerticalVec.y = PlayerTargetVec.x * sinf(D3DXToRadian(-90.0f)) + PlayerTargetVec.y * cosf(D3DXToRadian(-90.0f));

	// 正規化
	D3DXVec2Normalize(&PlayerTargetVec, &PlayerTargetVec);
	D3DXVec2Normalize(&TargetCameraVec, &TargetCameraVec);
	D3DXVec2Normalize(&VerticalVec, &VerticalVec);
	// カメラは時計回りか反時計回りか計算する
	CameraDotVector = D3DXVec2Dot(&VerticalVec, &TargetCameraVec);
	// カメラの回転角度を計算する
	DotVector = D3DXVec2Dot(&PlayerTargetVec, &TargetCameraVec);
	AngleOfVector = acosf(DotVector);

	if (AngleOfVector >= 0.001f && AngleOfVector < D3DX_PI)
	{
		// カメラの場所はプレイたーとターゲット直線の左側
		// 反時計回り回転
		if (CameraDotVector > 0)
		{
			Camera_3rd.Angle_Yaxis -= AngleOfVector * 0.2f;
		}
		// カメラの場所はプレイたーとターゲット直線の右側
		// 時計回り回転
		else
		{
			Camera_3rd.Angle_Yaxis += AngleOfVector * 0.2f;
		}

		// Angle_Yaxis角度の範囲は-3.14～3.14の間
		if (Camera_3rd.Angle_Yaxis > D3DX_PI)
		{
			Camera_3rd.Angle_Yaxis -= D3DX_2PI;
		}
		else if (Camera_3rd.Angle_Yaxis < -D3DX_PI)
		{
			Camera_3rd.Angle_Yaxis += D3DX_2PI;
		}
	}

	return;
}


//=============================================================================
// カメラを取得する
//=============================================================================
CAMERA_3RD *GetCamera_3rd(void)
{
	return &Camera_3rd;
}

//=============================================================================
// カメラのビュー行列を取得する
//=============================================================================
D3DXMATRIX GetViewMatrix(void)
{
	return ViewMatrix;
}

//=============================================================================
// カメラの射影変換行列を取得する
//=============================================================================
D3DXMATRIX GetProjMatrix(void)
{
	return ProjMatrix;
}
