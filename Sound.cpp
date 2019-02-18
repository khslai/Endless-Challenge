//=============================================================================
//
// サウンド処理 [Sound.cpp]
// Author：HAL東京　ゲーム学科1年生　頼凱興
//
//=============================================================================
#include "main.h"
#include "Sound.h"
#include "Player.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
// サウンドフェイドアウトの速度
#define VolumeDecreaseSpeed (15)

// サウンドファイルのパス（hの通しナンバーと合わせること）
const TCHAR* SoundFileName[] =
{
	_T("data/Music/BGM/Battle_Phase1_Intro.wav"),
	_T("data/Music/BGM/Battle_Phase1_Loop.wav"),
	_T("data/Music/BGM/Battle_Phase2_Intro.wav"),
	_T("data/Music/BGM/Battle_Phase2_Loop.wav"),
	_T("data/Music/BGM/Title_001.wav"),
	_T("data/Music/BGM/GameOver.wav"),
	_T("data/Music/BGM/Tutorial_Intro.wav"),
	_T("data/Music/BGM/Tutorial_Loop.wav"),
	_T("data/Music/SE/Menu.wav"),
	_T("data/Music/SE/Determine_Yes.wav"),
	_T("data/Music/SE/Determine_No.wav"),
	_T("data/Music/SE/SelectMove.wav"),
	_T("data/Music/SE/ShowTutorial.wav"),
};

