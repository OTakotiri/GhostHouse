#include "SoundManager.h"
#include <crtdbg.h>
#include <filesystem>	// C++17 必須.
namespace fs = std::filesystem;
CSoundManager::CSoundManager()
	: m_bEndGame		( false )
	, m_bEmergencyStop	( false )
	, m_bFristCall		( true )
{
}

CSoundManager::~CSoundManager()
{
}
// インスタンスの作成.
CSoundManager* CSoundManager::GetInstance()
{
	static std::unique_ptr<CSoundManager> pInstance =
		std::make_unique<CSoundManager>();	// インスタンスの作成.
	return pInstance.get();
}

//========================================================================================
//	サウンドデータ作成.
//====
void CSoundManager::CreateSoundData()
{
	if (GetInstance()->m_bFristCall == true) {

		auto eachLoad = [&](const fs::directory_entry& entry)
		{
			const std::string extension = entry.path().extension().string();	// 拡張子.
			const std::string filePath = entry.path().string();					// ファイルパス.
			const std::string fileName = entry.path().stem().string();			// ファイル名.

			if (extension != ".wav" && extension != ".WAV") return;	// 拡張子が "wavファイル" じゃなければ終了.
			//ファイルパス内にBGMがあればBGM、なければSE.
			if (filePath.find("BGM") == std::string::npos) {
				// WavLoadクラス作成.
				GetInstance()->pWaveData[fileName] = std::make_shared<clsXAudio2WaveLoad>();
				// Wavフォーマット取得.
				GetInstance()->pWaveData[fileName]->GetWaveFormat(fileName.c_str());
				// PlaySoundクラス作成.
				GetInstance()->pSeSource[fileName] = std::make_shared<clsXAudio2PlaySound>();
				// SEのSoundSource作成.
				GetInstance()->pSeSource[fileName]->CreateSound(GetInstance()->pWaveData[fileName], GetInstance()->pWaveData[fileName]->GetFileName());
				// フォルダの名前をSE名前リストに入れる.
				GetInstance()->m_vsSENameList.emplace_back(fileName);
			}
			else {
				// WavLoadクラス作成.
				GetInstance()->pWaveData[fileName] = std::make_shared<clsXAudio2WaveLoad>();
				// Wavフォーマット取得.
				GetInstance()->pWaveData[fileName]->GetWaveFormat(fileName.c_str());
				// PlaySoundクラス作成.
				GetInstance()->pBgmSource[fileName] = std::make_shared<clsXAudio2PlaySound>();
				// BGMのSoundSource作成.
				GetInstance()->pBgmSource[fileName]->CreateSound(GetInstance()->pWaveData[fileName], GetInstance()->pWaveData[fileName]->GetFileName());
				// フォルダの名前をSE名前リストに入れる.
				GetInstance()->m_vsBGMNameList.emplace_back(fileName);
			}
		};

		try {
			//指定パス(FILE_PATH)ディレクトリ内を再帰的に走査
			fs::recursive_directory_iterator dir_itr(GetInstance()->FILE_PATH), end_itr;
			// eachLoad関数を行う.
			std::for_each(dir_itr, end_itr, eachLoad);

		}
		// エラーキャッチ.
		catch (const fs::filesystem_error& e) {
			const char* errorMessage = e.path1().string().c_str();
			_ASSERT_EXPR(false, L"サウンドデータ作成失敗");
			MessageBox(nullptr, errorMessage, "サウンドデータ作成失敗", MB_OK);
		}

		GetInstance()->m_bFristCall = false;
	}
}
//========================================================================================
//	BGM.
//====

