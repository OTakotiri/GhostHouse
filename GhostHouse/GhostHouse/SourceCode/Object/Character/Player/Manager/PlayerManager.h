#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <memory>
#include "..\..\ObjectBase\ObjectBase.h"

class CObjectBase;

// Ç≥Ç‚Ç©ÅAÇ∆ÇÌÇä«óùÇ∑ÇÈ.
class CPlayerManager
{
	const int	MAX_LIFE_POINT = 3;
	const float LIFE_IMAGE_RENDER_POS_X = 60.0f;
	const float LIFE_IMAGE_RENDER_POS_Y = 60.0f;
public:
	CPlayerManager();
	~CPlayerManager();

	void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	void Update();
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera );
	void Collsiton( std::shared_ptr<CObjectBase> pObj );
	void UIRender();

	void OneGroundHit( vector<shared_ptr<CObjectBase>> pMap );
	void ChangeSceneInit( vector<shared_ptr<CObjectBase>>& pMap );

	void ResetPlayer()
	{
		m_pSayaka = nullptr;
		m_pTowa = nullptr;
	}

	void SetGamePause( const bool& isPause )
	{
		if( m_pSayaka == nullptr ) return;
		m_pSayaka->SetGamePause( isPause );

		if( m_pTowa == nullptr ) return;
		m_pTowa->SetGamePause( isPause );
	}

	void SetSayaka( std::shared_ptr<CObjectBase> pSayaka ){ m_pSayaka = pSayaka; }
	void SetTowa( std::shared_ptr<CObjectBase> pTowa ){ m_pTowa = pTowa; }
	std::shared_ptr<CObjectBase> GetSayaka(){ return m_pSayaka; }
	std::shared_ptr<CObjectBase> GetTowa(){ return m_pTowa; }

	void SetStageNumber( const int& number ){ m_StageNumber = number;  }

private:
	void UILoad();

private:
	std::shared_ptr<CObjectBase> m_pSayaka;
	std::shared_ptr<CObjectBase>m_pTowa;
	shared_ptr<CSpriteUI> m_LifeUI;
	shared_ptr<CSpriteUI> m_LifeNoneUI;
	shared_ptr<CSpriteUI> m_SayakaMoveUI;
	shared_ptr<CSpriteUI> m_TowaMoveUI;

	bool m_isSetOnGround;
	bool m_isLifePointRender;
	float m_LifeImageScale;
	int m_StageNumber;
	bool m_isInitTowaState;
};

#endif	// #ifndef PLAYER_MANAGER_H.