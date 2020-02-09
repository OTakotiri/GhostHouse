#include "CollisionManager.h"

CCollisionManager::CCollisionManager( D3DXVECTOR3* pvPos, float* pfRot_y )
	: m_pCollBox	( make_shared<CCollisionBox>( pvPos ) )
	, m_pCollSphere	( make_shared<CCollisionSphere>( pvPos ) )
	, m_pCollRay	( make_shared<CCollisionRay>( pfRot_y ) )
{
}

CCollisionManager::CCollisionManager()
	: m_pCollBox	( nullptr )
	, m_pCollSphere	( nullptr )
	, m_pCollRay	( nullptr )
{
}

CCollisionManager::~CCollisionManager()
{
}

//-----------------.
// ŠeŽí‰Šú‰»ŠÖ”.
//-----------------.
void CCollisionManager::Init( LPD3DXMESH pMesh )
{
	m_pCollBox->InitBBox( pMesh );
	m_pCollSphere->InitSphere( pMesh );
}