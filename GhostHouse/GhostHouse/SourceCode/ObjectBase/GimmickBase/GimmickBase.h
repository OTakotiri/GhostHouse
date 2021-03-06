#ifndef GIMMICK_BASE_H
#define GIMMICK_BASE_H

#include "..\ObjectBase.h"

class CGimmickBase : public CObjectBase
{
protected:
	CGimmickBase()
		: m_bMoveFlag	( false )
	{
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
	};
	virtual ~CGimmickBase(){};

	// 各動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj ) = 0;

protected:
	// 移動フラグ.
	bool	m_bMoveFlag;

};

#endif	// #ifndef GIMMICK_BASE_H.