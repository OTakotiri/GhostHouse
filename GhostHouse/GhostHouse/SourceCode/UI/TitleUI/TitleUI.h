#ifndef TITLE_UI_H
#define TITLE_UI_H

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"

class CTitleUI
{
public:
	enum enTitleMenuState
	{
		None,

		Start,	// ゲームスタート.
		End,	// ゲーム終了.
		Edit,

		Max,
	};

private:
	float MAX_CURSOR_POSITION_Y;		// カーソルの最大値.
	float MIN_CURSOR_POSITION_Y;		// カーソルの最小値.
	float CURSOR_MOVE_SPEED;			// カーソルの移動値.
	const int STICK_IDLING	= 30000;	// スティックの遊び.


public:
	CTitleUI();
	~CTitleUI();

	bool Load();

	// 更新関数.
	void Update();
	// 描画関数.
	void Render();

	// カーソル, 上への操作.
	void GiveCursorUP();
	// カーソル, 下への操作.
	void GiveCursorDOWN();
	// 現在のメニュー状態取得関数
	enTitleMenuState GetNowMenuState(){ return m_enTitleMenuState; }

private:
	// ------ 各画像のロード関数 ------.
	// すべての画像.
	bool LoadAllImage();	
	// タイトル画像. 
	bool LoadTitltImage();	
	// "スタート"テキスト画像.
	bool LoadStartImage();
	// "終了"テキスト画像.
	bool LoadEndImage();
	// 選択カーソルの画像.
	bool LoadCursorImage();

private:
	// タイトルメニュー.
	enTitleMenuState	m_enTitleMenuState;

	// タイトル画像.
	shared_ptr<CSpriteUI>	m_pTitleImage;
	// ゲームスタート画像.
	shared_ptr<CSpriteUI>	m_pStartImage;
	// ゲーム終了画像.
	shared_ptr<CSpriteUI>	m_pEndImage;
	// カーソル画像.
	shared_ptr<CSpriteUI>	m_pCursor;

	// カーソル座標.
	D3DXVECTOR3	m_vCursorPosition[2];
	bool m_bPushDownKey;
	bool m_bPushUpKey;
};

#endif	// #ifndef TITLE_UI_H.