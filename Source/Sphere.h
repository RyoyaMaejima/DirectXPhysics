#pragma once
#include "VertexData.h"

class Sphere {
public:
	//Spehre�̃V���O���g���C���X�^���X�𓾂�
	static Sphere& Instance();

	void Init(Vertex* vertices, unsigned int* indices);
	void Begin(XMMATRIX& worldMat);
	void Update(float deltaTime, XMMATRIX& worldMat, Vertex* fVertices);

	Sphere();
	~Sphere();

private:
	//�l�n���p
	XMFLOAT3 _vertices[sVertNum] = {};//���_���W
	XMFLOAT3 _normals[sVertNum] = {};//�@���x�N�g��
	unsigned int _indices[sIndicesNum] = {};//�C���f�b�N�X���W
	XMFLOAT3 _fCorners[4] = {};//�ʂ̎l��

	//�p�����[�^
	float r = 0.5f;//���a
	float g = 0.1f;//�d�͉����x
	float e = 1.0f;//�����W��

	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };//���S���W
	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };//���x

	//���쐬
	void CreateSphere();

	//���W�̃R�s�[
	void CopyVertex(Vertex* vertices);

	//�ڐG����
	bool Collision();

	//����
	void Fall();

	//�ђʐ[�x�̌v�Z
	float PenDepth();

	//����
	void Bound();
};
