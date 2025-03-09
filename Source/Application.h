#pragma once
#include <Windows.h>
#include <tchar.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <wrl.h>

#include "Sphere.h"
#include "Face.h"
#include "PMDActor.h"

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
	ComPtr<ID3D12Resource> _sVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _sVbView = {};//頂点バッファービュー
	//面
	ComPtr<ID3D12Resource> _fVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _fVbView = {};//頂点バッファービュー
	//PMDアクター
	ComPtr<ID3D12Resource> _pVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _pVbView = {};//頂点バッファービュー

	//シェーダーオブジェクト
	ComPtr<ID3DBlob> _vsBlob = nullptr;
	ComPtr<ID3DBlob> _psBlob = nullptr;

	//パイプラインステート
	ComPtr<ID3D12PipelineState> _pipelinestate = nullptr;

	//ルートシグネチャ
	ComPtr<ID3D12RootSignature> _rootsignature = nullptr;

	//ビューポートとシザー矩形
	CD3DX12_VIEWPORT _viewport;//ビューポート
	CD3DX12_RECT _scissorrect;//シザー矩形

	//インデックス関係
	//球
	ComPtr<ID3D12Resource> _sIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _sIbView = {};;//インデックスバッファービュー
	//面
	ComPtr<ID3D12Resource> _fIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _fIbView = {};;//インデックスバッファービュー
	//PMDアクター
	ComPtr<ID3D12Resource> _pIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _pIbView = {};;//インデックスバッファービュー

	//テクスチャ関係
	float _tw = 2.0f;//縦の長さの半分
	XMFLOAT3 _tDefPos = { 12.0f, 8.0f, 0.0f };
	Vertex _tVertData[tVertNum] = {};//頂点座標
	ComPtr<ID3D12Resource> _tVertBuff = nullptr;//頂点バッファー
	D3D12_VERTEX_BUFFER_VIEW _tVbView = {};//頂点バッファービュー
	unsigned int _tIndices[tIndicesNum] = {};//インデックス座標
	ComPtr<ID3D12Resource> _tIdxBuff = nullptr;//インデックスバッファー
	D3D12_INDEX_BUFFER_VIEW _tIbView = {};;//インデックスバッファービュー

	ComPtr<ID3D12Resource> _texBuff = nullptr;//テクスチャバッファー
	vector<ComPtr<ID3D12Resource>> _textureResources;//PMD用テクスチャバッファー
	ComPtr<ID3D12Resource> _whiteBuff = nullptr;//白テクスチャ

	//定数関係
	//球
	ComPtr<ID3D12Resource> _sConstBuff = nullptr;//定数バッファー
	//面
	ComPtr<ID3D12Resource> _fConstBuff = nullptr;//定数バッファー
	//テクスチャ
	ComPtr<ID3D12Resource> _tConstBuff = nullptr;//定数バッファー
	//PMDアクター
	ComPtr<ID3D12Resource> _pConstBuff = nullptr;//定数バッファー

	//座標変換系行列
	struct MatricesData {
		XMMATRIX world;
		XMMATRIX viewproj;
	};
	//球
	MatricesData* _sMapMatrix;
	//面
	MatricesData* _fMapMatrix;
	//テクスチャ
	XMMATRIX _tWorldMat = XMMatrixIdentity();
	MatricesData* _tMapMatrix;
	//PMDアクター
	MatricesData* _pMapMatrix;

	//深度関係
	ComPtr<ID3D12Resource> _depthBuff = nullptr;//深度バッファー
	ComPtr<ID3D12DescriptorHeap> _dsvHeap = nullptr;//深度バッファ用ディスクリプタヒープ

	//マテリアル関係
	ComPtr<ID3D12Resource> _pMaterialBuff = nullptr;//マテリアルバッファー

	//ボーン関係
	ComPtr<ID3D12Resource> _pBoneBuff = nullptr;//ボーンバッファー
	XMMATRIX* _mappedMatrices = nullptr;

	//テクスチャ、定数、マテリアル、ボーン用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> _basicDescHeap = nullptr;

	//時間計測用
	LARGE_INTEGER _frequency;//周波数
	LARGE_INTEGER _prevTime;//1つ前の時刻
	LARGE_INTEGER _currentTime;//現在の時刻

	//球
	Sphere _sphere;

	//面
	Face _face;

	//PMDアクター
	PMDActor _pmdActor;

	//入力変数
	einput_state _state;

	//アニメーションするか
	bool _isAnim = false;

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
		D3D12_INDEX_BUFFER_VIEW& ibView
	);

	//シェーダーファイル読み込み
	HRESULT LoadShaderFile();

	//グラフィックスパイプラインステート作成
	HRESULT CreateGraphicsPipelineState();

	//ルートシグネチャ作成
	HRESULT CreateRootSignature();

	//テクスチャファイル読み込み
	ID3D12Resource* LoadTextureFile(bool isIns, string& texPath);

	//白テクスチャ作成
	ID3D12Resource* CreateWhiteTexture();

	//テクスチャバッファー作成
	HRESULT CreateTextureBuffer();

	//定数バッファー作成
	HRESULT CreateConstBuffer(
		figure_id id,
		ComPtr<ID3D12Resource>& constBuff,
		XMMATRIX& worldMat
	);

	//深度バッファービュー作成
	HRESULT CreateDepthStencilView();

	//マテリアルバッファー作成
	HRESULT CreateMaterialBuffer();

	//ボーンバッファー作成
	HRESULT CreateBoneBuffer();

	//ディスクリプタヒープ作成
	HRESULT CreateBasicDescHeap();

	//頂点データの初期化
	void InitiallizeVertexData();

	//キー入力受け取り
	void Input();

	//アニメーション入力受け取り
	void AnimInput();

	//シングルトンのためにコンストラクタをprivateに
	//さらにコピーと代入を禁止に
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

public:
	~Application();
};