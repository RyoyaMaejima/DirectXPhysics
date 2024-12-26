#include "Face.h"
#include "XMFLOAT3Math.h"

#include <random>
#include <cmath>

Face& Face::Instance() {
	static Face instance;
	return instance;
}

void Face::Init(Vertex* vertices, unsigned int* indices) {
	CreateFace();

	CopyVertex(vertices);
	copy(begin(_indices), end(_indices), indices);
}

void Face::Begin(XMMATRIX& worldMat, Vertex* vertices) {
	//�e�ϐ��̏�����
	worldMat = XMMatrixIdentity();
	CreateFace();
	center = { 0.0f, 0.0f, 0.0f };
	xTheta = 0.0f;

	XMFLOAT3 pos = { 0.0f, -2.0f, 0.0f };
	float xPos = XM_PIDIV2;

	//-��/4�`��/4�܂ł̃����_���Ȓl��ݒ�
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dist(-XM_PIDIV4, XM_PIDIV4);
	float zPos = dist(gen);
	zTheta = zPos;

	//��]�̏����ݒ�
	worldMat *= XMMatrixRotationX(xPos);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateXVector(xPos, _vertices[i]);
	}
	worldMat *= XMMatrixRotationZ(zPos);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateZVector(zPos, _vertices[i]);
	}

	//���W�̏����ݒ�
	worldMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], pos);
	}

	//���W��n��
	CopyVertex(vertices);
	center = AddVector(center, pos);
}

void Face::Update(float deltaTime, einput_state state, XMMATRIX& worldMat, Vertex* vertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	float xMove = 0.0f;
	float zMove = 0.0f;

	//���͏���
	switch (state)
	{
		case move_up:
			move.z = v * deltaTime;
			break;
		case move_down:
			move.z = -v * deltaTime;
			break;
		case move_right:
			move.x = v * deltaTime;
			break;
		case move_left:
			move.x = -v * deltaTime;
			break;
		case rotateX_positive:
			if (xTheta < limTheta) {
				xMove = rv * deltaTime;
				xTheta += xMove;
			}
			break;
		case rotateX_negative:
			if (xTheta > -limTheta) {
				xMove = -rv * deltaTime;
				xTheta += xMove;
			}
			break;
		case rotateZ_positive:
			if (zTheta > -limTheta) {
				zMove = -rv * deltaTime;
				zTheta += zMove;
			}
			break;
		case rotateZ_negative:
			if (zTheta < limTheta) {
				zMove = rv * deltaTime;
				zTheta += zMove;
			}
			break;
		case invalid:
			break;
		default:
			break;
	}

	//���_�Ɉړ�
	worldMat *= XMMatrixTranslation(-center.x, -center.y, -center.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], ScalarVecror(-1.0f, center));
	}
	//��]
	worldMat *= XMMatrixRotationX(xMove);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateXVector(xMove, _vertices[i]);
	}
	worldMat *= XMMatrixRotationZ(zMove);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateZVector(zMove, _vertices[i]);
	}
	//�ړ���������߂�
	worldMat *= XMMatrixTranslation(center.x, center.y, center.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], center);
	}

	//�ړ�
	worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], move);
	}

	//���W��n��
	CopyVertex(vertices);
	center = AddVector(center, move);
}

Face::Face() {

}

Face::~Face() {

}

void Face::CreateFace() {
	//���_���W�̐ݒ�
	_vertices[0] = { -w, -w, 0.0f };
	_vertices[1] = { -w,  w, 0.0f };
	_vertices[2] = {  w, -w, 0.0f };
	_vertices[3] = {  w,  w, 0.0f };

	//�C���f�b�N�X���W�̐ݒ�
	_indices[0] = 0; _indices[1] = 1; _indices[2] = 2;
	_indices[3] = 2; _indices[4] = 1; _indices[5] = 3;
}

void Face::CopyVertex(Vertex* vertices) {
	for (int i = 0; i < fVertNum; i++) {
		vertices[i].pos = _vertices[i];
	}
}