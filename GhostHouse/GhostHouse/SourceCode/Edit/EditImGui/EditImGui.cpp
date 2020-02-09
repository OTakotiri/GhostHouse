#include "EditImGui.h"
#include "..\FileManager\FileManager.h"
#include "..\..\XInput\XInput.h"

#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx11.h"
#include "..\..\ImGui\imgui_impl_win32.h"

CEditImGui::CEditImGui()
	: m_ObjectCount		( 0 )
	, m_pStageNameList	()
	, m_isEndPopRender	( false )
{
}

CEditImGui::~CEditImGui()
{
}

//------------------------------------.
// 描画関数.
//------------------------------------.
void CEditImGui::Render( bool& isEditEnd )
{
	ManipulationDescriptionRender( isEditEnd );
	EditEndPopRender( isEditEnd );
	if(( GetAsyncKeyState(VK_SPACE) & 0x0001 ) || 
		CXInput::IsPress( XINPUT_GAMEPAD_LEFT_THUMB ) ){
		m_isEndPopRender = !m_isEndPopRender;
	}
}

//------------------------------------.
// 操作説明描画関数.
//------------------------------------.
void CEditImGui::ManipulationDescriptionRender( bool& isEditEnd )
{
	ImGui::SetNextWindowSize( ImVec2( IMGUI_WND_W-100.0f, IMGUI_WND_H-60.0f ), ImGuiCond_::ImGuiCond_Once );
	ImGui::SetNextWindowPos( ImVec2( IMGUI_WND_X+120.0f, 10.0f ), ImGuiCond_::ImGuiCond_Once );
	ImGui::GetWindowSize();
	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_NoResize;
	flag |= ImGuiWindowFlags_NoMove;
	flag |= ImGuiWindowFlags_NoScrollbar;
	flag |= ImGuiWindowFlags_NoScrollWithMouse;
	flag |= ImGuiWindowFlags_NoNav;
	flag |= ImGuiWindowFlags_NoSavedSettings;
	flag |= ImGuiWindowFlags_AlwaysAutoResize;

	bool isOpen = true;
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_WindowBg] = ImVec4( 0.3f, 0.3f, 0.3f, 0.9f );
	ImGui::Begin( "ManipulationDescription", &isOpen, flag );

	auto textDraw = []( const char* text, const char* button, const ImVec4& color )
	{
		ImGui::Text( text ); ImGui::SameLine();
		ImGui::TextColored( color, button );
	};

	ImVec4 color = ImVec4( 1.0f, 1.0f, 1.0f, 1.0f );
	ImGui::Separator();
	ImGui::Text( "Object Count : %d", m_ObjectCount );
	ImGui::Separator();
	ImGui::Text("");
	textDraw( "Camera Zoom ------- ", "R Trigger",		ImVec4( 0.2f, 0.5f, 1.0f, 1.0f ) );
	textDraw( "Camera Reduction -- ", "L Trigger",		ImVec4( 0.2f, 0.5f, 1.0f, 1.0f ) );
	textDraw( "Camera Move ------- ", "RIGHT STICK",	ImVec4( 0.0f, 0.5f, 0.5f, 1.0f ) );
	ImGui::Separator();
	textDraw( "Cursor Move ------- ", "DPAD",	ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Decision ---------- ", "A",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Cancel ------------ ", "B",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Delete Object ----- ", "X",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Spin Object ------- ", "Y",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Move Up Object ---- ", "RB",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Move Down Object -- ", "LB",		ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	textDraw( "Object Wire Draw -- ", "RIGHT STICK BUTTON", ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	ImGui::Separator();
	textDraw( "Edit End ---------- ", "Start", ImVec4( 0.8f, 0.5f, 0.5f, 1.0f ) );
	ImGui::Separator();

	ImGui::End();
}

//------------------------------------.
// エディット終了ウィンドウ描画関数.
//------------------------------------.
void CEditImGui::EditEndPopRender( bool& isEditEnd )
{
	if( m_isEndPopRender == false ) return;

	ImGuiIO& io = ImGui::GetIO();
	// ゲームパッドの使用を許可する.
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::SetNextWindowSize( ImVec2( 250, 70 ), ImGuiCond_::ImGuiCond_Once );
	ImGui::SetNextWindowPos( ImVec2( WND_W/2-250/2, 300 ), ImGuiCond_::ImGuiCond_Once );
	ImGui::GetWindowSize();
	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_NoResize;
	flag |= ImGuiWindowFlags_NoMove;
	flag |= ImGuiWindowFlags_NoScrollbar;
	flag |= ImGuiWindowFlags_NoScrollWithMouse;
	flag |= ImGuiWindowFlags_NoSavedSettings;

	bool isOpen = true;
	ImGui::GetStyle().Colors[ImGuiCol_::ImGuiCol_WindowBg] = ImVec4( 0.3f, 0.3f, 0.3f, 0.9f );
	ImGui::Begin( "Do you want to exit edit mode?", &isOpen, flag );

	ImGui::Text("\n");

	ImGui::Text("           ");ImGui::SameLine();
	if( ImGui::Button("Yes") ) isEditEnd = true;
	ImGui::SameLine();
	if( ImGui::Button("No") ) m_isEndPopRender = false;
	ImGui::End();
}