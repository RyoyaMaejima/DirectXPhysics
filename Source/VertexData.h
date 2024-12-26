#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//頂点構造体
struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT2 uv;
	unsigned int id;
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
