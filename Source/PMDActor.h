#pragma once
#include <d3d12.h>
#include <map>

#include "VertexData.h"

class PMDActor {
public:
	//値渡し用
	Vertex _vertData[pVertNum] = {};//頂点データ
	unsigned int _indices[pIndicesNum] = {};//インデックス座標
	XMMATRIX _worldMat = XMMatrixIdentity();//座標変換行列
	Material _materials[pMaterialNum] = {};//マテリアル
	vector<XMMATRIX> _boneMatrices;//ボーン行列
	XMFLOAT3 _rootPos = { 0.0f, 0.0f, 0.0f };//足の位置

	//PMDActorのシングルトンインスタンスを得る
	static PMDActor& Instance();

	void Init();
	void Begin();
	void Update(float deltaTime, einput_state state);

	PMDActor();
	~PMDActor();

private:
	//ボーンツリー
	struct BoneNode {
		int boneIdx;//ボーンインデックス
		XMFLOAT3 startPos;//ボーン基準点(回転中心)
		vector<BoneNode*> children;//子ノード
	};
	map<string, BoneNode> _boneNodeTable;//ボーン名前対応

	//計算用
	BoneNode _legNode;
	BoneNode _kneeNode;
	BoneNode _ankleNode;

	//パラメータ
	float _v = 30.0f;//移動速度
	float _rv = XM_PI / 0.6f;//回転速度
	float _limTheta = XM_PIDIV4;//回転範囲
	XMFLOAT3 _defPos = { 0.0f, -12.0f, 10.0f };//初期位置
	XMFLOAT3 _defRotLeftLeg = { XM_PIDIV4, 0.0f, XM_PIDIV4 };//初期回転（左足）
	XMFLOAT3 _defRotRightLeg = { XM_PIDIV4, 0.0f, -XM_PIDIV4 };//初期回転（右足）
	XMFLOAT3 _defRotLeftKnee = { 0.0f, 0.0f, -XM_PIDIV4 * 2 };//初期回転（左ひざ）
	XMFLOAT3 _defRotRightKnee = { 0.0f, 0.0f, XM_PIDIV4 * 2 };//初期回転（右ひざ）
	XMFLOAT3 _defRotLeftAnkle = { 0.0f, -XM_PIDIV4, 0.0f };//初期回転（左足首）
	XMFLOAT3 _defRotRightAnkle = { 0.0f, XM_PIDIV4, 0.0f };//初期回転（右足首）

	XMFLOAT3 _theta = { 0.0f, 0.f, 0.0f };//現在の回転角度
	bool _isLeftLeg = true;//左足か右足か
	bool _isPut = false;//ボタンを押しているか

	//PMDファイル読み込み
	HRESULT LoadPMDFile();

	//再帰処理
	void RecursiveMatrixMultipy(BoneNode* node, XMMATRIX& mat);

	//足のセット
	void SetLeg();

	//足回転
	void RotateLeg(XMFLOAT3 legTheta, XMFLOAT3 kneeTheta, XMFLOAT3 ankleTheta);
};
