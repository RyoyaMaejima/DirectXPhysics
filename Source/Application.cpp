#include "Application.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

//�E�B���h�E�v���V�[�W���֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//�E�B���h�E���j�����ꂽ��Ă΂��
		PostQuitMessage(0);//OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//�K��̏������s��
}

//�f�o�b�O���C���[��L���ɂ���
void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}

Application& Application::Instance() {
	static Application instance;
	return instance;
}

bool Application::Init() {
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);
	CreateGameWindow();

#ifdef _DEBUG
	//�f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif
	//DirectX12�֘A������
	if (FAILED(InitializeDXGIDevice())) {
		assert(0);
		return false;
	}
	if (FAILED(InitializeCommand())) {
		assert(0);
		return false;
	}
	if (FAILED(CreateSwapChain())) {
		assert(0);
		return false;
	}
	if (FAILED(CreateRenderTargetView())) {
		assert(0);
		return false;
	}

	//�[�x�o�b�t�@�[�r���[�쐬
	if (FAILED(CreateDepthStencilView())) {
		assert(0);
		return false;
	}

	//�t�F���X�쐬
	if (FAILED(_dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	//�^�C�}�[�̏�����
	QueryPerformanceFrequency(&_frequency);
	QueryPerformanceCounter(&_prevTime);

	//���_�f�[�^������
	InitiallizeVertexData();

	//�V�F�[�_�[�t�@�C���ǂݍ���
	if (FAILED(LoadShaderFile())) {
		assert(0);
		return false;
	}

	//�e�N�X�`���t�@�C���ǂݍ���
	if (FAILED(LoadTextureFile())) {
		assert(0);
		return false;
	}

	//���_�ƃC���f�b�N�X�̃o�b�t�@�[�r���[�쐬
	if (FAILED(CreateVertexAndIndexBufferView(_sVertices[0].id, _sVertBuff, _sVbView, _sIdxBuff, _sIbView))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateVertexAndIndexBufferView(_fVertices[0].id, _fVertBuff, _fVbView, _fIdxBuff, _fIbView))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateVertexAndIndexBufferView(_tVertices[0].id, _tVertBuff, _tVbView, _tIdxBuff, _tIbView))) {
		assert(0);
		return false;
	}

	//�萔�o�b�t�@�[�r���[�쐬
	if (FAILED(CreateConstBuffer(_sVertices[0].id, _sConstBuff, _sWorldMat))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateConstBuffer(_fVertices[0].id, _fConstBuff, _fWorldMat))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateConstBuffer(_tVertices[0].id, _tConstBuff, _tWorldMat))) {
		assert(0);
		return false;
	}

	//�e�N�X�`���ƒ萔�p�f�B�X�N���v�^�q�[�v�쐬
	if (FAILED(CreateBasicDescHeap())) {
		assert(0);
		return false;
	}

	//���[�g�V�O�l�`���쐬
	if (FAILED(CreateRootSignature())) {
		assert(0);
		return false;
	}

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�쐬
	if (FAILED(CreateGraphicsPipelineState())) {
		assert(0);
		return false;
	}

	return true;
}

