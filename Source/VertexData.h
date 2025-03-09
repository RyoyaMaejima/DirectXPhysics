#pragma once
#include <vector>
#include <DirectXMath.h>
#include <sstream>

using namespace std;
using namespace DirectX;

//図形ID
enum figure_id
{
	sphere_id,
	face_id,
	texture_id,
	pmdActor_id,
	invalid_id
};

//頂点構造体
struct Vertex {
	XMFLOAT3 pos;//頂点座標
	XMFLOAT3 normal;//法線ベクトル
	XMFLOAT2 uv;//uv座標
	XMFLOAT3 color;//色
	figure_id id;//図形ID
	unsigned short boneNo[2];//ボーン番号
	unsigned char boneWeight;//ボーン影響度

	Vertex() : pos({ 0, 0, 0 }), normal({ 0, 0, 0 }), uv({ 0, 0 }), color({ 0, 0, 0 }), id(invalid_id), boneNo{ 0, 0 }, boneWeight(0) {}//デフォルトコンストラクタ
};

//入力状態
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

//マテリアル構造体
//シェーダー転送用
struct MaterialForHlsl {
	XMFLOAT3 diffuse;//ディフューズ色
	float alpha;//ディフューズα
	XMFLOAT3 specular;//スペキュラ色
	float specularity;//スペキュラの強さ(乗算値)
	XMFLOAT3 ambient;//アンビエント色
};
//マテリアルデータ
struct Material {
	MaterialForHlsl material;
	unsigned int indicesNum;//インデックス数
	string texPath;//テクスチャファイルパス
};

//球データ
const unsigned int latiNum = 12;
const unsigned int longNum = 24;
const unsigned int sVertNum = (latiNum + 1) * longNum;//頂点数
const unsigned int sIndicesNum = 6 * latiNum * longNum;//インデックス数

//面データ
const unsigned int rowNum = 1;
const unsigned int colNum = 1;
const unsigned int fVertNum = (rowNum + 1) * (colNum + 1);//頂点数
const unsigned int fIndicesNum = 6 * rowNum * colNum;//インデックス数
const unsigned int fCornerNums[4] = { rowNum * (colNum + 1), 0, fVertNum - 1, colNum };//四隅の番号（左下、左上、右下、右上）

//テクスチャデータ
const unsigned int tVertNum = 4;//頂点数
const unsigned int tIndicesNum = 6;//インデックス数

//PMDアクター
const unsigned int pVertNum = 9036;//頂点数
const unsigned int pIndicesNum = 44991;//インデックス数
const unsigned int pMaterialNum = 17;//マテリアル数
const unsigned int pBoneNum = 122;//ボーン数