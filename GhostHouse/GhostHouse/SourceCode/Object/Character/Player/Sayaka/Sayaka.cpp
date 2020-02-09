#include "Sayaka.h"
#include "..\..\..\..\XAudio2\PlaySEThread.h"
#include "..\..\Resource\MeshResource\MeshResource.h"
#include "..\..\UI\Fade\FadeUI.h"

CSayaka::CSayaka()
	: m_pSkinMesh	( nullptr )
	, m_pCamera		( make_shared<CCamera>() )
	, m_pCollLight	( make_unique<CCollisionLight>() )
	, m_pTexture	( nullptr )
	, m_pBox		( make_unique<CBox>() )

	, m_NowGroundType	( enObjectNo::None )
	, m_LightAxisZ		( 0.0f, 0.0f, 0.0f )

	, m_enNowAnimNo		( SayakaAnim::Wait )
	, m_enOldAnimNo		( SayakaAnim::Wait )
	, m_dAnimeSpeed		( ANIM_SPEED.at(m_enNowAnimNo) )
	, m_dDeadAnimTime	( 0.0 )

	, m_bInvincibleCount	( 0 )
	, m_FlashCount			( 0 )
	, m_bDispFlag			( true )
	, m_isUpdate			( false )
	, m_bChangeTexture		( false )

{
}

CSayaka::CSayaka( const stObjectInfo& objInfo )
	: CSayaka	()
{
	SetObjectInfo( objInfo );
	m_LifePoint = MAX_LIFE_POINT;
	m_vPosition.y += 0.0f;
	BitFlagOFF(BitFlag::isCameraMoveUp);
	BitFlagOFF(BitFlag::isInvincible);
	BitFlagON( BitFlag::isMovement );
	m_fScale		= MODEL_SCALE;
}

CSayaka::~CSayaka()
{
}

void CSayaka::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	if( m_pSkinMesh != nullptr ) return;
	m_pSkinMesh = CMeshResorce::GetSkin( MAIN_MODEL_NAME );
	if( m_pSkinMesh == nullptr ) return;
	m_pSkinMesh->ChangeAnimSet( SayakaAnim::Wait );

	m_pCollision->Init( m_pSkinMesh->GetMesh() );
	m_pCollision->GetSphere()->SetRadius( SPHERE_COLLISION_RADIUS );
	m_pCollision->GetBox()->BoxScaleSet( m_fScale );
	m_pBox->Init(
		pDevice11, pContext11,
		m_pCollision->GetBox()->GetPositionMax(),
		m_pCollision->GetBox()->GetPositionMin() );

	TextureLoad( pDevice11 );
	m_pSkinMesh->SetChangeTextures( m_pTexture );
}

void CSayaka::Update( shared_ptr<CObjectBase> pObj )
{
	if( m_pSkinMesh == nullptr ) return;

	if( GetBitFlag( BitFlag::isGoal ) == true ){
		m_bChangeTexture = true;
		LookCameraPos();
		return;
	}
	AnimWalkSoundPlay();
	//カメラの位置切り替え(ステージ回り or 見えない床の上).
	if( GetBitFlag( BitFlag::isStopCamera ) == false ){
		if( GetBitFlag( BitFlag::isCameraMoveUp ) ){
			/*m_pCamera->GimmickUpCamera(stCamera.InvisibleCenter);*/
		} else{
			m_pCamera->UpdateCamera();
		}
	}

	//各ゴーストからヒットフラグ(攻撃)が返ってきた&自身が無敵状態ではない場合.
	if( GetBitFlag( BitFlag::isInvincible ) == false &&
		GetBitFlag(BitFlag::isHit) == true ){
		CPlaySEThread::SetSEName( "Damage" );
		m_LifePoint--;			// ライフ1減少.
		BitFlagON( BitFlag::isInvincible );
		BitFlagOFF( BitFlag::isHit );
	}

	// 無敵時間カウント.
	InvincibleTime();
	// 無敵中サヤカ点滅.
	SayakaFlasing();

	if( pObj != nullptr ){
		if( pObj->GetBitFlag( BitFlag::isMovement ) == true ){
			BitFlagOFF( BitFlag::isMovement );
			m_enOldAnimNo = SayakaAnim::Wait;
			if( m_LifePoint <= 0 && m_enOldAnimNo != SayakaAnim::Dead ){
				m_enOldAnimNo = SayakaAnim::Dead;
			}
			m_isWalk = false;
			Respawn();
			return;
		} else {
			BitFlagON( BitFlag::isMovement );
		}
	}

	if( GetBitFlag( BitFlag::isMovement ) == false ) return;
	ControllSayaka();

	Respawn();
	m_isUpdate = true;
	if (GetAsyncKeyState('W') & 0x8000) {
		m_vPosition.z += 0.1f;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		m_vPosition.z -= 0.1f;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		m_vPosition.x -= 0.1f;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		m_vPosition.x += 0.1f;
	}
}

