#ifndef LOAD_IMAGE_H
#define LOAD_IMAGE_H

#include "..\..\Resource\SpriteResouce\Sprite\CSpriteUI.h"

class CLoadImage
{
public:
	CLoadImage();
	~CLoadImage();

	static CLoadImage* GetInstance()
	{
		static unique_ptr<CLoadImage> pInstance = make_unique<CLoadImage>();
		return pInstance.get();
	}

	static void Load( ID3D11Device* pDevice11,ID3D11DeviceContext* pContext11 );

	static void Render();

private:
	unique_ptr<CSpriteUI> m_pLoadImage;
	int m_Count;
};

#endif	// #ifndef LOAD_IMAGE_H.