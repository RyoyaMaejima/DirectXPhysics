#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//加算
XMFLOAT3 AddVector(XMFLOAT3 v1, XMFLOAT3 v2);

//減算
XMFLOAT3 SubVector(XMFLOAT3 v1, XMFLOAT3 v2);

//スカラー倍
XMFLOAT3 ScalarVecror(float f, XMFLOAT3 v);

//x軸中心で回転
XMFLOAT3 RotateXVector(float theta, XMFLOAT3 v);

//z軸中心で回転
XMFLOAT3 RotateZVector(float theta, XMFLOAT3 v);

//ベクトルの大きさ
float LengthVector(XMFLOAT3 v);

//正規化
XMFLOAT3 NormalizeVector(XMFLOAT3 v);

//内積
float DotVector(XMFLOAT3 v1, XMFLOAT3 v2);

//外積
XMFLOAT3 CrossVector(XMFLOAT3 v1, XMFLOAT3 v2);

//2点間の距離
float CalcDistance(XMFLOAT3 v1, XMFLOAT3 v2);

//点と線分の最近点
XMFLOAT3 CalcLineNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2);

//点と面の最近点
XMFLOAT3 CalcFaceNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3, XMFLOAT3 v4, bool& isEdge);