//=============================================================================
//
// ���C���w�b�_�[ [main.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#ifndef _MAIN_H_
#define _MAIN_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#define D3D_DEBUG_INFO

#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>

// Effekseer
#include <Effekseer.h>
#include <EffekseerRendererDX9.h>
#include <EffekseerSoundXAudio2.h>

// DirectX 9.0
#define DIRECTINPUT_VERSION (0x0800)	// �x���΍�
#include <dinput.h>
#include <dsound.h>
#include <d3dx9.h>

//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#if 1	// [������"0"�ɂ����ꍇ�A"�\���v���p�e�B" -> "�����J" -> "����" -> "�ǉ��̈ˑ��t�@�C��"�ɑΏۃ��C�u������ݒ肷��]
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dsound.lib")

#if _DEBUG
#pragma comment(lib, "Debug/Effekseer.lib" )
#pragma comment(lib, "Debug/EffekseerRendererDX9.lib" )
#pragma comment(lib, "Debug/EffekseerSoundXAudio2.lib" )
#else
#pragma comment(lib, "Release/Effekseer.lib" )
#pragma comment(lib, "Release/EffekseerRendererDX9.lib" )
#pragma comment(lib, "Release/EffekseerSoundXAudio2.lib" )
#endif

#endif

//*****************************************************************************
// �}�N����`
//*****************************************************************************
// �Q�c�|���S�����_�t�H�[�}�b�g( ���_���W[2D] / ���ˌ� / �e�N�X�`�����W )
#define	FVF_VERTEX_2D	(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// �R�c�|���S�����_�t�H�[�}�b�g( ���_���W[3D] / �@�� / ���ˌ� / �e�N�X�`�����W )
#define	FVF_VERTEX_3D	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)
// �R�c�������_�t�H�[�}�b�g( ���_���W[3D] / ���ˌ� )
#define	FVF_3DLine		(D3DFVF_XYZ | D3DFVF_DIFFUSE)

#define Screen_Width		(1920)					// �E�C���h�E�̕�
#define Screen_Height		(1080)					// �E�C���h�E�̍���
#define Screen_Center_X		(Screen_Width / 2)		// �E�C���h�E�̒��SX���W
#define Screen_Center_Y		(Screen_Height / 2)		// �E�C���h�E�̒��SY���W
#define Stage_Size			(1025.0f)				// �X�e�[�W�̃T�C�Y
#define	MoveInertia			(0.20f)					// �ړ������W��
#define	RotateInertia		(0.20f)					// ��]�����W��
#define TimePerFrame		(0.0167f)				// 1 / 60�b
#define D3DX_HalfPI			(1.5708f)				// D3DX_PI�̔���
#define D3DX_2PI			(6.28318f)				// D3DX_PI�̓�{
#define	VERTEX_NUM			(4)						// ��̕��ʂ�`�悷�鎞�K�v�Ȓ��_��
#define	POLYGON_NUM			(2)						// ��̕��ʂ�`�悷�鎞�K�v�ȃ|���S����
#define DXtoEffekVec(Vec)	::Effekseer::Vector3D(Vec.x, Vec.y, Vec.z)

// �J�����֘A
#define	VIEW_ANGLE			(D3DXToRadian(60.0f))		// ����p�x
// �r���[���ʂ̃A�X�y�N�g��
#define	VIEW_ASPECT			((float)Screen_Width / (float)Screen_Height)	
#define	VIEW_NEAR_Z			(10.0f)						// �r���[���ʂ�NearZ�l�A������ŒZ����
#define	VIEW_FAR_Z			(5000.0f)					// �r���[���ʂ�FarZ�l�A������Œ�����

// �I�������[�v
#define RepeatCount (60)
#define RepeatSpeed (5)

// �������W
#define PositionZero		(D3DXVECTOR3(0.0f, 0.0f, 0.0f))
// �����̑傫��
#define DefaultScale		(D3DXVECTOR3(1.0f, 1.0f, 1.0f))
// ��̃x�N�g��
#define UpVector			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
// �E�̃x�N�g��
#define RightVector			(D3DXVECTOR3(1.0f, 0.0f, 0.0f))

// ���j���[�e�N�X�`��
#define Texture_BlackScreen			_T("data/Texture/BlackScreen.png")
#define Texture_BlackScreen_Width	(1920)
#define Texture_BlackScreen_Height	(1080)
#define Texture_SelectBox			_T("data/Texture/SelectBox.png")
#define Texture_SelectBox_Width		(500)
#define Texture_SelectBox_Height	(150)

// �����������[�X
// new
#define SafeDelete(Object)		{ if (Object) { delete(Object);			(Object)=NULL; } }
#define SafeDeleteArray(Object)	{ if (Object) { delete[](Object);		(Object)=NULL; } }
// malloc,calloc
#define SafeFree(Object)		{ if (Object) { free(Object);			(Object)=NULL; } }
// DirectX
#define SafeRelease(Object)		{ if (Object) { (Object)->Release();	(Object)=NULL; } }