void CSayaka::Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& Light, stCAMERA& stCamera )
{
	if( m_pSkinMesh == nullptr ) return;

	if( GetBitFlag( BitFlag::isGoal ) == false ){
		m_bChangeTexture = false;
	}

	AnimetionControll();
	DeadAnimControll();
	// さやかの描画.
	m_pSkinMesh->SetPosition( m_vPosition );
	m_pSkinMesh->SetScale( MODEL_SCALE );
	m_pSkinMesh->SetAnimSpeed( m_dAnimeSpeed );//ｱﾆﾒｰｼｮﾝ速度設定.
	if( m_bDispFlag == true ){
		m_pSkinMesh->SetBlend( true );
		m_pSkinMesh->SetAlpha( m_fAlpha );
		m_pSkinMesh->Render( mView, mProj, Light, stCamera, m_bChangeTexture );
		m_pSkinMesh->SetBlend( false );
	}

	//ライトをサヤカに合わせて回転.
	if( GetBitFlag( BitFlag::isStopCamera ) == false && m_LifePoint > 0
		&& CFadeUI::isActive() == false && m_isUpdate == true && m_isGamePause == false ){
		SetLightMatrix( Light );
	}
	m_ViewMat = mView;

	if( GetBitFlag( BitFlag::isStopCamera ) == false ){
		if( GetBitFlag(BitFlag::isCameraMoveUp) == true ){
	//		m_pCamera->GimmickUpCamera( stCamera.InvisibleCenter );
			stCamera.Pos = m_pCamera->GimmickUpCamera( stCamera.InvisibleCenter, stCamera.InvisibleCollisionPos );

		} else {
			if( m_isUpdate == true ){
				if( GetBitFlag( BitFlag::isMovement ) == true ){
					stCamera.Pos = m_pCamera->MoveCameraPos( stCamera.vLook );
					stCamera.vLook = m_vPosition;
				}
			} else {
				stCamera.vLook = m_vPosition;
			}
		}
	} else {
		stCamera.vLook = D3DXVECTOR3( m_vPosition.x, m_vPosition.y + 5.0f, m_vPosition.z );
		stCamera.Pos = m_pCamera->MoveCameraPos( stCamera.vLook );
		m_pCamera->ChangeDegree( D3DXToDegree( ( m_vRotation.y ) ) );
		D3DXVECTOR3 Tmp( m_vRotation.x, m_vRotation.y, m_vRotation.z );
		m_pSkinMesh->SetRotation( Tmp );
	}

	if( GetBitFlag( BitFlag::isGoal ) == true ){
		stCamera.fDegree = 2.0f;
		LookCameraPos();
		m_pSkinMesh->SetRotation( m_vRotation );
		return;
	}

//リリースビルドはD3DX_PIを消す.
#ifdef _DEBUG
	// スティックの遊びを超えたら入力検知.
	if( GetBitFlag( BitFlag::isStopCamera ) == false && m_LifePoint > 0 
		&& CFadeUI::isActive() == false && m_isUpdate == true && m_isGamePause == false ){
		if( CXInput::GetLThumbX() > INPUT_THUMB_MAX || CXInput::GetLThumbX() < INPUT_THUMB_MIN ||
			CXInput::GetLThumbY() > INPUT_THUMB_MAX || CXInput::GetLThumbY() < INPUT_THUMB_MIN ){
			if( GetBitFlag( BitFlag::isCameraMoveUp ) == true ){
				D3DXVECTOR3 vRot( 0.0f, m_pCamera->GetCameraRadian() + m_vRotation.y + D3DX_PI, 0.0f );
				m_pSkinMesh->SetRotation( vRot );	//Y軸を回転.
			} else{
				D3DXVECTOR3 vRot( 0.0f, m_pCamera->GetCameraRadian() + m_vRotation.y, 0.0f );
				m_pSkinMesh->SetRotation( vRot );	//Y軸を回転.
			}
		}
	}
#else
	if( GetBitFlag( BitFlag::isStopCamera ) == false && m_LifePoint > 0 
		&& CFadeUI::isActive() == false && m_isUpdate == true && m_isGamePause == false ){
		if( CXInput::GetLThumbX() > INPUT_THUMB_MAX || CXInput::GetLThumbX() < INPUT_THUMB_MIN ||
			CXInput::GetLThumbY() > INPUT_THUMB_MAX || CXInput::GetLThumbY() < INPUT_THUMB_MIN ){
			if( GetBitFlag( BitFlag::isCameraMoveUp ) == true ){
				D3DXVECTOR3 vRot( 0.0f, m_pCamera->GetCameraRadian() + m_vRotation.y + D3DX_PI, 0.0f );
				m_pSkinMesh->SetRotation( vRot );	//Y軸を回転.
			} else{
				D3DXVECTOR3 vRot( 0.0f, m_pCamera->GetCameraRadian() + m_vRotation.y, 0.0f );
				m_pSkinMesh->SetRotation( vRot );	//Y軸を回転.
		}
	}
}
#endif
	if( m_isUpdate == false ){
		m_pCamera->SetDegree( 0.1f );
		stCamera.Pos = m_pCamera->MoveCameraPos( stCamera.vLook );
	}
}

