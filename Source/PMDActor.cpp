#include "PMDActor.h"
#include "XMFLOAT3Math.h"

///モデルのパスとテクスチャのパスから合成パスを得る
string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath) {
	int pathIndex1 = modelPath.rfind('/');
	int pathIndex2 = modelPath.rfind('\\');
	auto pathIndex = max(pathIndex1, pathIndex2);
	auto folderPath = modelPath.substr(0, pathIndex + 1);
	return folderPath + texPath;
}

PMDActor& PMDActor::Instance() {
	static PMDActor instance;
	return instance;
}

void PMDActor::Init() {
	//PMDファイル読み込み
	if (FAILED(LoadPMDFile())) {
		assert(0);
	}

	for (int i = 0; i < pVertNum; i++) {
		_vertData[i].id = pmdActor_id;
	}

	//ボーンの初期設定
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());
}

void PMDActor::Begin() {
	//各変数の初期化
	_worldMat = XMMatrixIdentity();
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());

	//足のセット
	_isLeftLeg = true;
	SetLeg();

	//座標の初期設定
	_worldMat *= XMMatrixTranslation(_defPos.x, _defPos.y, _defPos.z);

	//回転の初期設定
	RotateLeg(_defRotLeftLeg, _defRotLeftKnee, _defRotLeftAnkle);
}

void PMDActor::Update(float deltaTime, einput_state state) {
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };

	//入力処理
	switch (state)
	{
	case move_up:
		move.z = _v * deltaTime;
		break;
	case move_down:
		move.z = -_v * deltaTime;
		break;
	case move_right:
		move.x = _v * deltaTime;
		break;
	case move_left:
		move.x = -_v * deltaTime;
		break;
	case change_leg:
		if (!_isPut) {
			_isLeftLeg = !_isLeftLeg;
			_isPut = true;
			SetLeg();
		}
		break;
	case rotateZ_positive:
		if (_theta.z < _limTheta) {
			rot.z = _rv * deltaTime;
			_theta.z += rot.z;
		}
		break;
	case rotateZ_negative:
		if (_theta.z > -_limTheta) {
			rot.z = -_rv * deltaTime;
			_theta.z += rot.z;
		}
		break;
	case invalid:
		_isPut = false;
		break;
	default:
		break;
	}

	//回転
	if (_isLeftLeg) {
		RotateLeg(AddVector(_defRotLeftLeg, _theta), _defRotLeftKnee, _defRotLeftAnkle);
	}
	else {
		RotateLeg(AddVector(_defRotRightLeg, _theta), _defRotRightKnee, _defRotRightAnkle);
	}

	//移動
	_worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
}

PMDActor::PMDActor() {

}

PMDActor::~PMDActor() {

}

HRESULT PMDActor::LoadPMDFile() {
	//PMDヘッダ構造体
	struct PMDHeader {
		float version;
		char model_name[20];//モデル名
		char comment[256];//モデルコメント
	};
	char signature[3];
	PMDHeader pmdheader = {};

	//PMDファイル読み込み
	string strModelPath = "Model/初音ミク.pmd";
	auto fp = fopen(strModelPath.c_str(), "rb");
	if (fp == nullptr) {
		//エラー処理
		return ERROR_FILE_NOT_FOUND;
	}
	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

#pragma pack(1)
	//PMD頂点構造体
	struct PMDVertex {
		XMFLOAT3 pos;//頂点座標
		XMFLOAT3 normal;//法線ベクトル
		XMFLOAT2 uv;//uv座標
		unsigned short boneNo[2];//ボーン番号
		unsigned char boneWeight;//ボーン影響度
		unsigned char edgeFlg;//輪郭線フラグ
	};//合計38バイト
#pragma pack()

	//頂点数の取得
	unsigned int vertNum;
	fread(&vertNum, sizeof(vertNum), 1, fp);

	//頂点データの取得
	vector<PMDVertex> vertData(vertNum);
	fread(vertData.data(), vertData.size() * sizeof(PMDVertex), 1, fp);

	//インデックス数の取得
	unsigned int indicesNum;
	fread(&indicesNum, sizeof(indicesNum), 1, fp);

	//インデックスデータの取得
	vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

#pragma pack(1)
	//PMDマテリアル構造体
	struct PMDMaterial {
		XMFLOAT3 diffuse;//ディフューズ色
		float alpha;//ディフューズα
		float specularity;//スペキュラの強さ(乗算値)
		XMFLOAT3 specular;//スペキュラ色
		XMFLOAT3 ambient;//アンビエント色
		unsigned char toonIdx;//トゥーン番号
		unsigned char edgeFlg;//マテリアル毎の輪郭線フラグ
		//2バイトのパディング
		unsigned int indicesNum;//このマテリアルが割り当たるインデックス数
		char texFilePath[20];//テクスチャファイル名
	};//パディングが発生するため合計72バイト
#pragma pack()

	//マテリアル数の取得
	unsigned int materialNum;
	fread(&materialNum, sizeof(materialNum), 1, fp);

	//マテリアルデータの取得
	vector<PMDMaterial> materials(materialNum);
	fread(materials.data(), materials.size() * sizeof(PMDMaterial), 1, fp);

#pragma pack(1)
	//読み込み用ボーン構造体
	struct PMDBone {
		char boneName[20];//ボーン名
		unsigned short parentNo;//親ボーン番号
		unsigned short nextNo;//先端のボーン番号
		unsigned char type;//ボーン種別
		unsigned short ikBoneNo;//IKボーン番号
		XMFLOAT3 pos;//ボーンの基準点座標
	};
#pragma pack()

	//ボーン数の取得
	unsigned short boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	//ボーンデータの取得
	vector<PMDBone> bones(boneNum);
	fread(bones.data(), bones.size() * sizeof(PMDBone), 1, fp);

	fclose(fp);

	//データのコピー
	//頂点データ
	for (int i = 0; i < pVertNum; i++) {
		_vertData[i].pos = vertData[i].pos;
		_vertData[i].normal = vertData[i].normal;
		_vertData[i].uv = vertData[i].uv;
		_vertData[i].boneNo[0] = vertData[i].boneNo[0];
		_vertData[i].boneNo[1] = vertData[i].boneNo[1];
		_vertData[i].boneWeight = vertData[i].boneWeight;
	}

	//インデックスデータ
	copy(indices.begin(), indices.end(), _indices);

	//マテリアルデータ
	for (int i = 0; i < pMaterialNum; i++) {
		_materials[i].material.diffuse = materials[i].diffuse;
		_materials[i].material.alpha = materials[i].alpha;
		_materials[i].material.specular = materials[i].specular;
		_materials[i].material.specularity = materials[i].specularity;
		_materials[i].material.ambient = materials[i].ambient;
		_materials[i].indicesNum = materials[i].indicesNum;

		if (strlen(materials[i].texFilePath) == 0) {
			_materials[i].texPath = "";
		}
		else {
			string texFilePath = materials[i].texFilePath;
			_materials[i].texPath = GetTexturePathFromModelAndTexPath(strModelPath, texFilePath.c_str());
		}
	}

	//ボーンデータ
	vector<string> boneNames(pBoneNum);//インデックスと名前の関係構築用
	//ボーンノードマップを作る
	for (int i = 0; i < pBoneNum; i++) {
		auto& b = bones[i];
		boneNames[i] = b.boneName;
		auto& node = _boneNodeTable[b.boneName];
		node.boneIdx = i;
		node.startPos = b.pos;
	}
	//親子関係を構築する
	for (auto& b : bones) {
		//親インデックスをチェック
		if (b.parentNo >= pBoneNum) {
			continue;
		}
		auto parentName = boneNames[b.parentNo];
		_boneNodeTable[parentName].children.emplace_back(&_boneNodeTable[b.boneName]);
	}
	_boneMatrices.resize(pBoneNum);

	return S_OK;
}

