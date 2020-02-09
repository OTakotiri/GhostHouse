#include <process.h>
#include "GameScene.h"
#include "..\GameEnd\GameEndScene.h"
#include "..\..\UI\Iiris\Iris.h"
#include "..\..\XAudio2\SoundManager.h"
#include "..\..\XAudio2\PlaySEThread.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"
#include "..\..\Resource\TutorialText\TutorialTextLoad.h"

CGameScene::CGameScene( shared_ptr<CSceneManager> &sceneManager )
	: CSceneBase		( sceneManager )

	, m_pSatage			( make_unique<CStage>() )
	, m_pPauseUI		( make_unique<CPauseUI>() )
	, m_pGameOverUI		( make_unique<CGameOverUI>() )
	, m_fAlpha			( 0.0f )
	, m_bisGoal			( false )
	, m_enNowSceneState	( enSceneState::Game )
	, m_bPushDownKey	( false )
	, m_bPushUpKey		( false )
	, m_isEnd			( false )
	, ThreadExitCode	( -1 )
	, m_bThreadRelease	( false )
	, m_TextArrayNum	( 0 )
{
	CSoundManager::StopSE("OK");
	if( CSpriteResource::GetSprite( "Fade" ) == nullptr ) return;
	CSpriteResource::GetSprite( "Fade" )->SetAlpha( 0.0f );
	CFadeUI::Init( false );

	auto st = [&]()
	{
		CSoundManager::PlayBGM( MAIN_BGM_NAME, m_isEnd, true );
		m_isEnd = true;
	};
	BGMThread = std::thread( st );
	InThreadID = BGMThread.get_id();
	//BGMをフェードインさせる為に、ボリュームを0に.
	CSoundManager::SetBGMVolme( MAIN_BGM_NAME, 0.0f );
}

CGameScene::~CGameScene()
{
	m_isEnd = true;
	while( Release() == false ){}
}


void CGameScene::Load( HWND hWnd, ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
{
	// ステージの読み込み.
	m_pSatage->Load( pDevice11, pContext11 );

	if( m_pWipeEffectUI != nullptr ) return;
	m_pWipeEffectUI = make_unique<CWipeEffectUI>( pDevice11, pContext11 );
	m_pWipeEffectUI->SetStart();
	CTutorialText::SetArrayNumber(0);
}

void CGameScene::Update()
{
	if (CSoundManager::GetBGMVolme(MAIN_BGM_NAME) != -1) {
		if (CSoundManager::GetBGMVolme(MAIN_BGM_NAME) != 1.0f) {
			CSoundManager::FadeInBGM(MAIN_BGM_NAME);
		}
	}
	switch( m_enNowSceneState ){
		case enSceneState::Game:
			GameUpdate();
			break;
		case enSceneState::Pause:
//			if( CXInput::GetLThumbY() < STICK_IDLING )	m_bPushUpKey = false;
//			if( CXInput::GetLThumbY() > -STICK_IDLING )	m_bPushDownKey = false;
			PauseUpdate();
			break;
		case enSceneState::GameOver:
//			if( CXInput::GetLThumbY() < STICK_IDLING )	m_bPushUpKey = false;
//			if( CXInput::GetLThumbY() > -STICK_IDLING )	m_bPushDownKey = false;
			GameOverUpdate();
			break;
		default:
			break;
	}
}


void CGameScene::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	CPlaySEThread::Updata();
	switch( m_enNowSceneState ){
		case enSceneState::Game:
			GameRender( mView, mProj, stLight, stCamera );
			break;
		case enSceneState::Pause:
			PauseRender( mView, mProj, stLight, stCamera );
			break;
		case enSceneState::GameOver:
			GameOverRender( mView, mProj, stLight, stCamera );
			break;
		default:
			break;
	}

	if( m_pWipeEffectUI == nullptr ) return;
	// ワイプエフェクトの表示.
	D3DXVECTOR2 targetPos = 
		CDX9Mesh::WorldtoScreenPos( mView, mProj, m_pSatage->GetPlayerPosition() );
	m_pWipeEffectUI->SetTargetPos( targetPos );
	m_pWipeEffectUI->Render();
}

