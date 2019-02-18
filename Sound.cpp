//=============================================================================
//
// �T�E���h���� [Sound.cpp]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#include "main.h"
#include "Sound.h"
#include "Player.h"
#include "Boss.h"
#include "ThirdPersonCamera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �T�E���h�t�F�C�h�A�E�g�̑��x
#define VolumeDecreaseSpeed (15)

// �T�E���h�t�@�C���̃p�X�ih�̒ʂ��i���o�[�ƍ��킹�邱�Ɓj
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
// �v���g�^�C�v�錾
//*****************************************************************************
// Listener�I�u�W�F�N�g�����
HRESULT CreateListener(void);

// �T�E���h�̃��[�h
LPDIRECTSOUNDBUFFER8 LoadSound(int Sound_No);
LPDIRECTSOUNDBUFFER8 LoadSound_3D(int Sound_No);

// BGM�𗬂��Ă��邩�𒲍�
bool BGMPlaying(int Sound_No);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
IDirectSound8			*DirectSound = NULL;				// �T�E���h�C���^�[�t�F�[�X
LPDIRECTSOUNDBUFFER8	SoundBuffer[Sound_Max] = { NULL };	// �T�E���h�p�o�b�t�@
LPDIRECTSOUNDBUFFER8	Sound3DBuffer[Sound3D_Max] = { NULL };	// 3D�T�E���h�p�o�b�t�@
LPDIRECTSOUND3DBUFFER8	Buffer_3D[Sound3D_Max] = { NULL };	// 3D�o�b�t�@
LPDIRECTSOUND3DLISTENER Listener = NULL;					// 3D Listener
DS3DLISTENER			ListenerParams;						// Listener Properties
int						PlayingBGM_No = 0;					// �����Ă���BGM�̔ԍ�
bool					SetIntroOver = false;				// �O�t��ݒu�����t���O

