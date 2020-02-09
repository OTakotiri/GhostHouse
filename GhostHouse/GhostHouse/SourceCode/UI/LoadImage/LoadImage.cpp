#include "LoadImage.h"

CLoadImage::CLoadImage()
	: m_pLoadImage	( nullptr )
	, m_Count		( 0 )

{
}

CLoadImage::~CLoadImage()
{
}

void CLoadImage::Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
	CSpriteUI::SPRITE_STATE ss;
	GetInstance()->m_pLoadImage = make_unique<CSpriteUI>();
	ss.Base.w = 300.0f;
	ss.Base.h = 800.0f;
	ss.Disp.w = 300.0f;
	ss.Disp.h = 100.0f;
	ss.Stride.w = 300.0f;
	ss.Stride.h = 100.0f;
	ss.vPos = D3DXVECTOR3( 1100.0f, 650.0f, 0.0f );
	GetInstance()->m_pLoadImage->Init( pDevice11, pContext11, "Data\\Texture\\Load\\Load.png", ss );
}

void CLoadImage::Render()
{
	GetInstance()->m_Count++;
	if( GetInstance()->m_Count >= 22 ) GetInstance()->m_Count = 0;
	GetInstance()->m_pLoadImage->Render( 0.0f, static_cast<float>( GetInstance()->m_Count / 3 ) );
}