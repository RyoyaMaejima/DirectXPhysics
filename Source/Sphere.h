#pragma once
#include "VertexData.h"

class Sphere {
public:
	//�l�n���p
	Vertex _vertData[sVertNum] = {};//���_�f�[�^
	unsigned int _indices[sIndicesNum] = {};//�C���f�b�N�X���W
	XMMATRIX _worldMat = XMMatrixIdentity();//���W�ϊ��s��

	//Spehre�̃V���O���g���C���X�^���X�𓾂�
	static Sphere& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, Vertex* fVertices);

	Sphere();
	~Sphere();

private:
	//�v�Z�p
	XMFLOAT3 _vertices[sVertNum] = {};//���_���W
	XMFLOAT3 _normals[sVertNum] = {};//�@���x�N�g��
	XMFLOAT3 _colors[sVertNum] = {};//�F
	XMFLOAT3 _fCorners[4] = {};//�ʂ̎l��

	//�p�����[�^
	float _r = 1.0f;//���a
	float _g = 0.1f;//�d�͉����x
	float _e = 1.0f;//�����W��
	XMFLOAT3 _defPos = { 0.0f, 8.0f, 7.0f };//�����ʒu

	XMFLOAT3 _center = { 0.0f, 0.0f, 0.0f };//���S���W
	XMFLOAT3 _v = { 0.0f, 0.0f, 0.0f };//���x

	//���쐬
	void CreateSphere();

	//���_�f�[�^�̎󂯓n��
	void PassVertexValue();

	//�ڐG����
	bool Collision();

	//����
	void Fall();

	//�ђʐ[�x�̌v�Z
	float PenDepth();

	//����
	void Bound();
};