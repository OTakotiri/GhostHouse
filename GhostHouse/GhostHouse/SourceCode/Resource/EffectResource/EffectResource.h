#ifndef EFFECT_RESOURCE_H
#define EFFECT_RESOURCE_H

#include "Effect/CEffect.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

class CEffectResource
{
	const char* FILE_PATH = "Data\\Effect";
	typedef std::unordered_map<std::string, std::shared_ptr<CEffect>> effect_map;
public:
	CEffectResource();
	~CEffectResource();
	// インスタンスの取得.
	static CEffectResource* GetInstance();

	// 読み込み.
	static void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	// スレッドの解放.	
	static bool ThreadRelease();
	// エフェクト取得関数.
	static std::shared_ptr<CEffect> GetEffect( const std::string& fileName );

private:
	void EffectLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );

private:
	effect_map m_EffectList;	// エフェクトリスト.

	std::thread m_thread;		// スレッド.

	bool m_isLoadEnd;			// ロードが終了したか.
	bool m_isThreadJoined;		// スレッドが解放されたか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CEffectResource( const CEffectResource & )					= delete;
	CEffectResource& operator = ( const CEffectResource & )		= delete;
	CEffectResource( CEffectResource && )						= delete;
	CEffectResource& operator = ( CEffectResource && )			= delete;
};

#endif	// #ifndef EFFECT_RESOURCE_H.