void CSayaka::Collision( shared_ptr<CObjectBase> pObj )
{
	if( pObj == nullptr ) return;
	if( GetBitFlag( BitFlag::isMovement ) == true ){
		m_pCamera->RayColl( pObj );
	}
	RayHits( pObj );
	EventBoxHit( pObj );
	TextEventBoxHit( pObj );
	GoalBoxHit( pObj );
}

void CSayaka::ControllSayaka()
{
	if( CSpriteResource::GetSprite( "Fade" )->GetAlpha() >= 0.1f ){
		m_enOldAnimNo = SayakaAnim::Wait;
		m_isWalk = false;
		return;
	}

	if( m_LifePoint <= 0 ) return;
	Move();
}

void CSayaka::SetLightMatrix( Light& light )
{
	if( m_LifePoint <= 0 ) return;
	light.cone = 90.0f;
	light.range = 7.0f;
	light.pos = (D3DXVECTOR3(m_vPosition.x, m_vPosition.y + 0.5f, m_vPosition.z));
	if( CXInput::GetLThumbX() > INPUT_THUMB_MAX || CXInput::GetLThumbX() < INPUT_THUMB_MIN ||
		CXInput::GetLThumbY() > INPUT_THUMB_MAX || CXInput::GetLThumbY() < INPUT_THUMB_MIN ){
		//---------------------------------------------------------------
		//	ライトに渡す回転行列作成.
		// カメラの持つ回転Radian値 + コントローラのスティックのRadian値 + PI
		D3DXMATRIX mRot, mYaw, mPitch, mRoll;
		if( GetBitFlag(BitFlag::isCameraMoveUp) == true ){
			D3DXMatrixRotationY( &mYaw, m_pCamera->GetCameraRadian() + m_vRotation.y );	// 単純にY軸回転.
		} else{
			D3DXMatrixRotationY( &mYaw, m_pCamera->GetCameraRadian() + m_vRotation.y + D3DX_PI );	// 単純にY軸回転.
		}

		D3DXMatrixRotationX( &mPitch, 0.0f );	// 単純にX軸回転.
		D3DXMatrixRotationZ( &mRoll, 0.0f );	// 単純にZ軸回転.

		//Z軸ベクトルそのものを回転状態により変換する.
		D3DXVec3TransformCoord( &vecAxisZ,	//(out)変換後ベクトル.
			&vecAxisZ, &mYaw );
		// 回転行列を作成.
		//mRot = mYaw * mPitch * mRoll;
		mRot = mYaw * mPitch * mRoll;
		D3DXMatrixTranspose( &mRot, &mRot );
		//ライト構造体に回転行列を渡す.
		light.Rot = mRot;
		D3DXVec3TransformCoord( &m_LightAxisZ,	//(out)変換後ベクトル.
			&vecAxisZ, &mRot );
		//--------------------------------------------------------------
		//ライトにポジションを渡す.
		light.pos = ( D3DXVECTOR3( m_vPosition.x, m_vPosition.y+0.5f, m_vPosition.z ) );
	}
}


