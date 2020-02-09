#ifndef COLLISION_RAY_H
#define COLLISION_RAY_H

#include "..\..\Main\Global.h"

class CDX9Mesh;
class CObjectBase;

class CCollisionRay
{
public:
	CCollisionRay( float* pfYaw );
	~CCollisionRay();

	// レイとメッシュの当たり判定.
	bool Intersect( shared_ptr<CObjectBase> pTargetObj,
		float* pfDistance,			// (out)距離.
		D3DXVECTOR3* pvIntersect, D3DXVECTOR3 endpos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );	// (out)交差座標.

	// 交差位置のポリゴンの頂点を見つける.
	HRESULT FindVerticesOnPoly(
		LPD3DXMESH pMeshForRay, DWORD dwPolyIndex,
		D3DXVECTOR3* pVecVertices );	// (out)頂点座標(ローカル座標),

	// 回転情報設定関数.
	void SetAxis( const float& fx, const float& fy, const float& fz )
	{ m_vAxis = D3DXVECTOR3( fx, fy, fz ); }
	// 回転情報設定関数.
	void SetAxis( const D3DXVECTOR3& vAxis ) { m_vAxis = vAxis; }
	// レイの設定.
	void SetRay( const D3DXVECTOR3& vRay ) { m_vRay = vRay; }

private:
	CCollisionRay(){};
	float*			m_pfYaw;			// Y軸回転値.
	D3DXVECTOR3		m_vAxis;			// 回転.
	D3DXVECTOR3		m_vRay;				// レイ.
};

#endif	// #ifndef COLLISION_RAY_H.
