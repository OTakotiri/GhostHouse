#ifndef COMMAND_H
#define COMMAND_H

#include "..\SingletonBase\SingletonBase.h"

// コナミコマンド.
namespace COMMAND{
	enum HashKey
	{
		None,

		Up1,	
		Up2,	
		Down1,	
		Down2,	
		Left1,	
		Right1,	
		Left2,	
		Right2,	
		B,		
		A,		

		// 最後の'A'を最後として使用.
		Max = A,
	};
};

class CCommand : public std::CSingletonBase<CCommand>
{
public:
	CCommand();
	~CCommand();

	// 更新関数.
	static void Update();
	// 初期化関数 : タイトルシーンのコンストラクタで使用.
	static void Initialize();
	// 成功かどうか取得.
	static bool GetSuccessCommand(){ return GetInstance()->m_isSuccessCommand; };

private:
	// 初期化関数.
	void Init();
	// コマンド入力関数.
	void KeyInput();
	// コマンド成功確認関数.
	void CheckCommand();
	// 上下キーを押した際の処理.
	void PushUpDownKey();
	// 左右キーを押した際の処理.
	void PushLeftRightKey( int cnt, int cnt2 );
	// ボタンを押した際のカウント処理.
	void TimeCount();

private:
	int		m_CommandAdd;		// コマンド入力カウント.
	int		m_TimeCount;		// 入力されてからのタイムカウント.
	bool	m_isStart;			// スタートしたかどうか.
	int		m_isTwoPush;		// 二回目の入力チェック.

	bool m_isSuccessCommand;	// コマンドが成功したかどうか.

};

#endif	// #ifndef COMMAND_H.