void Application::Run() {
	ShowWindow(_hwnd, SW_SHOW);//�E�B���h�E�\��

	MSG msg = {};

	//�e�}�`������
	_sphere.Begin(_sWorldMat);
	_sMapMatrix->world = _sWorldMat;
	_face.Begin(_fWorldMat, _fVertices);
	_fMapMatrix->world = _fWorldMat;
	_tWorldMat *= XMMatrixTranslation(12.0f, 8.0f, 0.0f);
	_tMapMatrix->world = _tWorldMat;

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//�A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT) {
			break;
		}

		//DeltaTime�擾
		QueryPerformanceCounter(&_currentTime);
		float deltaTime = static_cast<float>(_currentTime.QuadPart - _prevTime.QuadPart) / _frequency.QuadPart;
		_prevTime = _currentTime;//���݂̎��Ԃ����̃t���[���̂��߂ɕۑ�

		//�A�j���[�V�������͏���
		AnimInput();

		//�A�j���[�V����
		if (_isAnim) {
			//���͏���
			Input();
			_face.Update(deltaTime, _state, _fWorldMat, _fVertices);
			_fMapMatrix->world = _fWorldMat;

			_sphere.Update(deltaTime, _sWorldMat, _fVertices);
			_sMapMatrix->world = _sWorldMat;
		}

		//DirectX����
		//�o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIdx = _swapChain->GetCurrentBackBufferIndex();

		//���\�[�X�o���A�̐ݒ�
		auto rBarriers = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmdList->ResourceBarrier(1, &rBarriers);

		//�����_�[�^�[�Q�b�g���w��
		auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
		_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);//�N���A

		//��ʃN���A
		float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		//�f�B�X�N���v�^�q�[�v��ݒ�
		ID3D12DescriptorHeap* bdh[] = { _basicDescHeap.Get() };
		_cmdList->SetDescriptorHeaps(1, bdh);

		//�`�施��
		_cmdList->SetPipelineState(_pipelinestate.Get());
		_cmdList->SetGraphicsRootSignature(_rootsignature.Get());
		_cmdList->RSSetViewports(1, &_viewport);
		_cmdList->RSSetScissorRects(1, &_scissorrect);
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�e�N�X�`���̕`��
		auto heapHandle = _basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->IASetVertexBuffers(0, 1, &_tVbView);
		_cmdList->IASetIndexBuffer(&_tIbView);
		_cmdList->DrawIndexedInstanced(tIndicesNum, 1, 0, 0, 0);

		//���̕`��
		heapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->IASetVertexBuffers(0, 1, &_sVbView);
		_cmdList->IASetIndexBuffer(&_sIbView);
		_cmdList->DrawIndexedInstanced(sIndicesNum, 1, 0, 0, 0);

		//�ʂ̕`��
		heapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->IASetVertexBuffers(0, 1, &_fVbView);
		_cmdList->IASetIndexBuffer(&_fIbView);
		_cmdList->DrawIndexedInstanced(fIndicesNum, 1, 0, 0, 0);

		//���\�[�X�o���A�̑O�ゾ������ւ���
		rBarriers = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		_cmdList->ResourceBarrier(1, &rBarriers);

		//���߂̃N���[�Y
		_cmdList->Close();

		//�R�}���h���X�g�̎��s
		ID3D12CommandList* cmdlists[] = { _cmdList.Get() };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		//�҂�
		_cmdQueue->Signal(_fence.Get(), ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		_cmdAllocator->Reset();//�L���[���N���A
		_cmdList->Reset(_cmdAllocator.Get(), nullptr);//�ĂуR�}���h���X�g�����߂鏀��

		//�t���b�v
		_swapChain->Present(1, 0);
	}
}

void Application::Terminate() {
	//�N���X�̓o�^����
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}

void Application::CreateGameWindow() {
	HINSTANCE hInst = GetModuleHandle(nullptr);
	//�E�B���h�E�N���X�쐬���o�^
	_windowClass.cbSize = sizeof(WNDCLASSEX);
	_windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��
	_windowClass.lpszClassName = _T("DirectXTest");//�A�v���P�[�V�����N���X��
	_windowClass.hInstance = GetModuleHandle(0);//�n���h���̎擾
	RegisterClassEx(&_windowClass);//�A�v���P�[�V�����N���X

	RECT wrc = { 0,0, window_width, window_height };//�E�B���h�E�T�C�Y�����߂�
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//�E�B���h�E�̃T�C�Y��␳����

	//�E�B���h�E�I�u�W�F�N�g�̍쐬
	_hwnd = CreateWindow(_windowClass.lpszClassName,//�N���X���w��
		_T("DX12���t�@�N�^�����O"),//�^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,//�\��X���W
		CW_USEDEFAULT,//�\��Y���W
		wrc.right - wrc.left,//�E�B���h�E��
		wrc.bottom - wrc.top,//�E�B���h�E��
		nullptr,//�e�E�B���h�E�n���h��
		nullptr,//���j���[�n���h��
		_windowClass.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
		nullptr);//�ǉ��p�����[�^
}

