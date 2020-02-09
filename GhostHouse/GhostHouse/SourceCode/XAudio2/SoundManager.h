#ifndef C_SOUNDMANAGER_H
#define C_SOUNDMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include "XAudio2Play/XAudio2PlaySound.h"
#include "XAudio2WaveLoad/XAudio2WaveLoad.h"
class CSoundManager
{
private:
	// Soundデータが入っているディレクトリパス.
	const char* FILE_PATH = "Data\\Sound";
public:
	CSoundManager();
	~CSoundManager();

	// インスタンスの取得.
	static CSoundManager* GetInstance();

	// サウンドデータ作成.
	static void CreateSoundData();
//========================================================================================
//	BGM
//====
	// BGM再生関数.
	static void PlayBGM(std::string Name, bool& isEnd, bool LoopFlag);
	// BGM停止関数.
	static void StopBGM(std::string Name);
	// BGMフェードアウト関数.
	static bool FadeOutBGM(std::string Name);
	// BGMフェードイン関数.
	static bool FadeInBGM(std::string Name);
	// BGMの音量取得関数.
	static float GetBGMVolme(std::string Name);
	// BGMの音量セット関数.
	static void SetBGMVolme(std::string Name, float Volme);
//========================================================================================
//	SE
//====
	// SE再生関数.
	static void PlaySE(std::string& Name, bool & isEnd, bool & ChangeSE,bool& IsPlay,bool& RePlay);
	// SE停止関数.
	static void StopSE(std::string Name);
	// 解放処理関数.
	static void Release();
	// 異常停止フラグセット関数.
	static void SetEmergencyStopFlag(bool StopFlag) { GetInstance()->m_bEmergencyStop = StopFlag; }

private:
	// Wavデータクラスの箱.
	std::map <std::string, std::shared_ptr<clsXAudio2WaveLoad>> pWaveData;
	// BGM用PlaySoundクラスの箱.
	std::map <std::string, std::shared_ptr<clsXAudio2PlaySound>> pBgmSource;

	// BGMの名前リスト.
	// 解放時に必要.
	std::vector<std::string>	m_vsBGMNameList;

	// SEの名前リスト.
	// 解放時に必要.
	std::vector<std::string>	m_vsSENameList;
	// SE用PlaySoundクラスの箱.
	std::map <std::string, std::shared_ptr<clsXAudio2PlaySound>> pSeSource;
	// サウンドデータ取得を一回だけ通るためフラグ.
	bool m_bFristCall;
	// 終了用フラグ.
	bool	m_bEndGame;
	// 異常終了用フラグ.
	bool	m_bEmergencyStop;
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSoundManager(const CSoundManager &) = delete;
	CSoundManager& operator = (const CSoundManager &) = delete;
	CSoundManager(CSoundManager &&) = delete;
	CSoundManager& operator = (CSoundManager &&) = delete;
};



#endif // #ifndef C_SOUNDMANAGER_H
