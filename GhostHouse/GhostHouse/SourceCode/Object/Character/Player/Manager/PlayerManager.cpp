#include "PlayerManager.h"
#include "..\..\XInput\XInput.h"
#include "..\..\Object\Stage\Stage.h"

CPlayerManager::CPlayerManager()
	: m_pSayaka				( nullptr )
	, m_pTowa				( nullptr )
	, m_LifeUI				( nullptr )
	, m_LifeNoneUI			( nullptr )
	, m_isSetOnGround		( false )
	, m_isLifePointRender	( false )
	, m_LifeImageScale		( 1.0f ) 
	, m_StageNumber			( 0 )
	, m_isInitTowaState		( false )
{}

CPlayerManager::~CPlayerManager()
{
}

void CPlayerManager::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	UILoad();

	if( m_pSayaka == nullptr ) return;
	m_pSayaka->Load( pDevice11, pContext11 );
	if( m_pTowa == nullptr ) return;
	m_pTowa->Load( pDevice11, pContext11 );
}

void CPlayerManager::Update()
{
	if( m_pSayaka == nullptr ) return;
	m_isLifePointRender = true;			// UIのレンダリングをさせる.

	if( m_pTowa != nullptr ){
		if( m_StageNumber >= static_cast<int>(CStage::MapScene::In_the_house_B1_Area2) && m_isInitTowaState == false ){
			m_pTowa->BitFlagON( BitFlag::isTowaAfterHelping );
			m_isInitTowaState = true;
		}
		m_pSayaka->Update( m_pTowa );	// プレイヤーの更新.
		m_pTowa->Update( m_pSayaka );


		if( m_pSayaka->GetBitFlag( BitFlag::isInvincible ) == false ){
			m_LifeImageScale = 1.0f;
		}
	} else {
		m_pSayaka->Update( nullptr );	// プレイヤーの更新.

		if( m_pSayaka->GetBitFlag( BitFlag::isInvincible ) == false ){
			m_LifeImageScale = 1.0f;
		}
	}

}

void CPlayerManager::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera )
{
	if( m_pSayaka == nullptr ) return;
	m_pSayaka->Render( mView, mProj, stLight, stCamera );

	if( m_pTowa == nullptr ) return;
	m_pTowa->Render( mView, mProj, stLight, stCamera );
}

void CPlayerManager::Collsiton( std::shared_ptr<CObjectBase> pObj )
{
	if( m_pSayaka == nullptr ) return;
	m_pSayaka->Collision( pObj );

	if( m_pTowa == nullptr ) return;
	m_pTowa->Collision( pObj );
}

void CPlayerManager::OneGroundHit( vector<shared_ptr<CObjectBase>> pMap )
{
	if( m_isSetOnGround == true ) return;
	for( auto& m : pMap ){
		Collsiton( m );
	}
	m_isSetOnGround = true;
}

void CPlayerManager::UIRender()
{
	if( m_isLifePointRender == false ) return;

	for( int i = 0; i < MAX_LIFE_POINT; i++ ){
		m_LifeNoneUI->SetPosition(
			// 表示がぴちぴちになるので "10.0f" プラス.
			LIFE_IMAGE_RENDER_POS_X + ( ( m_LifeNoneUI->GetSpriteWidth() + 10.0f )*i ),
			LIFE_IMAGE_RENDER_POS_Y, 0.0f );
		m_LifeNoneUI->Render();
	}

	int lifeBuf = m_pSayaka->GetLifePoint();
	if( m_pSayaka->GetBitFlag( BitFlag::isInvincible ) == true ){
		lifeBuf++;
		m_LifeImageScale -= 0.04f;
		if( m_LifeImageScale <= 0.0f ) m_LifeImageScale = 0.0f;
	}

	for( int i = 0; i < lifeBuf; i++ ){
		m_LifeUI->SetPosition(
			// 表示がぴちぴちになるので "10.0f" プラス.
			LIFE_IMAGE_RENDER_POS_X + ( ( m_LifeUI->GetSpriteWidth() + 10.0f )*i ),
			LIFE_IMAGE_RENDER_POS_Y, 0.0f );
		if( i == lifeBuf - 1 ){
			m_LifeUI->SetScale( m_LifeImageScale );
		} else{
			m_LifeUI->SetScale( 1.0f );
		}
		m_LifeUI->Render();
	}

	if( m_pSayaka->GetBitFlag( BitFlag::isNowTowaControl ) == false ){
		m_SayakaMoveUI->SetScale( 1.0f );
		m_SayakaMoveUI->SetPosition( WND_W - 120.0f, 100.0f, 0.0f );
		m_SayakaMoveUI->Render();
	} else{
		m_TowaMoveUI->SetScale( 1.0f );
		m_TowaMoveUI->SetPosition( WND_W - 120.0f, 100.0f, 0.0f );
		m_TowaMoveUI->Render();
	}
}

void CPlayerManager::ChangeSceneInit( vector<shared_ptr<CObjectBase>>& pMap )
{
	// キャラクターの座標を入れ替える.
	auto ChangeCharacterPosition = [&]( shared_ptr<CObjectBase>& pCharacter, enObjectNo objNo )
	{
		for( size_t i = 0; i < pMap.size(); i++ ){
			if( pMap[i]->GetObjectNo() == objNo ){
				CXInput::SetActiveFlag( false );
				CXInput::InitController();
				pCharacter->BitFlagON( BitFlag::isStopCamera );
				pCharacter->SetPosition( pMap[i]->GetPosition() );
				pCharacter->SetRotation( pMap[i]->GetRotation() );
				pMap.erase( pMap.begin() + i );
				break;
			}
		}
	};

	// さやかの座標入れ替え.
	ChangeCharacterPosition( m_pSayaka, enObjectNo::Sayaka );
	// とわが既にいた場合.
	if( m_pTowa != nullptr ){
		// とわの座標を入れ替え.
		ChangeCharacterPosition( m_pTowa, enObjectNo::Towa );
	} else{
		// いない場合検索.
		for( size_t i = 0; i < pMap.size(); i++ ){
			if( pMap[i]->GetObjectNo() == enObjectNo::Towa ){
				m_pTowa = pMap[i];
				pMap.erase( pMap.begin() + i );
				break;
			}
		}
	}

	m_isSetOnGround = false;
	m_pSayaka->BitFlagOFF( BitFlag::isNextScene );
}

void CPlayerManager::UILoad()
{
	if( m_LifeUI != nullptr ) return;
	m_LifeUI = CSpriteResource::GetSprite( "life" );

	if( m_LifeNoneUI != nullptr ) return;
	m_LifeNoneUI = CSpriteResource::GetSprite( "life_kara" );

	if( m_SayakaMoveUI != nullptr ) return;
	m_SayakaMoveUI = CSpriteResource::GetSprite( "change1" );

	if( m_TowaMoveUI != nullptr ) return;
	m_TowaMoveUI = CSpriteResource::GetSprite( "change2" );
}