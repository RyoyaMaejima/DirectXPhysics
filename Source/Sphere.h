#pragma once
#include "VertexData.h"

class Sphere {
public:
	//値渡し用
	Vertex _vertData[sVertNum] = {};//頂点データ
	unsigned int _indices[sIndicesNum] = {};//インデックス座標
	XMMATRIX _worldMat = XMMatrixIdentity();//座標変換行列

	//Spehreのシングルトンインスタンスを得る
	static Sphere& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, Vertex* fVertices);

	Sphere();
	~Sphere();

private:
	//計算用
	XMFLOAT3 _vertices[sVertNum] = {};//頂点座標
	XMFLOAT3 _normals[sVertNum] = {};//法線ベクトル
	XMFLOAT3 _colors[sVertNum] = {};//色
	XMFLOAT3 _fCorners[4] = {};//面の四隅

	//パラメータ
	float _r = 1.0f;//半径
	float _g = 0.1f;//重力加速度
	float _e = 1.0f;//反発係数
	XMFLOAT3 _defPos = { 0.0f, 8.0f, 7.0f };//初期位置

	XMFLOAT3 _center = { 0.0f, 0.0f, 0.0f };//中心座標
	XMFLOAT3 _v = { 0.0f, 0.0f, 0.0f };//速度

	//球作成
	void CreateSphere();

	//頂点データの受け渡し
	void PassVertexValue();

	//接触判定
	bool Collision();

	//落下
	void Fall();

	//貫通深度の計算
	float PenDepth();

	//反発
	void Bound();
};