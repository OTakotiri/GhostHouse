#include "Ground.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"
#include "..\ModelResource\ModelResource.h"

CGround::CGround()
{
	m_vPosition = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

CGround::~CGround()
{}

//--------------------.
// 更新関数.
//--------------------.
void CGround::Update( shared_ptr<CObjectBase> pObj )
{}

//--------------------.
// 描画関数.
//--------------------.
void CGround::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera )
{
	if( m_pStaticMesh == nullptr ) return;

	m_pStaticMesh->SetPosition( m_vPosition );
	m_pStaticMesh->SetAlpha( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );
	m_pStaticMesh->SetAlpha( false );
}

//--------------------.
// 読み込み関数.
//--------------------.
void CGround::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pStaticMesh != nullptr ) return;
	m_pStaticMesh = CEditModelResource::GetMesh("ground");
}

//--------------------.
// メッシュ取得関数.
//--------------------.
LPD3DXMESH CGround::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}