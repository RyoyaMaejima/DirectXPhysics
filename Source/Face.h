#pragma once
#include "VertexData.h"

class Face {
public:
	//Faceのシングルトンインスタンスを得る
	static Face& Instance();

	void Init(Vertex* vertices, unsigned int* indices);
	void Begin(XMMATRIX& worldMat, Vertex* vertices);
	void Update(float deltaTime, einput_state state, XMMATRIX& worldMat, Vertex* vertices);

	Face();
	~Face();

private:
	//値渡し用
	XMFLOAT3 _vertices[fVertNum] = {};//頂点座標
	unsigned int _indices[fIndicesNum] = {};//インデックス座標

	//パラメータ
	float w = 1.0f;//1辺の長さの半分
	float v = 20.0f;//移動速度
	float rv = XM_PI / 0.6f;//回転速度
	float limTheta = XM_PIDIV4;//回転範囲

	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };//中心座標
	float xTheta = 0.0f;//x軸の回転角度
	float zTheta = 0.0f;//z軸の回転角度

	//面作成
	void CreateFace();

	//座標のコピー
	void CopyVertex(Vertex* vertices);
};