#include "Main.h"
#include <crtdbg.h>	//_ASSERT_EXPR()で必要.

#include "..\DebugMode\Sphere\Sphere.h"
#include "..\.\XAudio2\PlaySEThread.h"
#include "..\XAudio2\SoundManager.h"

#include "..\Resource\TutorialText\TutorialTextLoad.h"

#include "..\Resource\MeshResource\MeshResource.h"
#include "..\Resource\EffectResource\EffectResource.h"
#include "..\Edit\ModelResource\ModelResource.h"
#include "..\UI\LoadImage\LoadImage.h"

#include "..\Command\Command.h"

#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_dx11.h"
#include "..\ImGui\imgui_impl_win32.h"

//#pragma optimize("", off)

// ImGuiで使用.
extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//ｸﾞﾛｰﾊﾞﾙ変数.
CMain*	g_pCMain = nullptr;

//================================================
//	ﾒｲﾝ関数.
//================================================
INT WINAPI WinMain(
	HINSTANCE hInstance,	//ｲﾝｽﾀﾝｽ番号(ｳｨﾝﾄﾞｳの番号).
	HINSTANCE hPrevInstance,
	PSTR lpCmdLine,
	INT nCmdShow)
{
#ifdef _DEBUG
	// メモリリーク検出
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif	// #ifdef _DEBUG.

	g_pCMain = new CMain();	//初期化＆ｸﾗｽ宣言.

	if (g_pCMain != nullptr) {

		//ｳｨﾝﾄﾞｳ作成成功したら.
		if (SUCCEEDED(
			g_pCMain->InitWindow(
				hInstance,
				0, 0,
				WND_W, WND_H,
				WND_TITLE)))
		{
			//Dx11用の初期化.
			if (SUCCEEDED(g_pCMain->Init()))
			{
				//ﾒｯｾｰｼﾞﾙｰﾌﾟ.
				g_pCMain->Loop();
			}
		}
		//終了.
		g_pCMain->Destroy();//Direct3Dの解放.

		SAFE_DELETE(g_pCMain);//ｸﾗｽの破棄.
	}

	return 0;
}

//================================================
//	ｳｨﾝﾄﾞｳﾌﾟﾛｼｰｼﾞｬｰ.
//================================================
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT uMsg,
	WPARAM wPara, LPARAM lParam)
{
	//ﾌﾟﾛｼｰｼﾞｬｰ.
	return g_pCMain->MsgProc(hWnd, uMsg, wPara, lParam);
}

/**********************************************************************
*	メインクラス.
**/
//=================================================
//	コンテキスト.
//=================================================
CMain::CMain()
	//初期化リスト.
	: m_hWnd					( nullptr )
	, m_pDevice11				( nullptr )
	, m_pContext11				( nullptr )
	, m_pSwapChain				( nullptr )
	, m_pBackBuffer_TexRTV		( nullptr )
	, m_pBackBuffer_DSTex		( nullptr )
	, m_pBackBuffer_DSTexDSV	( nullptr )
	, m_pDepthStencilState		( nullptr )
	, m_pDepthStencilStateOff	( nullptr )
	, m_vLight					( 0.0f, 0.5f, -1.0f )	// ライト方向.
	, pSceneManager				( nullptr )
	, m_isLoadCheck				( false )
{
	// サウンド関係初期設定.
	CSoundManager::CreateSoundData();
	CPlaySEThread::SetUpThread();
	pSceneManager = make_shared<CSceneManager>();
	pSceneManager->Push( make_shared<CTitleScene>(pSceneManager));
	m_Light = Light();
}


//=================================================
//	デストラクタ.
//=================================================
CMain::~CMain()
{
}

