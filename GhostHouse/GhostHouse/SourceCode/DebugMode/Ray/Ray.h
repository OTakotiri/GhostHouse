#ifndef CRAY_H
#define CRAY_H

#include "..\..\Main\Global.h"

// レイ表示クラス.
class CRay
{
public:
	// レイ構造体.
	struct RAY
	{
		D3DXVECTOR3 Point[2];	// 始点, 終点.
		D3DXVECTOR3 Direction;	// 方向.
		D3DXVECTOR3 Pos;		// 位置.
		FLOAT		fYaw;		// Y軸回転値.
		RAY()
			: Point		()
			, Direction	()
			, Pos		()
			, fYaw		( 0.0F )
		{}
	};

	CRay();
	~CRay();

	// 初期化関数.
	HRESULT Init( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );

	//ｼｪｰﾀﾞ作成.
	HRESULT InitShader();

	// レイ初期化.
	HRESULT InitModel();

	// 描画.
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj );


	// レイ構造体.
	RAY		m_Ray;

private:
	//======================================
	//	構造体.
	//======================================
	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義.
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
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

	//↓ﾓﾃﾞﾙごとに用意.
	ID3D11Buffer*			m_pVertexBuffer;	//頂点ﾊﾞｯﾌｧ.
};

#endif // #ifndef CRAY_H.