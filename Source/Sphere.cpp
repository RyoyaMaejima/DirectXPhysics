#include "Sphere.h"
#include "XMFLOAT3Math.h"

Sphere& Sphere::Instance() {
	static Sphere instance;
	return instance;
}

void Sphere::Init(Vertex* vertices, unsigned int* indices) {
	CreateSphere();

	CopyVertex(vertices);
	copy(begin(_indices), end(_indices), indices);
}

void Sphere::Begin(XMMATRIX& worldMat) {
	//�e�ϐ��̏�����
	worldMat = XMMatrixIdentity();
	CreateSphere();
	center = { 0.0f, 0.0f, 0.0f };
	v = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 pos = { 0.0f, 4.0f, 0.0f };

	//���W�̏����ݒ�
	worldMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);
	center = AddVector(center, pos);
}

void Sphere::Update(float deltaTime, XMMATRIX& worldMat, Vertex* fVertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };

	//�ʂ̍��W���󂯎��
	for (int i = 0; i < fVertNum; i++) {
		_fVertices[i] = fVertices[i].pos;
	}

	//�Ԃ������Ƃ�
	if (Collision()) {
		//���˂�
		Bound();

		//�߂荞�񂾕��̈ʒu����
		XMFLOAT3 dir = ScalarVecror(PenDepth(), NormalizeVector(v));
		worldMat *= XMMatrixTranslation(dir.x, dir.y, dir.z);
		center = AddVector(center, dir);
	}
	//�Ԃ����ĂȂ��Ƃ�
	else {
		//������
		Fall();
	}

	move = ScalarVecror(deltaTime, v);

	//���W�ɔ��f
	worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	center = AddVector(center, move);
}

Sphere::Sphere() {

}

Sphere::~Sphere() {

}

void Sphere::CreateSphere() {
	const float PI = XM_PI;

	//���_���W�̐ݒ�
	int index = 0;
	for(int lat = 0; lat <= latiNum; lat++) {
		float latTheta = lat * PI / latiNum;
		for (int lon = 0; lon < longNum; lon++) {
			float lonTheta = lon * 2 * PI / longNum;

			float x = r * static_cast<float>(sin(latTheta) * cos(lonTheta));
			float y = r * static_cast<float>(cos(latTheta));
			float z = r * static_cast<float>(sin(latTheta) * sin(lonTheta));

			_vertices[index] = { x, y, z };
			index++;
		}
	}

	//�C���f�b�N�X���W�̐ݒ�
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

void Sphere::CopyVertex(Vertex* vertices) {
	for (int i = 0; i < sVertNum; i++) {
		vertices[i].pos = _vertices[i];
	}
}

bool Sphere::Collision() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//�ŋߓ_���擾
	return CalcDistance(center,vfnp) <= r;
}

void Sphere::Fall() {
	v.y -= g;
}

void Sphere::Bound() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//�ŋߓ_���擾
	XMFLOAT3 n;//�Փ˂̖@���x�N�g��

	//�ʂɏՓ˂����Ƃ�
	if (!isEdge) {
		XMFLOAT3 fV01 = SubVector(_fVertices[1], _fVertices[0]);
		XMFLOAT3 fV02 = SubVector(_fVertices[2], _fVertices[0]);
		n = CrossVector(fV01, fV02);//�ʂ̖@���x�N�g��
	}
	//�ӂ�p�ɏՓ˂����Ƃ�
	else {
		n = SubVector(center, vfnp);//�Փ˓_���璆�S�ւ̕����x�N�g��
	}

	XMFLOAT3 norN = NormalizeVector(n);//�@���x�N�g���̐��K��

	//���x��ʂ̖@�������Ɛڐ퐬���ɕ���
	XMFLOAT3 vN = ScalarVecror(DotVector(v, norN), norN);//�@������
	XMFLOAT3 vT = SubVector(v, vN);//�ڐ�����

	//�Փˌ�̖@���������v�Z
	vN = ScalarVecror(-e, vN);

	//�Փˌ�̑��x���v�Z
	v = AddVector(vN, vT);
}

float Sphere::PenDepth() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//�ŋߓ_���擾
	return r - CalcDistance(center, vfnp);
}