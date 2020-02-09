#include "PlayerBase.h"
#include "..\..\..\XInput\XInput.h"
#include "..\..\UI\Fade\FadeUI.h"

bool CPlayerBase::StickInclination( float divideValue )
{
	m_isWalk = false;
	if( CFadeUI::isActive() == true ) return false;
	// スティックの遊びを超えたら入力検知.
	if( CXInput::GetLThumbX() > INPUT_THUMB_MAX ||// / divideValue ||
		CXInput::GetLThumbX() < INPUT_THUMB_MIN ||// /divideValue  ||
		CXInput::GetLThumbY() > INPUT_THUMB_MAX ||// / divideValue ||
		CXInput::GetLThumbY() < INPUT_THUMB_MIN ){ // / divideValue) {

		m_fMoveVec.x = CXInput::GetLThumbX();			//コントローラの左右方向の入力を取得.
		m_fMoveVec.y = CXInput::GetLThumbY();			//コントローラの上下方向の入力を取得.

		m_fRadian = atan2f( m_fMoveVec.x, m_fMoveVec.y );	//atan2fでfloat型のラジアンに変換.
		m_isWalk = true;
		return true;
	}
	return false;
}

void CPlayerBase::Move()
{
	//m_fMoveVec.x = CXInput::GetLThumbX();			//コントローラの左右方向の入力を取得.
	//m_fMoveVec.y = CXInput::GetLThumbY();			//コントローラの上下方向の入力を取得.
	//m_vMoveXYtoVec2 = D3DXVECTOR2( m_fMoveVec.x, m_fMoveVec.y );	//Vec2に変換.
	/*float movespd = 0.17f;*/
	//auto SetMoveSpeed = [&](float spd, float divideValue, int count)
	//{
	//	if (m_StickCount < count) {
	//		if (StickInclination(divideValue) == true) {
	//			m_StickCount = count - 1;
	//			movespd = spd;
	//		}
	//	}
	//};
	//SetMoveSpeed(0.05f, 2.0f, 4);
	//SetMoveSpeed(0.01f, 3.0f, 3);
	//SetMoveSpeed(0.005f, 5.0f, 2);
	StickInclination( NULL );

	if( m_isWalk == true ){
		//軸ベクトルを用意.
		vecAxisZ = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );//変換前ベクトル.
		m_vRotation.y = m_fRadian;

		D3DXMATRIX mYaw;
		//Y軸回転.
		D3DXMatrixRotationY( &mYaw, m_fRadian );

		//Z軸ベクトルそのものを回転状態により変換する.
		D3DXVec3TransformCoord( &vecAxisZ,	//(out)変換後ベクトル.
			&vecAxisZ, &mYaw );

		//-----------------------------------------------------------------------------
		//		カメラの向きに対応させる.
		//////
		if( GetBitFlag( BitFlag::isCameraMoveUp ) == true ){
			//D3DXMatrixInverse(&m_SayakaMat, nullptr, &m_ViewMat);
			m_Mat.m[1][1] = 0;	//Y方向の動き固定.
			m_Mat.m[2][1] = 0;	//Y方向の動き固定.
			D3DXVec3TransformNormal( &vecAxisZ, &vecAxisZ, &m_Mat );

			//m_vPosition = m_vPosition + *D3DXVec3TransformNormal( &vecAxisZ, &vecAxisZ, &m_Mat ) / SLOW_DOWN_VALUE;
			m_vPosition.x = m_vPosition.x + vecAxisZ.x * SLOW_DOWN_VALUE;
			m_vPosition.z = m_vPosition.z + vecAxisZ.z * SLOW_DOWN_VALUE;
		} else{
			D3DXMatrixInverse( &m_Mat, nullptr, &m_ViewMat );	//カメラの向きに対応.
			m_Mat.m[1][1] = 0;	//Y方向の動き固定.
			m_Mat.m[2][1] = 0;	//Y方向の動き固定.
			D3DXVec3TransformNormal( &vecAxisZ, &vecAxisZ, &m_Mat );

			/*m_vPosition = m_vPosition + *D3DXVec3TransformNormal( &vecAxisZ, &vecAxisZ, &m_Mat ) / SLOW_DOWN_VALUE;*/
			m_vPosition.x = m_vPosition.x + vecAxisZ.x * SLOW_DOWN_VALUE;
			m_vPosition.z = m_vPosition.z + vecAxisZ.z * SLOW_DOWN_VALUE;
		}
	}
}
