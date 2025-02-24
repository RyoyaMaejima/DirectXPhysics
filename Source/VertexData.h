#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//�}�`ID
enum figure_id
{
	sphere_id,
	face_id,
	texture_id
};

//���_�\����
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	figure_id id;
};

//���͏��
enum einput_state
{
	move_up,
	move_down,
	move_right,
	move_left,
	rotateX_positive,
	rotateX_negative,
	rotateZ_positive,
	rotateZ_negative,
	invalid
};

//���f�[�^
const unsigned int latiNum = 12;
const unsigned int longNum = 24;
const unsigned int sVertNum = (latiNum + 1) * longNum;//���_��
const unsigned int sIndicesNum = 6 * latiNum * longNum;//�C���f�b�N�X��

//�ʃf�[�^
const unsigned int rowNum = 12;
const unsigned int colNum = 12;
const unsigned int fVertNum = (rowNum + 1) * (colNum + 1);//���_��
const unsigned int fIndicesNum = 6 * rowNum * colNum;//�C���f�b�N�X��
const unsigned int fCornerNums[4] = { rowNum * (colNum + 1), 0, fVertNum - 1, colNum };//�l���̔ԍ��i�����A����A�E���A�E��j

//�e�N�X�`���f�[�^
const unsigned int tVertNum = 4;//���_��
const unsigned int tIndicesNum = 6;//�C���f�b�N�X��
