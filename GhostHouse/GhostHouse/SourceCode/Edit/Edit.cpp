#include "Edit.h"

#include "..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"
#include "ModelResource/ModelResource.h"

#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx11.h"
#include "..\..\ImGui\imgui_impl_win32.h"

#include <fstream>
#include <string>
#include <sstream>

CEdit::CEdit()
	: m_pTmpStage			()
	, m_pObjArrangement		( make_unique<CObjectArrangement>() )
	, m_pSelectMenu			( make_unique<CSelectMenu>() )
	, m_pEditImGui			( make_unique<CEditImGui>() )
	, m_pPlayer				( make_shared<CPlayer>() )
	, m_pGround				( make_shared<CGround>() )
	, m_isEditEnd			( false )
	, m_TmpObj				()
{
	m_pSelectMenu->SetTmpStage( &m_pTmpStage );
}

CEdit::~CEdit()
{}

//------------------------------.
// 各オブジェクトの読み込み.
//------------------------------.
void CEdit::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	m_pPlayer->Load( pDevice11, pContext11 );
	m_pGround->Load( pDevice11, pContext11 );
}

//------------------------------.
// 更新処理.
//------------------------------.
void CEdit::Updete()
{
	Control();
	m_pPlayer->Update( m_pGround );
	ObjArrangementUpdate();
}

//------------------------------.
// 描画処理.
//------------------------------.
void CEdit::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ObjArrangementRender(  mView, mProj, stLight, stCamera );
	m_pGround->Render( mView, mProj, stLight, stCamera );
	m_pPlayer->Render( mView, mProj, stLight, stCamera );
	TmpStageRender( mView, mProj, stLight, stCamera );

	m_pEditImGui->SetObjectCount( m_pTmpStage.size() );
	m_pEditImGui->Render( m_isEditEnd );
	m_pSelectMenu->Render();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}

//------------------------------.
// 操作処理.
//------------------------------.
void CEdit::Control()
{
	// オブジェクトを置く.
	if(( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) || 
		CXInput::IsPress( XINPUT_GAMEPAD_A ) ){
		PutObject();
	}
	// メニューに戻る.
	if(( GetAsyncKeyState( VK_BACK ) & 0x0001 ) || 
		CXInput::IsPress( XINPUT_GAMEPAD_B ) ){
		m_TmpObj = edit::RENDER_OBJRCT();
		m_pSelectMenu->BackMenu();
	}
	// オブジェクトを消す.
	if(( GetAsyncKeyState( VK_DELETE ) & 0x0001 ) || 
		CXInput::IsPress( XINPUT_GAMEPAD_X ) ){
		DeleteObject();
	}
}

//------------------------------.
// 仮ステージの描画.
//------------------------------.
void CEdit::TmpStageRender(
	D3DXMATRIX& mView,
	D3DXMATRIX& mProj,
	Light& stLight,
	stCAMERA& stCamera )
{
	for( auto& obj : m_pTmpStage ){
		if( obj.pMesh == nullptr ){
			obj.pMesh = CEditModelResource::GetMesh( obj.Name );
			continue;
		}

		obj.pMesh->SetPosition( obj.vPos );
		obj.pMesh->SetRotation( obj.vRot );
		obj.pMesh->SetScale( edit::MODEL_SCALE );
		obj.pMesh->Render( mView, mProj, stLight, stCamera );
	}
}

//------------------------------.
// 選択オブジェクトの更新.
//------------------------------.
void CEdit::ObjArrangementUpdate()
{
	if( m_pSelectMenu->IsChoice() == true ) return;
	m_pPlayer->Control();

	if( m_pSelectMenu->GetObjectName() == "" ) return;
	m_pObjArrangement->SetObjectName( m_pSelectMenu->GetObjectName() );
	m_pObjArrangement->SetPosition( m_pPlayer->GetObjectPosition() );
	m_pObjArrangement->Update();
}

//------------------------------.
// 選択オブジェクトの描画.
//------------------------------.
void CEdit::ObjArrangementRender( 
	D3DXMATRIX& mView, 
	D3DXMATRIX& mProj,
	Light& stLight, 
	stCAMERA& stCamera )
{
	if( m_pSelectMenu->GetObjectName() == "" ) return;
	m_pObjArrangement->Render( mView, mProj, stLight, stCamera );
}

//------------------------------.
// オブジェクトを置く処理.
//------------------------------.
void CEdit::PutObject()
{
	if( m_pSelectMenu->GetObjectName() == "" ) return;
	m_TmpObj = m_pObjArrangement->GetObjectInfo();
	m_pTmpStage.emplace_back( m_TmpObj );
	m_TmpObj = edit::RENDER_OBJRCT();
}

//------------------------------.
// オブジェクトを消す処理.
//------------------------------.
void CEdit::DeleteObject()
{
	for( size_t i = 0; i < m_pTmpStage.size(); i++ ){
		if( m_pTmpStage[i].vPos == m_pPlayer->GetObjectPosition() ){
			m_pTmpStage.erase( m_pTmpStage.begin() + i );
			break;
		}
	}
}