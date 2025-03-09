#pragma once
#include <vector>
#include <DirectXMath.h>
#include <sstream>

using namespace std;
using namespace DirectX;

//�}�`ID
enum figure_id
{
	sphere_id,
	face_id,
	texture_id,
	pmdActor_id,
	invalid_id
};

//���_�\����
struct Vertex {
	XMFLOAT3 pos;//���_���W
	XMFLOAT3 normal;//�@���x�N�g��
	XMFLOAT2 uv;//uv���W
	XMFLOAT3 color;//�F
	figure_id id;//�}�`ID
	unsigned short boneNo[2];//�{�[���ԍ�
	unsigned char boneWeight;//�{�[���e���x

	Vertex() : pos({ 0, 0, 0 }), normal({ 0, 0, 0 }), uv({ 0, 0 }), color({ 0, 0, 0 }), id(invalid_id), boneNo{ 0, 0 }, boneWeight(0) {}//�f�t�H���g�R���X�g���N�^
};

//���͏��
enum einput_state
{
	move_up,
	move_down,
	move_right,
	move_left,
	change_leg,
	rotateZ_positive,
	rotateZ_negative,
	invalid
};

//�}�e���A���\����
//�V�F�[�_�[�]���p
struct MaterialForHlsl {
	XMFLOAT3 diffuse;//�f�B�t���[�Y�F
	float alpha;//�f�B�t���[�Y��
	XMFLOAT3 specular;//�X�y�L�����F
	float specularity;//�X�y�L�����̋���(��Z�l)
	XMFLOAT3 ambient;//�A���r�G���g�F
};
//�}�e���A���f�[�^
struct Material {
	MaterialForHlsl material;
	unsigned int indicesNum;//�C���f�b�N�X��
	string texPath;//�e�N�X�`���t�@�C���p�X
};

//���f�[�^
const unsigned int latiNum = 12;
const unsigned int longNum = 24;
const unsigned int sVertNum = (latiNum + 1) * longNum;//���_��
const unsigned int sIndicesNum = 6 * latiNum * longNum;//�C���f�b�N�X��

//�ʃf�[�^
const unsigned int rowNum = 1;
const unsigned int colNum = 1;
const unsigned int fVertNum = (rowNum + 1) * (colNum + 1);//���_��
const unsigned int fIndicesNum = 6 * rowNum * colNum;//�C���f�b�N�X��
const unsigned int fCornerNums[4] = { rowNum * (colNum + 1), 0, fVertNum - 1, colNum };//�l���̔ԍ��i�����A����A�E���A�E��j

//�e�N�X�`���f�[�^
const unsigned int tVertNum = 4;//���_��
const unsigned int tIndicesNum = 6;//�C���f�b�N�X��

//PMD�A�N�^�[
const unsigned int pVertNum = 9036;//���_��
const unsigned int pIndicesNum = 44991;//�C���f�b�N�X��
const unsigned int pMaterialNum = 17;//�}�e���A����
const unsigned int pBoneNum = 122;//�{�[����