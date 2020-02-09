#include "GameOverUI.h"
#include "..\..\XInput\XInput.h"
#include "..\..\XAudio2\PlaySEThread.h"

CGameOverUI::CGameOverUI()
	: m_pGameOverTitleImage( nullptr )
	, m_pContinueImage( nullptr )
	, m_pEndImage( nullptr )
	, m_pCursor( nullptr )
	, m_vCursorPosition()
	, m_bPushDownKey( false )
	, m_bPushUpKey( false )
	, m_enGameOverMenuState( enGameOverMenuState::Continue )
{
}

void CGameOverUI::Upadate()
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

// 描画関数.
void CGameOverUI::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	if( LoadAllImage() == false ) return;

	// 各画像の描画.
	m_pGameOverTitleImage->Render();
	m_pContinueImage->Render();
	m_pEndImage->Render();
	m_pCursor->SetPosition( m_vCursorPosition[0] );
	m_pCursor->Render();
	m_pCursor->SetPosition( m_vCursorPosition[1] );
	m_pCursor->Render();
}

// すべての画像.
bool CGameOverUI::LoadAllImage()
{
	if( LoadPauseTitltImage() == false ) return false;
	if( LoadToTilteImage() == false ) return false;
	if( LoadToGameImage() == false ) return false;
	if( LoadCursorImage() == false ) return false;

	return true;
}

// タイトル画像. 
bool CGameOverUI::LoadPauseTitltImage()
{
	if( m_pGameOverTitleImage != nullptr ) return true;
	m_pGameOverTitleImage = CSpriteResource::GetSprite( "GameOverTitle" );

	return false;
}

// "タイトルへ"テキスト画像.
bool CGameOverUI::LoadToTilteImage()
{
	if( m_pContinueImage != nullptr ) return true;
	while( m_pContinueImage == nullptr ){
		m_pContinueImage = CSpriteResource::GetSprite( "Continue" );
	}

	if( m_pContinueImage == nullptr ) return false;
	MIN_CURSOR_POSITION_Y = m_pContinueImage->GetPosition().y;

	return false;
}

// "ゲームへ"テキスト画像.
bool CGameOverUI::LoadToGameImage()
{
	if( m_pEndImage != nullptr ) return true;
	while( m_pEndImage == nullptr ){
		m_pEndImage = CSpriteResource::GetSprite( "GameEnd" );
	}

	if( m_pEndImage == nullptr ) return false;
	MAX_CURSOR_POSITION_Y = m_pEndImage->GetPosition().y;

	return false;
}

// 選択カーソルの画像.
bool CGameOverUI::LoadCursorImage()
{
	// 読み込まれていたら "true".
	if( m_pCursor != nullptr ) return true;
	m_pCursor = CSpriteResource::GetSprite( "Cursor" );

	if( m_pCursor == nullptr ) return false;

	CURSOR_MOVE_SPEED =
		m_pContinueImage->GetPosition().y - m_pContinueImage->GetPosition().y;

	m_vCursorPosition[0].y = m_vCursorPosition[1].y = m_pContinueImage->GetPosition().y;
	m_vCursorPosition[0].x =
		m_pContinueImage->GetPosition().x - m_pContinueImage->GetSpriteWidth() / 2 +
		m_pCursor->GetSpriteWidth() - 10;
	m_vCursorPosition[1].x =
		m_pContinueImage->GetPosition().x + m_pContinueImage->GetSpriteWidth() / 2 -
		m_pCursor->GetSpriteWidth() + 20;

	CURSOR_MOVE_SPEED =
		MAX_CURSOR_POSITION_Y - MIN_CURSOR_POSITION_Y;

	return false;
}

void CGameOverUI::GiveCursorUP()
{
	if( m_pContinueImage == nullptr ) return;
	if( m_pCursor == nullptr ) return;

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
			m_pContinueImage->GetPosition().x - m_pContinueImage->GetSpriteWidth() / 2 +
			m_pCursor->GetSpriteWidth() - 10;
		m_vCursorPosition[1].x =
			m_pContinueImage->GetPosition().x + m_pContinueImage->GetSpriteWidth() / 2 -
			m_pCursor->GetSpriteWidth() + 20;

		m_enGameOverMenuState = enGameOverMenuState::Continue;
	}
}

void CGameOverUI::GiveCursorDOWN()
{
	if( m_pEndImage == nullptr ) return;
	if( m_pCursor == nullptr ) return;

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
			( m_pCursor->GetSpriteWidth() + 30 );
		m_vCursorPosition[1].x =
			m_pEndImage->GetPosition().x + m_pEndImage->GetSpriteWidth() / 2 -
			( m_pCursor->GetSpriteWidth() + 30 );
		m_enGameOverMenuState = enGameOverMenuState::End;
	}
}