const TCHAR* Sound3DFileName[] =
{
	_T("data/Music/SE/PlayerFootStep.wav"),
	_T("data/Music/SE/Rolling.wav"),
	_T("data/Music/SE/Slash.wav"),
	_T("data/Music/SE/Slash2.wav"),
	_T("data/Music/SE/HPRestore.wav"),
	_T("data/Music/SE/BossFootStep.wav"),
	_T("data/Music/SE/BossSlash.wav"),
	_T("data/Music/SE/SlashFire.wav"),
	_T("data/Music/SE/ExplodeFire.wav"),
	_T("data/Music/SE/BossDeathWind.wav"),
	_T("data/Music/SE/PowerUp.wav"),
	_T("data/Music/SE/ExplosionWind.wav"),
	_T("data/Music/SE/SetFireBall.wav"),
	_T("data/Music/SE/Burnning.wav"),
	_T("data/Music/SE/FireBallExplode.wav"),
	_T("data/Music/SE/HitCube.wav"),
	_T("data/Music/SE/DestroyCube.wav"),
	_T("data/Music/SE/HitHuman.wav"),
	_T("data/Music/SE/HitFire.wav"),
	_T("data/Music/SE/FallingDown.wav"),
	_T("data/Music/SE/FallingDown2.wav"),
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// Listenerオブジェクトを作る
HRESULT CreateListener(void);

// サウンドのロード
LPDIRECTSOUNDBUFFER8 LoadSound(int Sound_No);
LPDIRECTSOUNDBUFFER8 LoadSound_3D(int Sound_No);

// BGMを流しているかを調査
bool BGMPlaying(int Sound_No);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
IDirectSound8			*DirectSound = NULL;				// サウンドインターフェース
LPDIRECTSOUNDBUFFER8	SoundBuffer[Sound_Max] = { NULL };	// サウンド用バッファ
LPDIRECTSOUNDBUFFER8	Sound3DBuffer[Sound3D_Max] = { NULL };	// 3Dサウンド用バッファ
LPDIRECTSOUND3DBUFFER8	Buffer_3D[Sound3D_Max] = { NULL };	// 3Dバッファ
LPDIRECTSOUND3DLISTENER Listener = NULL;					// 3D Listener
DS3DLISTENER			ListenerParams;						// Listener Properties
int						PlayingBGM_No = 0;					// 流しているBGMの番号
bool					SetIntroOver = false;				// 前奏を設置したフラグ

//=============================================================================
// 初期化処理
//=============================================================================
// hWnd:ウィンドウハンドル
HRESULT	InitSound(HWND hWnd)
{
	int Sound_No = 0;
	int Sound3D_No = 0;
	char Message[64];

	// DirectSoundオブジェクトの作成
	if (FAILED(DirectSoundCreate8(NULL, &DirectSound, NULL)))
	{
		return E_FAIL;
	}

	// 協調レベル設定 
	if (FAILED(DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		return E_FAIL;
	}

	// 3D Listenerの初期化
	if (FAILED(CreateListener()))
	{
		MessageBox(0, "Create Listener Failed！", "Error", 0);
		return E_FAIL;
	}

	// サウンドのロード
	for (Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		SoundBuffer[Sound_No] = LoadSound(Sound_No);
		if (SoundBuffer[Sound_No] == NULL)
		{
			sprintf(Message, "Load Sound No.%02d Failed！", Sound_No);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	// 3Dサウンドのロード
	for (Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		Sound3DBuffer[Sound3D_No] = LoadSound_3D(Sound3D_No);
		if (Sound3DBuffer[Sound3D_No] == NULL)
		{
			sprintf(Message, "Load 3D Sound No.%02d Failed！", Sound3D_No);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	// 開始位置に戻る
	for (Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		SoundBuffer[Sound_No]->SetCurrentPosition(0);
	}
	for (Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
#if _DEBUG
		Sound3DBuffer[Sound3D_No]->Play(0, 0, E_DS8_FLAG_NONE);
		StopSound(SE3D, Sound3D_No);
#endif
		Sound3DBuffer[Sound3D_No]->SetCurrentPosition(0);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitSound()
{
	// Listenerオブジェクトの開放
	SafeRelease(Listener);

	// サウンドバッファの開放
	for (int Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		SafeRelease(SoundBuffer[Sound_No]);
	}
	for (int Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		SafeRelease(Sound3DBuffer[Sound3D_No]);
		SafeRelease(Buffer_3D[Sound3D_No]);
	}

	// サウンドの開放
	SafeRelease(DirectSound);

	return;
}


//=============================================================================
// サウンドを更新する
//=============================================================================
void UpdateSound(void)
{
	int GameStage = GetGameStage();
	PLAYER *Player = GetPlayer();
	BOSS *Boss = GetBoss();
	CAMERA_3RD *Camera = GetCamera_3rd();

	Listener->SetPosition(Player->CenterPos.x, Player->CenterPos.y, Player->CenterPos.z, DS3D_DEFERRED);
	Listener->SetOrientation(Camera->Direction.x, Camera->Direction.y, Camera->Direction.z,
		0.0f, 1.0f, 0.0f, DS3D_DEFERRED);

	switch (GameStage)
	{
	case Stage_Title:

		// タイトルBGM
		SetSound(BGM, BGM_Title, E_DS8_FLAG_LOOP, false);
		break;

	case Stage_Tutorial:

		if (SetIntroOver == false)
		{
			SetSound(BGM, BGM_Tutorial_Intro, E_DS8_FLAG_NONE, false);
			SetIntroOver = true;
		}
		else if (SetIntroOver == true && BGMPlaying(BGM_Tutorial_Intro) == false)
		{
			SetSound(BGM, BGM_Tutorial_Loop, E_DS8_FLAG_LOOP, false);
		}
		break;

	case Stage_Game:

		// 第一段階
		if (Boss->Phase == Phase1)
		{
			if (SetIntroOver == false)
			{
				SetSound(BGM, BGM_Battle_Phase1_Intro, E_DS8_FLAG_NONE, false);
				SetIntroOver = true;
			}
			else if (SetIntroOver == true && BGMPlaying(BGM_Battle_Phase1_Intro) == false)
			{
				SetSound(BGM, BGM_Battle_Phase1_Loop, E_DS8_FLAG_LOOP, false);
			}
		}
		// 変身中
		else if (Boss->Phase == TurnToPhase2)
		{
			SoundFadeOut(PlayingBGM_No);
			SetIntroOver = false;
		}
		// 第二段階
		else if (Boss->Phase == Phase2)
		{
			if (SetIntroOver == false)
			{
				StopSound(BGM, PlayingBGM_No);
				SetSound(BGM, BGM_Battle_Phase2_Intro, E_DS8_FLAG_NONE, false);
				SetIntroOver = true;
			}
			else if (SetIntroOver == true && BGMPlaying(BGM_Battle_Phase2_Intro) == false)
			{
				SetSound(BGM, BGM_Battle_Phase2_Loop, E_DS8_FLAG_LOOP, false);
			}
		}
		break;

	case Stage_GameOver:

		SetSound(BGM, BGM_GameOver, E_DS8_FLAG_LOOP, false);
		break;

	default:
		break;
	}

	return;
}

//=============================================================================
// Listenerオブジェクトを作る
//=============================================================================
HRESULT CreateListener(void)
{
	LPDIRECTSOUNDBUFFER pDSBPrimary = NULL;
	DSBUFFERDESC dsbdesc;

	ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	dsbdesc.dwBufferBytes = 0;
	dsbdesc.dwReserved = 0;
	dsbdesc.lpwfxFormat = NULL;
	dsbdesc.guid3DAlgorithm = GUID_NULL;
	if (FAILED(DirectSound->CreateSoundBuffer(&dsbdesc, &pDSBPrimary, NULL)))
	{
		return E_FAIL;
	}

	if (FAILED(pDSBPrimary->QueryInterface(IID_IDirectSound3DListener, (VOID**)&Listener)))
	{
		return E_FAIL;
	}

	ListenerParams.dwSize = sizeof(DS3DLISTENER);
	ListenerParams.vPosition = PositionZero;
	ListenerParams.vOrientTop = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	ListenerParams.vOrientFront = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	ListenerParams.flDopplerFactor = 1.0f;
	ListenerParams.flRolloffFactor = 1.0f;
	ListenerParams.flDistanceFactor = 1.0f;
	Listener->SetAllParameters(&ListenerParams, DS3D_IMMEDIATE);

	SafeRelease(pDSBPrimary);

	return S_OK;
}

//=============================================================================
// サウンドのロード
//=============================================================================
LPDIRECTSOUNDBUFFER8 LoadSound(int Sound_No)
{
	// Sound_No:サウンドナンバー（ヘッダに定義された列挙型定数）
	// MMIO = マルチメディア入出力、の略。
	LPDIRECTSOUNDBUFFER  pBaseBuffer = NULL;	// 曲データの総合バッファ
	LPDIRECTSOUNDBUFFER8 pBuffer = NULL;		// 曲データのバッファ
	DSBUFFERDESC buff;							// バッファ設定構造体

	HMMIO hMmio = NULL;							// MMIOハンドル
	MMIOINFO mmioInfo;							// マルチメディアデータ構造体

	MMRESULT mmRes;								// MM処理結果格納変数
	MMCKINFO riffChunk, formatChunk, dataChunk;	// チャンクデータ構造体
	DWORD size;									// データサイズ格納用
	WAVEFORMATEX pcm;							// 曲データフォーマット構造体

	LPVOID pBlock;								// セカンダリバッファのデータ書き込み先アドレス
	DWORD  dwSize;								// セカンダリバッファのサイズ

	// 1.ハンドルをもとにファイルを開く
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)SoundFileName[Sound_No], &mmioInfo, MMIO_READ);
	if (!hMmio)
	{
		return NULL;
	}

	// 2.ファイル解析① RIFFチャンク検索
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// 検索キーワード
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.ファイル解析② フォーマットチャンク検索
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// 検索キーワード
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// 検索情報をもとに読み込み
	// 正しく読み込めなかったら異常終了
	if (size != formatChunk.cksize)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// 4.ファイル解析 データチャンク検索
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// 検索キーワード
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.データ読み込み
	char *pData = new char[dataChunk.cksize];					// 必要な大きさの領域を確保して
	size = mmioRead(hMmio, (HPSTR)pData, dataChunk.cksize);		// データを読み込む
	// 正しく読み込めなかったら異常終了
	if (size != dataChunk.cksize)
	{
		SafeDeleteArray(pData);
		return NULL;
	}

	// 6.曲を読み込む「セカンダリバッファ」を用意
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// まず初期化
	buff.dwSize = sizeof(DSBUFFERDESC);				// そこから各種設定
	buff.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;

	// 総合バッファを作る
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &pBaseBuffer, NULL)))
	{
		return NULL;
	}

	// サウンドバッファを取り出す
	if (FAILED(pBaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pBuffer)))
	{
		return NULL;
	}

	// 取り出したら総合バッファを捨てる
	SafeRelease(pBaseBuffer);

	// 7.作られたセカンダリバッファにデータを転送
	// まずは書き込みできるようバッファをロック
	if (FAILED(pBuffer->Lock(0, 0, &pBlock, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		return NULL;
	}

	memcpy(pBlock, pData, dwSize);	// データ転送
	SafeDeleteArray(pData);			// 元の読み込み領域を消す

	// バッファロック解除
	pBuffer->Unlock(pBlock, dwSize, 0, 0);

	// セカンダリバッファを返してようやく完了...
	return pBuffer;
}

//=============================================================================
// サウンドのロード(3Dバッファ)
//=============================================================================
LPDIRECTSOUNDBUFFER8 LoadSound_3D(int Sound_No)
{
	// Sound_No:サウンドナンバー（ヘッダに定義された列挙型定数）
	// MMIO = マルチメディア入出力、の略。
	LPDIRECTSOUNDBUFFER  pBaseBuffer = NULL;	// 曲データの総合バッファ
	LPDIRECTSOUNDBUFFER8 pBuffer = NULL;		// 曲データのバッファ
	DSBUFFERDESC buff;							// バッファ設定構造体
	DS3DBUFFER BufferParams;					// 3D Buffer Properties

	HMMIO hMmio = NULL;							// MMIOハンドル
	MMIOINFO mmioInfo;							// マルチメディアデータ構造体

	MMRESULT mmRes;								// MM処理結果格納変数
	MMCKINFO riffChunk, formatChunk, dataChunk;	// チャンクデータ構造体
	DWORD size;									// データサイズ格納用
	WAVEFORMATEX pcm;							// 曲データフォーマット構造体

	LPVOID pBlock;								// セカンダリバッファのデータ書き込み先アドレス
	DWORD  dwSize;								// セカンダリバッファのサイズ

	// 1.ハンドルをもとにファイルを開く
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)Sound3DFileName[Sound_No], &mmioInfo, MMIO_READ);
	if (!hMmio)
	{
		return NULL;
	}

	// 2.ファイル解析① RIFFチャンク検索
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// 検索キーワード
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.ファイル解析② フォーマットチャンク検索
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// 検索キーワード
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// 検索情報をもとに読み込み
	// 正しく読み込めなかったら異常終了
	if (size != formatChunk.cksize)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// 4.ファイル解析③ データチャンク検索
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// 検索キーワード
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// キーワードをもとに検索
	// 見つからなかったら異常終了
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.データ読み込み
	char *pData = new char[dataChunk.cksize];					// 必要な大きさの領域を確保して
	size = mmioRead(hMmio, (HPSTR)pData, dataChunk.cksize);		// データを読み込む
	// 正しく読み込めなかったら異常終了
	if (size != dataChunk.cksize)
	{
		SafeDeleteArray(pData);
		return NULL;
	}

	// 6.曲を読み込む「セカンダリバッファ」を用意
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// まず初期化
	buff.dwSize = sizeof(DSBUFFERDESC);				// そこから各種設定
	buff.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;
	buff.guid3DAlgorithm = DS3DALG_DEFAULT;

	// 総合バッファを作る
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &pBaseBuffer, NULL)))
	{
		return NULL;
	}

	// サウンドバッファを取り出す
	if (FAILED(pBaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pBuffer)))
	{
		return NULL;
	}

	// 取り出したら総合バッファを捨てる
	SafeRelease(pBaseBuffer);

	// 7.作られたセカンダリバッファにデータを転送
	// まずは書き込みできるようバッファをロック
	if (FAILED(pBuffer->Lock(0, 0, &pBlock, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		return NULL;
	}

	memcpy(pBlock, pData, dwSize);	// データ転送
	SafeDeleteArray(pData);			// 元の読み込み領域を消す

	// バッファロック解除
	pBuffer->Unlock(pBlock, dwSize, 0, 0);

	// Get the 3D interface to the secondary sound buffer.
	if (FAILED(pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&Buffer_3D[Sound_No])))
	{
		return NULL;
	}

	BufferParams.dwSize = sizeof(DS3DBUFFER);
	Buffer_3D[Sound_No]->GetAllParameters(&BufferParams);

	// 3DBufferの属性を設置する
	BufferParams.flMinDistance = 200.0f;
	Buffer_3D[Sound_No]->SetAllParameters(&BufferParams, DS3D_IMMEDIATE);

	// セカンダリバッファを返してようやく完了...
	return pBuffer;
}

