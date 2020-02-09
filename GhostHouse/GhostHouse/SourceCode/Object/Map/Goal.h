#ifndef GOAL_H
#define GOAL_H

#include "..\..\ObjectBase\MapBase\EventBoxBase\EventBoxBase.h"

class CGoal : public CEventBoxBase
{
public:
	CGoal( const stObjectInfo& objInfo )
	{
		SetObjectInfo( objInfo );
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
		m_fScale = MODEL_SCALE;
	}
	CGoal(){}
	virtual ~CGoal(){}
};

#endif	// #ifndef GOAL_H.