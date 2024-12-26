#include "Application.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

//ウィンドウプロシージャ関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_DESTROY) {//ウィンドウが破棄されたら呼ばれる
		PostQuitMessage(0);//OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//規定の処理を行う
}

//デバッグレイヤーを有効にする
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
	//デバッグレイヤーをオンに
	EnableDebugLayer();
#endif
	//DirectX12関連初期化
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

	//深度バッファー作成
	if (FAILED(CreateDepthStencilView())) {
		assert(0);
		return false;
	}

	//フェンス作成
	if (FAILED(_dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(_fence.ReleaseAndGetAddressOf())))) {
		assert(0);
		return false;
	}

	//タイマーの初期化
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&prevTime);

	//頂点データ初期化
	InitiallizeVertexData();

	//シェーダーファイル読み込み
	if (FAILED(LoadShaderFile())) {
		assert(0);
		return false;
	}

	//テクスチャファイル読み込み
	if (FAILED(LoadTextureFile())) {
		assert(0);
		return false;
	}

	//頂点とインデックスのバッファービュー作成
	if (FAILED(CreateVertexAndIndexBufferView(0, _sVertBuff, _sVbView, _sIdxBuff, _sIbView))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateVertexAndIndexBufferView(1, _fVertBuff, _fVbView, _fIdxBuff, _fIbView))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateVertexAndIndexBufferView(2, _tVertBuff, _tVbView, _tIdxBuff, _tIbView))) {
		assert(0);
		return false;
	}

	//定数バッファービュー作成
	if (FAILED(CreateConstBuffer(_sConstBuff, _sWorldMat))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateConstBuffer(_fConstBuff, _fWorldMat))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateConstBuffer(_tConstBuff, _tWorldMat))) {
		assert(0);
		return false;
	}

	//テクスチャと定数用ディスクリプタヒープ作成
	if (FAILED(CreateBasicDescHeap())) {
		assert(0);
		return false;
	}

	//ルートシグネチャ作成
	if (FAILED(CreateRootSignature())) {
		assert(0);
		return false;
	}

	//グラフィックスパイプラインステート作成
	if (FAILED(CreateGraphicsPipelineState(true, _tPipelinestate))) {
		assert(0);
		return false;
	}
	if (FAILED(CreateGraphicsPipelineState(false, _zPipelinestate))) {
		assert(0);
		return false;
	}

	return true;
}

