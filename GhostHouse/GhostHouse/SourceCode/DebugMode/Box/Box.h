#ifndef BOX_H
#define BOX_H

#include "..\..\Main\Global.h"

class CBox
{
public:
	CBox();
	~CBox();

	// 初期化関数.
	HRESULT Init( 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11, 
		const D3DXVECTOR3& vMaxPoint,
		const D3DXVECTOR3& vMinPoint );
	// 描画関数.
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj );

	// 座標セット関数.
	void SetPosition( const D3DXVECTOR3& vPos ){ m_vPosition = vPos; }
	// Y軸回転セット関数.
	void SetRotationYaw( const float& fYaw ){ m_fYaw = fYaw; }

private:
	// シェーダー作成.
	HRESULT InitShader();
	// キューブ,ボックスの作成.
	HRESULT InitCube( const D3DXVECTOR3& vMaxPoint, const D3DXVECTOR3& vMinPoint );
	// ブレンド作成.
	HRESULT InitBlend();

	// ブレンドの有効無効.
	void SetBlend( bool EnableAlpha );

	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義.
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct C_BUFFER
	{
		D3DXMATRIX	mWVP;		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成変換行列.	
		D3DXVECTOR4	vColor;		//ｶﾗｰ(RGBAの型に合わせる).
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
	};

	ID3D11Device*			m_pDevice11;
	ID3D11DeviceContext*	m_pContext11;

	//↓ﾓﾃﾞﾙの種類ごとに用意.
	ID3D11VertexShader*		m_pVertexShader;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*		m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*			m_pConstantBuffer;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.

	ID3D11BlendState*		m_pAlphaBlend;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ有効.
	ID3D11BlendState*		m_pNoAlphaBlend;	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ無効.

	ID3D11Buffer*			g_pIndexBuffer;
												//↓ﾓﾃﾞﾙごとに用意.
	ID3D11Buffer*			m_pVertexBuffer;	//頂点ﾊﾞｯﾌｧ.

	D3DXVECTOR3		m_vPosition;
	float			m_fYaw;

};

#endif	// #ifndef BOX_H.