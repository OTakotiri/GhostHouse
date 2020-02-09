#include "PlaySEThread.h"
#include "SoundManager.h"
CPlaySEThread::CPlaySEThread()
	: m_isEnd(false)
	, m_ChangeSE(false)
	, m_ChangeSE2(false)
	, m_ChangeSE3(false)
	, m_bIsPlay(false)
	, m_bIsPlay2(false)
	, m_bIsPlay3(false)
	, m_bReplaySE(false)
	, m_bReplaySE2(false)
	, m_bReplaySE3(false)
	, m_sSEName("")
	, m_sSEName2("")
	, m_sSEName3("")
	, m_bFristCreate(true)
{
	m_dThreadExitCode_1 = -1;
	m_dThreadExitCode_2 = -1;
	m_dThreadExitCode_3 = -1;
	m_bReleaseOK_1 = false;
	m_bReleaseOK_2 = false;
	m_bReleaseOK_3 = false;
}

CPlaySEThread::~CPlaySEThread()
{

}
// インスタンスの作成.
CPlaySEThread* CPlaySEThread::GetInstance()
{
	static std::unique_ptr<CPlaySEThread> pInstance =
		std::make_unique<CPlaySEThread>();	// インスタンスの作成.
	return pInstance.get();
}

void CPlaySEThread::SetUpThread()
{
	// 1回だけ通る.
	if (GetInstance()->m_bFristCreate == true) {
		// ラムダにSEを再生するための関数を入れる.
		auto SE1 = [&]()
		{
			CSoundManager::PlaySE(
				GetInstance()->m_sSEName, GetInstance()->m_isEnd, GetInstance()->m_ChangeSE,
					GetInstance()->m_bIsPlay, GetInstance()->m_bReplaySE);
		};
		GetInstance()->SEThread = std::thread(SE1);						// SE再生用スレッド1作成.
		GetInstance()->InSEThreadID = GetInstance()->SEThread.get_id();	// スレッド1のスレッドID退避.
		auto SE2 = [&]()
		{
			CSoundManager::PlaySE(
				GetInstance()->m_sSEName2, GetInstance()->m_isEnd, GetInstance()->m_ChangeSE2,
					GetInstance()->m_bIsPlay2, GetInstance()->m_bReplaySE2);
		};
		GetInstance()->SEThread2 = std::thread(SE2);						// SE再生用スレッド2作成.
		GetInstance()->InSEThreadID2 = GetInstance()->SEThread2.get_id();	// スレッド2のスレッドID退避.
		auto SE3 = [&]()
		{
			CSoundManager::PlaySE(
				GetInstance()->m_sSEName3, GetInstance()->m_isEnd, GetInstance()->m_ChangeSE3,
					GetInstance()->m_bIsPlay3, GetInstance()->m_bReplaySE3);
		};
		GetInstance()->SEThread3 = std::thread(SE3);						// SE再生用スレッド3作成.
		GetInstance()->InSEThreadID3 = GetInstance()->SEThread3.get_id();	// スレッド3のスレッドID退避.

		// 1回だけ呼ぶためのフラグを下す.
		GetInstance()->m_bFristCreate = false;
	}
}