void Application::Run() {
	ShowWindow(_hwnd, SW_SHOW);//ウィンドウ表示

	MSG msg = {};

	//各図形初期化
	sphere.Begin(_sWorldMat);
	*_sMapMatrix = _sWorldMat * _viewMat * _projMat;
	face.Begin(_fWorldMat, _fVertices);
	*_fMapMatrix = _fWorldMat * _viewMat * _projMat;
	_tWorldMat *= XMMatrixTranslation(12.0f, 8.0f, 0.0f);
	*_tMapMatrix = _tWorldMat * _viewMat * _projMat;

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//アプリケーションが終わる時にmessageがWM_QUITになる
		if (msg.message == WM_QUIT) {
			break;
		}

		//DeltaTime取得
		QueryPerformanceCounter(&currentTime);
		float deltaTime = static_cast<float>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
		prevTime = currentTime;//現在の時間を次のフレームのために保存

		//アニメーション入力処理
		AnimInput();

		//アニメーション
		if (isAnim) {
			//入力処理
			Input();
			face.Update(deltaTime, state, _fWorldMat, _fVertices);
			*_fMapMatrix = _fWorldMat * _viewMat * _projMat;

			sphere.Update(deltaTime, _sWorldMat, _fVertices);
			*_sMapMatrix = _sWorldMat * _viewMat * _projMat;
		}

		//DirectX処理
		//バックバッファのインデックスを取得
		auto bbIdx = _swapChain->GetCurrentBackBufferIndex();

		//リソースバリアの設定
		auto rBarriers = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		_cmdList->ResourceBarrier(1, &rBarriers);

		//レンダーターゲットを指定
		auto rtvH = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
		_cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);//クリア

		//画面クリア
		float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		//ディスクリプタヒープを設定
		ID3D12DescriptorHeap* bdh[] = { _basicDescHeap.Get() };
		_cmdList->SetDescriptorHeaps(1, bdh);

		//描画命令（テクスチャ）
		_cmdList->SetPipelineState(_tPipelinestate.Get());
		_cmdList->RSSetViewports(1, &_viewport);
		_cmdList->RSSetScissorRects(1, &_scissorrect);
		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_cmdList->SetGraphicsRootSignature(_rootsignature.Get());

		//テクスチャの描画
		auto heapHandle = _basicDescHeap->GetGPUDescriptorHandleForHeapStart();
		_cmdList->IASetVertexBuffers(0, 1, &_tVbView);
		_cmdList->IASetIndexBuffer(&_tIbView);
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->DrawIndexedInstanced(tIndicesNum, 1, 0, 0, 0);

		//描画命令（図形）
		_cmdList->SetPipelineState(_zPipelinestate.Get());
		_cmdList->SetGraphicsRootSignature(_rootsignature.Get());

		//球の描画
		heapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->IASetVertexBuffers(0, 1, &_sVbView);
		_cmdList->IASetIndexBuffer(&_sIbView);
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->DrawIndexedInstanced(sIndicesNum, 1, 0, 0, 0);

		//面の描画
		heapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		_cmdList->IASetVertexBuffers(0, 1, &_fVbView);
		_cmdList->IASetIndexBuffer(&_fIbView);
		_cmdList->SetGraphicsRootDescriptorTable(0, heapHandle);
		_cmdList->DrawIndexedInstanced(fIndicesNum, 1, 0, 0, 0);

		//リソースバリアの前後だけ入れ替える
		rBarriers = CD3DX12_RESOURCE_BARRIER::Transition(_backBuffers[bbIdx],
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		_cmdList->ResourceBarrier(1, &rBarriers);

		//命令のクローズ
		_cmdList->Close();

		//コマンドリストの実行
		ID3D12CommandList* cmdlists[] = { _cmdList.Get() };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		//待ち
		_cmdQueue->Signal(_fence.Get(), ++_fenceVal);
		if (_fence->GetCompletedValue() != _fenceVal) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}

		_cmdAllocator->Reset();//キューをクリア
		_cmdList->Reset(_cmdAllocator.Get(), nullptr);//再びコマンドリストをためる準備

		//フリップ
		_swapChain->Present(1, 0);
	}
}

void Application::Terminate() {
	//クラスの登録解除
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);
}

void Application::Input() {
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		state = move_up;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		state = move_down;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		state = move_right;
	}
	else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		state = move_left;
	}
	else if (GetAsyncKeyState('W') & 0x8000) {
		state = rotateX_positive;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		state = rotateX_negative;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		state = rotateZ_positive;
	}
	else if (GetAsyncKeyState('A') & 0x8000) {
		state = rotateZ_negative;
	}
	else {
		state = invalid;
	}
}

void Application::AnimInput() {
	if (GetAsyncKeyState('X') & 0x8000) {
		//各図形初期化
		sphere.Begin(_sWorldMat);
		*_sMapMatrix = _sWorldMat * _viewMat * _projMat;
		face.Begin(_fWorldMat, _fVertices);
		*_fMapMatrix = _fWorldMat * _viewMat * _projMat;

		//アニメーションフラグを下げる
		isAnim = false;
	}
	else if (GetAsyncKeyState('C') & 0x8000) {
		//アニメーションフラグを上げる
		isAnim = true;
	}
}

void Application::InitiallizeVertexData() {
	//球初期化
	sphere = Sphere::Instance();
	sphere.Init(_sVertices, _sIndices);
	for (int i = 0; i < sVertNum; i++) {
		_sVertices[i].uv = { 0.0f, 0.0f };
		_sVertices[i].id = 0;
	}

	//面初期化
	face = Face::Instance();
	face.Init(_fVertices, _fIndices);
	for (int i = 0; i < fVertNum; i++) {
		_fVertices[i].uv = { 0.0f, 0.0f };
		_fVertices[i].id = 1;
	}

	//テクスチャ初期化
	//頂点座標の初期化
	_tVertices[0] = { { -4.0f, -2.0f, 0.0f }, { 0.0f, 1.0f}, 2 };
	_tVertices[1] = { { -4.0f,  2.0f, 0.0f }, { 0.0f, 0.0f}, 2 };
	_tVertices[2] = { {  4.0f, -2.0f, 0.0f }, { 1.0f, 1.0f}, 2 };
	_tVertices[3] = { {  4.0f,  2.0f, 0.0f }, { 1.0f, 0.0f}, 2 };

	//インデックス座標の初期化
	_tIndices[0] = 0; _tIndices[1] = 1; _tIndices[2] = 2;
	_tIndices[3] = 2; _tIndices[4] = 1; _tIndices[5] = 3;
}

