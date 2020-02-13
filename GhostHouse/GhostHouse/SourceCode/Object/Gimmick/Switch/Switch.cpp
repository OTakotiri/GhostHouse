#include "Switch.h"
#include "..\..\..\XAudio2\PlaySEThread.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CSwitch::CSwitch( const stObjectInfo& objInfo )
	: m_isSayaka	( false )
	, m_isTowa		( false )
	, m_bisPush		( false )
{
	SetObjectInfo( objInfo );
	INIT_POSITION_Y = m_vPosition.y;
	m_fScale = MODEL_SCALE;
}

CSwitch::~CSwitch()
{
}

void CSwitch::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );
}

void CSwitch::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
{
	if( m_pStaticMesh == nullptr )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );
}

void CSwitch::Collision( shared_ptr<CObjectBase> pObj )
{
	if( pObj == nullptr ) return;
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == true ){
		pObj->BitFlagON(BitFlag::isMoveSwitch);
		if( pObj->GetObjectNo() == enObjectNo::Sayaka ){
			m_bMoveFlag = m_isSayaka = true;
		} else {
			m_bMoveFlag = m_isTowa = true;
		}
	} else{
		if( m_isSayaka == true ){
			m_isSayaka = false;
		} else if( m_isTowa == true ){
			m_isTowa = false;
		} else {
			m_bMoveFlag = false;
		}
	}
}

void CSwitch::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( 0.5f );
		}
	}
}


void CSwitch::Action( shared_ptr<CObjectBase> pObj )
{
	if( m_bMoveFlag == true ){
		if( m_vPosition.y >= INIT_POSITION_Y - 0.1f ){
			m_vPosition.y -= 0.1f;
		} else {
			pObj->BitFlagON(BitFlag::isMoveSwitch);
			if (m_bisPush == false) {
				CPlaySEThread::SetSEName("switch");
				m_bisPush = true;
			}
		}
	} else {
		if( m_vPosition.y < INIT_POSITION_Y ){
			m_vPosition.y += 0.01f;
			pObj->BitFlagOFF(BitFlag::isMoveSwitch);
			m_bisPush = false;
		}
	}
}

LPD3DXMESH CSwitch::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}