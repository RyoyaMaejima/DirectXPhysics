#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//図形ID
enum figure_id
{
	sphere_id,
	face_id,
	texture_id
};

//頂点構造体
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	figure_id id;
};

//入力状態
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

//球データ
const unsigned int latiNum = 12;
const unsigned int longNum = 24;
const unsigned int sVertNum = (latiNum + 1) * longNum;//頂点数
const unsigned int sIndicesNum = 6 * latiNum * longNum;//インデックス数

//面データ
const unsigned int fVertNum = 4;//頂点数
const unsigned int fIndicesNum = 6;//インデックス数

//テクスチャデータ
const unsigned int tVertNum = 4;//頂点数
const unsigned int tIndicesNum = 6;//インデックス数
