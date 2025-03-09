#include "Face.h"
#include "XMFLOAT3Math.h"

Face& Face::Instance() {
	static Face instance;
	return instance;
}

void Face::Init() {
	CreateFace();

	PassVertexValue();

	for (int i = 0; i < fVertNum; i++) {
		_vertData[i].id = face_id;
	}
}

void Face::Begin() {
	//�e�ϐ��̏�����
	_moveMat = XMMatrixIdentity();
	_mv = { 0.0f, 0.0f, 0.0f };

	//���̃Z�b�g
	_isLeftLeg = true;
	SetLeg();
	
	//���W��n��
	PassVertexValue();
}

void Face::Update(float deltaTime, einput_state state, XMFLOAT3 rotCenter) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };

	//���͏���
	switch (state)
	{
	case move_up:
		move.y = _v * deltaTime;
		break;
	case move_down:
		move.y = -_v * deltaTime;
		break;
	case move_right:
		move.x = _v * deltaTime;
		break;
	case move_left:
		move.x = -_v * deltaTime;
		break;
	case change_leg:
		if (!_isPut) {
			_isLeftLeg = !_isLeftLeg;
			_isPut = true;
			SetLeg();
		}
		break;
	case rotateZ_positive:
		if (_theta.z < _limTheta) {
			rot.z = _rv * deltaTime;
			_theta.z += rot.z;
		}
		break;
	case rotateZ_negative:
		if (_theta.z > -_limTheta) {
			rot.z = -_rv * deltaTime;
			_theta.z += rot.z;
		}
		break;
	case invalid:
		_isPut = false;
		break;
	default:
		break;
	}

	//���_�Ɉړ�
	_worldMat *= XMMatrixTranslation(-_mv.x, -_mv.y, -_mv.z);
	_worldMat *= XMMatrixTranslation(-rotCenter.x, -rotCenter.y, -rotCenter.z);
	//��]
	_worldMat *= XMMatrixRotationZ(rot.z);
	//�ړ���������߂�
	_worldMat *= XMMatrixTranslation(rotCenter.x, rotCenter.y, rotCenter.z);
	_worldMat *= XMMatrixTranslation(_mv.x, _mv.y, _mv.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateVector(rot, AddVector(_mv, rotCenter), _vertices[i]);
	}

	//�ړ�
	_worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	_moveMat *= XMMatrixTranslation(move.x, move.y, move.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], move);
	}
	_mv = AddVector(_mv, move);

	//���W��n��
	PassVertexValue();
}

Face::Face() {

}

Face::~Face() {

}

void Face::CreateFace() {
	const float dx = (2 * _w) / colNum;
	const float dz = (2 * _h) / rowNum;

	//���_���W�̐ݒ�
	int index = 0;
	for (int row = 0; row <= rowNum; row++) {
		for (int col = 0; col <= colNum; col++) {
			float x = -_w + col * dx;
			float z = -_w + row * dz;
			_vertices[index] = { x, 0.0f, z };

			index++;
		}
	}

	//�C���f�b�N�X�̐ݒ�
	index = 0;
	for (int row = 0; row < rowNum; row++) {
		for (int col = 0; col < colNum; col++) {
			int topLeft = row * (colNum + 1) + col;
			int topRight = topLeft + 1;
			int bottomLeft = (row + 1) * (colNum + 1) + col;
			int bottomRight = bottomLeft + 1;

			_indices[index] = topLeft;
			index++;
			_indices[index] = topRight;
			index++;
			_indices[index] = bottomLeft;
			index++;

			_indices[index] = topRight;
			index++;
			_indices[index] = bottomRight;
			index++;
			_indices[index] = bottomLeft;
			index++;
		}
	}
}

void Face::SetLeg() {
	//�e�ϐ��̏�����
	_worldMat = XMMatrixIdentity();
	CreateFace();
	_theta = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 origin = { 0.0f, 0.0f, 0.0f };//���_

	//����
	if (_isLeftLeg) {
		//��]�̐ݒ�
		_worldMat *= XMMatrixRotationZ(_defLeftRot.z);
		for (int i = 0; i < fVertNum; i++) {
			_vertices[i] = RotateVector(_defLeftRot, origin, _vertices[i]);
		}

		//���W�̐ݒ�
		_worldMat *= XMMatrixTranslation(_defLeftPos.x, _defLeftPos.y, _defLeftPos.z);
		for (int i = 0; i < fVertNum; i++) {
			_vertices[i] = AddVector(_vertices[i], _defLeftPos);
		}
	}
	//�E��
	else {
		//��]�̐ݒ�
		_worldMat *= XMMatrixRotationZ(_defRightRot.z);
		for (int i = 0; i < fVertNum; i++) {
			_vertices[i] = RotateVector(_defRightRot, origin, _vertices[i]);
		}

		//���W�̐ݒ�
		_worldMat *= XMMatrixTranslation(_defRightPos.x, _defRightPos.y, _defRightPos.z);
		for (int i = 0; i < fVertNum; i++) {
			_vertices[i] = AddVector(_vertices[i], _defRightPos);
		}
	}
	//�ړ����𔽉f
	_worldMat *= _moveMat;
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], _mv);
	}
}

void Face::PassVertexValue() {
	for (int i = 0; i < fVertNum; i++) {
		_vertData[i].pos = _vertices[i];
	}
}