//ｳｨﾝﾄﾞｳ初期化関数.
HRESULT CMain::InitWindow(
	HINSTANCE hInstance,	//ｲﾝｽﾀﾝｽ.
	INT x, INT y,			//ｳｨﾝﾄﾞｳx,y座標.
	INT width, INT height,	//ｳｨﾝﾄﾞｳ幅,高さ.
	const char* WindowName)	//ｳｨﾝﾄﾞｳ名.
{
	//ｳｨﾝﾄﾞｳの定義.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));//初期化(0を設定).

	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wc.lpszClassName = APP_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	//ｳｨﾝﾄﾞｳｸﾗｽをWindowsに登録.
	if (!RegisterClassEx(&wc)) {
//		MessageBox(nullptr,
//			"ｳｨﾝﾄﾞｳｸﾗｽの登録に失敗", "ｴﾗｰ", MB_OK);
		_ASSERT_EXPR(false, L"ｳｨﾝﾄﾞｳｸﾗｽの登録に失敗");
		return E_FAIL;
	}

	//ｳｨﾝﾄﾞｳの作成.
	m_hWnd = CreateWindow(
		APP_NAME,		//ｱﾌﾟﾘ名.
		WindowName,		//ｳｨﾝﾄﾞｳﾀｲﾄﾙ.
		WS_OVERLAPPEDWINDOW,	//ｳｨﾝﾄﾞｳ種別(普通).
		x, y,			//表示位置x,y座標.
		width, height,	//ｳｨﾝﾄﾞｳ幅,高さ.
		nullptr,		//親ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
		nullptr,		//ﾒﾆｭｰ設定.
		hInstance,		//ｲﾝｽﾀﾝｽ番号.
		nullptr);		//ｳｨﾝﾄﾞｳ作成時に発生するｲﾍﾞﾝﾄに渡すﾃﾞｰﾀ.
	if (!m_hWnd) {
		_ASSERT_EXPR(false, "ｳｨﾝﾄﾞｳ作成失敗");
		return E_FAIL;
	}

	RECT size;
	RECT wndsize;
	GetClientRect( m_hWnd, &size );
	GetWindowRect( m_hWnd, &wndsize );
	wndsize.right = wndsize.right - wndsize.left;
	wndsize.bottom = wndsize.bottom - wndsize.top;
	SetWindowPos(
		m_hWnd,
		nullptr,
		0, 0,
		width + wndsize.right - size.right,
		height + wndsize.bottom - size.bottom,
		SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOZORDER );

	//ｳｨﾝﾄﾞｳの表示.
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return S_OK;
}


//ｳｨﾝﾄﾞｳ関数(ﾒｯｾｰｼﾞ毎の処理).
LRESULT CMain::MsgProc(
	HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	if( ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ) )
		return true;
	IMGUI_API
	switch (uMsg) {
	case WM_DESTROY://ｳｨﾝﾄﾞｳが破棄されたとき.
		//ｱﾌﾟﾘｹｰｼｮﾝの終了をWindowsに通知する.
		PostQuitMessage(0);
		break;
	
	case WM_KEYDOWN://ｷｰﾎﾞｰﾄﾞが押されたとき.
		//ｷｰ別の処理.
		switch (static_cast<char>(wParam)) {
		case VK_ESCAPE:	//ESCｷｰ.
			if (MessageBox(nullptr,
				"ｹﾞｰﾑを終了しますか？", "警告", MB_YESNO) == IDYES)
			{
				//ｳｨﾝﾄﾞｳを破棄する.
				PostMessage( hWnd, WM_CLOSE, wParam, lParam );
			}
			break;
		}
		break;
	case WM_CLOSE:
		//×かEscでアプリウィンドウを閉じたときのためのBGM,SE終了命令.
		CSoundManager::SetEmergencyStopFlag(true);
		//SEスレッドがすべてリリースされるまでwhileループで待つ.
		while (CPlaySEThread::Release() == false) {};
		//サウンドマネージャーリリース.
		CSoundManager::Release();
		//ｳｨﾝﾄﾞｳを破棄する.
		DestroyWindow(hWnd);
		break;
	}

	//ﾒｲﾝに返す情報.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


