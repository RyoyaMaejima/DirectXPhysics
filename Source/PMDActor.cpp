#include "PMDActor.h"
#include "XMFLOAT3Math.h"

///���f���̃p�X�ƃe�N�X�`���̃p�X���獇���p�X�𓾂�
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
	//PMD�t�@�C���ǂݍ���
	if (FAILED(LoadPMDFile())) {
		assert(0);
	}

	for (int i = 0; i < pVertNum; i++) {
		_vertData[i].id = pmdActor_id;
	}

	//�{�[���̏����ݒ�
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());
}

void PMDActor::Begin() {
	//�e�ϐ��̏�����
	_worldMat = XMMatrixIdentity();
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());

	//���̃Z�b�g
	_isLeftLeg = true;
	SetLeg();

	//���W�̏����ݒ�
	_worldMat *= XMMatrixTranslation(_defPos.x, _defPos.y, _defPos.z);

	//��]�̏����ݒ�
	RotateLeg(_defRotLeftLeg, _defRotLeftKnee, _defRotLeftAnkle);
}

void PMDActor::Update(float deltaTime, einput_state state) {
	fill(_boneMatrices.begin(), _boneMatrices.end(), XMMatrixIdentity());
	XMFLOAT3 move = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };

	//���͏���
	switch (state)
	{
	case move_up:
		move.y = _v * deltaTime;
		break;
	case move_down:
		move.y = -_v * deltaTime;
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

	//��]
	if (_isLeftLeg) {
		RotateLeg(AddVector(_defRotLeftLeg, _theta), _defRotLeftKnee, _defRotLeftAnkle);
	}
	else {
		RotateLeg(AddVector(_defRotRightLeg, _theta), _defRotRightKnee, _defRotRightAnkle);
	}

	//�ړ�
	_worldMat *= XMMatrixTranslation(move.x, move.y, move.z);
}

PMDActor::PMDActor() {

}

PMDActor::~PMDActor() {

}

