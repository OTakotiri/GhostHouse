#ifndef C_TUTORIAL_TEXTLOAD_H
#define C_TUTORIAL_TEXTLOAD_H

#include "..\..\SingletonBase\SingletonBase.h"
#include "..\..\Main\Global.h"

using namespace std;

class CTutorialTextLoad : public CSingletonBase<CTutorialTextLoad>
{
private:
	const char* TEXT_FILENAME = "Data\\Tutorial.txt";	// テキストパス.
public:
	CTutorialTextLoad();
	~CTutorialTextLoad();
	// データロード.
	static void TextDataLoad();
	// テキストの子配列の数を返す関数.
	static int GetTextArraySize(int i)
	{
		return GetInstance()->TextBox[i].size();
	}
	// テキストの配列数を返す関数.
	static int GetAllTextSize()
	{
		return GetInstance()->TextBox.size();
	}
	// テキストに合わせた、フォントのUV座標を検索.
	static D3DXVECTOR2 SearchCharPosition(size_t TextBoxNum, size_t TwoArray);
	// アスタリスクを検索.
	static bool SearchFiristAsterisk(int ArrayNum);
	// エリアが変わるタイミングのテキストを検索(規則).
	static int SearchNowTextArea(int AreaNumber);
private:
	// 半角数字を全角数字に.
	string ConversionHalfChar(int Number);
	// ひらがなを入れておく箱.
	void InitHiraganaBox();
	// 二次元配列.
	vector<string> TextBox;
	// 規則検索用.
	vector<string> For_searchTextBox;
	//ひらがなを入れておく変数.
	string HiraganaBox;
	size_t	m_HiragnaBoxSize;
	vector<bool>	m_vbAsteriskFlag;

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CTutorialTextLoad(const CTutorialTextLoad &) = delete;
	CTutorialTextLoad& operator = (const CTutorialTextLoad &) = delete;
	CTutorialTextLoad(CTutorialTextLoad &&) = delete;
	CTutorialTextLoad& operator = (CTutorialTextLoad &&) = delete;
};



#endif // !C_TUTORIAL_TEXTLOAD_H