//ﾒｯｾｰｼﾞﾙｰﾌﾟ.
void CMain::Loop()
{
	//ﾒｯｼｭ読み込み関数.
	LoadMesh();

	//------------------------------------------------
	//	ﾌﾚｰﾑﾚｰﾄ調整準備.
	//------------------------------------------------
	float Rate = 0.0f;	//ﾚｰﾄ.
	float FPS = 30.0f;	//FPS値.
	DWORD sync_old = timeGetTime();	//過去時間.
	DWORD sync_now;					//現在時間.

	//時間処理のため、最小単位を1ﾐﾘ秒に変更.
	timeBeginPeriod(1);
	Rate = 1000.0f / FPS; //理想時間を算出.

	//ﾒｯｾｰｼﾞﾙｰﾌﾟ.
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT)
	{
		Sleep(1);
		sync_now = timeGetTime();	//現在の時間を取得.

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if( sync_now - sync_old >= Rate)
		{
			sync_old = sync_now;	//現在時間に置き換え.

			//ｱﾌﾟﾘｹｰｼｮﾝの処理はここから飛ぶ.
			AppMain();
		}
	}
	//ｱﾌﾟﾘｹｰｼｮﾝの終了.
}

//ｱﾌﾟﾘｹｰｼｮﾝﾒｲﾝ処理.
void CMain::AppMain()
{
	m_isLoadCheck = false;
	if( CMeshResorce::ThreadRelease() == false		||
		CMapLoad ::ThreadRelease() == false			||
		CSpriteResource::ThreadRelease() == false	||
		CEffectResource ::ThreadRelease() == false	||
		CEditModelResource::ThreadRelease() == false ){
		m_isLoadCheck = true;
	}

	// 更新.
	Update();
	// 描画.
	Render();
}

// 更新関数.
void CMain::Update()
{
	if( m_isLoadCheck == true ) return;
	CCommand::Update();
	CXInput::UpdateStatus();
	// 各シーンの更新処理.
	pSceneManager->Load( m_hWnd, m_pDevice11, m_pContext11, m_pDevice9 );
	pSceneManager->Update();
#ifdef _DEBUG
	// カメラ動作.
	CameraControl();
#endif	// #ifdef _DEBUG.
}

// 描画関数.
void CMain::Render()
{
	ClearBackBuffer();
	Camera();
	Proj();

	if( m_isLoadCheck == true ){
		CLoadImage::Render();
		//ﾚﾝﾀﾞﾘﾝｸﾞされたｲﾒｰｼﾞを表示.
		m_pSwapChain->Present( 0, 0 );
		return;
	}

	// 各シーンの描画処理.
	pSceneManager->Render( m_mView, m_mProj, m_Light, m_Camera );
	CSpriteResource::GetSprite("Fade")->Render();
	CFadeUI::Render();
	//ﾚﾝﾀﾞﾘﾝｸﾞされたｲﾒｰｼﾞを表示.
	m_pSwapChain->Present( 0, 0 );
}

HRESULT CMain::Init()
{
	InitDx9();
	InitD3D11();
	//////////////////////////////////////////////
	////ALT + Enterでフルスクリーンを無効化する.
	//IDXGIFactory* pfac = nullptr;
	////上で作ったIDXGISwapChainを使う.
	//m_pSwapChain->GetParent( __uuidof( IDXGIFactory ), (void**)&pfac );

	//// 余計な機能を無効にする設定をする。
	//pfac->MakeWindowAssociation( m_hWnd, DXGI_MWA_NO_WINDOW_CHANGES
	//	| DXGI_MWA_NO_ALT_ENTER );

	////////////////////////////////////////////
	return S_OK;
}

