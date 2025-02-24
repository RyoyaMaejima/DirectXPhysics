#pragma once
#include "VertexData.h"

class Sphere {
public:
	//Spehreのシングルトンインスタンスを得る
	static Sphere& Instance();

	void Init(Vertex* vertices, unsigned int* indices);
	void Begin(XMMATRIX& worldMat);
	void Update(float deltaTime, XMMATRIX& worldMat, Vertex* fVertices);

	Sphere();
	~Sphere();

private:
	//値渡し用
	XMFLOAT3 _vertices[sVertNum] = {};//頂点座標
	XMFLOAT3 _normals[sVertNum] = {};//法線ベクトル
	unsigned int _indices[sIndicesNum] = {};//インデックス座標
	XMFLOAT3 _fCorners[4] = {};//面の四隅

	//パラメータ
	float r = 0.5f;//半径
	float g = 0.1f;//重力加速度
	float e = 1.0f;//反発係数

	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };//中心座標
	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };//速度

	//球作成
	void CreateSphere();

	//座標のコピー
	void CopyVertex(Vertex* vertices);

	//接触判定
	bool Collision();

	//落下
	void Fall();

	//貫通深度の計算
	float PenDepth();

	//反発
	void Bound();
};