void CSayaka::Move()
{
	if( m_LifePoint <= 0 ) return;
	if( GetBitFlag( BitFlag::isCameraMoveUp ) == true ){
		if( m_pCamera->GetNowStep() == 0 ){
			D3DXMatrixInverse( &m_Mat, nullptr, &m_ViewMat );	//カメラの向きに対応.
		}
	}
	if( GetBitFlag( BitFlag::isSignBoardRender) == true ) return;
	if( m_pCamera->GetNowStep() == 0 || m_pCamera->GetNowStep() == 1 ){
		m_enOldAnimNo = SayakaAnim::Wait;
		m_isWalk = false;
		return;
	}
	if( m_pCamera->IsCameraUpMove() == true ){
		CPlayerBase::Move();
	}
}

void CSayaka::AnimetionControll()
{
	if( m_enNowAnimNo == SayakaAnim::Dead ) return;

	if( m_enOldAnimNo != SayakaAnim::Dead ){
		if( m_isWalk == true ){
			m_enOldAnimNo = SayakaAnim::Walk;
		} else {
			m_enOldAnimNo = SayakaAnim::Wait;
		}
	}

	if( GetBitFlag( BitFlag::isGoal ) == true ){
		m_enOldAnimNo = SayakaAnim::Happy;
	}

	if( m_isGamePause == true ){
		m_dAnimeSpeed = 0.0;
	} else {
		m_dAnimeSpeed = ANIM_SPEED.at(m_enOldAnimNo);
	}

	switch( m_enOldAnimNo ){
		case SayakaAnim::Dead:
			if( m_enNowAnimNo != m_enOldAnimNo ){
				m_enNowAnimNo = SayakaAnim::Dead;
				m_dAnimeSpeed = ANIM_SPEED.at(m_enOldAnimNo);
				m_pSkinMesh->ChangeAnimSet( m_enNowAnimNo );
			}
			break;
		case SayakaAnim::Wait:
			if( m_enNowAnimNo != m_enOldAnimNo ){
				m_enNowAnimNo = SayakaAnim::Wait;
				m_dAnimeSpeed = ANIM_SPEED.at(m_enOldAnimNo);
				m_pSkinMesh->ChangeAnimSet( m_enNowAnimNo );
			}
			break;
		case SayakaAnim::Walk:
			if( m_enNowAnimNo != m_enOldAnimNo ){
				m_enNowAnimNo = SayakaAnim::Walk;
				m_dAnimeSpeed = ANIM_SPEED.at(m_enOldAnimNo);
				m_pSkinMesh->ChangeAnimSet( m_enNowAnimNo );
				m_dCountAnimTime = 0.0;
				m_bLeftSound = false;
				m_bRightSound = false;
			}
			break;
		case SayakaAnim::Happy:
			if( m_enNowAnimNo != m_enOldAnimNo ){
				m_enNowAnimNo = SayakaAnim::Happy;
				m_dAnimeSpeed = ANIM_SPEED.at( m_enOldAnimNo );
				m_pSkinMesh->ChangeAnimSet( m_enNowAnimNo );
			}
			break;
		default:
			break;
	}
}

void CSayaka::DeadAnimControll()
{
	if( m_LifePoint <= 0 && m_enOldAnimNo != SayakaAnim::Dead ){
		m_enOldAnimNo = SayakaAnim::Dead;
	}
	if( m_enNowAnimNo == SayakaAnim::Dead ){
		m_dDeadAnimTime += m_pSkinMesh->GetAnimSpeed();

		if( m_dDeadAnimTime >= m_pSkinMesh->GetAnimPeriod( SayakaAnim::Dead ) ){
			BitFlagON( BitFlag::isDead );
			m_dAnimeSpeed = 0.0;
		}
	}
}

