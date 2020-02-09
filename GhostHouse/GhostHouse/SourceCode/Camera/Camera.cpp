#include "Camera.h"
#include <D3DX10Math.h>
#include "..\Main\MyMacro.h"
#include "..\ObjectBase\ObjectBase.h"


CCamera::CCamera()
	: m_vCamera( 0.0f, 0.0f, 0.0f )
	, m_vLook( 0.0f, 0.0f, 0.0f )
	, OldCamera()
	, m_bCameraNowMove( true )
	, m_vGimmickPosition( 0.0f, 0.0f, 11.0f )
	, m_vInvisiColPosiiton()
	, CameraRotationToRadian( 0.0f )
	, CameraRotationToDegree( 0.0f )
	, Oneflow( false )
	, m_pCollRay( make_unique<CCollisionRay>( &CameraRotationToRadian ) )
	, m_Step( 2 )
	, m_fColDistance( 0.0f )
	, m_fColRadian( 0.0f )
	, m_fInvGimmickDistance( 0.0f )
	, m_fInvGimmickRadian( 0.0f )
{

	Camera.m_CenterX = 0.0f;
	Camera.m_CenterY = 0.0f;
	Camera.m_CenterZ = 0.0f;
	Camera.m_Length = 5.0f;
	Camera.m_Degree = 0.0f;
	Camera.m_Degree_y = 2.0f;
	CameraRotationToRadian = 0.0f;
	Camera.m_vPos.x = 0.0f;
	Camera.m_vPos.y = 5.0f;
	Camera.m_vPos.z = 0.0f;

	stCamera.fDegree = 0.0f;
	stCamera.fLength = 18.0f;
	stCamera.Pos = D3DXVECTOR3( 0.0f, 10.0f, 0.0f );
	stCamera.vRot = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	stCamera.InvisibleCenter = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

CCamera::~CCamera()
{
}
// ゲームメインの周りをまわるカメラのアップデート.
void CCamera::UpdateCamera()
{
	if( CXInput::GetRThumbX() > STICKRANGE || GetAsyncKeyState( VK_RIGHT ) & 0x8000 ){
		stCamera.fDegree += CAMERA_ROTATION_SPEED;
	}

	if( CXInput::GetRThumbX() < -STICKRANGE || GetAsyncKeyState( VK_LEFT ) & 0x8000 ){
		stCamera.fDegree -= CAMERA_ROTATION_SPEED;
	}
}
// マップエディットモードカメラアップデート.
void CCamera::EditUpdateCamera()
{
	if( GetAsyncKeyState( VK_RIGHT ) & 0x8000 || CXInput::GetRThumbX() > INPUT_THUMB_MAX ){
		Camera.m_Degree += EDITCAMERA_DEGREE;
	}

	if( GetAsyncKeyState( VK_LEFT ) & 0x8000 || CXInput::GetRThumbX() < INPUT_THUMB_MIN ){
		Camera.m_Degree -= EDITCAMERA_DEGREE;
	}

	if( GetAsyncKeyState( VK_UP ) & 0x8000 || CXInput::GetRThumbY() > INPUT_THUMB_MAX ){
		Camera.m_Degree_y -= 1.0f;
		if( Camera.m_Degree_y <= 2.0f ){
			Camera.m_Degree_y = 2.0f;
		}
	}

	if( GetAsyncKeyState( VK_DOWN ) & 0x8000 || CXInput::GetRThumbY() < INPUT_THUMB_MIN ){
		Camera.m_Degree_y += 1.0f;
		if( Camera.m_Degree_y >= 80.0f ){
			Camera.m_Degree_y = 80.0f;
		}
	}

	if( GetAsyncKeyState( 'Q' ) & 0x0001 || CXInput::GetRTrigger() > 200 ){
		Camera.m_Length += 0.5f;
		if( Camera.m_Length >= 50.0f ){
			Camera.m_Length = 50.0f;
		}
	}
	if( GetAsyncKeyState( 'E' ) & 0x0001 || CXInput::GetLTrigger() > 200 ){
		Camera.m_Length -= 0.5f;
		if( Camera.m_Length <= 3.0f ){
			Camera.m_Length = 3.0f;
		}
	}
}

D3DXVECTOR3 CCamera::GimmickUpCamera( const D3DXVECTOR3& vInvisibleCenPos, const D3DXVECTOR3& vInvisibleColPos )
{
	if( m_fInvGimmickDistance > 0.8f ){
		if( Oneflow == false ){
			m_Step = 0;
			Oneflow = true;
			//m_bIsEndMove = false;
			SetGimickCameraMoveEnd( false );
		}
	}
	// 見えない床ギミックでカメラを上に向かわせるステップ.
	switch( m_Step ){
		case 0:		
			m_vInvisiColPosiiton = vInvisibleColPos;			// 見えない床のギミック始動当たり判定座標取得.
			// カメラの座標と見えない床の当たり判定座標の距離を取得.
			m_fColDistance = GetTwoDistance( stCamera.Pos, m_vInvisiColPosiiton );
			// カメラの座標と見えない床の当たり判定角度を取得.
			m_fColRadian = GetTwoRadian( stCamera.Pos, m_vInvisiColPosiiton );
			// 距離が指定値より大きい間中に入る.
			if( m_fColDistance > MAX_GIMMICKCOLPOS_DISTANCE){
				// カメラの方向を見えない床に向ける.
				stCamera.vRot.y = m_fColRadian;
				// カメラ移動.
				stCamera.Pos += AxisZProc( stCamera.vRot.y ) * MOVE_SPEED;
			} else{
				// 距離が十分なのでステップを進める.
				m_Step++;
			}
			CameraRotationToRadian = m_fColRadian;
			// カメラの座標更新.
			m_vCamera = { stCamera.Pos.x, stCamera.Pos.y + CAMERA_UP_DISTANCE, stCamera.Pos.z };

			return m_vCamera;
		case 1:
			m_vGimmickPosition = vInvisibleCenPos;	// 見えない床の中心座標取得.
			// カメラの座標と見えない床の中心座標の距離を取得.
			m_fInvGimmickDistance = GetTwoDistance( stCamera.Pos, m_vGimmickPosition );
			// カメラが戻った際に進行方向にカメラが向くように角度を変更.
			stCamera.fDegree = D3DXToDegree(GetTwoRadian(stCamera.Pos, m_vGimmickPosition));
			// カメラの座標と見えない床の中心座標の角度を取得.
			m_fInvGimmickRadian = GetTwoRadian( stCamera.Pos, m_vGimmickPosition );
			// 距離が指定値より大きい間中に入る.
			if( m_fInvGimmickDistance > MAX_GIMMICKPOS_DISTANCE){
				// カメラの方向を見えない床に向ける.
				stCamera.vRot.y = m_fInvGimmickRadian;
				// カメラ移動.
				stCamera.Pos += AxisZProc( stCamera.vRot.y ) * (0.1f);
			} else {
				// 距離は十分なのでステップを進める.
				m_Step++;
				// 上で設定した奴を180度回す
				stCamera.fDegree += 180.0f;
			}
			CameraRotationToRadian = m_fInvGimmickRadian;
			// カメラの座標更新.
			m_vCamera = { stCamera.Pos.x, stCamera.Pos.y + CAMERA_UP_DISTANCE, stCamera.Pos.z };
			// カメラの注視点を見えない床の中心座標に.
			stCamera.vLook = vInvisibleCenPos;
			return m_vCamera;
		case 2:
			m_vGimmickPosition = vInvisibleCenPos;
			m_fInvGimmickDistance = GetTwoDistance( stCamera.Pos, m_vGimmickPosition );
			m_fInvGimmickRadian = GetTwoRadian( stCamera.Pos, m_vGimmickPosition );
			
			Oneflow = false;
			CameraRotationToRadian = m_fInvGimmickRadian;
			m_vCamera = { stCamera.Pos.x, stCamera.Pos.y + CAMERA_UP_DISTANCE, stCamera.Pos.z };
			stCamera.vLook = vInvisibleCenPos;
			// カメラの移動を終了する.
			SetGimickCameraMoveEnd( true );
			return m_vCamera;
		default:
			break;
	}

	return m_vCamera;
}

D3DXVECTOR3 CCamera::MoveCameraPos( const D3DXVECTOR3& vCenPos )
{
	// ゲーム全体の視点の座標を取得する.
	Camera.m_CenterX = vCenPos.x;
	Camera.m_CenterY = vCenPos.y;
	Camera.m_CenterZ = vCenPos.z;

	//オーバーフロー対策.
	CheckOverFlow(stCamera.fDegree);
	//if( stCamera.fDegree >= 360.0f ){
	//	stCamera.fDegree -= 360.0f;
	//} else if( stCamera.fDegree <= -360.0f ){
	//	stCamera.fDegree += 360.0f;
	//}

	//角度をラジアン値に変換.
	CameraRotationToRadian =
		stCamera.fDegree * ( 3.14f / 180.0f );


	//sin,cos:-1.0～1.0f.
	stCamera.Pos.x =
		Camera.m_CenterX + ( sinf( CameraRotationToRadian ) * stCamera.fLength );

	stCamera.Pos.z =
		Camera.m_CenterZ + ( cosf( CameraRotationToRadian ) * stCamera.fLength );

	m_vCamera = { stCamera.Pos.x, stCamera.Pos.y, stCamera.Pos.z };

	return m_vCamera;
}

D3DXVECTOR3 CCamera::EditMoveCameraPos( const D3DXVECTOR3& vCenPos )
{
	// ゲーム全体の視点の座標を取得する.
	Camera.m_CenterX = vCenPos.x;
	Camera.m_CenterY = vCenPos.y;
	Camera.m_CenterZ = vCenPos.z;

	//オーバーフロー対策.
	CheckOverFlow(Camera.m_Degree);
	CheckOverFlow(Camera.m_Degree_y);

	//if( Camera.m_Degree >= 360.0f ){
	//	Camera.m_Degree -= 360.0f;
	//} else if( Camera.m_Degree <= -360.0f ){
	//	Camera.m_Degree += 360.0f;
	//}
	//if( Camera.m_Degree_y >= 360.0f ){
	//	Camera.m_Degree_y -= 360.0f;
	//} else if( Camera.m_Degree_y <= -360.0f ){
	//	Camera.m_Degree_y += 360.0f;
	//}

	//角度をラジアン値に変換.
	CameraRotationToRadian =
		Camera.m_Degree * ( 3.14f / 180.0f );

	float y = Camera.m_Degree_y * ( 3.14f / 180.0f );


	//sin,cos:-1.0～1.0f.
	Camera.m_vPos.x =
		Camera.m_CenterX + ( sinf( CameraRotationToRadian ) * Camera.m_Length );

	Camera.m_vPos.y =
		Camera.m_CenterY + ( sinf( y ) * Camera.m_Length );

	Camera.m_vPos.z =
		Camera.m_CenterZ + ( cosf( CameraRotationToRadian ) * Camera.m_Length );

	m_vCamera = { Camera.m_vPos.x, Camera.m_vPos.y, Camera.m_vPos.z };

	return m_vCamera;
}

void CCamera::RayColl( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() == enObjectNo::Ground ) return;
	float fDistance;
	D3DXVECTOR3 vIntersect;
	D3DXVECTOR3 vEndPos;
	D3DXVECTOR3 vCameraPos = stCamera.Pos;
	D3DXVECTOR3 vLookPos = D3DXVECTOR3( Camera.m_CenterX, Camera.m_CenterY, Camera.m_CenterZ );
	m_pCollRay->SetRay( vLookPos );

	float range = -0.5f;
	for( int i = 0; i < 50; i++ ){
		vCameraPos.x =
			Camera.m_CenterX + ( sinf( CameraRotationToRadian + range ) * 18.0f );
		vCameraPos.z =
			Camera.m_CenterZ + ( cosf( CameraRotationToRadian + range ) * 18.0f );

		if( m_pCollRay->Intersect( pObj, &fDistance, &vIntersect, vCameraPos ) == true ){
			pObj->SetAlpha( 0.51f );
		}
		range += 0.02f;
	}
}


// 2点の距離を測る関数.
float CCamera::GetTwoDistance( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float distance = sqrtf( ( vtargetPos.x - vMyPos.x ) * ( vtargetPos.x - vMyPos.x )
		+ ( vtargetPos.z - vMyPos.z ) * ( vtargetPos.z - vMyPos.z ) );

	return distance;
}

// 2点の角度を測る関数.
float CCamera::GetTwoRadian( const D3DXVECTOR3& vMyPos, const D3DXVECTOR3& vtargetPos )
{
	float radian = atan2f( vtargetPos.x - vMyPos.x, vtargetPos.z - vMyPos.z );

	return radian;
}

D3DXVECTOR3 CCamera::AxisZProc( float y )
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