void CGameScene::GameUpdate()
{
	// ステージの更新処理.
	if( m_pWipeEffectUI->isActive() == false ){
		m_pSatage->Updata();
		if(( CXInput::IsPress( XINPUT_GAMEPAD_START ) ) ||
			( GetAsyncKeyState( 'P' ) & 0x8000 )){
			if( m_pSatage->isSayakaLifePointZero() == true ) return;
			m_pSatage->SetPause( true ); 
			m_enNowSceneState = enSceneState::Pause;
			return;
		}
	}
	// ゲームクリア.
	if( m_pSatage->isGaol() ){
		m_pSatage->Updata();
		m_bisGoal = true;
		m_fAlpha += 0.01f;
		m_pWipeEffectUI->SetEnd();
		if( m_fAlpha >= 1.0f ){
			CSoundManager::StopBGM(MAIN_BGM_NAME);
			m_pSceneManager->Change( make_shared<CGameEndScene>( m_pSceneManager ) );
			return;
		}
	}
	// サヤカが倒れる.
	if( m_pSatage->isSayakaDead() ){
		m_isEnd = true;
		CSoundManager::StopBGM( MAIN_BGM_NAME );
		BGMThread.detach();
		auto st = [&]()
		{
			CSoundManager::PlayBGM( OVER_BGM_NAME, m_isEnd, true );
			if( InThreadID == BGMThread.get_id() ){

			}
		};
		BGMThread = std::thread( st );
		InThreadID = BGMThread.get_id();
		m_enNowSceneState = enSceneState::GameOver;
		return;
	}
}

void CGameScene::GameRender( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	// ステージの描画.
	m_pSatage->Render( mView, mProj, stLight, stCamera );
}

void CGameScene::PauseUpdate()
{
	m_pPauseUI->Update();

	if( ( CXInput::IsPress( XINPUT_GAMEPAD_B ) ) ||
		( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) ){
		CPlaySEThread::SetSEName( "OK" );
		m_pSatage->SetPause( false );
		PauseNextScene();
	}
}

void CGameScene::PauseRender( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	// ステージの描画.
	m_pSatage->Render( mView, mProj, stLight, stCamera );
	m_pPauseUI->Render();
}

void CGameScene::GameOverUpdate()
{
	CPlaySEThread::Updata();
	m_pGameOverUI->Upadate();


	if( ( CXInput::IsPress( XINPUT_GAMEPAD_B ) ) ||
		( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) ){
		GameOverNextScene();
	}
}

void CGameScene::GameOverRender( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	m_pSatage->Render( mView, mProj, stLight, stCamera );
	m_pGameOverUI->Render( mView, mProj, stLight, stCamera );
}


void CGameScene::PauseNextScene()
{
	switch( m_pPauseUI->GetNowMenuState() ){
		case CPauseUI::ToTitle:
			// 次のシーンへ移動.
			m_isEnd = true;
			CSoundManager::StopBGM( MAIN_BGM_NAME );
			while( Release() == false ){}
			m_pSceneManager->Swap( make_shared<CTitleScene>( m_pSceneManager ) );

			break;
		case CPauseUI::ToGame:
			m_enNowSceneState = enSceneState::Game;

			break;
		default:
			break;
	}
}

void CGameScene::GameOverNextScene()
{
	switch( m_pGameOverUI->GetNowMenuState() ){
		case CGameOverUI::Continue:
			CPlaySEThread::SetSEName( "OK" );
			// 次のシーンへ移動.
			m_isEnd = true;
			CSoundManager::StopBGM( OVER_BGM_NAME );
			while (Release() == false) {}
			BGMThread = std::thread( [&](){ 
				CSoundManager::PlayBGM( MAIN_BGM_NAME, m_isEnd, true ); 
			} );
			InThreadID = BGMThread.get_id();
			//BGMをフェードインさせる為に、ボリュームを0に.
			CSoundManager::SetBGMVolme( MAIN_BGM_NAME, 0.0f );

			m_pSatage->ContinueStageReset();
			m_enNowSceneState = enSceneState::Game;
			
			break;
		case CGameOverUI::End:
			CPlaySEThread::SetSEName( "OK" );
			m_isEnd = true;
			CSoundManager::StopBGM( OVER_BGM_NAME );
			while( Release() == false ){}
			m_pSceneManager->Swap( make_shared<CTitleScene>( m_pSceneManager ) );

			break;
		default:
			break;
	}
}

bool CGameScene::Release()
{
	ThreadExitCode = 0;
	GetExitCodeThread( BGMThread.native_handle(), &ThreadExitCode );

	if( ThreadExitCode == 0 ){
		if( InThreadID == BGMThread.get_id() ){
			m_isEnd = true;				//曲をストップした場合、ストップを命令したコードの中で
										//falseの間ループで待たせているので、SoundManagerを解放してから
										//trueでスレッド内のwhileループからでる。
			BGMThread.detach();
			m_bThreadRelease = true;
			return true;
		}
		if( m_bThreadRelease == true ){
			return true;
		}
		return false;
	}
	return false;
}