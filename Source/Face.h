#pragma once
#include "VertexData.h"

class Face {
public:
	//値渡し用
	Vertex _vertData[fVertNum] = {};//頂点データ
	unsigned int _indices[fIndicesNum] = {};//インデックス座標
	XMMATRIX _worldMat = XMMatrixIdentity();//座標変換行列

	//Faceのシングルトンインスタンスを得る
	static Face& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, einput_state state, XMFLOAT3 rotCenter);

	Face();
	~Face();

private:
	//計算用
	XMFLOAT3 _vertices[fVertNum] = {};//頂点座標
	XMMATRIX _moveMat = XMMatrixIdentity();//座標移動記録用

	//パラメータ
	float _w = 3.1f;//横の長さの半分
	float _h = 1.1f;//縦の長さの半分
	float _v = 30.0f;//移動速度
	float _rv = XM_PI / 0.6f;//回転速度
	float _limTheta = XM_PIDIV4;//回転範囲
	XMFLOAT3 _defLeftPos = { 1.0f, -5.5f, 8.0f };//初期位置（左足）
	XMFLOAT3 _defRightPos = { -1.0f, -5.5f, 8.0f };//初期位置（右足）
	XMFLOAT3 _defLeftRot = { 0.0f, 0.0f, XM_PIDIV4 };//初期回転（左足）
	XMFLOAT3 _defRightRot = { 0.0f, 0.0f, -XM_PIDIV4 };//初期回転（右足）

	XMFLOAT3 _mv = { 0.0f, 0.0f, 0.0f };//移動量
	XMFLOAT3 _theta = { 0.0f, 0.f, 0.0f };//現在の回転角度
	bool _isLeftLeg = true;//左足か右足か
	bool _isPut = false;//ボタンを押しているか

	//面作成
	void CreateFace();

	//足のセット
	void SetLeg();

	//頂点データの受け渡し
	void PassVertexValue();
};