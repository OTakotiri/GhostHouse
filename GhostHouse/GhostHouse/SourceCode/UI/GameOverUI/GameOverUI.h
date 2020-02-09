#ifndef GAME_OVER_UI_H
#define GAME_OVER_UI_H

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"

class CGameOverUI
{
public:
	enum enGameOverMenuState
	{
		None,

		Continue,	// ゲームへ.
		End,		// タイトルへ.

		Max,
	};
private:
	float MAX_CURSOR_POSITION_Y;		// カーソルの最大値.
	float MIN_CURSOR_POSITION_Y;		// カーソルの最小値.
	float CURSOR_MOVE_SPEED;			// カーソルの移動値.
	const int STICK_IDLING = 30000;		// スティックの遊び.

public:
	CGameOverUI();
	~CGameOverUI()
	{
	};

	// 更新関数.
	void Upadate();
	// 描画関数.
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera );

	// カーソル, 上への操作.
	void GiveCursorUP();
	// カーソル, 下への操作.
	void GiveCursorDOWN();
	// 現在のメニュー状態取得関数
	enGameOverMenuState GetNowMenuState()
	{
		return m_enGameOverMenuState;
	}

private:
	// ----- 各画像のロード関数 ------.
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
	enGameOverMenuState	m_enGameOverMenuState;

	// ポーズタイトル画像.
	shared_ptr<CSpriteUI>	m_pGameOverTitleImage;
	// タイトルへ戻る画像.
	shared_ptr<CSpriteUI>	m_pContinueImage;
	// ゲームへ戻る画像.
	shared_ptr<CSpriteUI>	m_pEndImage;
	// カーソル画像.
	shared_ptr<CSpriteUI>	m_pCursor;

	// カーソル座標.
	D3DXVECTOR3	m_vCursorPosition[2];
	bool m_bPushDownKey;
	bool m_bPushUpKey;
};

#endif	// #ifndef GAME_OVER_UI_H.