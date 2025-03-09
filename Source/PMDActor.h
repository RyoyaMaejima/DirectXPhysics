#pragma once
#include <d3d12.h>
#include <map>

#include "VertexData.h"

class PMDActor {
public:
	//�l�n���p
	Vertex _vertData[pVertNum] = {};//���_�f�[�^
	unsigned int _indices[pIndicesNum] = {};//�C���f�b�N�X���W
	XMMATRIX _worldMat = XMMatrixIdentity();//���W�ϊ��s��
	Material _materials[pMaterialNum] = {};//�}�e���A��
	vector<XMMATRIX> _boneMatrices;//�{�[���s��
	XMFLOAT3 _rootPos = { 0.0f, 0.0f, 0.0f };//���̈ʒu

	//PMDActor�̃V���O���g���C���X�^���X�𓾂�
	static PMDActor& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, einput_state state);

	PMDActor();
	~PMDActor();

private:
	//�{�[���c���[
	struct BoneNode {
		int boneIdx;//�{�[���C���f�b�N�X
		XMFLOAT3 startPos;//�{�[����_(��]���S)
		vector<BoneNode*> children;//�q�m�[�h
	};
	map<string, BoneNode> _boneNodeTable;//�{�[�����O�Ή�

	//�v�Z�p
	BoneNode _legNode;
	BoneNode _kneeNode;
	BoneNode _ankleNode;

	//�p�����[�^
	float _v = 30.0f;//�ړ����x
	float _rv = XM_PI / 0.6f;//��]���x
	float _limTheta = XM_PIDIV4;//��]�͈�
	XMFLOAT3 _defPos = { 0.0f, -12.0f, 10.0f };//�����ʒu
	XMFLOAT3 _defRotLeftLeg = { XM_PIDIV4, 0.0f, XM_PIDIV4 };//������]�i�����j
	XMFLOAT3 _defRotRightLeg = { XM_PIDIV4, 0.0f, -XM_PIDIV4 };//������]�i�E���j
	XMFLOAT3 _defRotLeftKnee = { 0.0f, 0.0f, -XM_PIDIV4 * 2 };//������]�i���Ђ��j
	XMFLOAT3 _defRotRightKnee = { 0.0f, 0.0f, XM_PIDIV4 * 2 };//������]�i�E�Ђ��j
	XMFLOAT3 _defRotLeftAnkle = { 0.0f, -XM_PIDIV4, 0.0f };//������]�i������j
	XMFLOAT3 _defRotRightAnkle = { 0.0f, XM_PIDIV4, 0.0f };//������]�i�E����j

	XMFLOAT3 _theta = { 0.0f, 0.f, 0.0f };//���݂̉�]�p�x
	bool _isLeftLeg = true;//�������E����
	bool _isPut = false;//�{�^���������Ă��邩

	//PMD�t�@�C���ǂݍ���
	HRESULT LoadPMDFile();

	//�ċA����
	void RecursiveMatrixMultipy(BoneNode* node, XMMATRIX& mat);

	//���̃Z�b�g
	void SetLeg();

	//����]
	void RotateLeg(XMFLOAT3 legTheta, XMFLOAT3 kneeTheta, XMFLOAT3 ankleTheta);
};