void CPlaySEThread::Updata()
{
	Sound();
}
// スレッドで回している関数の引数にセットされている変数を変更して、音を鳴らさせる関数.
void CPlaySEThread::Sound()
{
	// 変数に名前が入れば中に入る.
	if (GetInstance()->WaitInSEName != "") {
		// 1つめのスレッドが何も再生していなければ中に入る.
		if (GetInstance()->m_bIsPlay == false) {
			GetInstance()->m_sSEName = GetInstance()->WaitInSEName;	// PlaySEの引数になっている変数に名前を入れる.
			GetInstance()->m_sOldSEName = GetInstance()->m_sSEName;	// 名前を過去再生した名前を入れる変数に入れる.
			GetInstance()->m_ChangeSE = true;						// PlaySEの引数になっている変数をtrueにする.
		}
		else {
			// 1つめのスレッドが再生しているが、前に再生したSEと名前が同じ場合中に入る.
			if (GetInstance()->WaitInSEName == GetInstance()->m_sOldSEName)
			{
				GetInstance()->m_sSEName = GetInstance()->WaitInSEName;	// PlaySEの引数になっている変数に名前を入れる.
				GetInstance()->m_ChangeSE = true;						// PlaySEの引数になっている変数をtrueにする.
				GetInstance()->m_bReplaySE = true;						// PlaySEの引数になっている変数をtrueにする.
			}
			else {
				// 2つめのスレッドが何も再生していなければ中に入る.
				if (GetInstance()->m_bIsPlay2 == false) {
					GetInstance()->m_sSEName2 = GetInstance()->WaitInSEName;
					GetInstance()->m_sOldSEName2 = GetInstance()->m_sSEName2;
					GetInstance()->m_ChangeSE2 = true;
				}
				else {
					// 2つめのスレッドが再生しているが、前に再生したSEと名前が同じ場合中に入る.
					if (GetInstance()->WaitInSEName == GetInstance()->m_sOldSEName2)
					{
						GetInstance()->m_sSEName2 = GetInstance()->WaitInSEName;
						GetInstance()->m_ChangeSE2 = true;
						GetInstance()->m_bReplaySE2 = true;
					}
					else {
						// 3つめのスレッドが何も再生していなければ中に入る.
						if (GetInstance()->m_bIsPlay3 == false)
						{
							GetInstance()->m_sSEName3 = GetInstance()->WaitInSEName;
							GetInstance()->m_ChangeSE3 = true;
							GetInstance()->m_bReplaySE3 = true;
						}
					}
				}
			}
		}
		// 再生しようとしているSEの名前を入れる変数を初期化.
		GetInstance()->WaitInSEName = "";
		return;
	}
}
// スレッド開放処理関数.
// この関数はwhileで、この関数がtrueを返すまで回す.
bool CPlaySEThread::Release()
{
	GetInstance()->m_isEnd = true;
	// スレッド1がまだ開放されていない場合中に入る.
	while (GetInstance()->m_bReleaseOK_1 == false)
	{

		GetExitCodeThread(GetInstance()->SEThread.native_handle(), &GetInstance()->m_dThreadExitCode_1);
		// スレッド1から帰ってくるhandleが0だったら、そのスレッドは安全に開放できるので中に入る.
		if (GetInstance()->m_dThreadExitCode_1 == 0) {
			// スレッドを作成した際に退避したIDと現在のIDが同じなら中に入る.
			if (GetInstance()->InSEThreadID == GetInstance()->SEThread.get_id()) {
				GetInstance()->SEThread.detach();		// スレッドをデタッチ.
				GetInstance()->m_bReleaseOK_1 = true;	// スレッド1の開放が終わったことを示す.
			}

		}
		return false;	// 開放がまだできなければfalseをかえす.
	}
	// スレッド2がまだ開放されていない場合中に入る.
	while (GetInstance()->m_bReleaseOK_2 == false)
	{
		GetExitCodeThread(GetInstance()->SEThread2.native_handle(), &GetInstance()->m_dThreadExitCode_2);
		if (GetInstance()->m_dThreadExitCode_2 == 0) {
			if (GetInstance()->InSEThreadID2 == GetInstance()->SEThread2.get_id()) {
				GetInstance()->SEThread2.detach();
				GetInstance()->m_bReleaseOK_2 = true;
			}
		}
		return false;
	}
	// スレッド3がまだ開放されていない場合中に入る.
	while (GetInstance()->m_bReleaseOK_3 == false)
	{
		GetExitCodeThread(GetInstance()->SEThread3.native_handle(), &GetInstance()->m_dThreadExitCode_3);
		if (GetInstance()->m_dThreadExitCode_3 == 0) {
			if (GetInstance()->InSEThreadID3 == GetInstance()->SEThread3.get_id()) {
				GetInstance()->SEThread3.detach();
				GetInstance()->m_bReleaseOK_3 = true;
			}
		}
		return false;
	}
	return true;	// 全てのスレッドの開放が終わったのでtrueを返す.
}
