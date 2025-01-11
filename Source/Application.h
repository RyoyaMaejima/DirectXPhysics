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

//ウィンドウ定数
const unsigned int window_width = 1280;
const unsigned int window_height = 720;

class Application
{
public:
	//Applicationのシングルトンインスタンスを得る
	static Application& Instance();

	//初期化
	bool Init();

	//ループ起動
	void Run();

	//後処理
	void Terminate();

private:
	//ウィンドウ関係
	WNDCLASSEX _windowClass = {};
	HWND _hwnd = nullptr;

	//DXGI関係
	ComPtr<IDXGIFactory6> _dxgiFactory = nullptr;//DXGIインターフェイス
	ComPtr<ID3D12Device> _dev = nullptr;//デバイス

	//コマンド関係
	ComPtr<ID3D12CommandAllocator> _cmdAllocator = nullptr;//コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> _cmdList = nullptr;//コマンドリスト
	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;//コマンドキュー

	//スワップチェイン
	ComPtr <IDXGISwapChain4> _swapChain = nullptr;

	//レンダーターゲット関係
	vector<ID3D12Resource*> _backBuffers = {};//バックバッファー
	ComPtr<ID3D12DescriptorHeap> _rtvHeaps = nullptr;//レンダーターゲット用ディスクリプタヒープ

	//フェンス関係
	ComPtr<ID3D12Fence> _fence = nullptr;
	UINT64 _fenceVal = 0;

	//頂点関係
	//球
	Vertex _sVertices[sVertNum] = {};//頂点座標
	ComPtr<ID3D12Resource> _sVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _sVbView = {};//頂点バッファービュー
	//面
	Vertex _fVertices[fVertNum] = {};//頂点座標
	ComPtr<ID3D12Resource> _fVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _fVbView = {};//頂点バッファービュー

	//シェーダーオブジェクト
	ComPtr<ID3DBlob> _vsBlob = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	//パイプラインステート
	ComPtr<ID3D12PipelineState> _tPipelinestate = nullptr;//テクスチャ用
	ComPtr<ID3D12PipelineState> _zPipelinestate = nullptr;//図形用

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> _rootsignature = nullptr;

	//ビューポートとシザー矩形
	CD3DX12_VIEWPORT _viewport;//ビューポート
	CD3DX12_RECT _scissorrect;//シザー矩形

	//インデックス関係
	//球
	unsigned int _sIndices[sIndicesNum] = {};//インデックス座標
	ComPtr<ID3D12Resource> _sIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _sIbView = {};;//インデックスバッファービュー
	//面
	unsigned int _fIndices[fIndicesNum] = {};//インデックス座標
	ComPtr<ID3D12Resource> _fIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _fIbView = {};;//インデックスバッファービュー

	//テクスチャ関係
	Vertex _tVertices[tVertNum] = {};//頂点座標
	ComPtr<ID3D12Resource> _tVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _tVbView = {};//頂点バッファービュー
	unsigned int _tIndices[tIndicesNum] = {};//インデックス座標
	ComPtr<ID3D12Resource> _tIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _tIbView = {};;//インデックスバッファービュー

	TexMetadata _metadata = {};//メタデータ
	ComPtr<ID3D12Resource> _texBuff = nullptr;//テクスチャバッファー

	//定数関係
	//球
	ComPtr<ID3D12Resource> _sConstBuff = nullptr;//定数バッファー
	//面
	ComPtr<ID3D12Resource> _fConstBuff = nullptr;//定数バッファー
	//テクスチャ
	ComPtr<ID3D12Resource> _tConstBuff = nullptr;//定数バッファー

	//テクスチャと定数用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> _basicDescHeap = nullptr;

	//座標変換系行列
	XMMATRIX _viewMat;
	XMMATRIX _projMat;

	//球
	XMMATRIX _sWorldMat;
	XMMATRIX* _sMapMatrix;

	//面
	XMMATRIX _fWorldMat;
	XMMATRIX* _fMapMatrix;

	//テクスチャ
	XMMATRIX _tWorldMat;
	XMMATRIX* _tMapMatrix;

	//深度関係
	ComPtr<ID3D12Resource> _depthBuff = nullptr;//深度バッファー
	ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;//深度バッファ用ディスクリプタヒープ

	//時間計測用
	LARGE_INTEGER frequency;//周波数
	LARGE_INTEGER prevTime;//1つ前の時刻
	LARGE_INTEGER currentTime;//現在の時刻

	//球
	Sphere sphere;

	//面
	Face face;

	//入力変数
	einput_state state;

	//アニメーションするか
	bool isAnim = false;

	//頂点データの初期化
	void InitiallizeVertexData();

	//キー入力受け取り
	void Input();

	//アニメーション入力受け取り
	void AnimInput();

	//ウィンドウ作成
	void CreateGameWindow();

	//DXGI関係初期化
	HRESULT InitializeDXGIDevice();

	//コマンド関係初期化
	HRESULT InitializeCommand();

	//スワップチェイン作成
	HRESULT CreateSwapChain();

	//レンダーターゲットビュー作成
	HRESULT	CreateRenderTargetView();

	//頂点とインデックスのバッファービュー作成
	HRESULT CreateVertexAndIndexBufferView(
		figure_id id,
		ComPtr<ID3D12Resource>& vertBuff,
		D3D12_VERTEX_BUFFER_VIEW& vbView,
		ComPtr<ID3D12Resource>& idxBuff,
		D3D12_INDEX_BUFFER_VIEW& ibView);

	//シェーダーファイル読み込み
	HRESULT LoadShaderFile();

	//グラフィックスパイプラインステート作成
	HRESULT CreateGraphicsPipelineState(bool isTex, ComPtr<ID3D12PipelineState>& pipelinestate);

	//ルートシグネチャ作成
	HRESULT CreateRootSignature();

	//テクスチャファイル読み込み
	HRESULT LoadTextureFile();

	//定数バッファー作成
	HRESULT CreateConstBuffer(
		ComPtr<ID3D12Resource>& constBuff,
		XMMATRIX& worldMat);

	//テクスチャと定数用ディスクリプタヒープ作成
	HRESULT CreateBasicDescHeap();

	//深度バッファービュー作成
	HRESULT CreateDepthStencilView();

	//シングルトンのためにコンストラクタをprivateに
	//さらにコピーと代入を禁止に
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:
	~Application();
};
