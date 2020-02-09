#ifndef PLAYER_BASE_H
#define PLAYER_BASE_H

#include "../CharacterBase.h"

class CPlayerBase : public CCharacterBase
{
	//ˆÚ“®‘¬“x‚ðŠ„‚é’l.
	const float SLOW_DOWN_VALUE = 0.17f;
protected:
	CPlayerBase()
		: m_CanMoveFlag	( false )
		, m_onGround	( false )
		, m_ViewMat		()
		, m_Mat			()
		, vecAxisZ		( 0.0f, 0.0f, 0.0f )
		, m_isWalk		( false )
		, m_vOldPos		( 0.0f, 0.0f, 0.0f )
		, m_vMoveXYtoVec2()
		, m_fMoveVec	()
	{};
	virtual ~CPlayerBase(){};

	virtual void Move();
	virtual void RayHits( shared_ptr<CObjectBase> pObj ) = 0;
	
	bool StickInclination( float divideValue );

	int m_StickCount;
	float m_fRadian;
	bool m_CanMoveFlag;
	bool m_onGround;

	D3DXVECTOR3	m_vOldPos;

	D3DXVECTOR2 m_fMoveVec;
	D3DXVECTOR2 m_vMoveXYtoVec2;

	D3DXMATRIX m_ViewMat;
	D3DXMATRIX m_Mat;

	D3DXVECTOR3 vecAxisZ;
	bool m_isWalk;

};

#endif// #ifndef PLAYER_BASE_H.