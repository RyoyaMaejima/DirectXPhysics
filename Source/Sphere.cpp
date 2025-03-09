#include "Sphere.h"
#include "XMFLOAT3Math.h"

Sphere& Sphere::Instance() {
	static Sphere instance;
	return instance;
}

void Sphere::Init() {
	CreateSphere();

	PassVertexValue();

	for (int i = 0; i < sVertNum; i++) {
		_vertData[i].id = sphere_id;
	}
}

void Sphere::Begin() {
	//�e�ϐ��̏�����
	_worldMat = XMMatrixIdentity();
	_center = { 0.0f, 0.0f, 0.0f };
	_v = { 0.0f, 0.0f, 0.0f };

	//���W�̏����ݒ�
	_worldMat *= XMMatrixTranslation(_defPos.x, _defPos.y, _defPos.z);
	_center = AddVector(_center, _defPos);
}

void Sphere::Update(float deltaTime, Vertex* fVertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };

	//�ʂ̍��W���󂯎��
	for (int i = 0; i < 4; i++) {
		_fCorners[i] = fVertices[fCornerNums[i]].pos;
	}

	//�Ԃ������Ƃ�
	if (Collision()) {
		//���˂�
		Bound();

		//�߂荞�񂾕��̈ʒu����
		XMFLOAT3 dir = ScalarVecror(PenDepth(), NormalizeVector(_v));
		_worldMat *= XMMatrixTranslation(dir.x, dir.y, dir.z);
		_center = AddVector(_center, dir);
	}
	//�Ԃ����ĂȂ��Ƃ�
	else {
		//������
		Fall();
	}

	move = ScalarVecror(deltaTime, _v);

	//���W�ɔ��f
	_worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	_center = AddVector(_center, move);
}

Sphere::Sphere() {

}

Sphere::~Sphere() {

}

void Sphere::CreateSphere() {
	const float PI = XM_PI;

	//���_���W�̐ݒ�
	int index = 0;
	for (int lat = 0; lat <= latiNum; lat++) {
		float latTheta = lat * PI / latiNum;
		for (int lon = 0; lon < longNum; lon++) {
			float lonTheta = lon * 2 * PI / longNum;

			float x = _r * static_cast<float>(sin(latTheta) * cos(lonTheta));
			float y = _r * static_cast<float>(cos(latTheta));
			float z = _r * static_cast<float>(sin(latTheta) * sin(lonTheta));
			_vertices[index] = { x, y, z };

			//�@���x�N�g���̐ݒ�
			_normals[index] = NormalizeVector(_vertices[index]);

			if (fmod(sin(_vertices[index].x * 10) + cos(_vertices[index].z * 10), 2.0f) > 1.0f) {
				_colors[index] = { 0.0f, 0.0f, 0.0f };//��
			}
			else {
				_colors[index] = { 1.0f, 1.0f, 1.0f };//��
			}

			index++;
		}
	}

	//�C���f�b�N�X�̐ݒ�
	index = 0;
	for (int lat = 0; lat < latiNum; lat++) {
		for (int lon = 0; lon < longNum; lon++) {
			int first = lat * longNum + lon;
			int second = first + longNum;

			_indices[index] = first;
			index++;
			_indices[index] = second;
			index++;
			_indices[index] = first + 1;
			index++;

			_indices[index] = second;
			index++;
			_indices[index] = second + 1;
			index++;
			_indices[index] = first + 1;
			index++;
		}
	}
}

void Sphere::PassVertexValue() {
	for (int i = 0; i < sVertNum; i++) {
		_vertData[i].pos = _vertices[i];
		_vertData[i].normal = _normals[i];
		_vertData[i].color = _colors[i];
	}
}

bool Sphere::Collision() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//�ŋߓ_���擾
	return CalcDistance(_center, vfnp) <= _r;
}

void Sphere::Fall() {
	_v.y -= _g;
}

void Sphere::Bound() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//�ŋߓ_���擾
	XMFLOAT3 n;//�Փ˂̖@���x�N�g��

	//�ʂɏՓ˂����Ƃ�
	if (!isEdge) {
		XMFLOAT3 fV01 = SubVector(_fCorners[1], _fCorners[0]);
		XMFLOAT3 fV02 = SubVector(_fCorners[2], _fCorners[0]);
		n = CrossVector(fV01, fV02);//�ʂ̖@���x�N�g��
	}
	//�ӂ�p�ɏՓ˂����Ƃ�
	else {
		n = SubVector(_center, vfnp);//�Փ˓_���璆�S�ւ̕����x�N�g��
	}

	XMFLOAT3 norN = NormalizeVector(n);//�@���x�N�g���̐��K��

	//���x��ʂ̖@�������Ɛڐ퐬���ɕ���
	XMFLOAT3 vN = ScalarVecror(DotVector(_v, norN), norN);//�@������
	XMFLOAT3 vT = SubVector(_v, vN);//�ڐ�����

	//�Փˌ�̖@���������v�Z
	vN = ScalarVecror(-_e, vN);

	//�Փˌ�̑��x���v�Z
	_v = AddVector(vN, vT);
}

float Sphere::PenDepth() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//�ŋߓ_���擾
	return _r - CalcDistance(_center, vfnp);
}