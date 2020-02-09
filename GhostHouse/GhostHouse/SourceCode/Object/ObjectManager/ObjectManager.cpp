#include "ObjectManager.h"
#include <algorithm>

CObjectManager::CObjectManager()
	: m_pPaper	( nullptr )
	, m_isInit	( false )
	, m_vInvisibleFloorCenterPosition	()
	, m_vMaxPositin	()
	, m_vMinPositin	()
{
}

CObjectManager::~CObjectManager()
{
}

void CObjectManager::Init( 
	ID3D11Device* pDevice11, 
	ID3D11DeviceContext* pContext11, 
	std::vector<std::shared_ptr<CObjectBase>>& vMap,
	std::unique_ptr<CMultiTexture>& pPaper )
{
	if( m_isInit == true ) return;
	m_Map = vMap;

	InvisibleCollisionPositionSearch();
	InitSignboard( pDevice11, pContext11, pPaper );
	InvisibleFloorCenterPositionSearch();
	Sort();
	vMap.clear();
	vMap = m_Map;
	m_isInit = true;
}

void CObjectManager::UIRender( std::shared_ptr<CObjectBase> pObj )
{
	if( m_pPaper != nullptr ){
		m_pPaper->Render( pObj->GetBitFlag(BitFlag::isSignBoardRender) );
	}
}

D3DXVECTOR3 CObjectManager::GetNearPosition( std::shared_ptr<CObjectBase> pObj ) const
{
	if( pObj == nullptr ) return D3DXVECTOR3();

	D3DXVECTOR2 CalcTmp1 = D3DXVECTOR2( m_vMaxPositin.x - pObj->GetPosition().x, ( m_vMinPositin.z + m_vMaxPositin.z ) / 2 - pObj->GetPosition().z );
	D3DXVECTOR2 CalcTmp2 = D3DXVECTOR2( m_vMinPositin.x - pObj->GetPosition().x, ( m_vMinPositin.z + m_vMaxPositin.z ) / 2 - pObj->GetPosition().z );
	float distance1 = D3DXVec2Length( &CalcTmp1 );
	float distance2 = D3DXVec2Length( &CalcTmp2 );

	if( distance1 > distance2 ){
		return D3DXVECTOR3( m_vMinPositin.x-10.0f, (( m_vMaxPositin.y+ m_vMinPositin.y )) / 2.0f+5.0f, ( m_vMinPositin.z+m_vMaxPositin.z ) / 2.0f );
	} else{
		return D3DXVECTOR3( m_vMaxPositin.x+10.0f, (( m_vMaxPositin.y+ m_vMinPositin.y )) / 2.0f+5.0f, ( m_vMinPositin.z+m_vMaxPositin.z ) / 2.0f );
	}
}

void CObjectManager::Sort()
{
	auto cmp = []( auto& a, auto& b )
	{
		return a->GetObjectNo() > b->GetObjectNo(); 
	};
	std::sort( m_Map.begin(), m_Map.end(), cmp );
}

void CObjectManager::InvisibleCollisionPositionSearch()
{
	m_vMaxPositin = D3DXVECTOR3( -1000.0f, -1000.0f, -1000.0f );
	m_vMinPositin = D3DXVECTOR3( 1000.0f, 1000.0f, 1000.0f );

	// 各座標の最大座標を記憶.
	for( auto m : m_Map ){
		if( m->GetObjectNo() == enObjectNo::InvisibleStartFloor ||
			m->GetObjectNo() == enObjectNo::InvisibleEndFloor ){
			if( m->GetPosition().x >= m_vMaxPositin.x ){
				m_vMaxPositin.x = m->GetPosition().x;
			}
			if( m->GetPosition().y >= m_vMaxPositin.y ){
				m_vMaxPositin.y = m->GetPosition().y;
			}
			if( m->GetPosition().z >= m_vMaxPositin.z ){
				m_vMaxPositin.z = m->GetPosition().z;
			}
		}
	}

	// 各座標の最小座標を記憶.
	for( auto m : m_Map ){
		if( m->GetObjectNo() == enObjectNo::InvisibleStartFloor ||
			m->GetObjectNo() == enObjectNo::InvisibleEndFloor ){
			if( m->GetPosition().x <= m_vMinPositin.x ){
				m_vMinPositin.x = m->GetPosition().x;
			}
			if( m->GetPosition().y <= m_vMinPositin.y ){
				m_vMinPositin.y = m->GetPosition().y;
			}
			if( m->GetPosition().z <= m_vMinPositin.z ){
				m_vMinPositin.z = m->GetPosition().z;
			}
		}
	}
}