//=============================================================================
// BGM、2Dサウンドを設置する
//=============================================================================
void SetSound(int SoundType, int Sound_No, int Flag, bool FromStart)
{
	if (SoundType == BGM)
	{
		PlayingBGM_No = Sound_No;
	}

	if (FromStart == true)
	{
		// 続きから鳴らすので、最初から鳴らしたい場合はSetCurrentPosition(0)をすること
		SoundBuffer[Sound_No]->SetCurrentPosition(0);
	}
	// Flag   :1(E_DS8_FLAG_LOOP)ならループ再生
	SoundBuffer[Sound_No]->Play(0, 0, Flag);

	return;
}

//=============================================================================
// 3Dサウンドを設置する
//=============================================================================
void Set3DSound(D3DXVECTOR3 Pos, int Sound_No, int Flag, bool FromStart)
{
	PLAYER *Player = GetPlayer();
	CAMERA_3RD *Camera = GetCamera_3rd();

	// Listener情報更新
	Listener->CommitDeferredSettings();

	// サウンドの位置を設置する
	Buffer_3D[Sound_No]->SetPosition(Pos.x, Pos.y, Pos.z, DS3D_IMMEDIATE);

	if (FromStart == true)
	{
		// 続きから鳴らすので、最初から鳴らしたい場合はSetCurrentPosition(0)をすること
		Sound3DBuffer[Sound_No]->SetCurrentPosition(0);
	}
	// Flag   :1(E_DS8_FLAG_LOOP)ならループ再生
	Sound3DBuffer[Sound_No]->Play(0, 0, Flag);

	return;
}

