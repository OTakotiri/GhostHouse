#include "SpriteResouce.h"

#include "Sprite/CSpriteUI.h"

void CSpriteResource::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	auto spriteLoad = [&]( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
	{
		GetInstance()->SpriteLoad( pDevice11, pContext11 );
		GetInstance()->m_isLoadEnd = true;
	};
	GetInstance()->m_thread = std::thread( spriteLoad, pDevice11, pContext11 );
}

void CSpriteResource::SpriteLoad( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	vector<string> Spritepath;		// ファイルパス.
	vector<string> SpriteName;		// モデル名.
	vector<string> SpriteTextpath;	// スプライト情報.

	TextDataLoad( Spritepath, SpriteName, SpriteTextpath );

	// make_shared 動的メモリ確保.
	// CDX9Mesh の引数付きコンストラクタでメッシュの読み込み.
	for( int i = 0; i < static_cast<int>( Spritepath.size() ); i++ ){
		CSpriteUI::SPRITE_STATE ss;
		Init( &ss, SpriteTextpath[i] );
		m_pSpriteList[SpriteName[i]]
			= make_shared<CSpriteUI>( pDevice11, pContext11, Spritepath[i].c_str(), ss );
	}
}

shared_ptr<CSpriteUI> CSpriteResource::GetSprite( const string& SpriteName )
{
	size_t iErrer = 1;	// リストにない場合のエラー用の数字.
	if( iErrer > GetInstance()->m_pSpriteList.count( SpriteName ) ){
		return nullptr;	// リストにない場合 nullptrを返す.
	}
	return GetInstance()->m_pSpriteList[SpriteName];
}

void CSpriteResource::TextDataLoad( vector<string> &Spritepath, vector<string> &SpriteName, vector<string> &SpriteTextpath )
{
	// ファイルを開く.
	fstream filestream( SPRITE_LIST_TEXT_PAHT );
	if( !filestream.is_open() ) return;

	// ファイルを読み込む.
	while( !filestream.eof() ){

		string buffer;
		string buffer2;
		filestream >> buffer;
		Spritepath.emplace_back( buffer );					// モデルのファイルパスを入手.
		SpriteTextpath.emplace_back( GetInstance()->TextPathSearch( buffer ));
		SpriteName.emplace_back( GetInstance()->NameSearch( buffer ) );	// モデルの名前を入手.
	}
	// 余分な要素を捨てる.
	Spritepath.pop_back();
	SpriteTextpath.pop_back();
	SpriteName.pop_back();
}

string CSpriteResource::TextPathSearch( string buf )
{
	string path;
	for( int i = 0; i < static_cast<int>( buf.size() ); i++ ){
		path += buf[i];
		if( buf[i] == '.' ){
			path += "txt";
			break;
		}
	}
	return path;
}

string CSpriteResource::NameSearch( string buf )
{
	for( int i = 0; i < static_cast<int>(buf.size()); i++ ){
		if( buf[i] == '.' ){
			int en;
			for( int j = 0; buf[i-j] != '\\'; j++ ){
				en = i - j;
			}
			buf.erase( 0, en );
			buf.erase( i-en, buf.size() );
			break;
		}
	}
	return buf;
}

void CSpriteResource::Init( CSpriteUI::SPRITE_STATE* pss, string buf )
{
	// ファイルを開く.
	fstream filestream( buf );
	if( !filestream.is_open() ) return;

	int no = 0;
	int st = 0;

	// ファイルを読み込む.
	while( !filestream.eof() ){
		string buffer;
		filestream >> buffer;
		switch( no ){
		case 0:
			if( buffer[0] != 's' ){
				if( st == 0){
					pss->Base.w = static_cast<float>(stof( buffer ));
				} else if ( st == 1 ){
					pss->Base.h = static_cast<float>( stof( buffer ) );
				}
				st++;
				if( st == 2 ){
					st = 0;
					no = 1;
				}
			}
			break;
		case 1:
			if( buffer[0] != 's' ){
				if( st == 0 ){
					pss->Stride.w = static_cast<float>( stof( buffer ) );
				} else if( st == 1 ){
					pss->Stride.h = static_cast<float>( stof( buffer ) );
				}
				st++;
				if( st == 2 ){
					st = 0;
					no = 2;
				}
			}
			break;
		case 2:
			if( buffer[0] != 'd' ){
				if( st == 0 ){
					pss->Disp.w = static_cast<float>( stof( buffer ) );
				} else if( st == 1 ){
					pss->Disp.h = static_cast<float>( stof( buffer ) );
				}
				st++;
				if( st == 2 ){
					st = 0;
					no = 3;
				}
			}
			break;
		case 3:
			if( buffer[0] != 'p' ){
				if( st == 0 ){
					pss->vPos.x = static_cast<float>( stof( buffer ) );
				} else if( st == 1 ){
					pss->vPos.y = static_cast<float>( stof( buffer ) );
				} else if( st == 2 ){
					pss->vPos.z = static_cast<float>( stof( buffer ) );
				}
				st++;
				if( st == 3 ){
					st = 0;
					no = 4;
				}
			}
			break;
		case 4:
			if( buffer[0] != 'f' ){
				pss->FrameNum = stoi( buffer );
				st++;
				if( st == 1 ){
					st = 0;
					no++;
				}
			}
			break;
		default:
			break;
		}
	}
}

bool CSpriteResource::ThreadRelease()
{
	if( GetInstance()->m_isThreadJoined == true ) return true;
	if( GetInstance()->m_isLoadEnd == false ) return false;
	GetInstance()->m_thread.join();
	GetInstance()->m_isThreadJoined = true;
	return true;
}