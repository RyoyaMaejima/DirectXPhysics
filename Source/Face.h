#pragma once
#include "VertexData.h"

class Face {
public:
	//�l�n���p
	Vertex _vertData[fVertNum] = {};//���_�f�[�^
	unsigned int _indices[fIndicesNum] = {};//�C���f�b�N�X���W
	XMMATRIX _worldMat = XMMatrixIdentity();//���W�ϊ��s��

	//Face�̃V���O���g���C���X�^���X�𓾂�
	static Face& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, einput_state state, XMFLOAT3 rotCenter);

	Face();
	~Face();

private:
	//�v�Z�p
	XMFLOAT3 _vertices[fVertNum] = {};//���_���W
	XMMATRIX _moveMat = XMMatrixIdentity();//���W�ړ��L�^�p

	//�p�����[�^
	float _w = 3.1f;//���̒����̔���
	float _h = 1.1f;//�c�̒����̔���
	float _v = 30.0f;//�ړ����x
	float _rv = XM_PI / 0.6f;//��]���x
	float _limTheta = XM_PIDIV4;//��]�͈�
	XMFLOAT3 _defLeftPos = { 1.0f, -5.5f, 8.0f };//�����ʒu�i�����j
	XMFLOAT3 _defRightPos = { -1.0f, -5.5f, 8.0f };//�����ʒu�i�E���j
	XMFLOAT3 _defLeftRot = { 0.0f, 0.0f, XM_PIDIV4 };//������]�i�����j
	XMFLOAT3 _defRightRot = { 0.0f, 0.0f, -XM_PIDIV4 };//������]�i�E���j

	XMFLOAT3 _mv = { 0.0f, 0.0f, 0.0f };//�ړ���
	XMFLOAT3 _theta = { 0.0f, 0.f, 0.0f };//���݂̉�]�p�x
	bool _isLeftLeg = true;//�������E����
	bool _isPut = false;//�{�^���������Ă��邩

	//�ʍ쐬
	void CreateFace();

	//���̃Z�b�g
	void SetLeg();

	//���_�f�[�^�̎󂯓n��
	void PassVertexValue();
};