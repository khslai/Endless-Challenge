//=============================================================================
//
// �������f���w�b�_�[ [Equipment.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M��
//
//=============================================================================
#ifndef _EQUIPMENT_H_
#define _EQUIPMENT_H_

#include "CapsuleMesh.h"

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3			Pos;				// ���f���̈ʒu
	D3DXVECTOR3			Rot;				// ���f���̌���(��])
	D3DXVECTOR3			Scale;				// ���f���̑傫��(�X�P�[��)
	LPDIRECT3DTEXTURE9	*Texture;			// �e�N�X�`���ւ̃|�C���^
	LPD3DXMESH			Mesh;				// ���b�V�����ւ̃|�C���^
	LPD3DXBUFFER		MaterialBuffer;		// �}�e���A�����ւ̃|�C���^
	DWORD				MaterialNum;		// �}�e���A�����̐�
	D3DXMATRIX			WorldMatrix;		// ���[���h�}�g���b�N�X
	CAPSULE				HitCapsule;			// �����蔻��p�J�v�Z��
	int					EffectID;			// �G�t�F�N�g�ԍ�
}SWORD;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT InitEquipment(bool FirstInit);
// �I������
void UninitEquipment(void);
// �X�V����
void UpdateEquipment(void);
// �`�揈��
void DrawEquipment(void);
// ���̃|�C���^���擾����
SWORD *GetSword(const char* Owner);

#endif
