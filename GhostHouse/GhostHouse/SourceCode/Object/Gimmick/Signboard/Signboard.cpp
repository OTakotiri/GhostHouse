#include "Signboard.h"
#include "..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\XInput\XInput.h"

CSignboard::CSignboard()
	: m_pStaticMesh		( nullptr )
	, m_MoveDistance	( 0.0f )
{
	m_vPosition	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotation	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}


CSignboard::CSignboard( const stObjectInfo& objInfo )
	: CSignboard()
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CSignboard::~CSignboard()
{
}

void CSignboard::Update(shared_ptr<CObjectBase> pObj)
{
	if( m_pStaticMesh == nullptr )	return;
	FloatingMove();
	Action( pObj );
}

void CSignboard::Render(D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera)
{
	if( m_pStaticMesh == nullptr )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->SetCoverage( true );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );
	m_pStaticMesh->SetCoverage( false );
}

void CSignboard::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CSignboard::Load(ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11)
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( 3.0f );
		}
	}
}

void CSignboard::Action( shared_ptr<CObjectBase> pObj )
{
	// アクションボタンフラグを下す.
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == false ){
		if( pObj->GetBitFlag(BitFlag::isHitSignboard) == true ){
			pObj->BitFlagOFF(BitFlag::isHitSignboard);
			m_bMoveFlag = false;
		}
		return;
	}

	// キー情報を受け取る.
	if( CXInput::IsPress( XINPUT_GAMEPAD_B ) == false ) return;
	
	if( pObj->GetBitFlag(BitFlag::isHitSignboard) == false ){
		pObj->BitFlagON(BitFlag::isHitSignboard);
		pObj->GetBitFlag( BitFlag::isSignBoardRender ) == true ? 
			pObj->BitFlagOFF( BitFlag::isSignBoardRender ) :
			pObj->BitFlagON( BitFlag::isSignBoardRender );
	}
}

void CSignboard::FloatingMove()
{
	m_vPosition.y += sinf( m_MoveDistance ) * MOVE_DISTANCE;
	m_MoveDistance += 0.04f;
	if( m_MoveDistance >= 360.0f ){
		m_MoveDistance += MOVE_DISTANCE;
	}
}

LPD3DXMESH CSignboard::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}