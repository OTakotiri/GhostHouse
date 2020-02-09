#include "CharacterBase.h"

// 2点の距離を測る関数.
float CCharacterBase::GetTwoDistance( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float distance = sqrtf( ( vtargetPos.x - vMyPos.x ) * ( vtargetPos.x - vMyPos.x )
		+ ( vtargetPos.z - vMyPos.z ) * ( vtargetPos.z - vMyPos.z ) );

	return distance;
}

// 2点の角度を測る関数.
float CCharacterBase::GetTwoRadian( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float radian = atan2f( vtargetPos.x - vMyPos.x, vtargetPos.z - vMyPos.z );

	return radian;
}

D3DXVECTOR3 CCharacterBase::AxisZProc( float y )
{
	// 軸ベクトルを用意.
	D3DXVECTOR3 vecAxisZ( 0.0f, 0.0f, 1.0f );	// 変換前ベクトル.

	D3DXMATRIX mYaw;	// Y軸回転行列.

	// Y軸回転.
	D3DXMatrixRotationY( &mYaw, y );

	// Z軸ベクトルそのものを回転状態により変換する.
	D3DXVec3TransformCoord( &vecAxisZ,	// (out)返還後ベクトル.
		&vecAxisZ, &mYaw );

	return vecAxisZ;
}