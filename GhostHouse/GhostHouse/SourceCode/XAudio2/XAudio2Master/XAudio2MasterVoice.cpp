#include "XAudio2.h"
#include <iostream>
#include "XAudio2MasterVoice.h"

static bool OneCall = true;

clsXAudio2MasterVoice& clsXAudio2MasterVoice::GetGlobalSystem()
{
	static clsXAudio2MasterVoice xAudioMaster;
	return xAudioMaster;
}

clsXAudio2MasterVoice::clsXAudio2MasterVoice()
{
	Initialize();
}

clsXAudio2MasterVoice::~clsXAudio2MasterVoice()
{
}
// インスタンスの作成.
clsXAudio2MasterVoice* clsXAudio2MasterVoice::GetInstance()
{
	static std::unique_ptr<clsXAudio2MasterVoice> pInstance =
		std::make_unique<clsXAudio2MasterVoice>();	// インスタンスの作成.
	return pInstance.get();
}


// XAudio取得.
IXAudio2* clsXAudio2MasterVoice::GetInterface()const
{
	return m_pXAudioInterface;
}

// マスターボイス取得.
IXAudio2MasteringVoice* clsXAudio2MasterVoice::GetMasteringVoice()const
{
	return m_pMasterVoice;
}

//初期化 XAudio作成 マスターボイス作成.
bool clsXAudio2MasterVoice::Initialize()
{
	// COM の初期化
	if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)) &&
		FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		return false;
	}

	// XAudio2 インターフェースの作成
	if (FAILED(XAudio2Create(&m_pXAudioInterface, 0)))
	{
		return false;
	}

	// マスターボイスの作成
	if (FAILED(m_pXAudioInterface->CreateMasteringVoice(&m_pMasterVoice)))
	{
		return FALSE;
	}

	return true;
}

void clsXAudio2MasterVoice::Release()
{
	if (GetInstance()->m_pMasterVoice == nullptr) return;
	GetInstance()->m_pMasterVoice->DestroyVoice();
	GetInstance()->m_pMasterVoice = nullptr;
	COM_SAFE_RELEASE(GetInstance()->m_pXAudioInterface);
	GetInstance()->m_pXAudioInterface = nullptr;
	//CoUninitialize();
}