// �F��RGBA
#define WHITE(Alpha)			D3DCOLOR_RGBA(255, 255, 255, Alpha)
#define BLACK(Alpha)			D3DCOLOR_RGBA(0, 0, 0, Alpha)
#define GREEN(Alpha)			D3DCOLOR_RGBA(0, 255, 0, Alpha)
#define BLUE(Alpha)				D3DCOLOR_RGBA(0, 0, 255, Alpha)
#define YELLOW(Alpha)			D3DCOLOR_RGBA(255, 255, 0, Alpha)
#define RED(Alpha)				D3DCOLOR_RGBA(255, 0, 0, Alpha)
#define FIRECOLOR(Alpha)		D3DCOLOR_RGBA(253, 165, 15, Alpha)
#define BLUEFIRE(Alpha)			D3DCOLOR_RGBA(10, 10, 255, Alpha)
#define SKYBLUE(Alpha)			D3DCOLOR_RGBA(135, 206, 235, Alpha)
#define ORANGE(Alpha)			D3DCOLOR_RGBA(255, 165, 0, Alpha)
#define PURPLE(Alpha)			D3DCOLOR_RGBA(128, 0, 128, Alpha)
#define DARKRED(Alpha)			D3DCOLOR_RGBA(139, 0, 0, Alpha)

// �Q�[���X�e�[�W
enum StagePhase
{
	Stage_Title,
	Stage_Tutorial,
	Stage_Game,
	Stage_Pause,
	Stage_GameOver,
};

// �Q�[����Փx
enum GameDifficulty
{
	Easy,
	Normal,
	AppealDifficulty,
};

// ��ʑJ�ڂ̏��
enum TransitionState
{
	TransitionOver = 0,	// �����Ȃ����
	Fadein,				// �t�F�[�h�C������
	Fadeout,			// �t�F�[�h�A�E�g����
};

// UI�e�N�X�`���̎��
enum UITextureType
{
	BlackScreen,
	SelectBox,
	UITexture_Max,
};

// UI�t�H���g�̎��
enum FontType
{
	FontSize_108,
	FontSize_96,
	FontSize_72,
	FontSize_54,
	FontSize_40,
	Font_Max,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// ��L�Q�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂��`
typedef struct
{
	D3DXVECTOR3 vtx;		// ���_���W
	float rhw;				// �e�N�X�`���̃p�[�X�y�N�e�B�u�R���N�g�p
	D3DCOLOR diffuse;		// ���ˌ�
	D3DXVECTOR2 tex;		// �e�N�X�`�����W
} VERTEX_2D;

// ��L�R�c�|���S�����_�t�H�[�}�b�g�ɍ��킹���\���̂��`
typedef struct
{
	D3DXVECTOR3 vtx;		// ���_���W
	D3DXVECTOR3 nor;		// �@���x�N�g��
	D3DCOLOR diffuse;		// ���ˌ�
	D3DXVECTOR2 tex;		// �e�N�X�`�����W
} VERTEX_3D;

// 3D��ԂŒ����`��p�\���̂��`
typedef struct
{
	D3DXVECTOR3 Point;
	D3DCOLOR Color;
} VERTEX_3DLINE;

// �I�����\����
typedef struct
{
	VERTEX_2D		VertexWk[VERTEX_NUM];	// ���_���i�[���[�N
	D3DXVECTOR2		PrePos;					// �O�t���C���̍��W
	D3DXVECTOR2		Pos;					// ���W
	int				Phase;					// ���w���I����
	bool			InYes;					// ������"YES"��"No"��
}SELECT;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �f�o�C�X���擾����
LPDIRECT3DDEVICE9 GetDevice(void);
// UI�p�e�N�X�`�����擾����
LPDIRECT3DTEXTURE9 GetUITexture(int UITextureType);
// �Q�[���J�E���g���擾����
int GetGameCount(void);
// �J�v�Z���̕`��ݒ���擾����
bool GetDrawWireFrame(void);
// �J�v�Z���̕\���ݒ���擾����
bool GetDisplayCapsule(void);
// �Q�[���X�e�[�W���擾����
int GetGameStage(void);
// �Q�[���X�e�[�W�ݒ�֐��i��ʑJ�ځj
void SetGameStage(int Stage);
// ��ʃe�N�X�`���ǂݍ��ފ֐�
HRESULT SafeLoadTexture(LPCSTR SrcFile, LPDIRECT3DTEXTURE9* TexturePtr, const char* ErrorSrc);
// ���f���̃e�N�X�`����ǂݍ���
HRESULT SafeLoadModelTexture(LPDIRECT3DTEXTURE9	*Texture, LPD3DXBUFFER MaterialBufferconst, DWORD MaterialNum, const char* ErrorSrc);
// ���\���p�t�H���g���擾����
LPD3DXFONT GetFont(int FontType);
// �ď�����
void ReInitialize(void);
// 3D��ԂŒ����`��
void DrawLine(D3DXVECTOR3 P1, D3DXVECTOR3 P2, D3DCOLOR Color);

#endif