bool CObjectManager::InitSignboard( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, std::unique_ptr<CMultiTexture>& pPaper )
{
	// 看板の検索.
	shared_ptr<CObjectBase>	pTmpSignboard;

	for( size_t i = 0; i < m_Map.size(); i++ ){
		if( m_Map[i]->GetObjectNo() == enObjectNo::Signboard ){
			pTmpSignboard = m_Map[i];
			break;
		}
	}

	// 看板が無ければ終了.
	if( pTmpSignboard == nullptr ) return false;

	vector<CMultiTexture::SPRITE_STATE> ss;

	const int PAPER_SIZE = 850;
	float PaperPos_x = WND_W / 2 - PAPER_SIZE / 2;
	// 紙(台紙?)の作成.
	ss.emplace_back();
	ss[0].fFloorUV	= 0.0f;
	ss[0].Base.w	= PAPER_SIZE;
	ss[0].Base.h	= PAPER_SIZE;
	ss[0].Disp.w	= PAPER_SIZE;
	ss[0].Disp.h	= PAPER_SIZE;
	ss[0].FrameNum	= 1;
	ss[0].Stride.w	= PAPER_SIZE;
	ss[0].Stride.h	= PAPER_SIZE;
	ss[0].vPos		= D3DXVECTOR3( PaperPos_x, -100.0f, 0.3f );

	auto RouteExcluded = []( std::shared_ptr<CObjectBase> m) -> bool
	{
		if( m->GetObjectNo() == enObjectNo::InvisibleFloor		|| m->GetObjectNo() == enObjectNo::SlipThroughFloor ||
			m->GetObjectNo() == enObjectNo::InvisibleStartFloor || m->GetObjectNo() == enObjectNo::InvisibleEndFloor ){
			return true;
		}
		return false;
	};

	// 看板のルート作成.
	for( auto m : m_Map ){
		if( RouteExcluded(m) == true ){
			InitSignboardSpraite( m, ss );
		}
	}

	InitSignboardRoutePosition( ss );
	m_pPaper = make_unique<CMultiTexture>( pDevice11, pContext11, "", ss );

	pTmpSignboard = nullptr;
	return true;
}

void CObjectManager::InitSignboardSpraite( shared_ptr<CObjectBase> pObj, vector<CMultiTexture::SPRITE_STATE>& ss )
{
	// 乗れる床.
	if( pObj->GetObjectNo() == enObjectNo::InvisibleFloor ){
		ss.emplace_back();
		ss[ss.size() - 1].fFloorUV = 0.25f;
	// 乗れない床.
	} else if( pObj->GetObjectNo() == enObjectNo::SlipThroughFloor ){
		ss.emplace_back();
		ss[ss.size() - 1].fFloorUV = 0.5f;
	} else if( pObj->GetObjectNo() == enObjectNo::InvisibleStartFloor ){
		ss.emplace_back();
		ss[ss.size() - 1].fFloorUV = 0.75f;
	} else if( pObj->GetObjectNo() == enObjectNo::InvisibleEndFloor ){
		ss.emplace_back();
		ss[ss.size() - 1].fFloorUV = 0.0f;
	} else{
		return;
	}
	const int BASE_W = 88 * 4;
	const int SIZE_W = 22 * 4;
	const int SIZE_H = 22 * 4;
	// ルートの作成.
	ss[ss.size() - 1].Base.w = BASE_W;
	ss[ss.size() - 1].Base.h = SIZE_H;
	ss[ss.size() - 1].Disp.w = SIZE_W;
	ss[ss.size() - 1].Disp.h = SIZE_H;
	ss[ss.size() - 1].FrameNum = 1;
	ss[ss.size() - 1].Stride.w = SIZE_W;
	ss[ss.size() - 1].Stride.h = SIZE_H;
	float x, y;
	x = pObj->GetPosition().x;
	y = pObj->GetPosition().z;
	ss[ss.size() - 1].vPos = D3DXVECTOR3( fabsf( x ), ( y ), 0.3f );
}