//=============================================================================
// ����������
//=============================================================================
// hWnd:�E�B���h�E�n���h��
HRESULT	InitSound(HWND hWnd)
{
	int Sound_No = 0;
	int Sound3D_No = 0;
	char Message[64];

	// DirectSound�I�u�W�F�N�g�̍쐬
	if (FAILED(DirectSoundCreate8(NULL, &DirectSound, NULL)))
	{
		return E_FAIL;
	}

	// �������x���ݒ� 
	if (FAILED(DirectSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)))
	{
		return E_FAIL;
	}

	// 3D Listener�̏�����
	if (FAILED(CreateListener()))
	{
		MessageBox(0, "Create Listener Failed�I", "Error", 0);
		return E_FAIL;
	}

	// �T�E���h�̃��[�h
	for (Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		SoundBuffer[Sound_No] = LoadSound(Sound_No);
		if (SoundBuffer[Sound_No] == NULL)
		{
			sprintf(Message, "Load Sound No.%02d Failed�I", Sound_No);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	// 3D�T�E���h�̃��[�h
	for (Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		Sound3DBuffer[Sound3D_No] = LoadSound_3D(Sound3D_No);
		if (Sound3DBuffer[Sound3D_No] == NULL)
		{
			sprintf(Message, "Load 3D Sound No.%02d Failed�I", Sound3D_No);
			MessageBox(0, Message, "Error", 0);
			return E_FAIL;
		}
	}

	// �J�n�ʒu�ɖ߂�
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
// �I������
//=============================================================================
void UninitSound()
{
	// Listener�I�u�W�F�N�g�̊J��
	SafeRelease(Listener);

	// �T�E���h�o�b�t�@�̊J��
	for (int Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		SafeRelease(SoundBuffer[Sound_No]);
	}
	for (int Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		SafeRelease(Sound3DBuffer[Sound3D_No]);
		SafeRelease(Buffer_3D[Sound3D_No]);
	}

	// �T�E���h�̊J��
	SafeRelease(DirectSound);

	return;
}


//=============================================================================
// �T�E���h���X�V����
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

		// �^�C�g��BGM
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

		// ���i�K
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
		// �ϐg��
		else if (Boss->Phase == TurnToPhase2)
		{
			SoundFadeOut(PlayingBGM_No);
			SetIntroOver = false;
		}
		// ���i�K
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
// Listener�I�u�W�F�N�g�����
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
// �T�E���h�̃��[�h
//=============================================================================
LPDIRECTSOUNDBUFFER8 LoadSound(int Sound_No)
{
	// Sound_No:�T�E���h�i���o�[�i�w�b�_�ɒ�`���ꂽ�񋓌^�萔�j
	// MMIO = �}���`���f�B�A���o�́A�̗��B
	LPDIRECTSOUNDBUFFER  pBaseBuffer = NULL;	// �ȃf�[�^�̑����o�b�t�@
	LPDIRECTSOUNDBUFFER8 pBuffer = NULL;		// �ȃf�[�^�̃o�b�t�@
	DSBUFFERDESC buff;							// �o�b�t�@�ݒ�\����

	HMMIO hMmio = NULL;							// MMIO�n���h��
	MMIOINFO mmioInfo;							// �}���`���f�B�A�f�[�^�\����

	MMRESULT mmRes;								// MM�������ʊi�[�ϐ�
	MMCKINFO riffChunk, formatChunk, dataChunk;	// �`�����N�f�[�^�\����
	DWORD size;									// �f�[�^�T�C�Y�i�[�p
	WAVEFORMATEX pcm;							// �ȃf�[�^�t�H�[�}�b�g�\����

	LPVOID pBlock;								// �Z�J���_���o�b�t�@�̃f�[�^�������ݐ�A�h���X
	DWORD  dwSize;								// �Z�J���_���o�b�t�@�̃T�C�Y

	// 1.�n���h�������ƂɃt�@�C�����J��
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)SoundFileName[Sound_No], &mmioInfo, MMIO_READ);
	if (!hMmio)
	{
		return NULL;
	}

	// 2.�t�@�C����͇@ RIFF�`�����N����
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.�t�@�C����͇A �t�H�[�}�b�g�`�����N����
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// �����������Ƃɓǂݍ���
	// �������ǂݍ��߂Ȃ�������ُ�I��
	if (size != formatChunk.cksize)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// 4.�t�@�C����� �f�[�^�`�����N����
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.�f�[�^�ǂݍ���
	char *pData = new char[dataChunk.cksize];					// �K�v�ȑ傫���̗̈���m�ۂ���
	size = mmioRead(hMmio, (HPSTR)pData, dataChunk.cksize);		// �f�[�^��ǂݍ���
	// �������ǂݍ��߂Ȃ�������ُ�I��
	if (size != dataChunk.cksize)
	{
		SafeDeleteArray(pData);
		return NULL;
	}

	// 6.�Ȃ�ǂݍ��ށu�Z�J���_���o�b�t�@�v��p��
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// �܂�������
	buff.dwSize = sizeof(DSBUFFERDESC);				// ��������e��ݒ�
	buff.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS | DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;

	// �����o�b�t�@�����
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &pBaseBuffer, NULL)))
	{
		return NULL;
	}

	// �T�E���h�o�b�t�@�����o��
	if (FAILED(pBaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pBuffer)))
	{
		return NULL;
	}

	// ���o�����瑍���o�b�t�@���̂Ă�
	SafeRelease(pBaseBuffer);

	// 7.���ꂽ�Z�J���_���o�b�t�@�Ƀf�[�^��]��
	// �܂��͏������݂ł���悤�o�b�t�@�����b�N
	if (FAILED(pBuffer->Lock(0, 0, &pBlock, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		return NULL;
	}

	memcpy(pBlock, pData, dwSize);	// �f�[�^�]��
	SafeDeleteArray(pData);			// ���̓ǂݍ��ݗ̈������

	// �o�b�t�@���b�N����
	pBuffer->Unlock(pBlock, dwSize, 0, 0);

	// �Z�J���_���o�b�t�@��Ԃ��Ă悤�₭����...
	return pBuffer;
}

//=============================================================================
// �T�E���h�̃��[�h(3D�o�b�t�@)
//=============================================================================
LPDIRECTSOUNDBUFFER8 LoadSound_3D(int Sound_No)
{
	// Sound_No:�T�E���h�i���o�[�i�w�b�_�ɒ�`���ꂽ�񋓌^�萔�j
	// MMIO = �}���`���f�B�A���o�́A�̗��B
	LPDIRECTSOUNDBUFFER  pBaseBuffer = NULL;	// �ȃf�[�^�̑����o�b�t�@
	LPDIRECTSOUNDBUFFER8 pBuffer = NULL;		// �ȃf�[�^�̃o�b�t�@
	DSBUFFERDESC buff;							// �o�b�t�@�ݒ�\����
	DS3DBUFFER BufferParams;					// 3D Buffer Properties

	HMMIO hMmio = NULL;							// MMIO�n���h��
	MMIOINFO mmioInfo;							// �}���`���f�B�A�f�[�^�\����

	MMRESULT mmRes;								// MM�������ʊi�[�ϐ�
	MMCKINFO riffChunk, formatChunk, dataChunk;	// �`�����N�f�[�^�\����
	DWORD size;									// �f�[�^�T�C�Y�i�[�p
	WAVEFORMATEX pcm;							// �ȃf�[�^�t�H�[�}�b�g�\����

	LPVOID pBlock;								// �Z�J���_���o�b�t�@�̃f�[�^�������ݐ�A�h���X
	DWORD  dwSize;								// �Z�J���_���o�b�t�@�̃T�C�Y

	// 1.�n���h�������ƂɃt�@�C�����J��
	memset(&mmioInfo, 0, sizeof(MMIOINFO));
	hMmio = mmioOpen((LPSTR)Sound3DFileName[Sound_No], &mmioInfo, MMIO_READ);
	if (!hMmio)
	{
		return NULL;
	}

	// 2.�t�@�C����͇@ RIFF�`�����N����
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');				// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &riffChunk, NULL, MMIO_FINDRIFF);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 3.�t�@�C����͇A �t�H�[�}�b�g�`�����N����
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');						// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	size = mmioRead(hMmio, (HPSTR)&pcm, formatChunk.cksize);	// �����������Ƃɓǂݍ���
	// �������ǂݍ��߂Ȃ�������ُ�I��
	if (size != formatChunk.cksize)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// 4.�t�@�C����͇B �f�[�^�`�����N����
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');					// �����L�[���[�h
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);	// �L�[���[�h�����ƂɌ���
	// ������Ȃ�������ُ�I��
	if (mmRes != MMSYSERR_NOERROR)
	{
		mmioClose(hMmio, 0);
		return NULL;
	}

	// 5.�f�[�^�ǂݍ���
	char *pData = new char[dataChunk.cksize];					// �K�v�ȑ傫���̗̈���m�ۂ���
	size = mmioRead(hMmio, (HPSTR)pData, dataChunk.cksize);		// �f�[�^��ǂݍ���
	// �������ǂݍ��߂Ȃ�������ُ�I��
	if (size != dataChunk.cksize)
	{
		SafeDeleteArray(pData);
		return NULL;
	}

	// 6.�Ȃ�ǂݍ��ށu�Z�J���_���o�b�t�@�v��p��
	ZeroMemory(&buff, sizeof(DSBUFFERDESC));		// �܂�������
	buff.dwSize = sizeof(DSBUFFERDESC);				// ��������e��ݒ�
	buff.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
	buff.dwBufferBytes = size;
	buff.lpwfxFormat = &pcm;
	buff.guid3DAlgorithm = DS3DALG_DEFAULT;

	// �����o�b�t�@�����
	if (FAILED(DirectSound->CreateSoundBuffer(&buff, &pBaseBuffer, NULL)))
	{
		return NULL;
	}

	// �T�E���h�o�b�t�@�����o��
	if (FAILED(pBaseBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pBuffer)))
	{
		return NULL;
	}

	// ���o�����瑍���o�b�t�@���̂Ă�
	SafeRelease(pBaseBuffer);

	// 7.���ꂽ�Z�J���_���o�b�t�@�Ƀf�[�^��]��
	// �܂��͏������݂ł���悤�o�b�t�@�����b�N
	if (FAILED(pBuffer->Lock(0, 0, &pBlock, &dwSize, NULL, NULL, DSBLOCK_ENTIREBUFFER)))
	{
		return NULL;
	}

	memcpy(pBlock, pData, dwSize);	// �f�[�^�]��
	SafeDeleteArray(pData);			// ���̓ǂݍ��ݗ̈������

	// �o�b�t�@���b�N����
	pBuffer->Unlock(pBlock, dwSize, 0, 0);

	// Get the 3D interface to the secondary sound buffer.
	if (FAILED(pBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&Buffer_3D[Sound_No])))
	{
		return NULL;
	}

	BufferParams.dwSize = sizeof(DS3DBUFFER);
	Buffer_3D[Sound_No]->GetAllParameters(&BufferParams);

	// 3DBuffer�̑�����ݒu����
	BufferParams.flMinDistance = 200.0f;
	Buffer_3D[Sound_No]->SetAllParameters(&BufferParams, DS3D_IMMEDIATE);

	// �Z�J���_���o�b�t�@��Ԃ��Ă悤�₭����...
	return pBuffer;
}

