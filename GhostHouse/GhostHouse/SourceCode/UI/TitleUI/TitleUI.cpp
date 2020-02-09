#include "TitleUI.h"
#include "..\..\XInput\XInput.h"
#include "..\..\XAudio2\PlaySEThread.h"
CTitleUI::CTitleUI()
	: m_pTitleImage( nullptr )
	, m_pStartImage( nullptr )
	, m_pEndImage( nullptr )
	, m_pCursor( nullptr )
	, m_vCursorPosition()
	, m_bPushDownKey( false )
	, m_bPushUpKey( false )
	, m_enTitleMenuState( enTitleMenuState::Start )
{
}

CTitleUI::~CTitleUI()
{
}

bool CTitleUI::Load()
{
	// 画像が読み込めていたらtrueを返す.
	if( LoadAllImage() == true ) return true;
	return false;
}

void CTitleUI::Update()
{
	// スティック上入力.
	if( CXInput::GetLThumbY() > STICK_IDLING ||
		( GetAsyncKeyState( VK_UP ) & 0x8000 ) ){
		if( m_bPushUpKey == false ){
			m_bPushUpKey = true;
			GiveCursorUP();
		}
	}
	// スティック下入力.
	if( CXInput::GetLThumbY() < -STICK_IDLING ||
		( GetAsyncKeyState( VK_DOWN ) & 0x8000 ) ){
		if( m_bPushDownKey == false ){
			m_bPushDownKey = true;
			GiveCursorDOWN();
		}
	}

	if( CXInput::GetLThumbY() < STICK_IDLING )	m_bPushUpKey = false;
	if( CXInput::GetLThumbY() > -STICK_IDLING )	m_bPushDownKey = false;
}


void CTitleUI::Render()
{
	// すべての画像が読み込めていなかったら終了.
	if( LoadAllImage() == false ) return;

	// 各画像の描画.
	m_pTitleImage->Render();
	m_pStartImage->Render();
	m_pEndImage->Render();
	m_pCursor->SetPosition( m_vCursorPosition[0] );
	m_pCursor->Render();
	m_pCursor->SetPosition( m_vCursorPosition[1] );
	m_pCursor->Render();
}

bool CTitleUI::LoadAllImage()
{
	if( LoadTitltImage() == false ) return false;	// タイトル画像の読み込み.
	if( LoadStartImage() == false ) return false;	// スタート画像の読み込み.
	if( LoadEndImage() == false ) return false;		// 終了画像の読み込み.
	if( LoadCursorImage() == false ) return false;	// カーソル画像の読み込み.
	return true;
}

bool CTitleUI::LoadTitltImage()
{
	// 読み込まれていたら "true".
	if( m_pTitleImage != nullptr ) return true;
	m_pTitleImage = CSpriteResource::GetSprite( "Title" );

	return false;
}

bool CTitleUI::LoadStartImage()
{
	// 読み込まれていたら "true".
	if( m_pStartImage != nullptr ) return true;
	m_pStartImage = CSpriteResource::GetSprite( "Start" );

	if( m_pStartImage == nullptr ) return false;
	MIN_CURSOR_POSITION_Y = m_pStartImage->GetPosition().y;

	return false;
}

bool CTitleUI::LoadEndImage()
{
	// 読み込まれていたら "true".
	if( m_pEndImage != nullptr ) return true;
	m_pEndImage = CSpriteResource::GetSprite( "End" );

	if( m_pEndImage == nullptr ) return false;
	MAX_CURSOR_POSITION_Y = m_pEndImage->GetPosition().y;

	return false;
}

bool CTitleUI::LoadCursorImage()
{
	// 読み込まれていたら "true".
	if( m_pCursor != nullptr ) return true;
	m_pCursor = CSpriteResource::GetSprite( "Cursor" );

	if( m_pCursor == nullptr ) return false;
	m_vCursorPosition[0].z = m_pCursor->GetPosition().z;

	CURSOR_MOVE_SPEED =
		m_pEndImage->GetPosition().y - m_pStartImage->GetPosition().y;

	m_vCursorPosition[0].y = m_vCursorPosition[1].y = m_pStartImage->GetPosition().y;
	m_vCursorPosition[0].x =
		m_pStartImage->GetPosition().x - m_pStartImage->GetSpriteWidth() / 2 +
		m_pCursor->GetSpriteWidth();
	m_vCursorPosition[1].x =
		m_pStartImage->GetPosition().x + m_pStartImage->GetSpriteWidth() / 2 -
		m_pCursor->GetSpriteWidth();

	return false;
}

void CTitleUI::GiveCursorUP()
{
	// 右スティックを上に傾けたとき.
	m_vCursorPosition[0].y -= CURSOR_MOVE_SPEED;	// カーソルの移動.
	if( m_vCursorPosition[0].y != MIN_CURSOR_POSITION_Y ){
		CPlaySEThread::SetSEName( "UnSelect" );
	} else{
		CPlaySEThread::SetSEName( "Select" );
	}
	// 移動した先が規定値より少なかったら座標の調整.
	if( m_vCursorPosition[0].y <= MIN_CURSOR_POSITION_Y ){
		m_vCursorPosition[0].y = m_vCursorPosition[1].y = MIN_CURSOR_POSITION_Y;

		m_vCursorPosition[0].x =
			m_pStartImage->GetPosition().x - m_pStartImage->GetSpriteWidth() / 2 +
			m_pCursor->GetSpriteWidth();
		m_vCursorPosition[1].x =
			m_pStartImage->GetPosition().x + m_pStartImage->GetSpriteWidth() / 2 -
			m_pCursor->GetSpriteWidth();

		m_enTitleMenuState = enTitleMenuState::Start;
	}
}
void CTitleUI::GiveCursorDOWN()
{
	m_vCursorPosition[0].y += CURSOR_MOVE_SPEED;	// カーソルの移動.
	if( m_vCursorPosition[0].y != MAX_CURSOR_POSITION_Y ){
		CPlaySEThread::SetSEName( "UnSelect" );
	} else{
		CPlaySEThread::SetSEName( "Select" );
	}
	// 移動した先が規定値より大きかったら座標の調整.
	if( m_vCursorPosition[0].y >= MAX_CURSOR_POSITION_Y ){
		m_vCursorPosition[0].y = m_vCursorPosition[1].y = MAX_CURSOR_POSITION_Y;

		m_vCursorPosition[0].x =
			m_pEndImage->GetPosition().x - m_pEndImage->GetSpriteWidth() / 2 +
			( m_pCursor->GetSpriteWidth() + 32 );
		m_vCursorPosition[1].x =
			m_pEndImage->GetPosition().x + m_pEndImage->GetSpriteWidth() / 2 -
			( m_pCursor->GetSpriteWidth() + 32 );

		m_enTitleMenuState = enTitleMenuState::End;
	}
}