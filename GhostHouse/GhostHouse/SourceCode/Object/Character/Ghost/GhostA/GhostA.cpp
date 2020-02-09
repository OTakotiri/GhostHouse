#include "GhostA.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CGhostA::CGhostA( const stObjectInfo& objInfo )
	: m_pStaticMesh		( nullptr )
	, m_bChangeTexture	( false )
{
	SetObjectInfo( objInfo );

	m_fWaitDistance	= MAX_WAIT_DISTANCE;
	// 作成されたモデルの高さが高いので引いてあげる.
	m_vPosition.y	-= INIT_POSITION_Y;
	m_fScale		= MODEL_SCALE;

	m_pSearchRange	= make_unique<CCollisionSphere>( &m_vPosition );
	m_pSearchRange->SetRadius( MIN_SEARCH_DISTANCE );
}

CGhostA::~CGhostA()
{
}

// 更新関数.
void CGhostA::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pStaticMesh == nullptr )	return;
	if( CSpriteResource::GetSprite( "Fade" )->GetAlpha() >= 0.1f ) return;
	if( pObj == nullptr ) return;
	if( pObj->GetBitFlag( BitFlag::isCameraMoveUp ) == true ) return;

	Action( pObj );

	IdleState();
}


// 描画関数.
void CGhostA::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	if( m_pStaticMesh == nullptr )	return;

	m_pStaticMesh->SetPosition( m_vPosition );
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( CGhostBase::FadeChange() );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->SetCullBackFlag( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera, m_bChangeTexture );
	m_pStaticMesh->SetCullBackFlag( false );
	m_pStaticMesh->SetBlend( false );

}

void CGhostA::Collision( shared_ptr<CObjectBase> pObj )
{
	if( CSpriteResource::GetSprite( "Fade" )->GetAlpha() >= 0.1f ) return;
	if( pObj == nullptr ) return;
	if( pObj->GetBitFlag( BitFlag::isCameraMoveUp ) == true ) return;
	if( pObj->GetObjectNo() == enObjectNo::Towa ) return;
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return;

	SearchCollision( pObj );

	if( m_bMoveFlag == false ) return;

	ObjSphereCollision( pObj );
}

void CGhostA::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( SPHERE_COLLISION_RADIUS );
		}
	}
	m_pTexture = make_shared<CDX9Mesh::CHANGE_TEXTURES>();

	strcpy_s( m_pTexture->szTargetTextureName, "gosct_a_tex.png" );
	strcpy_s( m_pTexture->szTextureName, "Data\\Model\\Ghost\\GhostA\\gosct_a_tex_yara.png" );
	

	if( m_pTexture->szTargetTextureName != 0 &&
		FAILED(
			D3DX11CreateShaderResourceViewFromFileA(
				pDevice11, m_pTexture->szTextureName,
				nullptr, nullptr, &m_pTexture->pTexture, nullptr ) ) ){
		MessageBox( NULL, "テクスチャ読み込み失敗",
			"Error", MB_OK );
		return;
	}
	m_pStaticMesh->SetChangeTextures( m_pTexture );
}
// 動作関数.
void CGhostA::Action( shared_ptr<CObjectBase> pObj )
{
	if( GetBitFlag( BitFlag::isHit ) == true ){
		m_bChangeTexture = true;	//ライトが当たっていたらテクスチャ切り替え.
		return;
	} else{
		m_bChangeTexture = false;	//ライトが当たっていないのでテクスチャをデフォルトに.
	}
	if( m_bMoveFlag == false ) return;
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return;
	if( GetBitFlag( BitFlag::isInvincible ) == true ) return;

	float fDistance = GetTwoDistance( m_vPosition, pObj->GetPosition() );
	float fRadian = GetTwoRadian( m_vPosition, pObj->GetPosition() );

	// 距離が'0.1f'より少ない場合は移動しても無駄なので終了.
	if( fDistance <= 0.1f ) return;

	m_vRotation.y = fRadian + D3DX_PI;
	m_vPosition	-= AxisZProc( m_vRotation.y ) * MOVE_SPEED;
}

bool CGhostA::SearchCollision( shared_ptr<CObjectBase> pObj )
{
	if( CGhostBase::SearchCollision( pObj ) == true ){
		m_pSearchRange->SetRadius( MAX_SEARCH_DISTANCE );
	} else {
		m_pSearchRange->SetRadius( MIN_SEARCH_DISTANCE );
	}
	return false;
} 

LPD3DXMESH CGhostA::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}