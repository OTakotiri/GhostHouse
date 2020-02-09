#ifndef C_TUTORIAL_TEXT_H
#define C_TUTORIAL_TEXT_H
#include "..\..\Resource\SpriteResouce\SpriteResouce.h"
#include "..\..\ObjectBase\ObjectBase.h"

class CTutorialText
{

public:
	CTutorialText();
	~CTutorialText();

	bool Load();
	// 更新関数.
	void Update();
	// 描画関数.
	void Render(int ArrayNumber, shared_ptr<CObjectBase> pObj);

	void CharRender(float UV_X, float UV_Y);

	void SetFontPosition(float PositionX, float PositionY, float PositionZ);
	bool LoadAllImage();

	void TextDraw(int ArrayNumber, shared_ptr<CObjectBase> pObj);

	static void SetArrayNumber(int arrayNumber)
	{
		m_sArrayNumber = arrayNumber;
	}
private:

	bool LoadFontImage();

	bool LoadWindowImage();
	shared_ptr<CSpriteUI>	m_pTutorialFont;		//チュートリアルテキスト用.
	shared_ptr<CSpriteUI>	m_pMessageWindow;		//チュートリアルメッセージウィンドウ.

	D3DXVECTOR3				m_vTextPosition;		//チュートリアルテキストポジション.
	D3DXVECTOR3				m_vMessageWndPosition;	//チュートリアルメッセージウィンドウポジション

	int		m_TextNumber;
	int		m_TextCount;
	int		m_OldTextArray;
	bool	m_bChange;
	int		m_Count;

	static int m_sArrayNumber;
};




#endif //#ifndef C_TUTORIAL_TEXT_H