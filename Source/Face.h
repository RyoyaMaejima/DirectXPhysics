#pragma once
#include "VertexData.h"

class Face {
public:
	//Face�̃V���O���g���C���X�^���X�𓾂�
	static Face& Instance();

	void Init(Vertex* vertices, unsigned int* indices);
	void Begin(XMMATRIX& worldMat, Vertex* vertices);
	void Update(float deltaTime, einput_state state, XMMATRIX& worldMat, Vertex* vertices);

	Face();
	~Face();

private:
	//�l�n���p
	XMFLOAT3 _vertices[fVertNum] = {};//���_���W
	unsigned int _indices[fIndicesNum] = {};//�C���f�b�N�X���W

	//�p�����[�^
	float w = 1.0f;//1�ӂ̒����̔���
	float v = 20.0f;//�ړ����x
	float rv = XM_PI / 0.6f;//��]���x
	float limTheta = XM_PIDIV4;//��]�͈�

	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };//���S���W
	float xTheta = 0.0f;//x���̉�]�p�x
	float zTheta = 0.0f;//z���̉�]�p�x

	//�ʍ쐬
	void CreateFace();

	//���W�̃R�s�[
	void CopyVertex(Vertex* vertices);
};