HRESULT Application::InitializeDXGIDevice() {
	UINT flagsDXGI = 0;
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
	//DXGI�C���^�[�t�F�C�X�쐬
	auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�t�B�[�`�����x����
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	result = S_FALSE;
	//�f�o�C�X�쐬
	D3D_FEATURE_LEVEL featureLevel;
	for (auto l : levels) {
		if (SUCCEEDED(D3D12CreateDevice(nullptr, l, IID_PPV_ARGS(_dev.ReleaseAndGetAddressOf())))) {
			featureLevel = l;
			result = S_OK;
			break;
		}
	}
	return result;
}

HRESULT Application::InitializeCommand() {
	//�R�}���h�A���P�[�^�쐬
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�R�}���h���X�g�쐬
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�R�}���h�L���[�쐬
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//�^�C���A�E�g�Ȃ�
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//�v���C�I���e�B���Ɏw��Ȃ�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;//�����̓R�}���h���X�g�ƍ��킹�Ă�������
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));//�R�}���h�L���[����
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::CreateSwapChain() {
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//�X���b�v�`�F�C���쐬
	auto result = _dxgiFactory->CreateSwapChainForHwnd(_cmdQueue.Get(),
		_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapChain.ReleaseAndGetAddressOf());

	return result;
}

HRESULT	Application::CreateRenderTargetView() {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;//�\���̂Q��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//���Ɏw��Ȃ�

	//�f�B�X�N���v�^�q�[�v�쐬
	auto result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�X���b�v�`�F�C���ƂЂ��t��
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapChain->GetDesc(&swcDesc);
	if (FAILED(result)) { return result; }
	_backBuffers.resize(swcDesc.BufferCount);

	//�����_�[�^�[�Q�b�g�r���[�쐬
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	//SRGB�����_�[�^�[�Q�b�g�r���[�ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < swcDesc.BufferCount; ++i) {
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		if (FAILED(result)) { return result; }
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//�r���[�|�[�g�ƃV�U�[��`�̏�����
	_viewport = CD3DX12_VIEWPORT(_backBuffers[0]);
	_scissorrect = CD3DX12_RECT(0, 0, window_width, window_height);

	return S_OK;
}

HRESULT Application::CreateVertexAndIndexBufferView(
	figure_id id,
	ComPtr<ID3D12Resource>& vertBuff,
	D3D12_VERTEX_BUFFER_VIEW& vbView,
	ComPtr<ID3D12Resource>& idxBuff,
	D3D12_INDEX_BUFFER_VIEW& ibView
) {
	UINT64 vSize = 0;//�S�o�C�g��
	UINT64 vStride = 0;//1���_������̃o�C�g��
	if (id == sphere_id) {
		vSize = sizeof(_sVertices);
		vStride = sizeof(_sVertices[0]);
	}
	else if (id == face_id){
		vSize = sizeof(_fVertices);
		vStride = sizeof(_fVertices[0]);
	}
	else {
		vSize = sizeof(_tVertices);
		vStride = sizeof(_tVertices[0]);
	}

	//���_�o�b�t�@�[�쐬
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//���_���̃R�s�[
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (FAILED(result)) { return result; }
	if (id == sphere_id) {
		copy(begin(_sVertices), end(_sVertices), vertMap);
	}
	else if (id == face_id) {
		copy(begin(_fVertices), end(_fVertices), vertMap);
	}
	else {
		copy(begin(_tVertices), end(_tVertices), vertMap);
	}
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�[�r���[�쐬
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();//�o�b�t�@�̉��z�A�h���X
	vbView.SizeInBytes = vSize;
	vbView.StrideInBytes = vStride;

	UINT64 iSize = 0;//�S�o�C�g��
	if (id == sphere_id) {
		iSize = sizeof(_sIndices);
	}
	else if (id == face_id) {
		iSize = sizeof(_fIndices);
	}
	else {
		iSize = sizeof(_tIndices);
	}

	//�C���f�b�N�X�o�b�t�@�[�쐬
	resdesc.Width = iSize;
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(idxBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�C���f�b�N�X���̃R�s�[
	unsigned short* mappedIdx = nullptr;
	result = idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	if (FAILED(result)) { return result; }
	if (id == sphere_id) {
		copy(begin(_sIndices), end(_sIndices), mappedIdx);
	}
	else if (id == face_id) {
		copy(begin(_fIndices), end(_fIndices), mappedIdx);
	}
	else {
		copy(begin(_tIndices), end(_tIndices), mappedIdx);
	}
	idxBuff->Unmap(0, nullptr);

	//�C���f�b�N�X�o�b�t�@�r���[�쐬
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = iSize;

	return S_OK;
}

HRESULT Application::LoadShaderFile() {
	ComPtr<ID3DBlob> errorBlob = nullptr;

	//���_�V�F�[�_�[�t�@�C���ǂݍ���
	auto result = D3DCompileFromFile(L"BasicVertexShader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &_vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}
	//�s�N�Z���V�F�[�_�[�t�@�C���ǂݍ���
	result = D3DCompileFromFile(L"BasicPixelShader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &_psBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else {
			string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		return result;
	}

	return S_OK;
}

HRESULT Application::CreateGraphicsPipelineState() {
	//���_���C�A�E�g�쐬
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",1,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	//�V�F�[�_�[
	gpipeline.pRootSignature = _rootsignature.Get();
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob.Get());

	//�T���v���}�X�N
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//���g��0xffffffff

	//�u�����h�X�e�[�g
	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	//���X�^���C�Y�X�e�[�g
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�

	//�[�x�X�e���V��
	gpipeline.DepthStencilState.DepthEnable = true;//�[�x
	gpipeline.DepthStencilState.StencilEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	//���̓��C�A�E�g
	gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�擪�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//�X�g���b�v���̃J�b�g�Ȃ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��

	//�����_�[�^�[�Q�b�g
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`1�ɐ��K�����ꂽRGBA

	//�A���`�G�C���A�V���O
	gpipeline.SampleDesc.Count = 1;//�T���v�����O��1�s�N�Z���ɂ��P
	gpipeline.SampleDesc.Quality = 0;//�N�I���e�B�͍Œ�

	//�O���t�B�b�N�X�p�C�v���C���X�e�[�g�쐬
	auto result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(_pipelinestate.ReleaseAndGetAddressOf()));

	return result;
}

HRESULT Application::CreateRootSignature() {
	//�����W
	CD3DX12_DESCRIPTOR_RANGE descTblRange[2] = {};//�e�N�X�`���ƒ萔�̂Q��
	descTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//�e�N�X�`��
	descTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//�萔

	//���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootparam = {};
	rootparam.InitAsDescriptorTable(2, descTblRange);//�f�B�X�N���v�^�e�[�u��

	//�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Init(0);

	//���[�g�V�O�l�`���f�B�X�N�ݒ�
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(1, &rootparam, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//�o�C�i���R�[�h�쐬
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	if (FAILED(result)) { return result; }

	//���[�g�V�O�l�`���쐬
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::LoadTextureFile() {
	//WIC�e�N�X�`���̃��[�h
	ScratchImage scratchImg = {};
	auto result = LoadFromWICFile(L"img/instruction.png", WIC_FLAGS_NONE, &_metadata, scratchImg);
	if (FAILED(result)) { return result; }
	auto img = scratchImg.GetImage(0, 0, 0);//���f�[�^���o

	//WriteToSubresource�œ]������p�̃q�[�v�ݒ�
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto resdesc = CD3DX12_RESOURCE_DESC::Tex2D(_metadata.format, _metadata.width, _metadata.height, _metadata.arraySize, _metadata.mipLevels);

	//�e�N�X�`���o�b�t�@�[�쐬
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,//���Ɏw��Ȃ�
		&resdesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//�e�N�X�`���p
		nullptr,
		IID_PPV_ARGS(_texBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) { return result; }

	result = _texBuff->WriteToSubresource(0,
		nullptr,//�S�̈�փR�s�[
		img->pixels,//���f�[�^�A�h���X
		img->rowPitch,//1���C���T�C�Y
		img->slicePitch//�S�T�C�Y
	);
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::CreateConstBuffer(
	figure_id id,
	ComPtr<ID3D12Resource>& constBuff,
	XMMATRIX& worldMat
) {
	//�萔�o�b�t�@�[�쐬
	worldMat = XMMatrixIdentity();
	XMFLOAT3 eye(0, 0, -10);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	XMMATRIX viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	XMMATRIX projMat = XMMatrixPerspectiveFovLH(XM_PIDIV2,
		static_cast<float>(window_width) / static_cast<float>(window_height),//�A�X��
		1.0f,//�߂���
		100.0f//������
	);

	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(MatricesData) + 0xff) & ~0xff);
	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) { return result; }

	//�萔���̃R�s�[
	if (id == sphere_id) {
		result = constBuff->Map(0, nullptr, (void**)&_sMapMatrix);//�}�b�v
		_sMapMatrix->world = worldMat;
		_sMapMatrix->viewproj = viewMat * projMat;
	}
	else if (id == face_id) {
		result = constBuff->Map(0, nullptr, (void**)&_fMapMatrix);//�}�b�v
		_fMapMatrix->world = worldMat;
		_fMapMatrix->viewproj = viewMat * projMat;
	}
	else {
		result = constBuff->Map(0, nullptr, (void**)&_tMapMatrix);//�}�b�v
		_tMapMatrix->world = worldMat;
		_tMapMatrix->viewproj = viewMat * projMat;
	}
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::CreateBasicDescHeap() {
	//�e�N�X�`���ƒ萔�p�f�B�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	descHeapDesc.NodeMask = 0;//�}�X�N��0
	descHeapDesc.NumDescriptors = 4;//�e�N�X�`��1�ƒ萔3��
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//�f�B�X�N���v�^�q�[�v���
	auto result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�萔�o�b�t�@�r���[�쐬
	//�f�B�X�N���v�^�̐擪�n���h�����擾
	auto basicHeapHandle = _basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	//�ʏ�e�N�X�`���r���[�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = _metadata.format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBA(0.0f�`1.0f�ɐ��K��)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;//�~�b�v�}�b�v�͎g�p���Ȃ��̂�1
	_dev->CreateShaderResourceView(_texBuff.Get(),//�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //��قǐݒ肵���e�N�X�`���ݒ���
		basicHeapHandle
	);

	//���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//�e�N�X�`���̒萔�o�b�t�@�[�r���[�쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC tCbvDesc = {};
	tCbvDesc.BufferLocation = _tConstBuff->GetGPUVirtualAddress();
	tCbvDesc.SizeInBytes = _tConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&tCbvDesc, basicHeapHandle);

	//���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//���̒萔�o�b�t�@�[�r���[�쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC sCbvDesc = {};
	sCbvDesc.BufferLocation = _sConstBuff->GetGPUVirtualAddress();
	sCbvDesc.SizeInBytes = _sConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&sCbvDesc, basicHeapHandle);

	//���̏ꏊ�Ɉړ�
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//�ʂ̒萔�o�b�t�@�[�r���[�쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC fCbvDesc = {};
	fCbvDesc.BufferLocation = _fConstBuff->GetGPUVirtualAddress();
	fCbvDesc.SizeInBytes = _fConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&fCbvDesc, basicHeapHandle);

	return S_OK;
}

HRESULT Application::CreateDepthStencilView() {
	//�[�x�o�b�t�@�[�쐬
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resdesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
		window_width, window_height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	CD3DX12_CLEAR_VALUE clearvalue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);//�N���A�o�����[
	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�f�v�X�������݂Ɏg�p
		&clearvalue,
		IID_PPV_ARGS(_depthBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�f�B�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//�[�x�o�b�t�@�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�f�v�X�l��32bit�g�p
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;//�t���O�͓��ɂȂ�
	_dev->CreateDepthStencilView(_depthBuff.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

void Application::InitiallizeVertexData() {
	//��������
	_sphere = Sphere::Instance();
	_sphere.Init(_sVertices, _sIndices);
	for (int i = 0; i < sVertNum; i++) {
		_sVertices[i].uv = { 0.0f, 0.0f };
		_sVertices[i].id = sphere_id;
	}

	//�ʏ�����
	_face = Face::Instance();
	_face.Init(_fVertices, _fIndices);
	for (int i = 0; i < fVertNum; i++) {
		_fVertices[i].uv = { 0.0f, 0.0f };
		_fVertices[i].id = face_id;
	}

	//�e�N�X�`��������
	//���_���W�̏�����
	_tVertices[0] = { { -4.0f, -2.0f, 0.0f },{ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f}, texture_id };//����
	_tVertices[1] = { { -4.0f,  2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f}, texture_id };//����
	_tVertices[2] = { {  4.0f, -2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f}, texture_id };//�E��
	_tVertices[3] = { {  4.0f,  2.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f}, texture_id };//�E��

	//�C���f�b�N�X���W�̏�����
	_tIndices[0] = 0; _tIndices[1] = 1; _tIndices[2] = 2;
	_tIndices[3] = 2; _tIndices[4] = 1; _tIndices[5] = 3;
}

void Application::Input() {
	//���͏���
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		_state = move_up;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		_state = move_down;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		_state = move_right;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		_state = move_left;
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		_state = rotateX_positive;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		_state = rotateX_negative;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		_state = rotateZ_positive;
	}
	else if (GetAsyncKeyState('A') & 0x8000) {
		_state = rotateZ_negative;
	}
	else {
		_state = invalid;
	}
}

void Application::AnimInput() {
	if (GetAsyncKeyState('X') & 0x8000) {
		//�e�}�`������
		_sphere.Begin(_sWorldMat);
		_sMapMatrix->world = _sWorldMat;
		_face.Begin(_fWorldMat, _fVertices);
		_fMapMatrix->world = _fWorldMat;

		//�A�j���[�V�����t���O��������
		_isAnim = false;
	}
	else if (GetAsyncKeyState('C') & 0x8000) {
		//�A�j���[�V�����t���O���グ��
		_isAnim = true;
	}
}

Application::Application() {

}

Application::~Application() {

}