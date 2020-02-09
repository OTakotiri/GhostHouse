#include "ObjectArrangement.h"
#include "..\..\ModelResource\ModelResource.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"
#include "..\..\DebugMode\DebugText\DebugText.h"
#include "..\..\XInput\XInput.h"

CObjectArrangement::CObjectArrangement()
	: m_pModel		( nullptr )
	, m_ObjctInfo	()
	, m_isWierDips	( false )
{
}

CObjectArrangement::~CObjectArrangement()
{
}

void CObjectArrangement::Update()
{
	if( ModelLoad() == false ) return;

	if( CXInput::IsPress( XINPUT_GAMEPAD_Y ) ){
		m_ObjctInfo.vRot.y += static_cast<float>(D3DXToRadian(90.0));
	}
	if( ( GetAsyncKeyState( 'C' ) & 0x0001 ) ||
		(CXInput::IsPress( XINPUT_GAMEPAD_RIGHT_THUMB )) ){
		m_isWierDips = !m_isWierDips;
	}
}

void CObjectArrangement::Render( 
	D3DXMATRIX& mView, 
	D3DXMATRIX& mProj,
	Light& stLight, 
	stCAMERA& stCamera )
{
	if( ModelLoad() == false ) return;
	
	m_pModel->SetRotation( m_ObjctInfo.vRot );
	m_pModel->SetPosition( m_ObjctInfo.vPos );
	m_pModel->SetScale( edit::MODEL_SCALE );
	m_pModel->SetWireFlag( m_isWierDips );
	m_pModel->Render( mView, mProj, stLight, stCamera);
	m_pModel->SetWireFlag( false );

	float textSize = CDebugText::GetScale();
	CDebugText::SetPosition( D3DXVECTOR3( 10.0f, 20.0f, 0.0f ) );
	CDebugText::Render( "Position x : ", m_ObjctInfo.vPos.x );
	CDebugText::SetPosition( D3DXVECTOR3( 10.0f, 20.0f+textSize, 0.0f ) );
	CDebugText::Render( "Position y : ", m_ObjctInfo.vPos.y );
	CDebugText::SetPosition( D3DXVECTOR3( 10.0f, 20.0f+textSize*2.0f, 0.0f ) );
	CDebugText::Render( "Position z : ", m_ObjctInfo.vPos.z );
}

bool CObjectArrangement::ModelLoad()
{
	if( m_pModel != nullptr ) return true;
	m_pModel = CEditModelResource::GetMesh(m_ObjctInfo.Name);

	if( m_pModel != nullptr ) return true;
	return false;
}