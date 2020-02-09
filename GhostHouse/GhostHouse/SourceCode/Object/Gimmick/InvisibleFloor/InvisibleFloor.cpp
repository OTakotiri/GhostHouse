#include "InvisibleFloor.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CInvisibleFloor::CInvisibleFloor()
{
	m_vPosition	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotation	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

CInvisibleFloor::CInvisibleFloor( const stObjectInfo& objInfo )
	: m_AlphaCount	( 0.0f )
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CInvisibleFloor::~CInvisibleFloor()
{
}

void CInvisibleFloor::Update(shared_ptr<CObjectBase> pObj)
{
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );
}

void CInvisibleFloor::Render(D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera)
{
	if( m_pStaticMesh == nullptr )	return;
	if( m_bMoveFlag == false )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( m_fAlpha );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetWireFlag( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetWireFlag( false );
}

void CInvisibleFloor::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CInvisibleFloor::Load(ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11)
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
		}
	}
}

void CInvisibleFloor::Action(shared_ptr<CObjectBase> pObj)
{
	if( pObj->GetBitFlag( BitFlag::isCameraMoveUp ) == false ){
		m_bMoveFlag = false;
		return;
	}
	m_bMoveFlag = true;
}

LPD3DXMESH CInvisibleFloor::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}