void CSayaka::AnimWalkSoundPlay()
{
	if( m_enNowAnimNo == SayakaAnim::Walk ){
		if( m_dCountAnimTime >= 2.6 ){
			m_dCountAnimTime = 0.1;
			m_bLeftSound = false;
			m_bRightSound = false;
		}
		m_pSkinMesh->ChangeAnimSet_StartPos( SayakaAnim::Walk, m_dCountAnimTime );
		m_dCountAnimTime += 0.1;
		string walkSoundName = "";
		// さやかの地面が外の地面だった場合.
		if( m_NowGroundType == enObjectNo::Ground ||
			m_NowGroundType == enObjectNo::Ground_I ||
			m_NowGroundType == enObjectNo::Ground_L ){
			walkSoundName = "Grass";
		}
		// さやかの地面が家の中の床だった場合.
		if( m_NowGroundType == enObjectNo::Flor ||
			m_NowGroundType == enObjectNo::Flor_I ||
			m_NowGroundType == enObjectNo::Flor_L ||
			m_NowGroundType == enObjectNo::Flor_I_one||
			m_NowGroundType == enObjectNo::Flor_L_one){
			walkSoundName = "Foor";
		}
		if (m_NowGroundType == enObjectNo::NarrowRoad1 ||
			m_NowGroundType == enObjectNo::NarrowRoad2 ||
			m_NowGroundType == enObjectNo::NarrowRoad3 ||
			m_NowGroundType == enObjectNo::NarrowRoad4) {
			walkSoundName = "Wood";
		}

		if( m_dCountAnimTime >= 1.4 && m_bRightSound == false ){
			CPlaySEThread::SetSEName( walkSoundName );
			m_bRightSound = true;
		}
		if( m_dCountAnimTime >= 2.6 && m_bLeftSound == false ){
			CPlaySEThread::SetSEName( walkSoundName );
			m_bLeftSound = true;
		}
	}
}

void CSayaka::RayHits( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() == enObjectNo::Event ) return;
	if(	pObj->GetObjectNo() == enObjectNo::Goal ) return;
	if( pObj->GetObjectNo() == enObjectNo::TextEvent ) return;
	if( pObj->GetObjectNo() == enObjectNo::Key ) return;

	GroundHit( pObj );

	// ゴーストがいればライトの当たり判定.
	if( pObj->GetObjectNo() == enObjectNo::GhostA ||
		pObj->GetObjectNo() == enObjectNo::GhostB ) {
		LightHit( pObj );	// ライトの当たり判定.
	} else {
		WallJudge( pObj );	// 壁との当たり判定.
	}
}

void CSayaka::LightHit( shared_ptr<CObjectBase> pObj )
{
	if( GetBitFlag( BitFlag::isStopCamera ) == true ) return;
	if( m_LightAxisZ == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) ) return;
	// ライトの当たり判定.
	if( m_pCollLight->isLightHit( pObj, m_vPosition, m_LightAxisZ ) ){
		pObj->BitFlagON(BitFlag::isHit);
	}
}

void CSayaka::InvincibleTime()
{
	if( m_LifePoint <= 0 ) return;
	if( GetBitFlag(BitFlag::isInvincible) == true ){
		if( m_bInvincibleCount <= INVINCIBLE_TIME ){
			m_bInvincibleCount++;
		} else{
			BitFlagOFF(BitFlag::isInvincible);
			m_bInvincibleCount = 0;
		}
	}
}

void CSayaka::SayakaFlasing()
{
	if( m_LifePoint <= 0 ) return;
	if( GetBitFlag(BitFlag::isInvincible) == true ){
		m_bDispFlag = true;
		m_fAlpha = fabsf(sinf( static_cast<float>(m_FlashCount) * 0.15f )) + 0.35f;
		m_FlashCount++;
	} else{
		m_FlashCount = 0;
		m_bDispFlag = true;
	}
}

void CSayaka::Respawn()
{
	if( m_onGround == true ){
		m_onGround = false;
		if( m_vPosition.y < 0.0f ){
			m_vPosition.y = 1.0f;
		}
		return;
	}
	m_vPosition.y -= FALL_SPEED;
	m_NowGroundType = enObjectNo::SlipThroughFloor;
	if( m_vPosition.y <= -4.0f ){
		m_vPosition = m_vOldPos;
		if (GetBitFlag(BitFlag::isInvincible) == false) {
			BitFlagON(BitFlag::isHit);
		}
	}
}