void Application::CreateGameWindow() {
	HINSTANCE hInst = GetModuleHandle(nullptr);
	//ウィンドウクラス作成＆登録
	_windowClass.cbSize = sizeof(WNDCLASSEX);
	_windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
	_windowClass.lpszClassName = _T("DirectXTest");//アプリケーションクラス名
	_windowClass.hInstance = GetModuleHandle(0);//ハンドルの取得
	RegisterClassEx(&_windowClass);//アプリケーションクラス

	RECT wrc = { 0,0, window_width, window_height };//ウィンドウサイズを決める
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);//ウィンドウのサイズを補正する

	//ウィンドウオブジェクトの作成
	_hwnd = CreateWindow(_windowClass.lpszClassName,//クラス名指定
		_T("DX12リファクタリング"),//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,//表示X座標
		CW_USEDEFAULT,//表示Y座標
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		_windowClass.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//追加パラメータ
}

HRESULT Application::InitializeDXGIDevice() {
	UINT flagsDXGI = 0;
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
	//DXGIインターフェイス作成
	auto result = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//フィーチャレベル列挙
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	result = S_FALSE;
	//デバイス作成
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
	//コマンドアロケータ作成
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//コマンドリスト作成
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator.Get(), nullptr, IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//コマンドキュー作成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;//タイムアウトなし
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;//プライオリティ特に指定なし
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;//ここはコマンドリストと合わせてください
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));//コマンドキュー生成
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

	//スワップチェイン作成
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
	heapDesc.NumDescriptors = 2;//表裏の２つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;//特に指定なし

	//ディスクリプタヒープ作成
	auto result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_rtvHeaps.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//スワップチェインとひも付け
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapChain->GetDesc(&swcDesc);
	if (FAILED(result)) { return result; }
	_backBuffers.resize(swcDesc.BufferCount);

	//レンダーターゲットビュー作成
	D3D12_CPU_DESCRIPTOR_HANDLE handle = _rtvHeaps->GetCPUDescriptorHandleForHeapStart();

	//SRGBレンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < swcDesc.BufferCount; ++i) {
		result = _swapChain->GetBuffer(i, IID_PPV_ARGS(&_backBuffers[i]));
		if (FAILED(result)) { return result; }
		_dev->CreateRenderTargetView(_backBuffers[i], &rtvDesc, handle);
		handle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	//ビューポートとシザー矩形の初期化
	_viewport = CD3DX12_VIEWPORT(_backBuffers[0]);
	_scissorrect = CD3DX12_RECT(0, 0, window_width, window_height);

	return S_OK;
}

