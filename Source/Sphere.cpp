#include "Sphere.h"
#include "XMFLOAT3Math.h"

Sphere& Sphere::Instance() {
	static Sphere instance;
	return instance;
}

void Sphere::Init() {
	CreateSphere();

	PassVertexValue();

	for (int i = 0; i < sVertNum; i++) {
		_vertData[i].id = sphere_id;
	}
}

void Sphere::Begin() {
	//各変数の初期化
	_worldMat = XMMatrixIdentity();
	_center = { 0.0f, 0.0f, 0.0f };
	_v = { 0.0f, 0.0f, 0.0f };

	//座標の初期設定
	_worldMat *= XMMatrixTranslation(_defPos.x, _defPos.y, _defPos.z);
	_center = AddVector(_center, _defPos);
}

void Sphere::Update(float deltaTime, Vertex* fVertices) {
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };

	//面の座標を受け取る
	for (int i = 0; i < 4; i++) {
		_fCorners[i] = fVertices[fCornerNums[i]].pos;
	}

	//ぶつかったとき
	if (Collision()) {
		//跳ねる
		Bound();

		//めり込んだ分の位置調整
		XMFLOAT3 dir = ScalarVecror(PenDepth(), NormalizeVector(_v));
		_worldMat *= XMMatrixTranslation(dir.x, dir.y, dir.z);
		_center = AddVector(_center, dir);
	}
	//ぶつかってないとき
	else {
		//落ちる
		Fall();
	}

	move = ScalarVecror(deltaTime, _v);

	//座標に反映
	_worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
	_center = AddVector(_center, move);
}

Sphere::Sphere() {

}

Sphere::~Sphere() {

}

void Sphere::CreateSphere() {
	const float PI = XM_PI;

	//頂点座標の設定
	int index = 0;
	for (int lat = 0; lat <= latiNum; lat++) {
		float latTheta = lat * PI / latiNum;
		for (int lon = 0; lon < longNum; lon++) {
			float lonTheta = lon * 2 * PI / longNum;

			float x = _r * static_cast<float>(sin(latTheta) * cos(lonTheta));
			float y = _r * static_cast<float>(cos(latTheta));
			float z = _r * static_cast<float>(sin(latTheta) * sin(lonTheta));
			_vertices[index] = { x, y, z };

			//法線ベクトルの設定
			_normals[index] = NormalizeVector(_vertices[index]);

			if (fmod(sin(_vertices[index].x * 10) + cos(_vertices[index].z * 10), 2.0f) > 1.0f) {
				_colors[index] = { 0.0f, 0.0f, 0.0f };//黒
			}
			else {
				_colors[index] = { 1.0f, 1.0f, 1.0f };//白
			}

			index++;
		}
	}

	//インデックスの設定
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

void Sphere::PassVertexValue() {
	for (int i = 0; i < sVertNum; i++) {
		_vertData[i].pos = _vertices[i];
		_vertData[i].normal = _normals[i];
		_vertData[i].color = _colors[i];
	}
}

bool Sphere::Collision() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//最近点を取得
	return CalcDistance(_center, vfnp) <= _r;
}

void Sphere::Fall() {
	_v.y -= _g;
}

void Sphere::Bound() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//最近点を取得
	XMFLOAT3 n;//衝突の法線ベクトル

	//面に衝突したとき
	if (!isEdge) {
		XMFLOAT3 fV01 = SubVector(_fCorners[1], _fCorners[0]);
		XMFLOAT3 fV02 = SubVector(_fCorners[2], _fCorners[0]);
		n = CrossVector(fV01, fV02);//面の法線ベクトル
	}
	//辺や角に衝突したとき
	else {
		n = SubVector(_center, vfnp);//衝突点から中心への方向ベクトル
	}

	XMFLOAT3 norN = NormalizeVector(n);//法線ベクトルの正規化

	//速度を面の法線成分と接戦成分に分解
	XMFLOAT3 vN = ScalarVecror(DotVector(_v, norN), norN);//法線成分
	XMFLOAT3 vT = SubVector(_v, vN);//接線成分

	//衝突後の法線成分を計算
	vN = ScalarVecror(-_e, vN);

	//衝突後の速度を計算
	_v = AddVector(vN, vT);
}

float Sphere::PenDepth() {
	bool isEdge = false;
	XMFLOAT3 vfnp = CalcFaceNearestPoint(_center, _fCorners[0], _fCorners[1], _fCorners[2], _fCorners[3], isEdge);//最近点を取得
	return _r - CalcDistance(_center, vfnp);
}