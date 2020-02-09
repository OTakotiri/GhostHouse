#include "CollisionLight.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

//--------------------------.
// ライト判定処理.
//--------------------------.
bool CCollisionLight::isLightHit( shared_ptr<CObjectBase> pObj, D3DXVECTOR3 vMyPos, D3DXVECTOR3 fRot_y )
{
	float r = LIGHT_RADIUS;						// ライトの半径.

	D3DXVECTOR3 sayakaPos = vMyPos;
	sayakaPos.y += 1.0f;
	D3DXVECTOR3 lightPos = sayakaPos;		// ライトの先の座標用.
	D3DXMATRIX mAttackRot;	// 回転行列.

	// ライトの座標に回転座標を合成する.
	lightPos += fRot_y * LIGHT_DISTANCE;

	D3DXVECTOR3 vTarPosToMyPos		= D3DXVector3Destance( pObj->GetPosition(), sayakaPos );
	D3DXVECTOR3 vLightPosToMyPos	= D3DXVector3Destance( lightPos, sayakaPos );

	float h = fabsf( D3DXVector3Size( sayakaPos ) - D3DXVector3Size( lightPos ) );
	float vDot = D3DXVec3Dot( &vTarPosToMyPos, &vLightPosToMyPos );

	D3DXVECTOR3 vCloss;
	D3DXVec3Cross( &vCloss, &vTarPosToMyPos, &vLightPosToMyPos );

	if( ( 0.0f <= vDot && vDot <= h*h ) &&
		( fabsf( D3DXVector3Size(vCloss) ) <= fabsf( D3DXVector3Size(vTarPosToMyPos) ) * r*h / sqrtf(r*r + h*h) ) ){
		return true;
	}
	return false;
	
}

//--------------------------.
// 二つの距離を計算.
//--------------------------.
D3DXVECTOR3 CCollisionLight::D3DXVector3Destance( D3DXVECTOR3 vec, D3DXVECTOR3 vec2 )
{
	D3DXVECTOR3 c;

	c.y = vec.y - vec2.y;
	c.z = vec.z - vec2.z;
	c.x = vec.x - vec2.x;

	return c;
}

//--------------------------.
// ベクターのサイズを取得.
//--------------------------.
float CCollisionLight::D3DXVector3Size( D3DXVECTOR3 vec )
{
	return sqrtf( powf( vec.x, 2 ) + powf( vec.y, 2 ) + powf( vec.z, 2 ) );
}