/************************************************************
*	ここから3D.
*/
//DirectX初期化.
HRESULT CMain::InitD3D11()
{
	//---------------------------------------------------------
	//	ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝ関係.
	//---------------------------------------------------------

	//ｽﾜｯﾌﾟﾁｪｰﾝ構造体.
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));//0で初期化.
	sd.BufferCount = 1;	//ﾊﾞｯｸﾊﾞｯﾌｧの数.
	sd.BufferDesc.Width = WND_W;//ﾊﾞｯｸﾊﾞｯﾌｧの幅.
	sd.BufferDesc.Height = WND_H;//ﾊﾞｯｸﾊﾞｯﾌｧの高さ.
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//ﾌｫｰﾏｯﾄ(32ﾋﾞｯﾄｶﾗｰ).
	sd.BufferDesc.RefreshRate.Numerator = 60;//ﾘﾌﾚｯｼｭﾚｰﾄ(分母) ※FPS:60.
	sd.BufferDesc.RefreshRate.Denominator = 1;//ﾘﾌﾚｯｼｭﾚｰﾄ(分子).
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//使い方(表示先).
	sd.OutputWindow = m_hWnd;//ｳｨﾝﾄﾞｳﾊﾝﾄﾞﾙ.
	sd.SampleDesc.Count = 1;//ﾏﾙﾁｻﾝﾌﾟﾙの数.
	sd.SampleDesc.Quality = 0;//ﾏﾙﾁｻﾝﾌﾟﾙのｸｵﾘﾃｨ.
	sd.Windowed = TRUE;//ｳｨﾝﾄﾞｳﾓｰﾄﾞ(ﾌﾙｽｸﾘｰﾝ時はFALSE).

	//作成を試みる機能ﾚﾍﾞﾙの優先を指定.
	// (GPUがｻﾎﾟｰﾄする機能ｾｯﾄの定義).
	// D3D_FEATURE_LEVEL列挙型の配列.
	// D3D_FEATURE_LEVEL_11_0:Direct3D 11.0 の GPUﾚﾍﾞﾙ.
	D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL* pFeatureLevel = nullptr;//配列の要素数.

	//ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝの作成.
	//ﾊｰﾄﾞｳｪｱ(GPU)ﾃﾞﾊﾞｲｽでの作成.
	if (FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,		//ﾋﾞﾃﾞｵｱﾀﾞﾌﾟﾀへのﾎﾟｲﾝﾀ.
			D3D_DRIVER_TYPE_HARDWARE,//作成するﾃﾞﾊﾞｲｽの種類.
			nullptr,		//ｿﾌﾄｳｪｱ ﾗｽﾀﾗｲｻﾞを実装するDLLのﾊﾝﾄﾞﾙ.
			0,				//有効にするﾗﾝﾀｲﾑﾚｲﾔｰ.
			&pFeatureLevels,//作成を試みる機能ﾚﾍﾞﾙの順序を指定する配列へのﾎﾟｲﾝﾀ.
			1,				//↑の要素数.
			D3D11_SDK_VERSION,//SDKのﾊﾞｰｼﾞｮﾝ.
			&sd,			//ｽﾜｯﾌﾟﾁｪｰﾝの初期化ﾊﾟﾗﾒｰﾀのﾎﾟｲﾝﾀ.
			&m_pSwapChain,	//(out)ﾚﾝﾀﾞﾘﾝｸﾞに使用するｽﾜｯﾌﾟﾁｪｰﾝ.
			&m_pDevice11,	//(out)作成されたﾃﾞﾊﾞｲｽ.
			pFeatureLevel,	//機能ﾚﾍﾞﾙの配列にある最初の要素を表すﾎﾟｲﾝﾀ.
			&m_pContext11)))//(out)ﾃﾞﾊﾞｲｽ ｺﾝﾃｷｽﾄ.
	{
		//WARPﾃﾞﾊﾞｲｽの作成.
		// D3D_FEATURE_LEVEL_9_1～D3D_FEATURE_LEVEL_10_1.
		if (FAILED(
			D3D11CreateDeviceAndSwapChain(
				nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
				0, &pFeatureLevels, 1, D3D11_SDK_VERSION,
				&sd, &m_pSwapChain, &m_pDevice11,
				pFeatureLevel, &m_pContext11)))
		{
			//ﾘﾌｧﾚﾝｽﾃﾞﾊﾞｲｽの作成.
			// DirectX SDKがｲﾝｽﾄｰﾙされていないと使えない.
			if (FAILED(
				D3D11CreateDeviceAndSwapChain(
					nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr,
					0, &pFeatureLevels, 1, D3D11_SDK_VERSION,
					&sd, &m_pSwapChain, &m_pDevice11,
					pFeatureLevel, &m_pContext11)))
			{
				MessageBox(nullptr,
					"ﾃﾞﾊﾞｲｽとｽﾜｯﾌﾟﾁｪｰﾝ作成失敗",
					"Error", MB_OK);
				return E_FAIL;
			}
		}
	}

	//各種ﾃｸｽﾁｬとそれに付帯する各種ﾋﾞｭｰ(画面)を作成.

	//----------------------------------------------------------
	//	ﾊﾞｯｸﾊﾞｯﾌｧ準備：ｶﾗｰﾊﾞｯﾌｧ設定.
	//----------------------------------------------------------
	//ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬを取得(既にあるので作成ではない).
	ID3D11Texture2D* pBackBuffer_Tex = nullptr;
	m_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),	//__uuidof:式に関連付けされたGUIDを取得
									//         Texture2Dの唯一の物として扱う
		(LPVOID*)&pBackBuffer_Tex);	//(out)ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬ.

	//そのﾃｸｽﾁｬに対してﾚﾝﾀﾞｰﾀｰｹﾞｯﾄﾋﾞｭｰ(RTV)を作成.
	m_pDevice11->CreateRenderTargetView(
		pBackBuffer_Tex,
		nullptr,
		&m_pBackBuffer_TexRTV);	//(out)RTV.
	//ﾊﾞｯｸﾊﾞｯﾌｧﾃｸｽﾁｬを解放.
	SAFE_RELEASE(pBackBuffer_Tex);

	//----------------------------------------------------------
	//	ﾊﾞｯｸﾊﾞｯﾌｧ準備：ﾃﾞﾌﾟｽ(深度)ｽﾃﾝｼﾙ関係.
	//----------------------------------------------------------
	//ﾃﾞﾌﾟｽ(深さor深度)ｽﾃﾝｼﾙﾋﾞｭｰ用のﾃｸｽﾁｬを作成.
	D3D11_TEXTURE2D_DESC	descDepth;
	descDepth.Width = WND_W;	//幅.
	descDepth.Height = WND_H;	//高さ.
	descDepth.MipLevels = 1;	//ﾐｯﾌﾟﾏｯﾌﾟﾚﾍﾞﾙ:1.
	descDepth.ArraySize = 1;	//配列数:1.
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;//32ﾋﾞｯﾄﾌｫｰﾏｯﾄ.
	descDepth.SampleDesc.Count = 1;		//ﾏﾙﾁｻﾝﾌﾟﾙの数.
	descDepth.SampleDesc.Quality = 0;	//ﾏﾙﾁｻﾝﾌﾟﾙのｸｵﾘﾃｨ.
	descDepth.Usage = D3D11_USAGE_DEFAULT;//使用方法:ﾃﾞﾌｫﾙﾄ.
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;//深度(ｽﾃﾝｼﾙとして使用).
	descDepth.CPUAccessFlags = 0;	//CPUからはｱｸｾｽしない.
	descDepth.MiscFlags = 0;		//その他の設定なし.

	m_pDevice11->CreateTexture2D(
		&descDepth,
		nullptr,
		&m_pBackBuffer_DSTex);	//(out)ﾃﾞﾌﾟｽｽﾃﾝｼﾙ用ﾃｸｽﾁｬ.

	//そのﾃｸｽﾁｬに対してﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰ(DSV)を作成.
	m_pDevice11->CreateDepthStencilView(
		m_pBackBuffer_DSTex,
		nullptr,
		&m_pBackBuffer_DSTexDSV);	//(out)DSV.
	//ﾚﾝﾀﾞｰﾀｰｹﾞｯﾄﾋﾞｭｰとﾃﾞﾌﾟｽｽﾃﾝｼﾙﾋﾞｭｰをﾊﾟｲﾌﾟﾗｲﾝにｾｯﾄ.
	m_pContext11->OMSetRenderTargets(
		1,
		&m_pBackBuffer_TexRTV,
		m_pBackBuffer_DSTexDSV);
