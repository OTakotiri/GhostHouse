#ifndef GHOST_BASE_H
#define GHOST_BASE_H

#include "..\CharacterBase.h"

#include <math.h>

class CGhostBase : public CCharacterBase
{
private:
	// 半径.
	const float RADIUS = 0.001f;

protected:
	CGhostBase();
	virtual ~CGhostBase(){};

	// 各動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj ) = 0;
	// 待機関数.
	virtual void IdleState();
	// 怯み動作関数.
	virtual float FadeChange();
	// 索敵範囲の当たり判定処理関数.
	virtual bool SearchCollision( shared_ptr<CObjectBase> pObj );


	// 2点の距離を測る関数.
	virtual float GetTwoDistance( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos );
	// 2点の角度を測る関数.
	virtual float GetTwoRadian( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos );
		
	// オブジェクトとの当たり判定関数.
	bool ObjSphereCollision( shared_ptr<CObjectBase> pObj );


	// 移動フラグ.
	bool	m_bMoveFlag;

	float	m_fWaitDistance;
	bool	m_bAlphaFlag;
	float	m_fCount;

	unique_ptr<CCollisionSphere>	m_pSearchRange;

	D3DXVECTOR3 AxisZProc( float y );
	
private:
	int		m_iAngle;

};

#endif	// #ifndef GHOST_BASE_H.