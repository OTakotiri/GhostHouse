#ifndef SPRITE_RESOURCE_H
#define SPRITE_RESOURCE_H

#include "..\..\SingletonBase\SingletonBase.h"
#include "Sprite/CSpriteUI.h"

#include <thread>

class CSpriteResource : public CSingletonBase<CSpriteResource>
{
	const char* SPRITE_LIST_TEXT_PAHT	= "Data\\PathText\\SpriteList.csv";	// モデルリストテキストのファイルパス.

public:
	CSpriteResource()
		: m_isLoadEnd		( false )
		, m_isThreadJoined	( false )
	{}
	virtual ~CSpriteResource(){}

	static void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	static shared_ptr<CSpriteUI> GetSprite( const string& modelName );
	static bool ThreadRelease();

private:
	void SpriteLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	void TextDataLoad( vector<string> &Modelpath, vector<string> &ModelName, vector<string> &SpriteTextoath );
	string TextPathSearch( string buf );
	string NameSearch( string buf );
	void Init( CSpriteUI::SPRITE_STATE* pss, string buf );

private:
	map<string, shared_ptr<CSpriteUI>>	m_pSpriteList;			// スプライトクラス.

	std::thread		m_thread;	// スレッド.

	bool	m_isLoadEnd;		// ロードが終了したか.
	bool	m_isThreadJoined;	// スレッドが解放されたか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CSpriteResource( const CSpriteResource & ) = delete;
	CSpriteResource& operator = ( const CSpriteResource & ) = delete;
	CSpriteResource( CSpriteResource && ) = delete;
	CSpriteResource& operator = ( CSpriteResource && ) = delete;
};

#endif	// #ifndef SPRITE_RESOURCE_H.