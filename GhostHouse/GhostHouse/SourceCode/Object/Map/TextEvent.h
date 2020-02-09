#ifndef C_TEXTEVENT_H
#define C_TEXTEVENT_H

#include "..\..\ObjectBase\MapBase\EventBoxBase\EventBoxBase.h"

class CTextEvent : public CEventBoxBase
{
public:
	CTextEvent( const stObjectInfo& objInfo )
	{
		SetObjectInfo( objInfo );
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
		m_fScale = MODEL_SCALE;
	}
	CTextEvent()
	{
	}
	virtual ~CTextEvent()
	{
	}
};

#endif	// #ifndef C_TEXTIVENT_H.