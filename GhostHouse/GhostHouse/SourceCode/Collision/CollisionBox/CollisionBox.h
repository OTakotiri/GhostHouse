#ifndef COLLISION_BASE_H
#define COLLISION_BASE_H

#include "..\..\Main\Global.h"
#include "..\..\DebugMode\Box\Box.h"

class CDX9Mesh;

class CCollisionBox
{
public:
	CCollisionBox( D3DXVECTOR3* pvPos );
	~CCollisionBox();
	
	// 判定処理.
	bool isCollision( CCollisionBox* collBox );
	
	// ボックス初期化.
	HRESULT InitBBox( LPD3DXMESH pMesh );
	// ボックスのサイズを変更する.
	void BoxScaleSet( const float& size )
	{
		m_vPositionMax = m_vPositionMax * size;
		m_vPositionMin = m_vPositionMin * size;
	}

	// 各情報取得関数.
	D3DXVECTOR3	GetPosition()	 const { return *m_pvPosition; }
	D3DXVECTOR3	GetPositionMax() const { return m_vPositionMax; }
	D3DXVECTOR3	GetPositionMin() const { return m_vPositionMin; }
	D3DXVECTOR3 GetLength()		 const { return m_vLength; }

private:
	CCollisionBox();

private:
	D3DXVECTOR3*	m_pvPosition;		// 位置情報.		
	D3DXVECTOR3		m_vPositionMax;		// 最大頂点.
	D3DXVECTOR3		m_vPositionMin;		// 最小頂点.

	//	↓ デバッグ用.
	D3DXVECTOR3		m_vLength;			// 長さ.
	float*			m_pfYaw;			// Y軸回転値.

};

#endif	// #ifndef COLLISION_BASE_H.
