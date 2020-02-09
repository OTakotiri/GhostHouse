#include "PitfallRight.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CPitfallRight::CPitfallRight( stObjectInfo objInfo )
	: m_iCount	( 0 )
	, m_enState	( enPitfallSatate::CLOSE )
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CPitfallRight::~CPitfallRight()
{
}

void CPitfallRight::Update( shared_ptr<CObjectBase> pObj )
{
	// スタティックメッシュがnullptrだったら終了.
	if( m_pStaticMesh == nullptr )	return;

	// 動作処理.
	Action( pObj );
}

void CPitfallRight::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
{
	// スタティックメッシュがnullptrだったら終了.
	if( m_pStaticMesh == nullptr )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );

}

void CPitfallRight::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CPitfallRight::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( "Ziki" );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
		}
	}
}

void CPitfallRight::Action( shared_ptr<CObjectBase> pObj )
{
	switch( m_enState )
	{
	case enPitfallSatate::NOW_OPEN:
		m_vRotation.z = D3DXToRadian( -m_iCount );
		m_iCount += MOVE_SPEED;
		if( m_iCount >= MAX_ANGLE ){
			m_enState = enPitfallSatate::OPEN;
			m_iCount = 0;
		}
		break;
	case enPitfallSatate::OPEN:
		m_vRotation.z = D3DXToRadian( -MAX_ANGLE );
		m_iCount++;
		if( m_iCount >= MAX_WAIT_TIME ){
			m_enState = enPitfallSatate::NOW_CLOSE;
			m_iCount = MAX_ANGLE;
		}
		break;
	case enPitfallSatate::NOW_CLOSE:
		m_vRotation.z = D3DXToRadian( -m_iCount );
		m_iCount--;
		if( m_iCount <= 0 ){
			m_enState = enPitfallSatate::CLOSE;
			m_iCount = 0;
		}
		break;
	case enPitfallSatate::CLOSE:
		m_vRotation.z = D3DXToRadian( 0 );
		m_iCount++;
		if( m_iCount >= MAX_WAIT_TIME / 2 ){
			m_vPosition.y = sinf( static_cast<float>(m_iCount) ) * 0.01f;
		}
		if( m_iCount >= MAX_WAIT_TIME ){
			m_enState = enPitfallSatate::NOW_OPEN;
			m_iCount = 0;
		}
		break;
	}

}
