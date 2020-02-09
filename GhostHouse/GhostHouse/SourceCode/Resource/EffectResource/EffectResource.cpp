#include "EffectResource.h"

CEffectResource::CEffectResource()
	: m_EffectList	()
{
}

CEffectResource::~CEffectResource()
{
	m_EffectList.clear();
}

CEffectResource* CEffectResource::GetInstance()
{
	static std::unique_ptr<CEffectResource> pInstance =
		std::make_unique<CEffectResource>();	// インスタンスの作成.
	return pInstance.get();
}


void CEffectResource::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	auto effectLoad = [&]( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
	{
		GetInstance()->EffectLoad( pDevice11, pContext11 );
		GetInstance()->m_isLoadEnd = true;
	};
	GetInstance()->m_thread = std::thread( effectLoad, pDevice11, pContext11 );
}

bool CEffectResource::ThreadRelease()
{
	if( GetInstance()->m_isThreadJoined == true ) return true;
	if( GetInstance()->m_isLoadEnd == false ) return false;
	GetInstance()->m_thread.join();
	GetInstance()->m_isThreadJoined = true;
	return true;
}

std::shared_ptr<CEffect> CEffectResource::GetEffect( const std::string& fileName )
{
	for( auto& m : GetInstance()->m_EffectList ){
		if( m.first == fileName ) return m.second;
	}

	if( GetInstance()->m_isLoadEnd == false ) return nullptr;
	_ASSERT_EXPR( false, L"スタティックメッシュが見つかりません" );
	MessageBox( nullptr, fileName.c_str(), "スタティックメッシュが見つかりません", MB_OK );
	return nullptr;
}

void CEffectResource::EffectLoad(
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11 )
{
	auto eachLoad = [&]( const fs::directory_entry& entry )
	{
		const std::string exe		= entry.path().extension().string();	// 拡張子.
		const std::string filePath	= entry.path().string();				// ファイルパス.
		const std::string fileName	= entry.path().stem().string();			// ファイル名.

		if( exe != ".efk" ) return;	// 拡張子が "Xファイル" じゃなければ終了.

		m_EffectList[fileName] = 
			std::make_shared<CEffect>( pDevice11, pContext11, filePath );
	};

	try {
		fs::recursive_directory_iterator dir_itr(FILE_PATH), end_itr;
		std::for_each( dir_itr, end_itr, eachLoad );
	} catch( const fs::filesystem_error& e ){
		const char* errorMessage = e.path1().string().c_str();
		MessageBox( nullptr, errorMessage, "エフェクト読み込み失敗", MB_OK );
	}
}