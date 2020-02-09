#include "UpDownFloor.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CUpDownFloor::CUpDownFloor( const stObjectInfo& objInfo )
	: m_fAngleCount	( 0.0f )
	, m_iWaitCount	( 0 )
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CUpDownFloor::~CUpDownFloor()
{
}

void CUpDownFloor::Update( shared_ptr<CObjectBase> pObj )
{
	// スタティックメッシュがnullptrだったら終了.
	if( m_pStaticMesh == nullptr )	return;

	// 動作処理.
	Action( pObj );
}

void CUpDownFloor::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
{
	// スタティックメッシュがnullptrだったら終了.
	if( m_pStaticMesh == nullptr )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );

}

void CUpDownFloor::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CUpDownFloor::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
		}
	}
}

void CUpDownFloor::Action( shared_ptr<CObjectBase> pObj )
{
	// sin波で上下に移動する.
	float ff = sinf( D3DXToRadian( m_fAngleCount ) );

	if( -0.01f < ff && ff < 0.01f ){
		m_bMoveFlag = false;
		if( m_iWaitCount >= MAX_WAIT_TIME ){
			m_iWaitCount = 0;
			m_bMoveFlag = true;
		}
		m_iWaitCount++;
	}
	
	if( m_bMoveFlag == true ){
		m_vPosition.y += ff * RADIUS * 10.0f;
		// 角度が360以上になったら0に戻す.
		if( m_fAngleCount >= 360 )	m_fAngleCount = 0;

		m_fAngleCount++;	// 角度を増やす.
	}
}

LPD3DXMESH CUpDownFloor::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}