//=============================================================================
// BGM�A2D�T�E���h��ݒu����
//=============================================================================
void SetSound(int SoundType, int Sound_No, int Flag, bool FromStart)
{
	if (SoundType == BGM)
	{
		PlayingBGM_No = Sound_No;
	}

	if (FromStart == true)
	{
		// ��������炷�̂ŁA�ŏ�����炵�����ꍇ��SetCurrentPosition(0)�����邱��
		SoundBuffer[Sound_No]->SetCurrentPosition(0);
	}
	// Flag   :1(E_DS8_FLAG_LOOP)�Ȃ烋�[�v�Đ�
	SoundBuffer[Sound_No]->Play(0, 0, Flag);

	return;
}

//=============================================================================
// 3D�T�E���h��ݒu����
//=============================================================================
void Set3DSound(D3DXVECTOR3 Pos, int Sound_No, int Flag, bool FromStart)
{
	PLAYER *Player = GetPlayer();
	CAMERA_3RD *Camera = GetCamera_3rd();

	// Listener���X�V
	Listener->CommitDeferredSettings();

	// �T�E���h�̈ʒu��ݒu����
	Buffer_3D[Sound_No]->SetPosition(Pos.x, Pos.y, Pos.z, DS3D_IMMEDIATE);

	if (FromStart == true)
	{
		// ��������炷�̂ŁA�ŏ�����炵�����ꍇ��SetCurrentPosition(0)�����邱��
		Sound3DBuffer[Sound_No]->SetCurrentPosition(0);
	}
	// Flag   :1(E_DS8_FLAG_LOOP)�Ȃ烋�[�v�Đ�
	Sound3DBuffer[Sound_No]->Play(0, 0, Flag);

	return;
}

