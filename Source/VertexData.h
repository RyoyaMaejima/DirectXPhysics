#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//���_�\����
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	unsigned int id;
};

//���f�[�^
const unsigned int latiNum = 12;
const unsigned int longNum = 24;
const unsigned int sVertNum = (latiNum + 1) * longNum;//���_��
const unsigned int sIndicesNum = 6 * latiNum * longNum;//�C���f�b�N�X��

//�ʃf�[�^
const unsigned int fVertNum = 4;//���_��
const unsigned int fIndicesNum = 6;//�C���f�b�N�X��

//�e�N�X�`���f�[�^
const unsigned int tVertNum = 4;//���_��
const unsigned int tIndicesNum = 6;//�C���f�b�N�X��