// BGM再生関数.
void CSoundManager::PlayBGM(std::string Name, bool& isEnd, bool LoopFlag)
{
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return;
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;
	// ループフラグがtrueならループで再生.
	if (LoopFlag == true) {
		GetInstance()->pBgmSource[Name]->LoopPlay(GetInstance()->pWaveData[Name]->GetFileName(), isEnd, GetInstance()->m_bEmergencyStop);
	}
	else {
		GetInstance()->pBgmSource[Name]->Play(GetInstance()->pWaveData[Name]->GetFileName(), isEnd, GetInstance()->m_bEmergencyStop);
	}
}
// BGM停止関数.
void CSoundManager::StopBGM(std::string Name)
{
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return;
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;
	// 指定したBGM停止.
	GetInstance()->pBgmSource[Name]->Stop();
}
// BGMフェードアウト関数.
bool CSoundManager::FadeOutBGM(std::string Name)
{
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return true;

	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return true;
	// 指定したBGMの音量が0より大きければフェードアウトフラグを立てる.
	if (GetInstance()->pBgmSource[Name]->GetVolume() > 0) {
		GetInstance()->pBgmSource[Name]->SetFadeOutFlag(true);
	}
	else if (GetInstance()->pBgmSource[Name]->GetVolume() == 0) {
		return true;
	}

	return true;
}
// BGMフェードイン関数.
bool CSoundManager::FadeInBGM(std::string Name)
{
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return true;

	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return true;	
	// 指定したBGMの音量が1.0f(最大)より小さければフェードインフラグを立てる.
	if (GetInstance()->pBgmSource[Name]->GetVolume() < 1.0f) {
		GetInstance()->pBgmSource[Name]->SetFadeInFlag(true);
	}
	else if (GetInstance()->pBgmSource[Name]->GetVolume() == 1.0f) {
		return true;
	}

	return true;
}
// 指定したBGMの音量を取得する関数.
float CSoundManager::GetBGMVolme(std::string Name)
{
	// ゲーム終了フラグがたっていればリターン -1.
	if (GetInstance()->m_bEndGame == true) return -1;	
	// 存在しなければリターン.
	if (GetInstance()->pBgmSource[Name] == nullptr) return -1;

	return GetInstance()->pBgmSource[Name]->GetVolume();
}
// 指定したBGMの音量をセットする関数.
void CSoundManager::SetBGMVolme(std::string Name, float Volme)
{
	// ゲーム終了フラグがたっていればリターン.
	if (GetInstance()->m_bEndGame == true) return;

	GetInstance()->pBgmSource[Name]->SetVolume(Volme);
}

//========================================================================================
//	SE.
//====

// SE再生関数.
void CSoundManager::PlaySE(
	std::string& Name, bool & isEnd, bool & ChangeSE, bool& IsPlay, bool& RePlay)
{
	bool Reset = true;			// サウンド(SE)をはじめから再生する.
	bool IsError = false;		// エラー対策.
	bool ReturnTop = false;		// 下で再生終了してwhile待機している実行位置をこのブロックの上に戻すフラグ.
	// 終了フラグが立っていない場合ループ内へ.
	while (isEnd == false)
	{
		// 変数リセット.
		Reset = true;			
		IsError = false;		
		ReturnTop = false;		

		// 存在しなければエラー.
		if (GetInstance()->pSeSource[Name] == nullptr) {
			IsError = true;
		}
		if(IsError == false)
		{
			size_t Number = GetInstance()->pWaveData.size() - GetInstance()->m_vsSENameList.size();
			// 再生.
			GetInstance()->pSeSource[Name]->SEPlay(
				GetInstance()->pWaveData[Name]->GetFileName(), isEnd, Reset, IsPlay, RePlay, GetInstance()->m_bEmergencyStop);
			// ここに来たらSEの再生が終わっているので、再生中に立っているフラグを下す.
			IsPlay = false;
		}
		// 再生終了後、新たなSEが入って来るまで待機.
		while ( ReturnTop == false)
		{
			// 新たなSE再生命令 or 終了解放命令が飛んで来たら中へ.
			if (ChangeSE == true) {
				ChangeSE = false;
				ReturnTop = true;
			}
			if (isEnd == true || GetInstance()->m_bEmergencyStop == true) {
				break;
			}
		}
		// 上のwhileを抜けている場合ここに絶対入る.
		if (ReturnTop == true) {
			// 解放命令が出ていた場合breakして、スレッドを解放する.
			if (isEnd == true || GetInstance()->m_bEmergencyStop == true) {
				break;
			}
			continue;
		}

	}

}
// SE停止関数.
void CSoundManager::StopSE(std::string Name)
{
	GetInstance()->pSeSource[Name]->Stop();
}
// 解放関数.
void CSoundManager::Release()
{
	// SEの名前リスト数分ループを回す.
	for (size_t i = 0; i < GetInstance()->m_vsSENameList.size(); i++) {
		GetInstance()->pWaveData[GetInstance()->m_vsSENameList[i]]->Close();			//WavDataを閉じる.
		GetInstance()->pSeSource[GetInstance()->m_vsSENameList[i]]->DestoroySource();	// SoundSourceを解放.
	}
	// BGMの名前リスト数分ループを回す.
	for (size_t i = 0; i < GetInstance()->m_vsBGMNameList.size(); i++) {	
		GetInstance()->pWaveData[GetInstance()->m_vsBGMNameList[i]]->Close();			//WavDataを閉じる.
		GetInstance()->pBgmSource[GetInstance()->m_vsBGMNameList[i]]->DestoroySource();	// SoundSourceを解放.
	}
	// ゲーム終了フラグを立てる.
	GetInstance()->m_bEndGame = true;
}
