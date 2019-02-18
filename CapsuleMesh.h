//=============================================================================
//
// �J�v�Z���w�b�_�[ [CapsuleMesh.h]
// Author�FHAL�����@�Q�[���w��1�N���@���M�� 
//
//=============================================================================
#ifndef _CAPSULEMESH_H_
#define _CAPSULEMESH_H_

typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 VtxBuffer;			// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9	IdxBuffer;			// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	D3DXMATRIX				WorldMatrix;		// ���[���h�}�g���b�N�X
	D3DXVECTOR3				P1;					// ���ƉE����̔����̂̒��S���W
	D3DXVECTOR3				P2;
	D3DXVECTOR3				PreP1;				// i�t���C���O�̍��ƉE����̔����̂̒��S���W
	D3DXVECTOR3				PreP2;
	D3DXVECTOR3				Pos;				// �J�v�Z���̍��W
	D3DXVECTOR3				Direction;			// �`�悷��Ƃ��̕����x�N�g��
	D3DXVECTOR3				DefaultDirVec;		// �J�v�Z�����쐬����Ƃ��ݒu����x�N�g��
												// �Ⴆ�΁ADirectionVec = (0.0f,1.0f,0.0f)�̂Ƃ�
												// �J�v�Z������]�����Ȃ��ꍇ�A�J�v�Z���͐^��Ɍ���
	D3DCOLOR				Color;				// �F
	int						NumOfVtxBuffer;		// �o�b�t�@�̒��_��
	int						NumOfTriangle;		// �O�p�`�|���S���̐�
	float					Length;				// �J�v�Z���̒���
	float					Radius;				// �J�v�Z���̔��a
	bool					FromCenter;			// �J�v�Z���`��̎n�_�͒��S���ǂ���
} CAPSULE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// ����������
HRESULT CreateCapsule(CAPSULE *Capsule, D3DXVECTOR3 Pos, D3DXVECTOR3 DirectionVec, float Length, float Radius, bool FromCenter);
// �I������
void UninitCapsule(CAPSULE *Capsule);
// �X�V����
void UpdateCapsule(CAPSULE *Capsule);
// �`�揈��
void DrawCapsule(CAPSULE *Capsule,const D3DXMATRIX *WorldMatrix);

#endif
