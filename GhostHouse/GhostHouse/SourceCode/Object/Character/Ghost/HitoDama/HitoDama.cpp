#include "HitoDama.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CHitoDama::CHitoDama()
	: m_pGhostB				( nullptr )
	, m_fShotCount			( MAX_SHOT_COUNT )
	, m_fScaleChangeCount	( 0.0f )
{
	m_vPosition	= WND_OUT_PISITION;
	m_vRotation	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bMoveFlag	= false;
	m_fScale	= MODEL_SCALE;
}

CHitoDama::~CHitoDama()
{
}

// 更新関数.
void CHitoDama::Update( shared_ptr<CObjectBase> pObj )
{
	// スキンメッシュが入ってなかったら.
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );	// 動作.

	IdleState();	// 待機.
}

// 描画関数.
void CHitoDama::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	// スキンメッシュが入ってなかったら.
	if( m_pStaticMesh == nullptr )	return;
	// 移動フラグが下りていたら.
	if( m_bMoveFlag == false )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標情報を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );	// 回転情報を描画用にセット.
	m_pStaticMesh->SetAlpha( m_fShotCount );	// アルファ値をセット.
	m_pStaticMesh->SetScale( m_fScale );
	m_pStaticMesh->SetCoverage( true );			// アルファブレンドをセット.
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetCoverage( false );		// アルファブレンドをセット.

}

void CHitoDama::Collision( shared_ptr<CObjectBase> pObj )
{
	if( pObj == nullptr ) return;
	if( m_bMoveFlag == false ) return;
	HitProc( pObj );	// 当たり処理.
}

void CHitoDama::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( SPHERE_COLLISION_RADIUS );
		}
	}
}

// 動作関数.
void CHitoDama::Action( shared_ptr<CObjectBase> pObj )
{
	if( m_bMoveFlag == false ) return;
	
	
	if( m_fScaleChangeCount >= 0.6f ){
		m_fScaleChangeCount += 0.007f;
		m_vRotation.y	= GetTwoRadian( m_vPosition, pObj->GetPosition() );
		m_vPosition.x += AxisZProc( m_vRotation.y ).x * MOVE_SPEED;
		m_vPosition.z += AxisZProc( m_vRotation.y ).z * MOVE_SPEED;
		m_vPosition.y = m_vPos.y;
		m_fShotCount	-= 0.1f;		// ショットカウントを減らす.
		m_fScale += sinf(m_fScaleChangeCount*20.0f) * 0.002f;
	} else {
		if( m_pGhostB->GetBitFlag( BitFlag::isHit ) == true ) return;
		m_fScaleChangeCount += 0.007f;
		m_fScale += 0.005f;
		m_vRotation.y = GetTwoRadian( m_vPosition, pObj->GetPosition() );

		m_vPosition.x = m_vPos.x + AxisZProc( m_vRotation.y ).x * 1.0f;
		m_vPosition.z = m_vPos.z + AxisZProc( m_vRotation.y ).z * 1.0f;
		m_vPos.y = m_vPosition.y -= 0.04f;
	}


	if( m_fShotCount <= 0.0f ){
		m_bMoveFlag = false;
	}
}

void CHitoDama::HitProc( shared_ptr<CObjectBase> pObj )
{
	if( m_bMoveFlag == false ) return;
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return;
	if( m_fScaleChangeCount < 0.6f ) return;

	m_vPosition.y += 1.5f;
	// オブジェクトと当たっていたら.
	if( ObjSphereCollision( pObj ) == true ){
		pObj->BitFlagON( BitFlag::isHit );
		m_bMoveFlag = false;
	} else{
		pObj->BitFlagOFF( BitFlag::isHit );
	}
	m_vPosition.y -= 1.5f;
}

// 待機関数.
void CHitoDama::IdleState()
{
	// 移動フラグが下りていたら.
	if( m_bMoveFlag == true ) return;
	if( m_vPosition == WND_OUT_PISITION ) return;

	// 描画外にセット.
	m_vPosition = WND_OUT_PISITION;
}

void CHitoDama::SetShot( const D3DXVECTOR3& vPos )
{
	if( m_pGhostB->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( m_bMoveFlag == false ){
		m_bMoveFlag = true;
		m_vPos = m_vPosition = vPos;
		m_vPosition.y += 2.0f;
		m_fShotCount = MAX_SHOT_COUNT;
		m_fScaleChangeCount = 0.0f;
		m_fScale = 0.0f;
	}
}

LPD3DXMESH CHitoDama::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}