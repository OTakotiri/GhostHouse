#include "EditScene.h"
#include "..\TItleScene\TitleScene.h"
#include "..\..\UI\LoadImage\LoadImage.h"

#include "..\..\Edit\ModelResource\ModelResource.h"
#include "..\..\UI\Fade\FadeUI.h"
#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx11.h"
#include "..\..\ImGui\imgui_impl_win32.h"

CEditScene::CEditScene( shared_ptr<CSceneManager> sceneManager )
	: CSceneBase		( sceneManager )
	, m_pEdit			( make_unique<CEdit>() )
	, m_isImGuiCreate	( false )
	, m_isLightSet		( false )
{
}

CEditScene::~CEditScene()
{
	// ImGuiの解放.
	ImGui_ImplWin32_Shutdown();
	ImGui_ImplDX11_Shutdown();
	ImGui::DestroyContext();
}

//-----------------------------------.
//	読込処理関数.
//-----------------------------------.
void CEditScene::Load( 
	HWND hWnd, 
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9 )
{
	// エディットクラスの読み込み.
	m_pEdit->Load( pDevice11, pContext11 );
	// ImGuiの作成.
	ImGuiCreate( hWnd, pDevice11, pContext11 );
}

//-----------------------------------.
//	更新処理関数.
//-----------------------------------.
void CEditScene::Update()
{
	// ImGuiが作成してなければ終了.
	if( m_isImGuiCreate == false ) return;
	// エディットの更新.
	m_pEdit->Updete();
	// シーン切り替え関数.
	ChangeScene();
}

//-----------------------------------.
//	描画処理関数.
//-----------------------------------.
void CEditScene::Render( 
	D3DXMATRIX& mView, 
	D3DXMATRIX& mProj,
	Light& stLight, 
	stCAMERA& stCamera )
{
	// ImGuiが作成してなければ終了.
	if( m_isImGuiCreate == false ) return;
	// ライトの設定.
	SetingLight( stLight );
	// エディットの描画.
	m_pEdit->Render( mView,  mProj, stLight, stCamera );
}

//-----------------------------------.
//	ImGuiの作成関数.
//-----------------------------------.
void CEditScene::ImGuiCreate(
	HWND hWnd,
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11 )
{
	// 既にImGuiが作成されていたら終了.
	if( m_isImGuiCreate == true ) return;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init( hWnd );
	ImGui_ImplDX11_Init( pDevice11, pContext11 );
	ImGui::StyleColorsDark();

	CFadeUI::FadeINStart();

	m_isImGuiCreate = true;
}

//-----------------------------------.
//	ライトの設定.
//-----------------------------------.
void CEditScene::SetingLight( Light& stLight )
{
	// 既にライトが設定されていたら終了.
	if( m_isLightSet == true ) return;

	CLoadImage::Render();
	stLight.dir		= { 0.0f, -1.0f, 0.0f };
	stLight.range	= 1000.0f;
	stLight.cone	= 1.0f;
	stLight.ambient = { 1.0f, 1.0f, 1.0f, 1.0f };
	stLight.diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	stLight.pos		= { 0.0f, 100.0f, 0.0f };

	m_isLightSet	= true;
}
//-----------------------------------.
//	シーン切り替え関数.
//-----------------------------------.
void CEditScene::ChangeScene()
{
	// エディットクラスの終了フラグが下りていたら終了.
	if( m_pEdit->IsEditEnd() == false ) return;
	// シーンの切り替え.
	m_pSceneManager->Change( make_shared<CTitleScene>( m_pSceneManager ) );
}