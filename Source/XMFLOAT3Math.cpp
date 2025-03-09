#include "XMFLOAT3Math.h"

XMFLOAT3 AddVector(XMFLOAT3 v1, XMFLOAT3 v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

XMFLOAT3 SubVector(XMFLOAT3 v1, XMFLOAT3 v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

XMFLOAT3 ScalarVecror(float f, XMFLOAT3 v) {
	v.x *= f;
	v.y *= f;
	v.z *= f;
	return v;
}

XMFLOAT3 RotateXVector(float theta, XMFLOAT3 v) {
	float y = v.y;
	float z = v.z;

	v.y = y * cos(theta) - z * sin(theta);
	v.z = y * sin(theta) + z * cos(theta);

	return v;
}

XMFLOAT3 RotateYVector(float theta, XMFLOAT3 v) {
	float z = v.z;
	float x = v.x;

	v.z = z * cos(theta) - x * sin(theta);
	v.x = z * sin(theta) + x * cos(theta);

	return v;
}

XMFLOAT3 RotateZVector(float theta, XMFLOAT3 v) {
	float x = v.x;
	float y = v.y;

	v.x = x * cos(theta) - y * sin(theta);
	v.y = x * sin(theta) + y * cos(theta);

	return v;
}

XMFLOAT3 RotateVector(XMFLOAT3 vTheta, XMFLOAT3 center, XMFLOAT3 v) {
	v = SubVector(v, center);
	v = RotateXVector(vTheta.x, v);
	v = RotateYVector(vTheta.y, v);
	v = RotateZVector(vTheta.z, v);
	v = AddVector(v, center);

	return v;
}

float LengthVector(XMFLOAT3 v) {
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

XMFLOAT3 NormalizeVector(XMFLOAT3 v) {
	return ScalarVecror(1.0f / LengthVector(v), v);
}

float DotVector(XMFLOAT3 v1, XMFLOAT3 v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

XMFLOAT3 CrossVector(XMFLOAT3 v1, XMFLOAT3 v2) {
	XMFLOAT3 v = { 0.0f, 0.0f, 0.0f };
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

float CalcDistance(XMFLOAT3 v1, XMFLOAT3 v2) {
	XMFLOAT3 v12 = SubVector(v2, v1);
	return LengthVector(v12);
}

XMFLOAT3 CalcLineNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2) {
	XMFLOAT3 v1p = SubVector(vp, v1);
	XMFLOAT3 v12 = SubVector(v2, v1);
	float t = DotVector(v1p, v12) / DotVector(v12, v12);

	if (t < 0.0f) {
		return v1;
	}
	else if (t > 1.0f) {
		return v2;
	}
	else {
		XMFLOAT3 vh = AddVector(v1, ScalarVecror(t, v12));
		return vh;
	}
}

XMFLOAT3 CalcFaceNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3, XMFLOAT3 v4, bool& isEdge) {
	XMFLOAT3 v12 = SubVector(v2, v1);
	XMFLOAT3 v13 = SubVector(v3, v1);
	XMFLOAT3 fn = CrossVector(v12, v13);//面の法線ベクトル
	XMFLOAT3 norfn = NormalizeVector(fn);//法線ベクトルの正規化

	//点pから平面までの距離を計算
	float d = fabs(DotVector(SubVector(vp, v1), norfn));

	//平面上の投影点を計算
	XMFLOAT3 vh = SubVector(vp, ScalarVecror(d, norfn));

	//投影点が内部にあれば最近点は投影点と一致する
	XMFLOAT3 v1h = SubVector(vh, v1);
	if (DotVector(v1h, v12) >= 0.0f && DotVector(v1h, v12) <= DotVector(v12, v12) &&
		DotVector(v1h, v13) >= 0.0f && DotVector(v1h, v13) <= DotVector(v13, v13)) {
		isEdge = false;
		return vh;
	}

	//投影点が外部にあれば一番近い辺との最近点を返す
	XMFLOAT3 vlnp[4];
	vlnp[0] = CalcLineNearestPoint(vp, v1, v2);
	vlnp[1] = CalcLineNearestPoint(vp, v2, v4);
	vlnp[2] = CalcLineNearestPoint(vp, v4, v3);
	vlnp[3] = CalcLineNearestPoint(vp, v3, v1);

	XMFLOAT3 vfnp = vlnp[0];
	float minD = CalcDistance(vp, vlnp[0]);
	for (int i = 1; i < 4; i++) {
		if (minD > CalcDistance(vp, vlnp[i])) {
			vfnp = vlnp[i];
			minD = CalcDistance(vp, vlnp[i]);
		}
	}
	isEdge = true;
	return vfnp;
}