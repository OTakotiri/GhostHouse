#include "StageSelectUI.h"
#include "..\..\XAudio2\SoundManager.h"
#include "..\..\XAudio2\PlaySEThread.h"

CStageSelectUI::CStageSelectUI()
	: m_pStageSelectTitle	( nullptr )
	, m_pUpArrows			( nullptr )
	, m_pDownArrows			( nullptr )
	, m_pStageNameFrame		( nullptr )
	, m_fArrowSize			( ARROW_INIT_SIZE )
	, m_fSizeCount			( 0.0f )
	, m_iNowStageNumber		( 0 )
	, m_bPlaySE				( false )
{

}

CStageSelectUI::~CStageSelectUI()
{

}

bool CStageSelectUI::Load()
{
	// 画像が読み込めていたらtrueを返す.
	if( LoadAllImage() == true ) return true;
	return false;
}

void CStageSelectUI::Render()
{
	// すべての画像が読み込めていなかったら終了.
	if( LoadAllImage() == false ) return;

	// ステージセレクトタイトル画像の表示.
	m_pStageSelectTitle->Render();
	// 上矢印画像の描画.
	UpArrowRender();
	// 下矢印画像の描画.
	DownArrowRender();
	
	shared_ptr<CSpriteUI> pNowStage = CMapLoad::GetStageImage( m_iNowStageNumber );
	pNowStage->Render();
	
	// ステージ名の表示.
	CDebugText::SetPosition( D3DXVECTOR3( 250.0f, 450.0f, 0.0f ) );
	CDebugText::Render( CMapLoad::GetStageName(m_iNowStageNumber) );

}

void CStageSelectUI::MoveUpStage()
{
	// 各移動フラグが立っていたら終了.
	if( m_bMoveUpStageFlag == true ) return;
	if( m_bMoveDownStageFlag == true ) return;

	// 上移動フラグを立てる.
	m_bMoveUpStageFlag = true;
}

void CStageSelectUI::MoveDownStage()
{
	// 各移動フラグが立っていたら終了.
	if( m_bMoveDownStageFlag == true ) return;
	if( m_bMoveUpStageFlag == true ) return;

	// 下移動フラグを立てる.
	m_bMoveDownStageFlag = true;
}

bool CStageSelectUI::SetStageName()
{
	// 各移動フラグが立っていたら終了.
	if( m_bMoveDownStageFlag == true ) return false;
	if( m_bMoveUpStageFlag == true ) return false;

	CMapLoad::SetStageName( CMapLoad::GetStageName(m_iNowStageNumber) );
	CMapLoad::SetStageNum( m_iNowStageNumber );

	return true;
}

void CStageSelectUI::UpArrowRender()
{
	// 上移動フラグが立っていたら.
	if( m_bMoveUpStageFlag == true ){
		ArrowSizeChange();	// 画像サイズを変更する.
		m_pUpArrows->SetScale( m_fArrowSize );		// 画像サイズをセットする.
	} else {
		m_pUpArrows->SetScale( ARROW_INIT_SIZE );	// 元サイズをセットする.
	}
	// 上矢印画像の描画.
	m_pUpArrows->Render();
}

void CStageSelectUI::DownArrowRender()
{
	// 下移動フラグが立っていたら.
	if( m_bMoveDownStageFlag == true ){
		ArrowSizeChange();	// 画像サイズを変更する.
		m_pDownArrows->SetScale( m_fArrowSize );	// 画像サイズをセットする.
	} else {
		m_pDownArrows->SetScale( ARROW_INIT_SIZE );	// 元サイズをセットする.
	}
	// 下矢印画像の描画.
	m_pDownArrows->Render();
}

void CStageSelectUI::ArrowSizeChange()
{
	m_fSizeCount += ARROW_SIZE_CHNGE_COUNT;
	float fsize = fabsf(cosf( D3DX_PI * 2.0f / 120.0f * m_fSizeCount ));
	if( fsize <= ARROW_SIZE_MINIMUM_VALUE ){
		if (m_bPlaySE == false) {
			m_bPlaySE = true;
			CPlaySEThread::SetSEName("Select");
		}
		fsize = ARROW_SIZE_MINIMUM_VALUE;
	}
	if( m_fSizeCount >= ARROW_SIZE_CHNGE_COUNT_MAX ){
		m_fSizeCount = 0.0f;
		m_bPlaySE = false;
		StageChange();
	}
	m_fArrowSize = fsize;
}

void CStageSelectUI::StageChange()
{
	if( m_bMoveUpStageFlag == true ){
		m_iNowStageNumber++;
		if( m_iNowStageNumber >= CMapLoad::GetMAXStageNumber() ){
			m_iNowStageNumber = 0;
		}
		m_bMoveUpStageFlag = false;

	} else if ( m_bMoveDownStageFlag == true ){
		m_iNowStageNumber--;
		if( m_iNowStageNumber < 0 ){
			m_iNowStageNumber = CMapLoad::GetMAXStageNumber()-1;
		}
		m_bMoveDownStageFlag = false;

	}
}



bool CStageSelectUI::LoadAllImage()
{
	if( LoadStageSelectTitle() == false ) return false;
	if( LoadUpArrow() == false ) return false;
	if( LoadDownArrow() == false ) return false;
	if( LoadStageNameFrame() == false ) return false;

	return true;
}

bool CStageSelectUI::LoadStageSelectTitle()
{
	// "ステージセレクト"のタイトル画像の読み込み.
	if( m_pStageSelectTitle != nullptr ) return true;
	m_pStageSelectTitle = CSpriteResource::GetSprite("StageSelectTitle");
	return false;
}

bool CStageSelectUI::LoadUpArrow()
{
	// 上矢印画像の読み込み.
	if( m_pUpArrows != nullptr ) return true;
	m_pUpArrows = CSpriteResource::GetSprite("CursorUp");
	return false;
}

bool CStageSelectUI::LoadDownArrow()
{
	// 下矢印画像の読み込み.
	if( m_pDownArrows != nullptr ) return true;
	m_pDownArrows = CSpriteResource::GetSprite("CursorDown");
	return false;
}

bool CStageSelectUI::LoadStageNameFrame()
{
	// ステージ名枠画像読み込み.
	if( m_pStageNameFrame != nullptr ) return true;
	m_pStageNameFrame = CSpriteResource::GetSprite("StageNameFrame");
	return false;
}