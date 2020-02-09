#include "Player.h"
#include "..\..\XInput\XInput.h"

#include "..\..\DebugMode\DebugText\DebugText.h"
#include "..\ModelResource\ModelResource.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"

CPlayer::CPlayer()
	: m_pCamera		( make_shared<CCamera>() )

	, m_pCollRay	( make_unique<CCollisionRay>( &m_vRotation.y) )
	, m_pbufObject	( nullptr )
	, m_vbufPosition( 0.0f, 0.0f, 0.0f )
	, m_isMove		( false )
	, m_StickCount	( 0 )
	, m_fRadian		( 0.0f )
	, m_vMove		( 0.0f, 0.0f )
	, m_mView		()
{
	m_vPosition = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
}

CPlayer::~CPlayer()
{}

//-------------------------.
// 更新関数.
//-------------------------.
void CPlayer::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pStaticMesh == nullptr ) return;

	m_pCamera->EditUpdateCamera();
}

//-------------------------.
// 描画関数.
//-------------------------.
void CPlayer::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera )
{
	m_mView = mView;
	if( m_pStaticMesh == nullptr ) return;

	stCamera.vLook = m_vPosition;
	stCamera.vLook.y += 2.0f;
	stCamera.Pos = m_pCamera->EditMoveCameraPos( stCamera.vLook );
	m_vRotation = D3DXVECTOR3( 0.0f,  m_pCamera->GetCameraRadian() + m_fRadian, 0.0f );
	m_pStaticMesh->SetRotation( m_vRotation ); 
	m_pStaticMesh->SetPosition( m_vPosition );
	m_pStaticMesh->SetScale( 0.08f );
	m_pStaticMesh->Render( mView, mProj, stLight, stCamera );
}

//-------------------------.
// 読み込み関数.
//-------------------------.
void CPlayer::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	auto MyMeshLoad = [=]()
	{
		if( m_pStaticMesh != nullptr ) return;
		m_pStaticMesh = CEditModelResource::GetMesh("Sayaka_s");
	};
	
	MyMeshLoad();
}

//-------------------------.
// スティックの傾き取得.
//-------------------------.
bool CPlayer::StickInclination( float divideValue )
{
	m_isMove = false;
	// スティックの遊びを超えたら入力検知.
	if( CXInput::GetLThumbX() > INPUT_THUMB_MAX ||
		CXInput::GetLThumbX() < INPUT_THUMB_MIN ||
		CXInput::GetLThumbY() > INPUT_THUMB_MAX || 
		CXInput::GetLThumbY() < INPUT_THUMB_MIN ){

		m_vMove.x = CXInput::GetLThumbX();			//コントローラの左右方向の入力を取得.
		m_vMove.y = CXInput::GetLThumbY();			//コントローラの上下方向の入力を取得.

		m_fRadian = atan2f( m_vMove.x, m_vMove.y );	//atan2fでfloat型のラジアンに変換.
		m_isMove = true;
		return true;
	}
	return false;
}

//-------------------------.
// 操作関数.
//-------------------------.
void CPlayer::Control()
{
	float movespd = 1.0f;

	auto SetMoveSpeed = [&]( float spd, int count )
	{
		if( m_StickCount < count ){
			if( StickInclination( 1.0f ) == true ){
				m_StickCount = count-1;
				movespd = spd;
			}
		}
	};
	SetMoveSpeed( 0.2f, 4 );

	if( m_isMove == true ){
		//軸ベクトルを用意.
		D3DXVECTOR3 vAxisZ = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );//変換前ベクトル.

		D3DXMATRIX mYaw;//Y軸回転行列.
		//Y軸回転.
		D3DXMatrixRotationY( &mYaw, m_fRadian );

		//Z軸ベクトルそのものを回転状態により変換する.
		D3DXVec3TransformCoord( &vAxisZ,	//(out)変換後ベクトル.
			&vAxisZ, &mYaw );

		//-----------------------------------------------------------------------------
		//		カメラの向きに対応させる.
		//
		D3DXMATRIX	m_SayakaMat;
		D3DXMatrixInverse( &m_SayakaMat, nullptr, &m_mView );
		m_SayakaMat.m[1][1] = 0;	//Y方向の動き固定.
		m_SayakaMat.m[2][1] = 0;	//Y方向の動き固定.

		D3DXVec3TransformNormal( &vAxisZ, &vAxisZ, &m_SayakaMat );
		
		m_vbufPosition.x = m_vPosition.x = m_vPosition.x + vAxisZ.x * movespd;
		m_vbufPosition.z = m_vPosition.z = m_vPosition.z + vAxisZ.z * movespd;
		m_vbufPosition = D3DXVECTOR3(
			ceilf(m_vbufPosition.x),
			ceilf(m_vbufPosition.y),
			ceilf(m_vbufPosition.z) );

		m_StickCount = 0;
	}
	if( CXInput::IsPress( XINPUT_GAMEPAD_LEFT_SHOULDER ) ){
		m_vbufPosition.y -= 1.0f;
	} else if( CXInput::IsPress( XINPUT_GAMEPAD_RIGHT_SHOULDER ) ){
		m_vbufPosition.y += 1.0f;
	}
}

//-------------------------.
// レイでの当たり判定.
//-------------------------.
void CPlayer::RayHits( shared_ptr<CObjectBase> pObj )
{
	float fDistance;
	D3DXVECTOR3 vIntersect;
	D3DXVECTOR3 vPos = m_vPosition;

	// 下向きレイの設定.
	m_pCollRay->SetAxis( 0.0f, -1.0f, 0.0f );

	// 対象オブジェクトが見えない床(落ちる床)の場合は終了.
	if( pObj->GetObjectNo() == enObjectNo::SlipThroughFloor )return;

	// 対象のオブジェクトとのレイ判定.
	if( m_pCollRay->Intersect( pObj, &fDistance, &vIntersect ) == true ){
		// 対象オブジェクトの上に設定する.
		m_vPosition.y = vIntersect.y;
	} else{
		// 下に何も無ければ下に移動させる.
		m_vPosition.y -= 0.001f;
	}
}

//-------------------------.
// メッシュ取得関数.
//-------------------------.
LPD3DXMESH CPlayer::GetMeshData()
{
	if( m_pStaticMesh == nullptr ) return nullptr;
	return m_pStaticMesh->GetMesh();
}