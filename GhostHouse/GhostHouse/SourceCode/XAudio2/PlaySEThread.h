#ifndef C_PLAYSE_THREAD_H
#define C_PLAYSE_THREAD_H
#include <process.h>
#include <thread>
#include <Windows.h>

class CPlaySEThread
{
public:
	CPlaySEThread();
	~CPlaySEThread();

	// インスタンスの取得.
	static CPlaySEThread* GetInstance();

	// スレッドを立ち上げる関数(一回のみ).
	static void SetUpThread();
	// アップデート関数.
	static void Updata();
	// アップデート関数で常に回す、SEを鳴らすための関数.
	static void Sound();
	// 後処理.
	static bool Release();
	// SEの名前をセットする関数.
	static void SetSEName(std::string SEName) {
		GetInstance()->WaitInSEName = SEName;
	}

private:
	// 一回だけ関数を通るようにするフラグ.
	bool m_bFristCreate;



	std::string m_sOldSEName;	// 前に1つ目のスレッドで再生したSEの名前を入れるstring型変数.
	std::string m_sOldSEName2;  // 前に2つ目のスレッドで再生したSEの名前を入れるstring型変数.
	std::string WaitInSEName;	// 再生しようとしているSEの名前を入れるstring型変数.

	std::string m_sSEName;		// 1つ目のスレッドで再生するSEの名前を入れるstring型変数.
	std::string m_sSEName2;		// 2つ目のスレッドで再生するSEの名前を入れるstring型変数.
	std::string m_sSEName3;		// 3つ目のスレッドで再生するSEの名前を入れるstring型変数.

	bool	m_ChangeSE;				// 待機しているスレッドに、新たなSE再生命令が走った際にtrueになる変数.
	bool	m_bIsPlay;				// SE再生中のスレッドはtrue.
	std::thread SEThread;			// SEを鳴らすスレッド.
	std::thread::id InSEThreadID;	// SEスレッドID退避.

	bool	m_ChangeSE2;			// 待機しているスレッドに、新たなSE再生命令が走った際にtrueになる変数.
	bool	m_bIsPlay2;				// SE再生中のスレッドはtrue.
	std::thread SEThread2;			// SEを鳴らすスレッド.
	std::thread::id InSEThreadID2;	// SEスレッドID退避.

	bool	m_ChangeSE3;			// 待機しているスレッドに、新たなSE再生命令が走った際にtrueになる変数.
	bool	m_bIsPlay3;				// SE再生中のスレッドはtrue.
	std::thread SEThread3;			// SEを鳴らすスレッド.
	std::thread::id InSEThreadID3;	// SEスレッドID退避.

	bool m_isEnd;					// ゲームを終了しようとしていたらtrue.

	bool m_bReplaySE;				// SEをはじめから鳴らしたいときにtrueにする変数.
	bool m_bReplaySE2;				// SEをはじめから鳴らしたいときにtrueにする変数.
	bool m_bReplaySE3;				// SEをはじめから鳴らしたいときにtrueにする変数.

	DWORD m_dThreadExitCode_1;		// スレッド1の処理が終了して、開放できるかどうかの値を入れるDWORD型変数.
	DWORD m_dThreadExitCode_2;		// スレッド2の処理が終了して、開放できるかどうかの値を入れるDWORD型変数.
	DWORD m_dThreadExitCode_3;		// スレッド3の処理が終了して、開放できるかどうかの値を入れるDWORD型変数.

	bool m_bReleaseOK_1;			// スレッド1の開放が安全にできるかどうかのフラグ.
	bool m_bReleaseOK_2;			// スレッド2の開放が安全にできるかどうかのフラグ.
	bool m_bReleaseOK_3;			// スレッド3の開放が安全にできるかどうかのフラグ.
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CPlaySEThread(const CPlaySEThread &) = delete;
	CPlaySEThread& operator = (const CPlaySEThread &) = delete;
	CPlaySEThread(CPlaySEThread &&) = delete;
	CPlaySEThread& operator = (CPlaySEThread &&) = delete;
};

#endif // !C_PLAYSE_THREAD_H