#ifndef C_XAUDIO2_MASTERVOICE_H
#define C_XAUDIO2_MASTERVOICE_H

#include "XAudio2.h"
#define COM_SAFE_RELEASE( p ) { if(p) { (p)->Release(); (p) = nullptr; } }

class clsXAudio2MasterVoice
{
public:
	clsXAudio2MasterVoice();
	~clsXAudio2MasterVoice();

	// インスタンスの取得.
	static clsXAudio2MasterVoice* GetInstance();

	IXAudio2* GetInterface()const;
	// マスターボイス取得.
	IXAudio2MasteringVoice* GetMasteringVoice()const;
	static clsXAudio2MasterVoice& GetGlobalSystem();
	// 開放関数.
	static void Release();
protected:
	// 初期化関数.
	bool Initialize();

	IXAudio2 *              m_pXAudioInterface;                //XAudio2インターフェース
	IXAudio2MasteringVoice *m_pMasterVoice;                    //マスタボイス
private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	clsXAudio2MasterVoice(const clsXAudio2MasterVoice &) = delete;
	clsXAudio2MasterVoice& operator = (const clsXAudio2MasterVoice &) = delete;
	clsXAudio2MasterVoice(clsXAudio2MasterVoice &&) = delete;
	clsXAudio2MasterVoice& operator = (clsXAudio2MasterVoice &&) = delete;
};

#endif //#ifndef C_XAUDIO2_MASTERVOICE_H