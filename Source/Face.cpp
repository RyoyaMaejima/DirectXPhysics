#include "Face.h"
#include "XMFLOAT3Math.h"

#include <random>
#include <cmath>

Face& Face::Instance() {
	static Face instance;
	return instance;
}

void Face::Init(Vertex* vertices, unsigned int* indices) {
	CreateFace();

	CopyVertex(vertices);
	copy(begin(_indices), end(_indices), indices);
}

void Face::Begin(XMMATRIX& worldMat, Vertex* vertices) {
	//各変数の初期化
	worldMat = XMMatrixIdentity();
	CreateFace();
	center = { 0.0f, 0.0f, 0.0f };
	xTheta = 0.0f;

	XMFLOAT3 pos = { 0.0f, -2.0f, 0.0f };
	float xPos = XM_PIDIV2;

	//-π/4～π/4までのランダムな値を設定
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dist(-XM_PIDIV4, XM_PIDIV4);
	float zPos = dist(gen);
	zTheta = zPos;

	//回転の初期設定
	worldMat *= XMMatrixRotationX(xPos);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateXVector(xPos, _vertices[i]);
	}
	worldMat *= XMMatrixRotationZ(zPos);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateZVector(zPos, _vertices[i]);
	}

	//座標の初期設定
	worldMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], pos);
	}

	//座標を渡す
	CopyVertex(vertices);
	center = AddVector(center, pos);
}

void Face::Update(float deltaTime, einput_state state, XMMATRIX& worldMat, Vertex* vertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	float xMove = 0.0f;
	float zMove = 0.0f;

	//入力処理
	switch (state)
	{
		case move_up:
			move.z = v * deltaTime;
			break;
		case move_down:
			move.z = -v * deltaTime;
			break;
		case move_right:
			move.x = v * deltaTime;
			break;
		case move_left:
			move.x = -v * deltaTime;
			break;
		case rotateX_positive:
			if (xTheta < limTheta) {
				xMove = rv * deltaTime;
				xTheta += xMove;
			}
			break;
		case rotateX_negative:
			if (xTheta > -limTheta) {
				xMove = -rv * deltaTime;
				xTheta += xMove;
			}
			break;
		case rotateZ_positive:
			if (zTheta > -limTheta) {
				zMove = -rv * deltaTime;
				zTheta += zMove;
			}
			break;
		case rotateZ_negative:
			if (zTheta < limTheta) {
				zMove = rv * deltaTime;
				zTheta += zMove;
			}
			break;
		case invalid:
			break;
		default:
			break;
	}

	//原点に移動
	worldMat *= XMMatrixTranslation(-center.x, -center.y, -center.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], ScalarVecror(-1.0f, center));
	}
	//回転
	worldMat *= XMMatrixRotationX(xMove);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateXVector(xMove, _vertices[i]);
	}
	worldMat *= XMMatrixRotationZ(zMove);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = RotateZVector(zMove, _vertices[i]);
	}
	//移動した分を戻す
	worldMat *= XMMatrixTranslation(center.x, center.y, center.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], center);
	}

	//移動
	worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	for (int i = 0; i < fVertNum; i++) {
		_vertices[i] = AddVector(_vertices[i], move);
	}

	//座標を渡す
	CopyVertex(vertices);
	center = AddVector(center, move);
}

Face::Face() {

}

Face::~Face() {

}

void Face::CreateFace() {
	//頂点座標の設定
	_vertices[0] = { -w, -w, 0.0f };
	_vertices[1] = { -w,  w, 0.0f };
	_vertices[2] = {  w, -w, 0.0f };
	_vertices[3] = {  w,  w, 0.0f };

	//インデックス座標の設定
	_indices[0] = 0; _indices[1] = 1; _indices[2] = 2;
	_indices[3] = 2; _indices[4] = 1; _indices[5] = 3;
}

void Face::CopyVertex(Vertex* vertices) {
	for (int i = 0; i < fVertNum; i++) {
		vertices[i].pos = _vertices[i];
	}
}