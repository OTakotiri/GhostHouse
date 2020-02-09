#ifndef EVENT_H
#define EVENT_H

#include "..\..\ObjectBase\MapBase\EventBoxBase\EventBoxBase.h"

class CEvent : public CEventBoxBase
{
public:
	CEvent( const stObjectInfo& objInfo )
	{
		SetObjectInfo( objInfo );
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
		m_fScale = MODEL_SCALE;
	}
	CEvent(){}
	virtual ~CEvent(){}
};

#endif	// #ifndef EVENT_H.