void CObjectManager::InitSignboardRoutePosition( vector<CMultiTexture::SPRITE_STATE>& ss )
{
	float max_Posx = ss[1].vPos.x, max_Posy = ss[1].vPos.y;
	float min_Posx = ss[1].vPos.x, min_Posy = ss[1].vPos.y;

	// 見えない床の最大座標と最小座標を検索.
	for( int i = 1; i < static_cast<int>( ss.size() ); i++ ){
		if( ss[i].vPos.x > max_Posx ){
			max_Posx = ss[i].vPos.x;
		} else if( ss[i].vPos.x < min_Posx ){
			min_Posx = ss[i].vPos.x;
		}
		if( ss[i].vPos.y > max_Posy ){
			max_Posy = ss[i].vPos.y;
		} else if( ss[i].vPos.y < min_Posy ){
			min_Posy = ss[i].vPos.y;
		}
	}
	// 縦横の個数を算出.
	float size_x = ( static_cast<float>( max_Posx / 2.0f - min_Posx / 2.0f ) + 1.0f );
	float size_y = ( static_cast<float>( max_Posy / 2.0f - min_Posy / 2.0f ) + 1.0f );

	// 座標を並び替える.
	for( int i = 1; i < static_cast<int>( ss.size() ); i++ ){
		float Pos_x = ( ( max_Posx / 2.0f ) - ( ss[i].vPos.x / 2.0f ) );
		float Pos_y = ( ( max_Posy / 2.0f ) - ( ss[i].vPos.y / 2.0f ) );
		CMultiTexture::SPRITE_STATE ssTmp;
		ssTmp = ss[i];
		ss[i] = ss[static_cast<int>( Pos_y * size_y + Pos_x ) + 1];
		ss[static_cast<int>( Pos_y * size_y + Pos_x ) + 1] = ssTmp;
	}

	// 看板に合うように、座標を調整.
	bool signboardSkip = false;
	for( auto& s : ss ){
		if( signboardSkip == false ){
			signboardSkip = true;
			continue;
		}
		float Pos_x = ( ( max_Posx / 2.0f ) - ( s.vPos.x / 2.0f ) );
		float Pos_y = ( ( max_Posy / 2.0f ) - ( s.vPos.y / 2.0f ) );
		s.vPos.x = ( 300 + Pos_x * s.Base.w / 2 ) - ( Pos_x * 75 );
		s.vPos.y = ( 450 - Pos_y * s.Base.h / 2 ) - ( Pos_y * 30 );
	}
}

void CObjectManager::InvisibleFloorCenterPositionSearch()
{
	float Max_x = -1000.0f, Max_y = -1000.0f, Max_z = -1000.0f;
	float Min_x = 1000.0f, Min_y = 1000.0f, Min_z = 1000.0f;

	// 各座標の最大座標を記憶.
	for( auto m : m_Map ){
		if( m->GetObjectNo() == enObjectNo::InvisibleFloor ||
			m->GetObjectNo() == enObjectNo::SlipThroughFloor ){
			if( m->GetPosition().x >= Max_x ){
				Max_x = m->GetPosition().x;
			}
			if( m->GetPosition().y >= Max_y ){
				Max_y = m->GetPosition().y;
			}
			if( m->GetPosition().z >= Max_z ){
				Max_z = m->GetPosition().z;
			}
		}
	}

	// 各座標の最小座標を記憶.
	for( auto m : m_Map ){
		if( m->GetObjectNo() == enObjectNo::InvisibleFloor ||
			m->GetObjectNo() == enObjectNo::SlipThroughFloor ){
			if( m->GetPosition().x <= Min_x ){
				Min_x = m->GetPosition().x;
			}
			if( m->GetPosition().y <= Min_y ){
				Min_y = m->GetPosition().y;
			}
			if( m->GetPosition().z <= Min_z ){
				Min_z = m->GetPosition().z;
			}
		}
	}
	m_vInvisibleFloorCenterPosition = D3DXVECTOR3( ( ( Max_x + Min_x ) ) / 2, ( ( Max_y + Min_y ) ) / 2, ( ( Max_z + Min_z ) ) / 2 );
}