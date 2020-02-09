#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include "..\CollisionBox\CollisionBox.h"
#include "..\CollisionSphere\CollisionSphere.h"
#include "..\CollisionRay\CollisionRay.h"

class CCollisionManager
{
public:
	CCollisionManager( D3DXVECTOR3* pvPos, float* pfRot_y );
	~CCollisionManager();

	// 各種初期化関数.
	void Init( LPD3DXMESH pMesh );

	// ボックス同士での当たり判定.
	bool isBoxCollision( CCollisionBox* collBox )
	{ return m_pCollBox->isCollision( collBox ); }
	// ボックス取得関数.
	CCollisionBox* GetBox(){ return m_pCollBox.get(); };

	// スフィア同士での当たり判定.
	bool isSphereCollision( CCollisionSphere* collSphere )
	{ return m_pCollSphere->isCollision( collSphere ); };
	// スフィア取得関数.
	CCollisionSphere* GetSphere(){ return m_pCollSphere.get(); };

	// レイでの当たり判定.
	bool isRayCollision( shared_ptr<CObjectBase> pTargetObj, float* pDistance, D3DXVECTOR3* pIntersect )
	{ return m_pCollRay->Intersect( pTargetObj, pDistance, pIntersect ); };
	// 回転情報設定関数.
	void RaySetAxis( const float& fx, const float& fy, const float& fz )
	{ m_pCollRay->SetAxis( fx, fy, fz ); }
	void RaySetAxis( const D3DXVECTOR3& vAxis )
	{ m_pCollRay->SetAxis( vAxis ); }
	// レイの方向設定関数.
	void RaySetRay( const D3DXVECTOR3& vRay )
	{ m_pCollRay->SetRay( vRay ); }

private:
	CCollisionManager();
	shared_ptr<CCollisionBox>		m_pCollBox;		// ボックス.
	shared_ptr<CCollisionSphere>	m_pCollSphere;	// スフィア.
	shared_ptr<CCollisionRay>		m_pCollRay;		// レイ.
};

#endif	// #ifndef COLLISION_MANAGER_H.