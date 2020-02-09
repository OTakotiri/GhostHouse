#ifndef STAGE_SELECT_UI_H
#define STAGE_SELECT_UI_H

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"
#include "..\..\Resource\MapLoad\MapLoad.h"
#include "..\..\DebugMode\DebugText\DebugText.h"


class CStageSelectUI
{
private:
	// 矢印画像初期サイズ値.
	float ARROW_INIT_SIZE = 1.0f;
	// 矢印サイズ変更カウント値.
	float ARROW_SIZE_CHNGE_COUNT = 3.0f;
	// 矢印サイズ変更カウント最大値.
	float ARROW_SIZE_CHNGE_COUNT_MAX = 60.0f;
	// 矢印最小サイズ値.
	float ARROW_SIZE_MINIMUM_VALUE = 0.8f;

public:
	CStageSelectUI();
	~CStageSelectUI();

	bool Load();

	// 描画関数.
	void Render();

	// ステージ上選択関数.
	void MoveUpStage();
	// ステージ下選択関数.
	void MoveDownStage();
	// ステージ名決定.
	bool SetStageName();

private:
	// 矢印画像サイズ変更関数.
	void ArrowSizeChange();
	// ステージ変更関数.
	void StageChange();

	/****************************
	*	↓ 画像系.
	*/
	// 上矢印画像描画.
	void UpArrowRender();
	// 下矢印画像描画.
	void DownArrowRender();

	// すべての画像の読み込み.
	bool LoadAllImage();
	// ステージセレクトタイトル画像読み込み.
	bool LoadStageSelectTitle();

	// 上矢印画像読み込み関数.
	bool LoadUpArrow();
	// 下矢印画像読み込み関数.
	bool LoadDownArrow();

	// ステージ名枠画像読み込み関数.
	bool LoadStageNameFrame();

private:
	// 現在のステージ番号.
	int m_iNowStageNumber;

	// ステージ上移動フラグ.
	bool m_bMoveUpStageFlag;
	// ステージ下移動フラグ.
	bool m_bMoveDownStageFlag;

	// 矢印画像サイズ.
	float m_fArrowSize;
	// サイズのカウント.
	float m_fSizeCount;

	bool	m_bPlaySE;

	/****************************
	*	↓ 画像系.
	*/

	// ステージセレクトタイトル画像.
	shared_ptr<CSpriteUI>	m_pStageSelectTitle;
	// 上矢印画像.
	shared_ptr<CSpriteUI>	m_pUpArrows;
	// 下矢印画像.
	shared_ptr<CSpriteUI>	m_pDownArrows;
	// ステージ名枠画像.
	shared_ptr<CSpriteUI>	m_pStageNameFrame;
};

#endif	// #ifndef STAGE_SELECT_UI_H.