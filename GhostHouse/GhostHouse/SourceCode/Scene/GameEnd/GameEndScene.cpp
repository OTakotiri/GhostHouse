#include "GameEndScene.h"
#include "..\TItleScene\TitleScene.h"

#include "..\..\XAudio2\SoundManager.h"
#include "..\..\XAudio2\PlaySEThread.h"

CGameEndScene::CGameEndScene( shared_ptr<CSceneManager> sceneManager )
	: CSceneBase		( sceneManager )

	, m_pGameEndImage	( nullptr )
	, m_pGameEndText	( nullptr )
	, m_isLoadEnd		( false )
	, m_isNextScene		( false )
	, m_isChangeScene	( false )
	, m_bPushDownKey	( false )
	, m_bPushUpKey		( false )
	, m_TextAlpha		( 1.0f )
	, m_AlphaCount		( 0.0f )
	, m_isEnd			( false )
	, ThreadExitCode	( -1 )
	, m_bThreadRelease	( false )
{

	CFadeUI::Init();

	CSoundManager::StopSE( "OK" );
	auto BGM = [&]()
	{
		CSoundManager::PlayBGM( "Title", m_isEnd, true );
	};
	BGMThread = std::thread( BGM );
	InThreadID = BGMThread.get_id();
	//BGMをフェードインさせる為に、ボリュームを0に.
	CSoundManager::SetBGMVolme( "Title", 0.2f );
}

CGameEndScene::~CGameEndScene()
{
	m_isEnd = true;
	while( Release() == false ){
	}
}

void CGameEndScene::Load( HWND hWnd, ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
{
	if( m_isLoadEnd == true )	return;
	m_hWnd = hWnd;	// ウィンドウハンドルを受け取る.
	
	if( m_pGameEndImage == nullptr ){
		m_pGameEndImage = CSpriteResource::GetSprite("GameEnding");
		if( m_pGameEndImage != nullptr ){
			CFadeUI::FadeINStart();
		}
	}
	if( m_pGameEndText == nullptr ){
		m_pGameEndText = CSpriteResource::GetSprite("GameEndText");
		if( m_pGameEndText != nullptr ){
			CFadeUI::FadeINStart();
		}
	}
}

void CGameEndScene::Update()
{
	if( CSoundManager::GetBGMVolme( "Title" ) != -1 ){
		if( CSoundManager::GetBGMVolme( "Title" ) != 1.0f && m_isNextScene == false ){
			CSoundManager::FadeInBGM( "Title" );
		}
		if( CSoundManager::GetBGMVolme( "Title" ) == 0.0f && m_isNextScene == true ){
			m_isChangeScene = true;
		}
	}
	NextScene();
	if( m_isChangeScene == true ){
		Sleep( 10 );
		m_pSceneManager->Swap( make_shared<CTitleScene>( m_pSceneManager ) );
	}
	CPlaySEThread::Updata();

	if( CFadeUI::isActive() == true ) return;


	// Rボタン入力.
	if( ( CXInput::IsPress( XINPUT_GAMEPAD_B ) ) ||
		( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) ){
		CPlaySEThread::SetSEName( "OK" );
		CSoundManager::FadeOutBGM( "Title" );
		CFadeUI::FadeOUTStart();
		m_isNextScene = true;
	}
	if( CXInput::GetLThumbY() < STICK_IDLING )	m_bPushUpKey = false;
	if( CXInput::GetLThumbY() > -STICK_IDLING )	m_bPushDownKey = false;
}

void CGameEndScene::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	if( m_pGameEndImage == nullptr ) return;
	if( m_pGameEndText == nullptr ) return;

	m_pGameEndImage->Render();

	m_AlphaCount += 0.01f;
	if( m_AlphaCount >= 360.0f ){
		m_AlphaCount = 0.0f;
	}
	m_TextAlpha = fabsf( sinf( m_AlphaCount ) ) + 0.01f;
	m_pGameEndText->SetAlpha( m_TextAlpha );
	m_pGameEndText->Render();
}

void CGameEndScene::NextScene()
{
	if( CFadeUI::isEnd() == false ) return;
	if( m_isNextScene == false ) return;
	if( m_pGameEndText == nullptr ) return;

	CFadeUI::Init();
	m_isEnd = true;
	while( Release() == false ){
	}
	
}

bool CGameEndScene::Release()
{
	GetExitCodeThread( BGMThread.native_handle(), &ThreadExitCode );

	if( ThreadExitCode == 0 ){
		if( InThreadID == BGMThread.get_id() ){
			m_isEnd = true;				//曲をストップした場合、ストップを命令したコードの中で
			//CSoundManager::CleanBGM("Title");
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