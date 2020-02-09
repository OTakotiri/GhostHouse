#ifndef FADE_UI_H
#define FADE_UI_H

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"
#include "..\..\SingletonBase\SingletonBase.h"

class CFadeUI : CSingletonBase<CFadeUI>
{
	const float MIN_ALPHA	= 0.0f;
	const float MAX_ALPHA	= 1.0f;
	const float FADE_VALUE	= 0.02f;

public:
	CFadeUI();
	~CFadeUI();

	// 初期化用関数.
	static void Init( bool isrender = true );
	// 描画関数.
	static void Render();

	// フェードイン開始関数.
	static void FadeINStart();
	// フェードアウト開始関数.
	static void FadeOUTStart();

	// アルファ値の初期化関数.
	static void InitAlpha(){ GetInstance()->m_ImageAlpha = 0.0f; }

	// フェード中か.
	static bool isActive(){ return GetInstance()->m_isRender; }
	// フェードが終了したか.
	static bool isEnd(){ return GetInstance()->m_isFadeEnd; }

private:
	// 読込関数(Renderに内包).
	void Load();
	// 更新関数(Renderに内包).
	void Update();

private:
	shared_ptr<CSpriteUI>	m_pFadeSprite;	// フェード用の画像.

	bool	m_bFadeIN;		// フェードインしているかどうか.
	bool	m_bFadeOUT;		// フェードアウトしているかどうか.
	float	m_ImageAlpha;	// 画像のアルファ値.
	bool	m_isRender;		// 描画しているか.
	bool	m_isFadeEnd;	// フェードが終了しているか.

};

#endif	// #ifndef FADE_UI_H.