void CSayaka::LookCameraPos()
{
	float fRadian = GetTwoRadian( m_vPosition, m_pCamera->GetCameraPos() );
	m_vRotation.y = fRadian + D3DX_PI;
}


void CSayaka::GroundHit( shared_ptr<CObjectBase> pObj )
{
	// 対象オブジェクトが見えない床(落ちる床)の場合は終了.
	if( pObj->GetObjectNo() == enObjectNo::SlipThroughFloor )return;
	if( pObj->GetObjectNo() == enObjectNo::GhostA )return;
	if( pObj->GetObjectNo() == enObjectNo::GhostB )return;
	if( pObj->GetObjectNo() == enObjectNo::BigGhost )return;


	float fDistance = 0;;
	D3DXVECTOR3 vIntersect = D3DXVECTOR3();
	D3DXVECTOR3 vPos = m_vPosition;
	// 下向きレイの設定.

	vPos.y += 0.5f;
	m_pCollision->RaySetRay( vPos );
	auto GroundHit = [&]()
	{
		// 対象のオブジェクトとのレイ判定.
		if( m_pCollision->isRayCollision( pObj, &fDistance, &vIntersect ) == true ){
			// 対象オブジェクトの上に設定する.
			m_vPosition.y = vIntersect.y;
			m_onGround = true;
			m_vOldPos = m_vPosition;
			m_NowGroundType = pObj->GetObjectNo();
		}
	};
	auto GroundHits = [&]()
	{
		// 対象のオブジェクトとのレイ判定.
		if( m_pCollision->isRayCollision( pObj, &fDistance, &vIntersect ) == true ){
			m_onGround = true;
		}
	};

	// 真下へのレイ.
	m_pCollision->RaySetAxis( 0.0f, -1.0f, 0.0f );
	GroundHit();

	// 前後左右へのレイ.
	m_pCollision->RaySetAxis( 0.1f, -1.0f, 0.1f );
	GroundHits();
	m_pCollision->RaySetAxis( 0.1f, -1.0f, -0.1f );
	GroundHits();
	m_pCollision->RaySetAxis( -0.1f, -1.0f, 0.1f );
	GroundHits();
	m_pCollision->RaySetAxis( -0.1f, -1.0f, -0.1f );
	GroundHits();
}

void CSayaka::EventBoxHit( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() != enObjectNo::Event ) return;
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == true ){
		BitFlagON( BitFlag::isNextScene );
	}
}


void CSayaka::TextEventBoxHit( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() != enObjectNo::TextEvent ) return;
	if( m_pCollision->isBoxCollision( pObj->GetBox() ) == true ){
		BitFlagON( BitFlag::isNextText );
	}
}

void CSayaka::GoalBoxHit( shared_ptr<CObjectBase> pObj )
{
	if( pObj->GetObjectNo() != enObjectNo::Goal ) return;
	if( m_pCollision->isSphereCollision( pObj->GetSphere() ) == true ){
		BitFlagON( BitFlag::isGoal );
	}
}


