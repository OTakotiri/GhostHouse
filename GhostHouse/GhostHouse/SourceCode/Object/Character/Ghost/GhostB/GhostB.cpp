#include "GhostB.h"
#include "..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\Resource\SpriteResouce\SpriteResouce.h"

CGhostB::CGhostB( const stObjectInfo& objInfo )
	: m_pHitoDama		( make_unique<CHitoDama>() )
	, m_pSkinMesh		( nullptr )
	, m_pAc				( nullptr )
	, m_AnimSpd			( 0.0  )
	, m_AnimSpeed		( 0.0 )
	, m_bChangeTexture	( false )
{
	SetObjectInfo( objInfo );
	m_fWaitDistance = MAX_WAIT_DISTANCE;
	m_bAlphaFlag	= false;
	m_pSearchRange	= make_unique<CCollisionSphere>( &m_vPosition );
	m_pSearchRange->SetRadius( MAX_SEARCH_DISTANCE );
	m_vPosition.y -= INIT_POSITION_Y;
	m_fScale		= MODEL_SCALE;

	m_pHitoDama->GhostBLink( this );
}

CGhostB::~CGhostB()
{
}

// 更新関数.
void CGhostB::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pSkinMesh == nullptr )	return;
	if( CSpriteResource::GetSprite( "Fade" )->GetAlpha() >= 0.1f ) return;
	if( pObj == nullptr ) return;
	if( pObj->GetBitFlag( BitFlag::isCameraMoveUp) == true ) return;

	Action( pObj );				// 動作.
	m_pHitoDama->Update( pObj );// 人魂の更新処理.

	if( m_pAc->GetTime() >= 1.35 ){
		AnimInit();
	}

	IdleState();				// 待機.
}

// 描画関数.
void CGhostB::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	if( m_pSkinMesh == nullptr )	return;

	m_AnimSpeed = m_isGamePause == false ? m_AnimSpd : 0.0;

	m_pSkinMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pSkinMesh->SetRotation( m_vRotation );
	m_pSkinMesh->SetAlpha( CGhostBase::FadeChange() );
	m_pSkinMesh->SetScale( MODEL_SCALE );
	m_pSkinMesh->SetAnimSpeed( m_AnimSpeed );
	m_pSkinMesh->SetBlend( true );
	m_pSkinMesh->Render( mView, mProj, stLight, stCamera, m_bChangeTexture, m_pAc );	// 描画.
	m_pSkinMesh->SetBlend( false );

	// 人魂の描画.
	m_pHitoDama->Render( mView, mProj, stLight, stCamera );
}

void CGhostB::Collision( shared_ptr<CObjectBase> pObj )
{
	if( CSpriteResource::GetSprite("Fade")->GetAlpha() >= 0.1f ) return;
	if( pObj == nullptr ) return;
	if( pObj->GetBitFlag( BitFlag::isCameraMoveUp ) == true ) return;
	if( pObj->GetObjectNo() == enObjectNo::Towa ) return;
	m_pHitoDama->Collision( pObj );
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return;
	if( m_pAc == nullptr ) return;

	SearchCollision( pObj );
	ObjSphereCollision( pObj );	// 当たり判定.
}


void CGhostB::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pSkinMesh == nullptr ){
		m_pSkinMesh = CMeshResorce::GetSkin( MAIN_MODEL_NAME );
		if( m_pSkinMesh != nullptr ){
			m_pCollision->Init( m_pSkinMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( SPHERE_COLLISION_RADIUS );
			m_pSkinMesh->ChangeAnimSet(0);
			HRESULT hr;
			hr = m_pSkinMesh->GetAnimController()->CloneAnimationController(
				m_pSkinMesh->GetAnimationController()->GetMaxNumAnimationOutputs(),
				m_pSkinMesh->GetAnimationController()->GetMaxNumAnimationSets(),
				m_pSkinMesh->GetAnimationController()->GetMaxNumTracks(),
				m_pSkinMesh->GetAnimationController()->GetMaxNumEvents(),
				&m_pAc );
			if( hr != S_OK ){
				_ASSERT_EXPR( false, L"取得失敗" );
			}

				
		}
	}
	m_pTexture = make_shared<CDX9SkinMesh::CHANGE_TEXTURES>();

	strcpy_s( m_pTexture->szTargetTextureName, "gosct_b_tex.png" );
	strcpy_s( m_pTexture->szTextureName, "Data\\Model\\Ghost\\GhostB\\gosct_b_tex_yara.png" );


	if( m_pTexture->szTargetTextureName != 0 &&
		FAILED(
			D3DX11CreateShaderResourceViewFromFileA(
				pDevice11, m_pTexture->szTextureName,
				nullptr, nullptr, &m_pTexture->pTexture, nullptr ) ) ){
		MessageBox( NULL, "テクスチャ読み込み失敗",
			"Error", MB_OK );
		return;
	}
	m_pSkinMesh->SetChangeTextures( m_pTexture );

	m_pHitoDama->Load( pDevice11, pContext11 );
}

// 動作関数.
void CGhostB::Action( shared_ptr<CObjectBase> pObj )
{
	if( GetBitFlag( BitFlag::isHit ) == true ){
		m_AnimSpd = 0.0;
		m_bChangeTexture = true;	//ライトが当たっていたらテクスチャ切り替え.
		return;
	} else{
		m_bChangeTexture = false;	//ライトが当たっていないのでテクスチャをデフォルトに.
	}
	if( m_bMoveFlag == false ) return;
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return;
	if( GetBitFlag( BitFlag::isHit ) == true ){
		m_AnimSpd = 0.01;
	}
	if( m_pAc == nullptr ) return;
	if( GetBitFlag( BitFlag::isInvincible ) == true ) return;


	m_bMoveFlag = false;

	m_vRotation.y = GetTwoRadian( m_vPosition, pObj->GetPosition() )+D3DX_PI;


	if( m_pHitoDama->IsAction() == false ){
		m_AnimSpd = 0.01;
	}

	if( m_pAc->GetTime() >= m_pSkinMesh->GetAnimPeriod(0) ){
		AnimInit();
	}

	m_pHitoDama->SetShot( m_vPosition );
}

void CGhostB::AnimInit()
{
	m_AnimSpd = 0.0;
	m_pAc->ResetTime();
}

LPD3DXMESH	CGhostB::GetMeshData()
{
	if( m_pSkinMesh == nullptr ) return nullptr;
	return m_pSkinMesh->GetMesh();
}

void CGhostB::AnimAdd()
{
}