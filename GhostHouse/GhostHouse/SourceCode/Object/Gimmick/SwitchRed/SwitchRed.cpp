#include "SwitchRed.h"
#include "..\..\..\XAudio2\PlaySEThread.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CSwitchRed::CSwitchRed( const stObjectInfo& objInfo )
	: m_isSayaka	( false )
	, m_isTowa		( false )
	, m_bisPush		( false )
{
	SetObjectInfo( objInfo );
	INIT_POSITION_Y = m_vPosition.y;
	m_fScale = MODEL_SCALE;
}

CSwitchRed::~CSwitchRed()
{
}

void CSwitchRed::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );
}

void CSwitchRed::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
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

void CSwitchRed::Collision( shared_ptr<CObjectBase> pObj )
{
	if( pObj == nullptr ) return;
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == true ){
		pObj->BitFlagON(BitFlag::isMoveSwitchRed);
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

void CSwitchRed::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( 0.5f );
		}
	}
}


void CSwitchRed::Action( shared_ptr<CObjectBase> pObj )
{
	if( m_bMoveFlag == true ){
		if( m_vPosition.y >= INIT_POSITION_Y - 0.1f ){
			m_vPosition.y -= 0.1f;
		} else {
			pObj->BitFlagON(BitFlag::isMoveSwitchRed);
			if (m_bisPush == false) {
				CPlaySEThread::SetSEName("switch");
				m_bisPush = true;
			}
		}
	} else {
		if( m_vPosition.y < INIT_POSITION_Y ){
			m_vPosition.y += 0.01f;
			pObj->BitFlagOFF(BitFlag::isMoveSwitchRed );
			m_bisPush = false;
		}
	}
}

LPD3DXMESH CSwitchRed::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}