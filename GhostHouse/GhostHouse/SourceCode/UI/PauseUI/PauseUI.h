#ifndef PAUSE_H
#define PAUSE_H

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"

class CPauseUI
{
public:
	enum enPauseMenuState
	{
		None,

		ToGame,		// ゲームへ.
		ToTitle,	// タイトルへ.

		Max,
	};

private:
	float MAX_CURSOR_POSITION_Y;		// カーソルの最大値.
	float MIN_CURSOR_POSITION_Y;		// カーソルの最小値.
	float CURSOR_MOVE_SPEED;			// カーソルの移動値.
	const int STICK_IDLING = 30000;		// スティックの遊び.


public:
	CPauseUI();
	~CPauseUI()
	{
	};

	// 更新関数.
	void Update();
	// 描画関数.
	void Render();

	// カーソル, 上への操作.
	void GiveCursorUP();
	// カーソル, 下への操作.
	void GiveCursorDOWN();
	// 現在のメニュー状態取得関数
	enPauseMenuState GetNowMenuState()
	{
		return m_enPauseMenuState;
	}

private:
	// ------ 各画像のロード関数 ------.
	// すべての画像.
	bool LoadAllImage();
	// タイトル画像. 
	bool LoadPauseTitltImage();
	// "タイトルへ"テキスト画像.
	bool LoadToTilteImage();
	// "ゲームへ"テキスト画像.
	bool LoadToGameImage();
	// 選択カーソルの画像.
	bool LoadCursorImage();

private:
	// ポーズメニュー.
	enPauseMenuState	m_enPauseMenuState;

	// ポーズタイトル画像.
	shared_ptr<CSpriteUI>	m_pPauseTitleImage;
	// タイトルへ戻る画像.
	shared_ptr<CSpriteUI>	m_pToTitleImage;
	// ゲームへ戻る画像.
	shared_ptr<CSpriteUI>	m_pToGameImage;
	// カーソル画像.
	shared_ptr<CSpriteUI>	m_pCursor;

	// カーソル座標.
	D3DXVECTOR3	m_vCursorPosition[2];
	bool m_bPushDownKey;
	bool m_bPushUpKey;
};

#endif	// #ifndef PAUSE_H.