HRESULT Application::CreateVertexAndIndexBufferView(
	int id,
	ComPtr<ID3D12Resource>& vertBuff,
	D3D12_VERTEX_BUFFER_VIEW& vbView,
	ComPtr<ID3D12Resource>& idxBuff,
	D3D12_INDEX_BUFFER_VIEW& ibView) {

	UINT64 vSize = 0;//全バイト数
	UINT64 vStride = 0;//1頂点あたりのバイト数
	if (id == 0) {
		vSize = sizeof(_sVertices);
		vStride = sizeof(_sVertices[0]);
	}
	else if (id == 1){
		vSize = sizeof(_fVertices);
		vStride = sizeof(_fVertices[0]);
	}
	else {
		vSize = sizeof(_tVertices);
		vStride = sizeof(_tVertices[0]);
	}

	//頂点バッファー作成
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

	//頂点情報のコピー
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	if (FAILED(result)) { return result; }
	if (id == 0) {
		copy(begin(_sVertices), end(_sVertices), vertMap);
	}
	else if (id == 1) {
		copy(begin(_fVertices), end(_fVertices), vertMap);
	}
	else {
		copy(begin(_tVertices), end(_tVertices), vertMap);
	}
	vertBuff->Unmap(0, nullptr);

	//頂点バッファービュー作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();//バッファの仮想アドレス
	vbView.SizeInBytes = vSize;
	vbView.StrideInBytes = vStride;

	UINT64 iSize = 0;//全バイト数
	if (id == 0) {
		iSize = sizeof(_sIndices);
	}
	else if (id == 1) {
		iSize = sizeof(_fIndices);
	}
	else {
		iSize = sizeof(_tIndices);
	}

	//インデックスバッファー作成
	resdesc.Width = iSize;
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(idxBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//インデックス情報のコピー
	unsigned short* mappedIdx = nullptr;
	result = idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	if (FAILED(result)) { return result; }
	if (id == 0) {
		copy(begin(_sIndices), end(_sIndices), mappedIdx);
	}
	else if (id == 1) {
		copy(begin(_fIndices), end(_fIndices), mappedIdx);
	}
	else {
		copy(begin(_tIndices), end(_tIndices), mappedIdx);
	}
	idxBuff->Unmap(0, nullptr);

	//インデックスバッファビュー作成
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = iSize;

	return S_OK;
}

HRESULT Application::LoadShaderFile() {
	ComPtr<ID3DBlob> errorBlob = nullptr;

	//頂点シェーダーファイル読み込み
	auto result = D3DCompileFromFile(L"BasicVertexShader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &_vsBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
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
	//ピクセルシェーダーファイル読み込み
	result = D3DCompileFromFile(L"BasicPixelShader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"BasicPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0, &_psBlob, &errorBlob);
	if (FAILED(result)) {
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
			::OutputDebugStringA("ファイルが見当たりません");
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

HRESULT Application::CreateGraphicsPipelineState(bool isTex, ComPtr<ID3D12PipelineState>& pipelinestate) {
	//頂点レイアウト作成
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",1,DXGI_FORMAT_R32_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	//グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	//シェーダー
	gpipeline.pRootSignature = _rootsignature.Get();
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(_vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(_psBlob.Get());

	//サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;//中身は0xffffffff

	//ブレンドステート
	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	//ラスタライズステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	if (!isTex) {
		gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;//枠線のみ
	}
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//カリングしない

	//深度ステンシル
	gpipeline.DepthStencilState.DepthEnable = true;//深度
	gpipeline.DepthStencilState.StencilEnable = false;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	//入力レイアウト
	gpipeline.InputLayout.pInputElementDescs = inputLayout;//レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout);//レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;//ストリップ時のカットなし
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//三角形で構成

	//レンダーターゲット
	gpipeline.NumRenderTargets = 1;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0～1に正規化されたRGBA

	//アンチエイリアシング
	gpipeline.SampleDesc.Count = 1;//サンプリングは1ピクセルにつき１
	gpipeline.SampleDesc.Quality = 0;//クオリティは最低

	//グラフィックスパイプラインステート作成
	auto result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate.ReleaseAndGetAddressOf()));

	return result;
}

HRESULT Application::CreateRootSignature() {
	//レンジ
	CD3DX12_DESCRIPTOR_RANGE descTblRange[2] = {};//テクスチャと定数の２つ
	descTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//テクスチャ
	descTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//定数

	//ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparam = {};
	rootparam.InitAsDescriptorTable(2, descTblRange);//ディスクリプタテーブル

	//サンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Init(0);

	//ルートシグネチャディスク設定
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(1, &rootparam, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//バイナリコード作成
	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	if (FAILED(result)) { return result; }

	//ルートシグネチャ作成
	result = _dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(_rootsignature.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::LoadTextureFile() {
	//WICテクスチャのロード
	ScratchImage scratchImg = {};
	auto result = LoadFromWICFile(L"img/instruction.png", WIC_FLAGS_NONE, &_metadata, scratchImg);
	if (FAILED(result)) { return result; }
	auto img = scratchImg.GetImage(0, 0, 0);//生データ抽出

	//WriteToSubresourceで転送する用のヒープ設定
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	auto resdesc = CD3DX12_RESOURCE_DESC::Tex2D(_metadata.format, _metadata.width, _metadata.height, _metadata.arraySize, _metadata.mipLevels);

	//テクスチャバッファー作成
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,//特に指定なし
		&resdesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,//テクスチャ用
		nullptr,
		IID_PPV_ARGS(_texBuff.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) { return result; }

	result = _texBuff->WriteToSubresource(0,
		nullptr,//全領域へコピー
		img->pixels,//元データアドレス
		img->rowPitch,//1ラインサイズ
		img->slicePitch//全サイズ
	);
	if (FAILED(result)) { return result; }

	return S_OK;
}

HRESULT Application::CreateConstBuffer(
	ComPtr<ID3D12Resource>& constBuff,
	XMMATRIX& worldMat) {

	//定数バッファー作成
	worldMat = XMMatrixIdentity();
	XMFLOAT3 eye(0, 0, -10);
	XMFLOAT3 target(0, 0, 0);
	XMFLOAT3 up(0, 1, 0);
	_viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	_projMat = XMMatrixPerspectiveFovLH(XM_PIDIV2,
		static_cast<float>(window_width) / static_cast<float>(window_height),//アス比
		1.0f,//近い方
		100.0f//遠い方
	);

	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(XMMATRIX) + 0xff) & ~0xff);
	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff.ReleaseAndGetAddressOf())
	);
	return result;
}

HRESULT Application::CreateBasicDescHeap() {
	//定数情報のコピー
	//球
	auto result = _sConstBuff->Map(0, nullptr, (void**)&_sMapMatrix);//マップ
	if (FAILED(result)) { return result; }
	*_sMapMatrix = _sWorldMat * _viewMat * _projMat;
	//面
	result = _fConstBuff->Map(0, nullptr, (void**)&_fMapMatrix);//マップ
	if (FAILED(result)) { return result; }
	*_fMapMatrix = _fWorldMat * _viewMat * _projMat;
	//テクスチャ
	result = _tConstBuff->Map(0, nullptr, (void**)&_tMapMatrix);//マップ
	if (FAILED(result)) { return result; }
	*_tMapMatrix = _tWorldMat * _viewMat * _projMat;
	
	//テクスチャと定数用ディスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見えるように
	descHeapDesc.NodeMask = 0;//マスクは0
	descHeapDesc.NumDescriptors = 4;//テクスチャ1つと定数3つ
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;//ディスクリプタヒープ種別
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(_basicDescHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//定数バッファビュー作成
	//ディスクリプタの先頭ハンドルを取得
	auto basicHeapHandle = _basicDescHeap->GetCPUDescriptorHandleForHeapStart();
	//通常テクスチャビュー作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = _metadata.format;//DXGI_FORMAT_R8G8B8A8_UNORM;//RGBA(0.0f～1.0fに正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;//ミップマップは使用しないので1
	_dev->CreateShaderResourceView(_texBuff.Get(),//ビューと関連付けるバッファ
		&srvDesc, //先ほど設定したテクスチャ設定情報
		basicHeapHandle
	);

	//次の場所に移動
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//テクスチャの定数バッファービュー作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC tCbvDesc = {};
	tCbvDesc.BufferLocation = _tConstBuff->GetGPUVirtualAddress();
	tCbvDesc.SizeInBytes = _tConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&tCbvDesc, basicHeapHandle);

	//次の場所に移動
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//球の定数バッファービュー作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC sCbvDesc = {};
	sCbvDesc.BufferLocation = _sConstBuff->GetGPUVirtualAddress();
	sCbvDesc.SizeInBytes = _sConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&sCbvDesc, basicHeapHandle);

	//次の場所に移動
	basicHeapHandle.ptr += _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//面の定数バッファービュー作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC fCbvDesc = {};
	fCbvDesc.BufferLocation = _fConstBuff->GetGPUVirtualAddress();
	fCbvDesc.SizeInBytes = _fConstBuff->GetDesc().Width;
	_dev->CreateConstantBufferView(&fCbvDesc, basicHeapHandle);

	return S_OK;
}

HRESULT Application::CreateDepthStencilView() {
	//深度バッファー作成
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resdesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
		window_width, window_height,
		1, 1, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	CD3DX12_CLEAR_VALUE clearvalue(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);//クリアバリュー
	auto result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//デプス書き込みに使用
		&clearvalue,
		IID_PPV_ARGS(_depthBuff.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//ディスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
	dsvHeapDesc.NumDescriptors = 1;//深度ビュー1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(_dsvHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) { return result; }

	//深度バッファビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//デプス値に32bit使用
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;//フラグは特になし
	_dev->CreateDepthStencilView(_depthBuff.Get(), &dsvDesc, _dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

Application::Application() {

}

Application::~Application() {

}