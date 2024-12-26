#include "Sphere.h"
#include "XMFLOAT3Math.h"

Sphere& Sphere::Instance() {
	static Sphere instance;
	return instance;
}

void Sphere::Init(Vertex* vertices, unsigned int* indices) {
	CreateSphere();

	CopyVertex(vertices);
	copy(begin(_indices), end(_indices), indices);
}

void Sphere::Begin(XMMATRIX& worldMat) {
	//各変数の初期化
	worldMat = XMMatrixIdentity();
	CreateSphere();
	center = { 0.0f, 0.0f, 0.0f };
	v = { 0.0f, 0.0f, 0.0f };

	XMFLOAT3 pos = { 0.0f, 4.0f, 0.0f };

	//座標の初期設定
	worldMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);
	center = AddVector(center, pos);
}

void Sphere::Update(float deltaTime, XMMATRIX& worldMat, Vertex* fVertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };

	//面の座標を受け取る
	for (int i = 0; i < fVertNum; i++) {
		_fVertices[i] = fVertices[i].pos;
	}

	//ぶつかったとき
	if (Collision()) {
		//跳ねる
		Bound();

		//めり込んだ分の位置調整
		XMFLOAT3 dir = ScalarVecror(PenDepth(), NormalizeVector(v));
		worldMat *= XMMatrixTranslation(dir.x, dir.y, dir.z);
		center = AddVector(center, dir);
	}
	//ぶつかってないとき
	else {
		//落ちる
		Fall();
	}

	move = ScalarVecror(deltaTime, v);

	//座標に反映
	worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	center = AddVector(center, move);
}

Sphere::Sphere() {

}

Sphere::~Sphere() {

}

void Sphere::CreateSphere() {
	const float PI = XM_PI;

	//頂点座標の設定
	int index = 0;
	for(int lat = 0; lat <= latiNum; lat++) {
		float latTheta = lat * PI / latiNum;
		for (int lon = 0; lon < longNum; lon++) {
			float lonTheta = lon * 2 * PI / longNum;

			float x = r * static_cast<float>(sin(latTheta) * cos(lonTheta));
			float y = r * static_cast<float>(cos(latTheta));
			float z = r * static_cast<float>(sin(latTheta) * sin(lonTheta));

			_vertices[index] = { x, y, z };
			index++;
		}
	}

	//インデックス座標の設定
	index = 0;
	for (int lat = 0; lat < latiNum; lat++) {
		for (int lon = 0; lon < longNum; lon++) {
			int first = lat * longNum + lon;
			int second = first + longNum;

			_indices[index] = first;
			index++;
			_indices[index] = second;
			index++;
			_indices[index] = first + 1;
			index++;

			_indices[index] = second;
			index++;
			_indices[index] = second + 1;
			index++;
			_indices[index] = first + 1;
			index++;
		}
	}
}

void Sphere::CopyVertex(Vertex* vertices) {
	for (int i = 0; i < sVertNum; i++) {
		vertices[i].pos = _vertices[i];
	}
}

bool Sphere::Collision() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//最近点を取得
	return CalcDistance(center,vfnp) <= r;
}

void Sphere::Fall() {
	v.y -= g;
}

void Sphere::Bound() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//最近点を取得
	XMFLOAT3 n;//衝突の法線ベクトル

	//面に衝突したとき
	if (!isEdge) {
		XMFLOAT3 fV01 = SubVector(_fVertices[1], _fVertices[0]);
		XMFLOAT3 fV02 = SubVector(_fVertices[2], _fVertices[0]);
		n = CrossVector(fV01, fV02);//面の法線ベクトル
	}
	//辺や角に衝突したとき
	else {
		n = SubVector(center, vfnp);//衝突点から中心への方向ベクトル
	}

	XMFLOAT3 norN = NormalizeVector(n);//法線ベクトルの正規化

	//速度を面の法線成分と接戦成分に分解
	XMFLOAT3 vN = ScalarVecror(DotVector(v, norN), norN);//法線成分
	XMFLOAT3 vT = SubVector(v, vN);//接線成分

	//衝突後の法線成分を計算
	vN = ScalarVecror(-e, vN);

	//衝突後の速度を計算
	v = AddVector(vN, vT);
}

float Sphere::PenDepth() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(center, _fVertices[0], _fVertices[1], _fVertices[2], _fVertices[3], isEdge);//最近点を取得
	return r - CalcDistance(center, vfnp);
}