//
	// 深度テスト(zテスト)を有効にする.
	D3D11_DEPTH_STENCIL_DESC dsDesc = D3D11_DEPTH_STENCIL_DESC();
	
	dsDesc.DepthEnable		= TRUE;	// 有効.
	dsDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc		= D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable	= FALSE;
	dsDesc.StencilReadMask	= D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	// 深度設定作成.
	if( FAILED( m_pDevice11->CreateDepthStencilState(
		&dsDesc, &m_pDepthStencilState )))
	{
		_ASSERT_EXPR( false, L"深度設定作成失敗" );
		return E_FAIL;
	}
	// 深度テスト(zテスト)を無効にする.
	dsDesc.DepthEnable = FALSE;	// 無効.
	// 深度設定作成.
	if( FAILED( m_pDevice11->CreateDepthStencilState(
		&dsDesc, &m_pDepthStencilStateOff )))
	{
		_ASSERT_EXPR( false, L"深度設定作成失敗" );
		return E_FAIL;
	}

	//------------------------------------------------
	//	ﾋﾞｭｰﾎﾟｰﾄ設定.
	//------------------------------------------------
	D3D11_VIEWPORT vp;
	vp.Width = WND_W;	//幅.
	vp.Height = WND_H;	//高さ.
	vp.MinDepth = 0.0f;	//最小深度(手前).
	vp.MaxDepth = 1.0f;	//最大深度(奥).
	vp.TopLeftX = 0.0f;	//左上位置x.
	vp.TopLeftY = 0.0f;	//左上位置y.

	m_pContext11->RSSetViewports(1, &vp);


	//------------------------------------------------
	//	ﾗｽﾀﾗｲｽﾞ(面の塗りつぶし方)の設定.
	//------------------------------------------------
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory(&rdc, sizeof(rdc));
	rdc.FillMode = D3D11_FILL_SOLID;	//塗りつぶし(ｿﾘｯﾄﾞ).
	rdc.CullMode = D3D11_CULL_BACK;		//D3D11_CULL_BACK:背面を描画しない.
										//D3D11_CULL_FRONT:正面を描画しない.
										//D3D11_CULL_NONE:ｶﾘﾝｸﾞを切る(正背面を描画する).
	rdc.FrontCounterClockwise = FALSE;//ﾎﾟﾘｺﾞﾝの表裏を決定するﾌﾗｸﾞ.
									//TRUE:左回りなら前向き。右回りなら後ろ向き。
									//FALSE:逆になる.
	rdc.DepthClipEnable = FALSE;	//距離についてのｸﾘｯﾋﾟﾝｸﾞ有効.

	ID3D11RasterizerState* pRs = nullptr;
	m_pDevice11->CreateRasterizerState(&rdc, &pRs);
	m_pContext11->RSSetState(pRs);
	SAFE_RELEASE(pRs);

	return S_OK;
}