//=============================================================================
// �T�E���h���~����
//=============================================================================
void StopSound(int SoundType, int Sound_No)
{
	DWORD status;

	if (SoundType == BGM || SoundType == NormalSE)
	{
		SoundBuffer[Sound_No]->GetStatus(&status);
		// ���Ă�����
		if (status & DSBSTATUS_PLAYING)
		{
			// �Ӗ��I�ɂ�Pause�ɂȂ�B
			SoundBuffer[Sound_No]->Stop();
		}
	}
	else if (SoundType == SE3D)
	{
		Sound3DBuffer[Sound_No]->GetStatus(&status);
		// ���Ă�����
		if (status & DSBSTATUS_PLAYING)
		{
			// �Ӗ��I�ɂ�Pause�ɂȂ�B
			Sound3DBuffer[Sound_No]->Stop();
		}
	}

	return;
}

//=============================================================================
// �Đ������ǂ������ׂ�
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
// �ď���������
//=============================================================================
void ReInitSound(void)
{
	// �{�����[��������
	LONG Volume = DSBVOLUME_MAX;

	// �t���O������
	SetIntroOver = false;

	for (int Sound_No = 0; Sound_No < Sound_Max; Sound_No++)
	{
		// �����Ă�T�E���h���~
		StopSound(BGM, Sound_No);

		// �{���̃{�����[���ɖ߂�
		SoundBuffer[Sound_No]->SetVolume(Volume);

		// �v���C�ʒu���擪�ɖ߂�
		SoundBuffer[Sound_No]->SetCurrentPosition(0);
	}
	for (int Sound3D_No = 0; Sound3D_No < Sound3D_Max; Sound3D_No++)
	{
		// �����Ă�T�E���h���~
		StopSound(SE3D, Sound3D_No);

		// �{���̃{�����[���ɖ߂�
		Sound3DBuffer[Sound3D_No]->SetVolume(Volume);

		// �v���C�ʒu���擪�ɖ߂�
		Sound3DBuffer[Sound3D_No]->SetCurrentPosition(0);
	}

	return;
}

//=============================================================================
// �T�E���h�t�F�C�h�A�E�g
//=============================================================================
void SoundFadeOut(int Sound_No)
{
	LONG Volume = 0;

	SoundBuffer[Sound_No]->GetVolume(&Volume);
	// �{�����[������������
	Volume -= VolumeDecreaseSpeed;

	// �{�����[���ݒ�
	SoundBuffer[Sound_No]->SetVolume(Volume);

	return;
}

//=============================================================================
// ���炵�Ă���BGM�̔ԍ����擾����
//=============================================================================
int GetPlayingBGM_No(void)
{
	return PlayingBGM_No;
}
