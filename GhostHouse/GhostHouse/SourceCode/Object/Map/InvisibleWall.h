#ifndef INVISIBLE_WALL_H
#define INVISIBLE_WALL_H

#include "..\..\ObjectBase\MapBase\EventBoxBase\EventBoxBase.h"

class CInvisibleWall : public CEventBoxBase
{
public:
	CInvisibleWall( const stObjectInfo& objInfo )
	{
		SetObjectInfo( objInfo );
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
		m_fScale = MODEL_SCALE;
	}
	CInvisibleWall()
	{
	}
	virtual ~CInvisibleWall()
	{
	}
};

#endif	// #ifndef INVISIBLE_WALL_H.