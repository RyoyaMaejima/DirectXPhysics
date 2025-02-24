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
	XMFLOAT3 normal;
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
const unsigned int rowNum = 12;
const unsigned int colNum = 12;
const unsigned int fVertNum = (rowNum + 1) * (colNum + 1);//頂点数
const unsigned int fIndicesNum = 6 * rowNum * colNum;//インデックス数
const unsigned int fCornerNums[4] = { rowNum * (colNum + 1), 0, fVertNum - 1, colNum };//四隅の番号（左下、左上、右下、右上）

//テクスチャデータ
const unsigned int tVertNum = 4;//頂点数
const unsigned int tIndicesNum = 6;//インデックス数
