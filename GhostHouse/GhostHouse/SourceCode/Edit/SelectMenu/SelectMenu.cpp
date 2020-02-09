#include "SelectMenu.h"
#include "..\ModelResource\ModelResource.h"
#include "..\FileManager\FileManager.h"
#include "..\..\XInput\XInput.h"

#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx11.h"
#include "..\..\ImGui\imgui_impl_win32.h"

CSelectMenu::CSelectMenu()
	: m_pFileManager	( std::make_unique<CFileManager>() )
	, m_SelectOjectName	( "" )
	, m_isChoice		( true )
{
	m_ObjectList.emplace_back( &m_BackGroundNameList );
	m_ObjectList.emplace_back( &m_GimmickNameList );
	m_ObjectList.emplace_back( &m_PlayerNameList );
	m_ObjectList.emplace_back( &m_GhostNameList );
}

CSelectMenu::~CSelectMenu()
{
}

//----------------------------------.
// 仮ステージの設定関数.
//----------------------------------.
void CSelectMenu::SetTmpStage( std::vector<edit::RENDER_OBJRCT>* pobj )
{
	m_pFileManager->SetTmpStage( pobj );
}

//----------------------------------.
// 描画関数.
//----------------------------------.
void CSelectMenu::Render()
{
	GetObjectListName();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = 0;

	if( m_isChoice == true ){
		m_SelectOjectName = "";	// 選択オブジェクト名の初期化.

		SettingWindow();
		
		ObjectTabRender();

		ImGui::End();
	}
	if( ( GetAsyncKeyState( 'M' ) & 0x0001 ) ||
		( CXInput::IsPress( XINPUT_GAMEPAD_START ) ) ){
		m_isChoice = !m_isChoice;
	}
}

//----------------------------------.
// ウィンドウの設定.
//----------------------------------.
void CSelectMenu::SettingWindow()
{
	ImGui::SetNextWindowSize( ImVec2( 400, 500 ), ImGuiCond_::ImGuiCond_Once );
	ImGui::SetNextWindowPos( ImVec2( 0, 0 ), ImGuiCond_::ImGuiCond_Once );
	ImGui::GetWindowSize();

	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_NoResize;				// ウィンドウをリサイズ不可.
	flag |= ImGuiWindowFlags_NoMove;				// ウィンドウを移動不可にします.
	flag |= ImGuiWindowFlags_NoScrollWithMouse;		// マウスホイールでのスクロール操作を無効にします.
	flag |= ImGuiWindowFlags_NoSavedSettings;

	bool isOpen = true;
	ImGui::Begin( "Select Object", &isOpen, flag );
}

//----------------------------------.
// オブジェクトのタブの表示.
//----------------------------------.
void CSelectMenu::ObjectTabRender()
{
	ImGuiIO& io = ImGui::GetIO();
	// ゲームパッドの使用を許可する.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	// タブの左端のドロップダウンメニューを表示.
	ImGuiTabBarFlags flag = ImGuiTabBarFlags_TabListPopupButton;

	int i = 0;
	ImGui::BeginTabBar( "Select Object", flag );
	for( auto& s : edit::EachItemNameList ){
		if( ObjectListRender( s, i ) == true ) break;
		i++;
	}
	ImGui::EndTabBar();
}

//----------------------------------.
// オブジェクトリストの表示.
//----------------------------------.
bool CSelectMenu::ObjectListRender( const std::string& objectListName, const int& index )
{
	if( ImGui::BeginTabItem( objectListName.c_str() ) == false ) return false;
	if( objectListName == "Save/Load" ){
		m_pFileManager->ImGuiRender();
		ImGui::EndTabItem();
		return false;
	}

	for( auto& obj : *m_ObjectList[index] ){
		if( SelectObject( obj ) == true ){
			ImGui::EndTabItem();
			return true;
		}
	}
	ImGui::EndTabItem();
	return false;
}

//----------------------------------.
// オブジェクトの選択.
//----------------------------------.
bool CSelectMenu::SelectObject( const std::string& objectName )
{
	if( ImGui::Button( objectName.c_str() ) ){
		m_SelectOjectName = objectName;
		m_isChoice = false;
		return true;
	}
	return false;
}

//----------------------------------.
// オブジェクト名のリストを取得.
//----------------------------------.
void CSelectMenu::GetObjectListName()
{
	m_GhostNameList			= CEditModelResource::GetGhostNameList();
	m_BackGroundNameList	= CEditModelResource::GetBackGroundNameList();
	m_GimmickNameList		= CEditModelResource::GetGimmickNameList();
	m_PlayerNameList		= CEditModelResource::GetGPlayerNameList();
}