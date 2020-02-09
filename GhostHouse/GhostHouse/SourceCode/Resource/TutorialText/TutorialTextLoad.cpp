#include "TutorialTextLoad.h"
#include <fstream>

CTutorialTextLoad::CTutorialTextLoad()
{

	InitHiraganaBox();
}
CTutorialTextLoad::~CTutorialTextLoad()
{
}

void CTutorialTextLoad::TextDataLoad()
{
	// ファイルを開く.
	fstream filestream(GetInstance()->TEXT_FILENAME);
	if (!filestream.is_open()) return;

	int i = 0;
	// ファイルを読み込む.
	while (!filestream.eof()) {
		string buffer;
		//Textファイルから読み取った1行をstring変数「buffer]
		filestream >> buffer;
		//要素を増やして初期化.
		//GetInstance()->TextBox.emplace_back();
		GetInstance()->m_vbAsteriskFlag.emplace_back(false);
		GetInstance()->TextBox.emplace_back(buffer);
		GetInstance()->For_searchTextBox.emplace_back(buffer);
		i++;
	}

	GetInstance()->m_HiragnaBoxSize = GetInstance()->HiraganaBox.size();

}

D3DXVECTOR2 CTutorialTextLoad::SearchCharPosition(size_t TextBoxNum, size_t TwoArray)
{
	if (TextBoxNum > GetInstance()->TextBox.size())return D3DXVECTOR2(7, 3);

	// 1文字取得.
	string Tmp;
	//日本語は2バイトなので、substrで
	//(開始位置「あいうえお」で「う」なら4, バイト数(日本語なら2)を指定して取り出す.
	Tmp = GetInstance()->TextBox[TextBoxNum].substr(TwoArray, 2);
	int X = 0;
	int Y = 0;
	int NegativNumX = 1;
	int NegativNumY = 5;
	int Count = 0;
	if (Tmp == " " || Tmp == "　") {
		return D3DXVECTOR2(0, 3);
	}
	// X = 〇〇 の数値は最大横幅.W 1600 なら 16.
	for (size_t i = 0; i < GetInstance()->m_HiragnaBoxSize - 1; i += 2) {
		if (Tmp.c_str() == GetInstance()->HiraganaBox.substr(i, 2)) {
			X = 16 - NegativNumX;
			Y = 5 - NegativNumY;
			return D3DXVECTOR2((FLOAT)X, (FLOAT)Y);
		}
		else {
			Count++;
			// 「っ」の下には何もないのでかうんとをマックスにする.
			if (GetInstance()->HiraganaBox.substr(i, 2) == "っ") {
				Count = 5;
			}
			NegativNumY -= 1;
		}
		if (Count == 5) {
			NegativNumX += 1;
			NegativNumY = 5;
			Count = 0;
		}
	}
	return D3DXVECTOR2(7, 3);


}

bool CTutorialTextLoad::SearchFiristAsterisk(int ArrayNum)
{
	if (ArrayNum < 0) {
		return false;
	}
	// "＊"が見つかったテキストの配列番号に発見フラグが立っていたらここでtrueを返す.
	if (GetInstance()->m_vbAsteriskFlag[ArrayNum] == true) {
		return true;
	}
	if (GetInstance()->TextBox[ArrayNum].find("＊") != std::string::npos) {
		// 先頭の"＊"を削除.
		// 次の文字と重なるので2回消す.
		GetInstance()->TextBox[ArrayNum].erase(GetInstance()->TextBox[ArrayNum].begin());
		GetInstance()->TextBox[ArrayNum].erase(GetInstance()->TextBox[ArrayNum].begin());
		GetInstance()->m_vbAsteriskFlag[ArrayNum] = true;
		return true;
	}
	return false;
}

int CTutorialTextLoad::SearchNowTextArea(int AreaNumber)
{
	if (AreaNumber < 0) {
		return false;
	}
	// "＿ＡＲＥＡ"にステージナンバーをくっつける.
	string AreaName = "＿ＡＲＥＡ" + GetInstance()->ConversionHalfChar(AreaNumber);
	int ArrayofNumber = GetInstance()->TextBox.size();
	for (int i = 0; i < ArrayofNumber; i++) {
		// テキストに "＿ＡＲＥＡ〇"がついている物を探す.
		if (GetInstance()->For_searchTextBox[i].find(AreaName) != std::string::npos) {
			// 表示用の"＿ＡＲＥＡ〇が消えていなければ消す."
			if (GetInstance()->TextBox[i].find(AreaName) != std::string::npos) {
				GetInstance()->TextBox[i].erase(GetInstance()->TextBox[i].find(AreaName), GetInstance()->TextBox[i].back());
			}
			// 発見したところのテキスト配列番号を返す.
			return i;
		}
	}

	return 0;
}

string CTutorialTextLoad::ConversionHalfChar(int Number)
{

	switch (Number)
	{
	case 0:
		return "０";
	case 1:
		return "１";
	case 2:
		return "２";
	case 3:
		return "３";
	case 4:
		return "４";
	case 5:
		return "５";
	case 6:
		return "６";
	default:
		break;
	}
	return "－１";
}

void CTutorialTextLoad::InitHiraganaBox()
{
	HiraganaBox = "あいうえお"
		"かきくけこ"
		"さしすせそ"
		"たちつてと"
		"なにぬねの"
		"はひふへほ"
		"まみむめも"
		"やゆよ？！"
		"らりるれろ"
		"わをん。、"
		"がぎぐげご"
		"ざじずぜぞ"
		"だぢづでど"
		"ばびぶべぼ"
		"ぱぴぷぺぽ"
		"ーっ";
}