HRESULT CMain::InitDx9()
{
	LPDIRECT3D9 m_pD3d9; //Dx9ｵﾌﾞｼﾞｪｸﾄ.

	//「Direct3D」ｵﾌﾞｼﾞｪｸﾄの作成.
	m_pD3d9 = Direct3DCreate9( D3D_SDK_VERSION );
	if( m_pD3d9 == nullptr ){
		_ASSERT_EXPR( false, L"Dx9ｵﾌﾞｼﾞｪｸﾄ作成失敗" );
		return E_FAIL;
	}

	// Diret3Dﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄの作成.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;	//ﾊﾞｯｸﾊﾞｯﾌｧのﾌｫｰﾏｯﾄ(ﾃﾞﾌｫﾙﾄ).
	d3dpp.BackBufferCount = 1;					//ﾊﾞｯｸﾊﾞｯﾌｧの数.
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	//ｽﾜｯﾌﾟｴﾌｪｸﾄ(ﾃﾞﾌｫﾙﾄ).
	d3dpp.Windowed = true;						//ｳｨﾝﾄﾞｳﾓｰﾄﾞ.
	d3dpp.EnableAutoDepthStencil = true;		//ｽﾃﾝｼﾙ有効.
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	//ｽﾃﾝｼﾙのﾌｫｰﾏｯﾄ(16bit).

	//ﾃﾞﾊﾞｲｽ作成(HALﾓｰﾄﾞ:描画と頂点処理をGPUで行う).
	if( FAILED( m_pD3d9->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp, &m_pDevice9 ) ) ){
		//ﾃﾞﾊﾞｲｽ作成(HALﾓｰﾄﾞ:描画はGPU、頂点処理をCPUで行う).
		if( FAILED( m_pD3d9->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &m_pDevice9 ) ) ){
			MessageBox( nullptr,
				"HALﾓｰﾄﾞでﾃﾞﾊﾞｲｽ作成できません\nREFﾓｰﾄﾞで再試行します",
				"警告", MB_OK );

			//ﾃﾞﾊﾞｲｽ作成(REFﾓｰﾄﾞ:描画はCPU、頂点処理をGPUで行う).
			if( FAILED( m_pD3d9->CreateDevice(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd,
				D3DCREATE_HARDWARE_VERTEXPROCESSING,
				&d3dpp, &m_pDevice9 ) ) ){
				//ﾃﾞﾊﾞｲｽ作成(REFﾓｰﾄﾞ:描画と頂点処理をCPUで行う).
				if( FAILED( m_pD3d9->CreateDevice(
					D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, m_hWnd,
					D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp, &m_pDevice9 ) ) ){
					MessageBox( nullptr,
						"Direct3Dﾃﾞﾊﾞｲｽ作成失敗", "警告", MB_OK );
					return E_FAIL;
				}
			}
		}
	}

	SAFE_RELEASE( m_pD3d9 );

	return S_OK;
}