void PMDActor::RecursiveMatrixMultipy(BoneNode* node, XMMATRIX& mat) {
	_boneMatrices[node->boneIdx] *= mat;
	for (auto& cnode : node->children) {
		RecursiveMatrixMultipy(cnode, _boneMatrices[node->boneIdx]);
	}
}

void PMDActor::SetLeg() {
	//角度の初期化
	_theta = { 0.0f, 0.0f, 0.0f };

	//左足
	if (_isLeftLeg) {
		//ボーンの設定
		_legNode = _boneNodeTable["左足"];
		_kneeNode = _boneNodeTable["左ひざ"];
		_ankleNode = _boneNodeTable["左足首"];

		//足の位置取得
		_rootPos = AddVector(_defPos, _legNode.startPos);
	}
	//右足
	else {
		//ボーンの設定
		_legNode = _boneNodeTable["右足"];
		_kneeNode = _boneNodeTable["右ひざ"];
		_ankleNode = _boneNodeTable["右足首"];

		//足の位置取得
		_rootPos = AddVector(_defPos, _legNode.startPos);
	}
}

void PMDActor::RotateLeg(XMFLOAT3 legTheta, XMFLOAT3 kneeTheta, XMFLOAT3 ankleTheta) {
	//足
	auto& legPos = _legNode.startPos;
	auto legMat = XMMatrixTranslation(-legPos.x, -legPos.y, -legPos.z);
	legMat *= XMMatrixRotationX(legTheta.x);
	legMat *= XMMatrixRotationY(legTheta.y);
	legMat *= XMMatrixRotationZ(legTheta.z);
	legMat *= XMMatrixTranslation(legPos.x, legPos.y, legPos.z);

	//ひざ
	auto& kneePos = _kneeNode.startPos;
	auto kneeMat = XMMatrixTranslation(-kneePos.x, -kneePos.y, -kneePos.z);
	kneeMat *= XMMatrixRotationX(kneeTheta.x);
	kneeMat *= XMMatrixRotationY(kneeTheta.y);
	kneeMat *= XMMatrixRotationZ(kneeTheta.z);
	kneeMat *= XMMatrixTranslation(kneePos.x, kneePos.y, kneePos.z);

	//足首
	auto& anklePos = _ankleNode.startPos;
	auto ankleMat = XMMatrixTranslation(-anklePos.x, -anklePos.y, -anklePos.z);
	ankleMat *= XMMatrixRotationX(ankleTheta.x);
	ankleMat *= XMMatrixRotationY(ankleTheta.y);
	ankleMat *= XMMatrixRotationZ(ankleTheta.z);
	ankleMat *= XMMatrixTranslation(anklePos.x, anklePos.y, anklePos.z);

	_boneMatrices[_legNode.boneIdx] = legMat;
	_boneMatrices[_kneeNode.boneIdx] = kneeMat;
	_boneMatrices[_ankleNode.boneIdx] = ankleMat;

	//全体に反映
	XMMATRIX mat = XMMatrixIdentity();
	RecursiveMatrixMultipy(&_boneNodeTable["センター"], mat);
}