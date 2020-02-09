#include "SwitchDoor.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CSwitchDoor::CSwitchDoor( const stObjectInfo& objInfo )
	: m_fCount		( 0.0f )
	, m_bFadeInFlag	( true )
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CSwitchDoor::~CSwitchDoor()
{
}

void CSwitchDoor::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );
}

void CSwitchDoor::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
{
	if( m_pStaticMesh == nullptr )	return;
	if( m_fAlpha <= 0.8f ){
		m_fAlpha = 1.0f;
		return;
	}

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetCoverage( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );

}

void CSwitchDoor::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CSwitchDoor::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
		}
	}
}


void CSwitchDoor::Action( shared_ptr<CObjectBase> pObj )
{
	
	if( pObj->GetBitFlag(BitFlag::isMoveSwitch) == true ){
		m_bMoveFlag = true;
	} else {
		m_bMoveFlag = false;
	}

	if( m_bMoveFlag == true ){
		m_pStaticMesh = CMeshResorce::GetStatic( "SwitchDoorOpen" );
	} else {
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
	}
}

void CSwitchDoor::FadeInOut()
{
	if( m_fCount >= 0.0f ){
		if( m_fCount >= 1.5f ) m_bFadeInFlag = false;
		if( m_bFadeInFlag == true ){
			m_fCount += 0.01f;
		} else{
			m_fCount -= 0.01f;
		}
	}
}

LPD3DXMESH CSwitchDoor::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}