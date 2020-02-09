#include "Key.h"
#include "..\..\..\XAudio2\PlaySEThread.h"
#include "..\..\..\XInput\XInput.h"
#include "..\..\..\UI\Tutorial\TutorialText.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CKey::CKey( const stObjectInfo& objInfo )
	: m_pStaticMesh	( nullptr )
	, m_pEffect		( nullptr )
	, m_Handle		( -1 )
{
	SetObjectInfo( objInfo );
	m_fScale = MODEL_SCALE;
}

CKey::~CKey()
{
}

void CKey::Update( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() == enObjectNo::Towa ) return;
	if( m_pStaticMesh == nullptr )	return;

	Action( pObj );
}

void CKey::Render( D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera )
{
	if( m_pStaticMesh == nullptr )	return;
	if( m_bMoveFlag == true )	return;

	m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation( m_vRotation );
	m_pStaticMesh->SetAlpha( 1.0f );
	m_pStaticMesh->SetScale( MODEL_SCALE );
	m_pStaticMesh->SetBlend( true );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
	m_pStaticMesh->SetBlend( false );

	if( m_pEffect != nullptr ){
		if( m_Handle == -1 ){
			m_Handle = m_pEffect->Play( D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
			m_pEffect->SetScale( m_Handle, D3DXVECTOR3( 2.0f, 2.0f, 2.0f ) );
		}
		m_pEffect->SetLocation( m_Handle, m_vPosition.x, m_vPosition.y + 1.0f, m_vPosition.z );
		m_pEffect->Render( mView, mProj, m_Handle );
	}

}

void CKey::Collision( shared_ptr<CObjectBase> pObj )
{
}

void CKey::Load( ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11 )
{
	if( m_pStaticMesh == nullptr ){
		m_pStaticMesh = CMeshResorce::GetStatic( MAIN_MODEL_NAME );
		if( m_pStaticMesh != nullptr ){
			m_pCollision->Init( m_pStaticMesh->GetMesh() );
			m_pCollision->GetSphere()->SetRadius( SPHERE_COLLISION_RADIUS );
		}
	}
	if( m_pEffect == nullptr ){
		m_pEffect = CEffectResource::GetEffect("key");
		m_pEffect->StopAll();
	}
}

void CKey::Action( shared_ptr<CObjectBase> pObj )
{
	// アクションボタンフラグを下す.

	// スフィアの当たり判定が当たっていなければ終了.
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == false )	return;

	// アクションボタンフラグを立てる
	CTutorialText::SetArrayNumber(1);
	
	// キー情報を受け取る.
	if( CXInput::IsPress( XINPUT_GAMEPAD_B ) == true ){
		CTutorialText::SetArrayNumber(2);
		m_bMoveFlag = true;
		CPlaySEThread::SetSEName( "GetKey" );
		pObj->BitFlagON( BitFlag::isHavKey );
		m_vPosition = WND_OUT;
	}
}

LPD3DXMESH CKey::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}