HRESULT PMDActor::LoadPMDFile() {
	//PMD�w�b�_�\����
	struct PMDHeader {
		float version;
		char model_name[20];//���f����
		char comment[256];//���f���R�����g
	};
	char signature[3];
	PMDHeader pmdheader = {};

	//PMD�t�@�C���ǂݍ���
	string strModelPath = "Model/�����~�N.pmd";
	auto fp = fopen(strModelPath.c_str(), "rb");
	if (fp == nullptr) {
		//�G���[����
		return ERROR_FILE_NOT_FOUND;
	}
	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

#pragma pack(1)
	//PMD���_�\����
	struct PMDVertex {
		XMFLOAT3 pos;//���_���W
		XMFLOAT3 normal;//�@���x�N�g��
		XMFLOAT2 uv;//uv���W
		unsigned short boneNo[2];//�{�[���ԍ�
		unsigned char boneWeight;//�{�[���e���x
		unsigned char edgeFlg;//�֊s���t���O
	};//���v38�o�C�g
#pragma pack()

	//���_���̎擾
	unsigned int vertNum;
	fread(&vertNum, sizeof(vertNum), 1, fp);

	//���_�f�[�^�̎擾
	vector<PMDVertex> vertData(vertNum);
	fread(vertData.data(), vertData.size() * sizeof(PMDVertex), 1, fp);

	//�C���f�b�N�X���̎擾
	unsigned int indicesNum;
	fread(&indicesNum, sizeof(indicesNum), 1, fp);

	//�C���f�b�N�X�f�[�^�̎擾
	vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

#pragma pack(1)
	//PMD�}�e���A���\����
	struct PMDMaterial {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;//�f�B�t���[�Y��
		float specularity;//�X�y�L�����̋���(��Z�l)
		XMFLOAT3 specular;//�X�y�L�����F
		XMFLOAT3 ambient;//�A���r�G���g�F
		unsigned char toonIdx;//�g�D�[���ԍ�
		unsigned char edgeFlg;//�}�e���A�����̗֊s���t���O
		//2�o�C�g�̃p�f�B���O
		unsigned int indicesNum;//���̃}�e���A�������蓖����C���f�b�N�X��
		char texFilePath[20];//�e�N�X�`���t�@�C����
	};//�p�f�B���O���������邽�ߍ��v72�o�C�g
#pragma pack()

	//�}�e���A�����̎擾
	unsigned int materialNum;
	fread(&materialNum, sizeof(materialNum), 1, fp);

	//�}�e���A���f�[�^�̎擾
	vector<PMDMaterial> materials(materialNum);
	fread(materials.data(), materials.size() * sizeof(PMDMaterial), 1, fp);

#pragma pack(1)
	//�ǂݍ��ݗp�{�[���\����
	struct PMDBone {
		char boneName[20];//�{�[����
		unsigned short parentNo;//�e�{�[���ԍ�
		unsigned short nextNo;//��[�̃{�[���ԍ�
		unsigned char type;//�{�[�����
		unsigned short ikBoneNo;//IK�{�[���ԍ�
		XMFLOAT3 pos;//�{�[���̊�_���W
	};
#pragma pack()

	//�{�[�����̎擾
	unsigned short boneNum = 0;
	fread(&boneNum, sizeof(boneNum), 1, fp);

	//�{�[���f�[�^�̎擾
	vector<PMDBone> bones(boneNum);
	fread(bones.data(), bones.size() * sizeof(PMDBone), 1, fp);

	fclose(fp);

	//�f�[�^�̃R�s�[
	//���_�f�[�^
	for (int i = 0; i < pVertNum; i++) {
		_vertData[i].pos = vertData[i].pos;
		_vertData[i].normal = vertData[i].normal;
		_vertData[i].uv = vertData[i].uv;
		_vertData[i].boneNo[0] = vertData[i].boneNo[0];
		_vertData[i].boneNo[1] = vertData[i].boneNo[1];
		_vertData[i].boneWeight = vertData[i].boneWeight;
	}

	//�C���f�b�N�X�f�[�^
	copy(indices.begin(), indices.end(), _indices);

	//�}�e���A���f�[�^
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

	//�{�[���f�[�^
	vector<string> boneNames(pBoneNum);//�C���f�b�N�X�Ɩ��O�̊֌W�\�z�p
	//�{�[���m�[�h�}�b�v�����
	for (int i = 0; i < pBoneNum; i++) {
		auto& b = bones[i];
		boneNames[i] = b.boneName;
		auto& node = _boneNodeTable[b.boneName];
		node.boneIdx = i;
		node.startPos = b.pos;
	}
	//�e�q�֌W���\�z����
	for (auto& b : bones) {
		//�e�C���f�b�N�X���`�F�b�N
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
	//�p�x�̏�����
	_theta = { 0.0f, 0.0f, 0.0f };

	//����
	if (_isLeftLeg) {
		//�{�[���̐ݒ�
		_legNode = _boneNodeTable["����"];
		_kneeNode = _boneNodeTable["���Ђ�"];
		_ankleNode = _boneNodeTable["������"];

		//���̈ʒu�擾
		_rootPos = AddVector(_defPos, _legNode.startPos);
	}
	//�E��
	else {
		//�{�[���̐ݒ�
		_legNode = _boneNodeTable["�E��"];
		_kneeNode = _boneNodeTable["�E�Ђ�"];
		_ankleNode = _boneNodeTable["�E����"];

		//���̈ʒu�擾
		_rootPos = AddVector(_defPos, _legNode.startPos);
	}
}

void PMDActor::RotateLeg(XMFLOAT3 legTheta, XMFLOAT3 kneeTheta, XMFLOAT3 ankleTheta) {
	//��
	auto& legPos = _legNode.startPos;
	auto legMat = XMMatrixTranslation(-legPos.x, -legPos.y, -legPos.z);
	legMat *= XMMatrixRotationX(legTheta.x);
	legMat *= XMMatrixRotationY(legTheta.y);
	legMat *= XMMatrixRotationZ(legTheta.z);
	legMat *= XMMatrixTranslation(legPos.x, legPos.y, legPos.z);

	//�Ђ�
	auto& kneePos = _kneeNode.startPos;
	auto kneeMat = XMMatrixTranslation(-kneePos.x, -kneePos.y, -kneePos.z);
	kneeMat *= XMMatrixRotationX(kneeTheta.x);
	kneeMat *= XMMatrixRotationY(kneeTheta.y);
	kneeMat *= XMMatrixRotationZ(kneeTheta.z);
	kneeMat *= XMMatrixTranslation(kneePos.x, kneePos.y, kneePos.z);

	//����
	auto& anklePos = _ankleNode.startPos;
	auto ankleMat = XMMatrixTranslation(-anklePos.x, -anklePos.y, -anklePos.z);
	ankleMat *= XMMatrixRotationX(ankleTheta.x);
	ankleMat *= XMMatrixRotationY(ankleTheta.y);
	ankleMat *= XMMatrixRotationZ(ankleTheta.z);
	ankleMat *= XMMatrixTranslation(anklePos.x, anklePos.y, anklePos.z);

	_boneMatrices[_legNode.boneIdx] = legMat;
	_boneMatrices[_kneeNode.boneIdx] = kneeMat;
	_boneMatrices[_ankleNode.boneIdx] = ankleMat;

	//�S�̂ɔ��f
	XMMATRIX mat = XMMatrixIdentity();
	RecursiveMatrixMultipy(&_boneNodeTable["�Z���^�["], mat);
}