//Direct3D終了処理.
void CMain::Destroy()
{
	pSceneManager->StackRelease();

	//Direct3Dｵﾌﾞｼﾞｪｸﾄを解放.
	SAFE_RELEASE( m_pDepthStencilStateOff );
	SAFE_RELEASE( m_pDepthStencilState );
	SAFE_RELEASE( m_pBackBuffer_DSTexDSV );
	SAFE_RELEASE( m_pBackBuffer_DSTex );
	SAFE_RELEASE( m_pBackBuffer_TexRTV );
	SAFE_RELEASE( m_pSwapChain );
	SAFE_RELEASE( m_pContext11 );
	SAFE_RELEASE( m_pDevice11 );
	SAFE_RELEASE( m_pDevice9 );

	m_hWnd = nullptr;
}

//ﾒｯｼｭの読み込み関数.
HRESULT CMain::LoadMesh()
{
	CLoadImage::Load( m_pDevice11, m_pContext11 );

	// スプライトの読み込み.
	CSpriteResource::Load( m_pDevice11, m_pContext11 );
	// ステージ名リストの取得.
	CMapLoad::StageNameListLoad( m_pDevice11, m_pContext11 );
	// メッシュ読み込み.
	CMeshResorce::Load( m_hWnd, m_pDevice11, m_pContext11, m_pDevice9 );
	// エフェクト読み込み.
	CEffectResource::Load( m_pDevice11, m_pContext11 );

	CEditModelResource::Load( m_hWnd, m_pDevice11, m_pContext11, m_pDevice9 );

	CDebugText::Init( m_pContext11, 40.0f, D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	CTutorialTextLoad::TextDataLoad();	//チュートリアルテキストロード.

	m_Camera.Pos	= D3DXVECTOR3( 0.0f, 4.0f, -10.0f );
	m_Camera.vLook	= D3DXVECTOR3( 0.0f, -2.0f, 10.0f );

	return S_OK;
}

// バックバッファクリア関数.
void CMain::ClearBackBuffer()
{
	//画面のｸﾘｱ.
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };	//ｸﾘｱ色(RGBAの順).
	//ｶﾗｰﾊﾞｯｸﾊﾞｯﾌｧ.
	m_pContext11->ClearRenderTargetView(
		m_pBackBuffer_TexRTV, ClearColor );
	//ﾃﾞﾌﾟｽｽﾃﾝｼﾙﾊﾞｯｸﾊﾞｯﾌｧ.
	m_pContext11->ClearDepthStencilView(
		m_pBackBuffer_DSTexDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0 );
}

