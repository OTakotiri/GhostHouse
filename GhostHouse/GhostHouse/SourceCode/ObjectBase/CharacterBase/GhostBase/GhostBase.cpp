#include "GhostBase.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CGhostBase::CGhostBase()
	: m_bMoveFlag		( false )
	, m_fWaitDistance	( 0.0f )
	, m_bAlphaFlag		( false )
	, m_fCount			( 0 )
	, m_pSearchRange	( nullptr )
	, m_iAngle			( 180 )
{
}

// 待機関数.
void CGhostBase::IdleState()
{
	// sin波で上下に移動する.
	m_vPosition.y += sinf( D3DXToRadian( m_iAngle ) ) * RADIUS * m_fWaitDistance;
	// 角度が360以上になったら0に戻す.
	if( m_iAngle >= 360 )	m_iAngle = 0;
	m_iAngle++;	// 角度を増やす.
}

// 怯み動作関数.
float CGhostBase::FadeChange()
{
	if( m_bAlphaFlag == true || 
		GetBitFlag( BitFlag::isHit ) == true ){
		m_fCount += 2;
		if( m_fCount >= 360 ){
			m_bAlphaFlag = false;
			BitFlagOFF( BitFlag::isHit );
			m_fCount = 0;
		}
		return fabsf(cosf( m_fCount * 0.03f ))+0.5f;
	} else {
		return 1.0f;
	}
}

// 索敵範囲の当たり判定処理.
bool CGhostBase::SearchCollision( shared_ptr<CObjectBase> pObj )
{
	if( m_pSearchRange->isCollision( pObj->GetSphere() ) == true ){
		m_bMoveFlag = true;
		return true;
	} else{
		m_bMoveFlag = false;
		return false;
	}
	return false;
}

// オブジェクトとの当たり判定関数.
bool CGhostBase::ObjSphereCollision( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetBitFlag( BitFlag::isHit ) == true ) return false ;
	if( pObj->GetBitFlag( BitFlag::isInvincible ) == true ) return false;
	if( pObj->GetBitFlag( BitFlag::isDead ) == true ) return false;
	if( GetBitFlag( BitFlag::isHit ) == true ) return false;
	if( GetBitFlag( BitFlag::isInvincible ) == true ) return false;

	// 当たり判定.
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == true ){
		pObj->BitFlagON( BitFlag::isHit );
		return true;
	} else {
		pObj->BitFlagOFF( BitFlag::isHit );
		return false;
	}
	return false;
}

// 2点の距離を測る関数.
float CGhostBase::GetTwoDistance( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float distance = sqrtf( ( vtargetPos.x - vMyPos.x ) * ( vtargetPos.x - vMyPos.x ) 
		+ ( vtargetPos.z - vMyPos.z ) * ( vtargetPos.z - vMyPos.z ) );

	return distance;
}

// 2点の角度を測る関数.
float CGhostBase::GetTwoRadian( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float radian = atan2f( vtargetPos.x - vMyPos.x, vtargetPos.z - vMyPos.z );

	return radian;
}

D3DXVECTOR3 CGhostBase::AxisZProc( float y )
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