//=============================================================================
// サウンドを停止する
//=============================================================================
void StopSound(int SoundType, int Sound_No)
{
	DWORD status;

	if (SoundType == BGM || SoundType == NormalSE)
	{
		SoundBuffer[Sound_No]->GetStatus(&status);
		// 鳴っていたら
		if (status & DSBSTATUS_PLAYING)
		{
			// 意味的にはPauseになる。
			SoundBuffer[Sound_No]->Stop();
		}
	}
	else if (SoundType == SE3D)
	{
		Sound3DBuffer[Sound_No]->GetStatus(&status);
		// 鳴っていたら
		if (status & DSBSTATUS_PLAYING)
		{
			// 意味的にはPauseになる。
			Sound3DBuffer[Sound_No]->Stop();
		}
	}

	return;
}

//=============================================================================
// 再生中かどうか調べる
//=============================================================================
bool BGMPlaying(int Sound_No)
{
	DWORD status;

	SoundBuffer[Sound_No]->GetStatus(&status);
	if (status & DSBSTATUS_PLAYING)
	{
		return true;
	}

	return false;
}

//=============================================================================
// 再初期化処理
//=============================================================================
void ReInitSound(void)
{
	// ボリューム初期化
	LONG Volume = DSBVOLUME_MAX;

	// フラグ初期化
	SetIntroOver = false;

	for (int Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		// 流してるサウンドを停止
		StopSound(BGM, Sound_No);

		// 本来のボリュームに戻す
		SoundBuffer[Sound_No]->SetVolume(Volume);

		// プレイ位置が先頭に戻る
		SoundBuffer[Sound_No]->SetCurrentPosition(0);
	}
	for (int Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		// 流してるサウンドを停止
		StopSound(SE3D, Sound3D_No);

		// 本来のボリュームに戻す
		Sound3DBuffer[Sound3D_No]->SetVolume(Volume);

		// プレイ位置が先頭に戻る
		Sound3DBuffer[Sound3D_No]->SetCurrentPosition(0);
	}

	return;
}

//=============================================================================
// サウンドフェイドアウト
//=============================================================================
void SoundFadeOut(int Sound_No)
{
	LONG Volume = 0;

	SoundBuffer[Sound_No]->GetVolume(&Volume);
	// ボリューム少しずつ減る
	Volume -= VolumeDecreaseSpeed;

	// ボリューム設定
	SoundBuffer[Sound_No]->SetVolume(Volume);

	return;
}

//=============================================================================
// 今鳴らしているBGMの番号を取得する
//=============================================================================
int GetPlayingBGM_No(void)
{
	return PlayingBGM_No;
}