// カメラ関数.
void CMain::Camera()
{
	//ﾋﾞｭｰ(ｶﾒﾗ)変換.
	D3DXVECTOR3	vUpVec( 0.0f, 1.0f, 0.0f );	//上方(ﾍﾞｸﾄﾙ).
	D3DXMatrixLookAtLH(
		&m_mView,	//(out)ﾋﾞｭｰ計算結果.
		&m_Camera.Pos, &m_Camera.vLook, &vUpVec );
}

// プロジェクション関数.
void CMain::Proj()
{
	//ﾌﾟﾛｼﾞｪｸｼｮﾝ(射影)変換.
	D3DXMatrixPerspectiveFovLH(
		&m_mProj,	//(out)ﾌﾟﾛｼﾞｪｸｼｮﾝ計算結果.
		static_cast<FLOAT>( D3DX_PI / 4.0f ),	//y方向の視野(ﾗｼﾞｱﾝ指定)数値を大きくしたら視野が狭くなる.
		static_cast<FLOAT>( WND_W ) / static_cast<FLOAT>( WND_H ),//ｱｽﾍﾟｸﾄ比(幅÷高さ).
		0.1f,		//近いﾋﾞｭｰ平面のz値.
		200.0f );	//遠いﾋﾞｭｰ平面のz値.
}


// 深度(Z)テストON/OFF切替え.
void CMain::SetDeprh( bool flag )
{
	ID3D11DepthStencilState* pTmp
		= ( flag == true ) ? m_pDepthStencilState : m_pDepthStencilStateOff;
	
	// 深度設定をセット.
	m_pContext11->OMSetDepthStencilState( pTmp, 1 );
}

// カメラコントロール.
void CMain::CameraControl()
{
	//// 注視位置の移動.
	//static bool isCamera = false;
	//if( isCamera == true ){
	//	if( GetAsyncKeyState( 'W' ) & 0x8000 )		m_Camera.vLook.y += 0.1f;
	//	if( GetAsyncKeyState( 'S' ) & 0x8000 )		m_Camera.vLook.y -= 0.1f;
	//	if( GetAsyncKeyState( 'D' ) & 0x8000 )		m_Camera.vLook.x += 0.1f;
	//	if( GetAsyncKeyState( 'A' ) & 0x8000 )		m_Camera.vLook.x -= 0.1f;
	//	if( GetAsyncKeyState( 'Q' ) & 0x8000 )		m_Camera.vLook.z += 0.1f;
	//	if( GetAsyncKeyState( 'E' ) & 0x8000 )		m_Camera.vLook.z -= 0.1f;
	//}
	//if( GetAsyncKeyState( 'C' ) & 0x8000 )	isCamera = !isCamera;
	//

	//// カメラ位置移動.
	//if( GetAsyncKeyState( VK_LEFT ) & 0x8000 )	m_Camera.Pos.x -= 0.1f;
	//if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 )	m_Camera.Pos.x += 0.1f;
	//if( GetAsyncKeyState( VK_UP ) & 0x8000 )	m_Camera.Pos.y += 0.1f;
	//if( GetAsyncKeyState( VK_DOWN ) & 0x8000 )	m_Camera.Pos.y -= 0.1f;
	//if( GetAsyncKeyState( VK_F1 ) & 0x8000 )	m_Camera.Pos.z += 0.1f;
	//if( GetAsyncKeyState( VK_F2 ) & 0x8000 )	m_Camera.Pos.z -= 0.1f;
}