void CSayaka::WallJudge( shared_ptr<CObjectBase> pObj )
{
	FLOAT fDistance[4] = {};			// 距離.
	D3DXVECTOR3 vIntersect[4] = {};	// 交差座標.
	float fDis = 0.0f, fYaw = 0.0f;			// 距離と回転(一時格納用).

	// レイの位置を設定.
	D3DXVECTOR3 vPos = m_vPosition;
	vPos.y += 0.5;
	m_pCollision->RaySetRay( vPos );


	// レイの向きにより当たる壁までの距離を求める.
	// 軸ベクトル前.
	m_pCollision->RaySetAxis( 0.0f, 0.0f, 1.0f );
	m_pCollision->isRayCollision( pObj, &fDistance[0], &vIntersect[0] );
	// 軸ベクトル後ろ.
	m_pCollision->RaySetAxis( 0.0f, 0.0f, -1.0f );
	m_pCollision->isRayCollision( pObj, &fDistance[1], &vIntersect[1] );
	// 軸ベクトル右.
	m_pCollision->RaySetAxis( 1.0f, 0.0f, 0.0f );
	m_pCollision->isRayCollision( pObj, &fDistance[2], &vIntersect[2] );
	// 軸ベクトル左.
	m_pCollision->RaySetAxis( -1.0f, 0.0f, 0.0f );
	m_pCollision->isRayCollision( pObj, &fDistance[3], &vIntersect[3] );

	fYaw = static_cast<float>( fabs( m_vRotation.y ) );	// fabs:絶対値(float版).
	dirCheck( &m_vRotation.y );		// 0～2πの間に収める.

	//--------------------------------------------------.
	// 前が壁に接近.
	//--------------------------------------------------.
	fDis = fDistance[0];
	if( 0.01f < fDis && fDis < 0.8f ){
		// 時計周り.
		if( m_vRotation.y < 0.0f ){
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.z -= 0.8f - fDis;
			}
			// 反時計周り.
		} else{
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.z -= 0.8f - fDis;
			}
		}
	}

	//--------------------------------------------------.
	// 後ろが壁に接近.
	//--------------------------------------------------.
	fDis = fDistance[1];
	if( 0.01f < fDis && fDis < 0.8f ){
		// 時計周り.
		if( m_vRotation.y < 0.0f ){
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.z += 0.8f - fDis;
			}
			// 反時計周り.
		} else{
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.z += 0.8f - fDis;
			}
		}
	}

	//--------------------------------------------------.
	// 右が壁に接近.
	//--------------------------------------------------.
	fDis = fDistance[2];
	if( 0.01f < fDis && fDis < 0.8f ){
		// 時計周り.
		if( m_vRotation.y < 0.0f ){
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.x -= 0.8f - fDis;
			}
			// 反時計周り.
		} else{
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.x += 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.x -= 0.8f - fDis;
			}
		}
	}

	//--------------------------------------------------.
	// 左が壁に接近.
	//--------------------------------------------------.
	fDis = fDistance[3];
	if( 0.01f < fDis && fDis < 0.8f ){
		// 時計周り.
		if( m_vRotation.y < 0.0f ){
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.x += 0.8f - fDis;
			}
			// 反時計周り.
		} else{
			// 右から.
			if( 0.785f <= fYaw && fYaw < 2.355f ){
				m_vPosition.z -= 0.8f - fDis;
			}
			// 前から.
			else if( 2.355f <= fYaw && fYaw < 3.925f ){
				m_vPosition.x -= 0.8f - fDis;
			}
			// 左から.
			else if( 3.925f <= fYaw && fYaw < 5.495f ){
				m_vPosition.z += 0.8f - fDis;
			}
			// 奥から.
			else{
				m_vPosition.x += 0.8f - fDis;
			}
		}
	}
}

void CSayaka::dirCheck( float* pYaw )
{
	// 1周以上いているか?
	if( *pYaw > static_cast<float>( D3DX_PI * 2.0 ) ){
		*pYaw -= static_cast<float>( D3DX_PI * 2.0 );		// 2π(360°)分引く.
	}

	// 再起関数.
	if( *pYaw > static_cast<float>( D3DX_PI * 2.0 ) ){
		dirCheck( pYaw );
	}
}

void CSayaka::TextureLoad( ID3D11Device* pDevice11 )
{
	m_pTexture = make_shared<CDX9SkinMesh::CHANGE_TEXTURES>();

	strcpy_s( m_pTexture->szTargetTextureName, "sayaka_tex1.png" );
	strcpy_s( m_pTexture->szTextureName, "Data\\Model\\Player\\Sayaka\\sayaka__smile_tex1.png" );


	if( m_pTexture->szTargetTextureName != 0 &&
		FAILED(
			D3DX11CreateShaderResourceViewFromFileA(
				pDevice11, m_pTexture->szTextureName,
				nullptr, nullptr, &m_pTexture->pTexture, nullptr ) ) ){
		MessageBox( NULL, "テクスチャ読み込み失敗",
			"Error", MB_OK );
		return;
	}
}

LPD3DXMESH CSayaka::GetMeshData()
{
	if( m_pSkinMesh == nullptr ) return nullptr;
	return m_pSkinMesh->GetMesh();
}