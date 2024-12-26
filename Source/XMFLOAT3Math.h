#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

//���Z
XMFLOAT3 AddVector(XMFLOAT3 v1, XMFLOAT3 v2);

//���Z
XMFLOAT3 SubVector(XMFLOAT3 v1, XMFLOAT3 v2);

//�X�J���[�{
XMFLOAT3 ScalarVecror(float f, XMFLOAT3 v);

//x�����S�ŉ�]
XMFLOAT3 RotateXVector(float theta, XMFLOAT3 v);

//z�����S�ŉ�]
XMFLOAT3 RotateZVector(float theta, XMFLOAT3 v);

//�x�N�g���̑傫��
float LengthVector(XMFLOAT3 v);

//���K��
XMFLOAT3 NormalizeVector(XMFLOAT3 v);

//����
float DotVector(XMFLOAT3 v1, XMFLOAT3 v2);

//�O��
XMFLOAT3 CrossVector(XMFLOAT3 v1, XMFLOAT3 v2);

//2�_�Ԃ̋���
float CalcDistance(XMFLOAT3 v1, XMFLOAT3 v2);

//�_�Ɛ����̍ŋߓ_
XMFLOAT3 CalcLineNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2);

//�_�Ɩʂ̍ŋߓ_
XMFLOAT3 CalcFaceNearestPoint(XMFLOAT3 vp, XMFLOAT3 v1, XMFLOAT3 v2, XMFLOAT3 v3, XMFLOAT3 v4, bool& isEdge);