#pragma once
#include <Windows.h>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <map>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <wrl.h>

#include "Sphere.h"
#include "Face.h"

using namespace Microsoft::WRL;

//�E�B���h�E�萔
const unsigned int window_width = 1280;
const unsigned int window_height = 720;

class Application
{
public:
	//Application�̃V���O���g���C���X�^���X�𓾂�
	static Application& Instance();

	//������
	bool Init();

	//���[�v�N��
	void Run();

	//�㏈��
	void Terminate();

private:
	//�E�B���h�E�֌W
	WNDCLASSEX _windowClass = {};
	HWND _hwnd = nullptr;

	//DXGI�֌W
	ComPtr<IDXGIFactory6> _dxgiFactory = nullptr;//DXGI�C���^�[�t�F�C�X
	ComPtr<ID3D12Device> _dev = nullptr;//�f�o�C�X

	//�R�}���h�֌W
	ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr;//�R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;//�R�}���h���X�g
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;//�R�}���h�L���[

	//�X���b�v�`�F�C��
	ComPtr <IDXGISwapChain4> _swapChain = nullptr;

	//�����_�[�^�[�Q�b�g�֌W
	vector<ID3D12Resource*> _backBuffers = {};//�o�b�N�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _rtvHeaps = nullptr;//�����_�[�^�[�Q�b�g�p�f�B�X�N���v�^�q�[�v

	//�t�F���X�֌W
	ComPtr<ID3D12Fence> _fence = nullptr;
	UINT64 _fenceVal = 0;

	//���_�֌W
	//��
	Vertex _sVertices[sVertNum] = {};//���_���W
	ComPtr<ID3D12Resource> _sVertBuff = nullptr;//���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _sVbView = {};//���_�o�b�t�@�[�r���[
	//��
	Vertex _fVertices[fVertNum] = {};//���_���W
	ComPtr<ID3D12Resource> _fVertBuff = nullptr;//���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _fVbView = {};//���_�o�b�t�@�[�r���[

	//�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> _vsBlob = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	//�p�C�v���C���X�e�[�g
	ComPtr<ID3D12PipelineState> _pipelinestate = nullptr;

	//���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> _rootsignature = nullptr;

	//�r���[�|�[�g�ƃV�U�[��`
	CD3DX12_VIEWPORT _viewport;//�r���[�|�[�g
	CD3DX12_RECT _scissorrect;//�V�U�[��`

	//�C���f�b�N�X�֌W
	//��
	unsigned int _sIndices[sIndicesNum] = {};//�C���f�b�N�X���W
	ComPtr<ID3D12Resource> _sIdxBuff = nullptr;//�C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _sIbView = {};;//�C���f�b�N�X�o�b�t�@�[�r���[
	//��
	unsigned int _fIndices[fIndicesNum] = {};//�C���f�b�N�X���W
	ComPtr<ID3D12Resource> _fIdxBuff = nullptr;//�C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _fIbView = {};;//�C���f�b�N�X�o�b�t�@�[�r���[

	//�e�N�X�`���֌W
	Vertex _tVertices[tVertNum] = {};//���_���W
	ComPtr<ID3D12Resource> _tVertBuff = nullptr;//���_�o�b�t�@�[
	D3D12_VERTEX_BUFFER_VIEW _tVbView = {};//���_�o�b�t�@�[�r���[
	unsigned int _tIndices[tIndicesNum] = {};//�C���f�b�N�X���W
	ComPtr<ID3D12Resource> _tIdxBuff = nullptr;//�C���f�b�N�X�o�b�t�@�[
	D3D12_INDEX_BUFFER_VIEW _tIbView = {};;//�C���f�b�N�X�o�b�t�@�[�r���[

	TexMetadata _metadata = {};//���^�f�[�^
	ComPtr<ID3D12Resource> _texBuff = nullptr;//�e�N�X�`���o�b�t�@�[

	//�萔�֌W
	//��
	ComPtr<ID3D12Resource> _sConstBuff = nullptr;//�萔�o�b�t�@�[
	//��
	ComPtr<ID3D12Resource> _fConstBuff = nullptr;//�萔�o�b�t�@�[
	//�e�N�X�`��
	ComPtr<ID3D12Resource> _tConstBuff = nullptr;//�萔�o�b�t�@�[

	//�e�N�X�`���ƒ萔�p�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> _basicDescHeap = nullptr;

	//���W�ϊ��n�s��
	struct MatricesData {
		XMMATRIX world;
		XMMATRIX viewproj;
	};

	//��
	XMMATRIX _sWorldMat;
	MatricesData* _sMapMatrix;

	//��
	XMMATRIX _fWorldMat;
	MatricesData* _fMapMatrix;

	//�e�N�X�`��
	XMMATRIX _tWorldMat;
	MatricesData* _tMapMatrix;

	//�[�x�֌W
	ComPtr<ID3D12Resource> _depthBuff = nullptr;//�[�x�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;//�[�x�o�b�t�@�p�f�B�X�N���v�^�q�[�v

	//���Ԍv���p
	LARGE_INTEGER _frequency;//���g��
	LARGE_INTEGER _prevTime;//1�O�̎���
	LARGE_INTEGER _currentTime;//���݂̎���

	//��
	Sphere _sphere;

	//��
	Face _face;

	//���͕ϐ�
	einput_state _state;

	//�A�j���[�V�������邩
	bool _isAnim = false;

	//�E�B���h�E�쐬
	void CreateGameWindow();

	//DXGI�֌W������
	HRESULT InitializeDXGIDevice();

	//�R�}���h�֌W������
	HRESULT InitializeCommand();

	//�X���b�v�`�F�C���쐬
	HRESULT CreateSwapChain();

	//�����_�[�^�[�Q�b�g�r���[�쐬
	HRESULT	CreateRenderTargetView();

	//���_�ƃC���f�b�N�X�̃o�b�t�@�[�r���[�쐬
	HRESULT CreateVertexAndIndexBufferView(
		figure_id id,
		ComPtr<ID3D12Resource>& vertBuff,
		D3D12_VERTEX_BUFFER_VIEW& vbView,
		ComPtr<ID3D12Resource>& idxBuff,
		D3D12_INDEX_BUFFER_VIEW& ibView
	);

	//�V�F�[�_�[�t�@�C���ǂݍ���
	HRESULT LoadShaderFile();

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�쐬
	HRESULT CreateGraphicsPipelineState();

	//���[�g�V�O�l�`���쐬
	HRESULT CreateRootSignature();

	//�e�N�X�`���t�@�C���ǂݍ���
	HRESULT LoadTextureFile();

	//�萔�o�b�t�@�[�쐬
	HRESULT CreateConstBuffer(
		figure_id id,
		ComPtr<ID3D12Resource>& constBuff,
		XMMATRIX& worldMat
	);

	//�e�N�X�`���ƒ萔�p�f�B�X�N���v�^�q�[�v�쐬
	HRESULT CreateBasicDescHeap();

	//�[�x�o�b�t�@�[�r���[�쐬
	HRESULT CreateDepthStencilView();

	//���_�f�[�^�̏�����
	void InitiallizeVertexData();

	//�L�[���͎󂯎��
	void Input();

	//�A�j���[�V�������͎󂯎��
	void AnimInput();

	//�V���O���g���̂��߂ɃR���X�g���N�^��private��
	//����ɃR�s�[�Ƒ�����֎~��
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:
	~Application();
};
