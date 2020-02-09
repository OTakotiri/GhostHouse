#include "CameraUpCollision.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CCameraUpCollision::CCameraUpCollision()
{
	m_vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_vRotation = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

CCameraUpCollision::CCameraUpCollision(const stObjectInfo& objInfo)
	: m_State		( 0 )
	, m_isNowHit	( false )
	, m_isOlsHit	( false )
	, m_pDebugSphere( make_unique<CSphere>() )
{
	SetObjectInfo(objInfo);
	m_fScale = MODEL_SCALE;
}

CCameraUpCollision::~CCameraUpCollision()
{
}

void CCameraUpCollision::Update(shared_ptr<CObjectBase> pObj)
{
	if (m_pStaticMesh == nullptr)	return;

	Action(pObj);
}

void CCameraUpCollision::Render(D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera)
{
	if (m_pStaticMesh == nullptr)	return;
	if (m_bMoveFlag == true)	return;

	if( m_enObjctNo == enObjectNo::InvisibleFloor ){
		m_pStaticMesh->SetPosition(m_vPosition);	// 座標を描画用にセット.
		m_pStaticMesh->SetRotation(m_vRotation);
		m_pStaticMesh->SetAlpha(MODEL_ALPHA);
		m_pStaticMesh->SetScale(MODEL_SCALE);
		m_pStaticMesh->SetBlend(true);
		m_pStaticMesh->Render(mView, mProj, stLight, stCamera);	// 描画.
		m_pStaticMesh->SetBlend(false);
	}
	//m_pDebugSphere->SetPosition(m_vPosition);
	//m_pDebugSphere->Render( mView, mProj );
}

void CCameraUpCollision::Collision(shared_ptr<CObjectBase> pObj)
{
	if( pObj == nullptr ) return;
	if( pObj->GetObjectNo() == enObjectNo::Towa ) return;

	switch( m_State )
	{
	case 0:
		if( pObj->m_CameraState == 1 ){
			m_State++;
		}
		if( m_pCollision->isSphereCollision(pObj->GetSphere()) == true ){
			pObj->BitFlagON( BitFlag::isHitSignboard );
			pObj->BitFlagON( BitFlag::isCameraMoveUp );
			m_State++;
			pObj->m_CameraState++;
		}
		break;
	case 1:
		if( m_pCollision->isSphereCollision(pObj->GetSphere()) == false ){
			if( pObj->m_CameraState == 1 ){
				m_State++;
			}
		}
		break;
	case 2:
		if( pObj->m_CameraState == 2 ){
			m_State++;
		}
		if( m_pCollision->isSphereCollision(pObj->GetSphere()) == true ){
			pObj->BitFlagOFF( BitFlag::isHitSignboard );
			pObj->BitFlagOFF( BitFlag::isCameraMoveUp );
			m_State++;
			pObj->m_CameraState++;
		}
		break;
	case 3:
		if( m_pCollision->isSphereCollision(pObj->GetSphere()) == false ){
			m_State = 0;
			pObj->m_CameraState = 0;
		}
		break;
	default:
		break;
	}
}

void CCameraUpCollision::Load(ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11)
{
	if (m_pStaticMesh == nullptr) {
		m_pStaticMesh = CMeshResorce::GetStatic(MAIN_MODEL_NAME);
		if (m_pStaticMesh != nullptr) {
			m_pCollision->Init(m_pStaticMesh->GetMesh());
			m_pCollision->GetSphere()->SetRadius( 1.5f );
			m_pDebugSphere->Init( pDevice11, pContext11, 1.5f );
		}
	}

}


void CCameraUpCollision::Action(shared_ptr<CObjectBase> pObj)
{
}

LPD3DXMESH CCameraUpCollision::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}