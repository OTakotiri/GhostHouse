#ifndef MULTI_TEXTURE_H
#define MULTI_TEXTURE_H

#include "..\..\..\..\Main\Global.h"

class CMultiTexture
{
public:
	// 幅高さ構造体(float型).
	struct WHDIZE_FLOAT
	{
		float w;
		float h;
	};
	// スプライト構造体.
	struct SPRITE_STATE
	{
		WHDIZE_FLOAT	Disp;		// 表示幅,高さ.
		WHDIZE_FLOAT	Base;		// 元画像の幅,高さ.
		WHDIZE_FLOAT	Stride;		// 1コマ当たりの幅,高さ.
		D3DXVECTOR3		vPos;		// 座標.
		float			fFloorUV;
		int				FrameNum;	// フレーム数.
	};

	CMultiTexture( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		const char* fileName, const vector<SPRITE_STATE> pSs );	//ｺﾝｽﾄﾗｸﾀ.
	~CMultiTexture();	//ﾃﾞｽﾄﾗｸﾀ.

	//ﾚﾝﾀﾞﾘﾝｸﾞ用.
	void Render( const bool isRender );

private:
	CMultiTexture();	//ｺﾝｽﾄﾗｸﾀ.

	//初期化.
	HRESULT Init( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, 
		const char* fileName, const vector<SPRITE_STATE> pSs );

	//解放.
	void Release();

	//ｼｪｰﾀﾞ作成.
	HRESULT InitShader();
	//ﾓﾃﾞﾙ作成.
	HRESULT InitModel();
	// テクスチャ作成.
	HRESULT CreateTexture();

	HRESULT InitBlend();
	void SetBlend( bool EnableAlpha );
	//======================================
	//	構造体.
	//======================================
	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義.
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
	{
		ALIGN16 D3DMATRIX	mW;				// ワールド行列.
		ALIGN16 float		fViewPortWidth;	// ビューポート幅.
		ALIGN16 float		fViewPortHeight;// ビューポート高さ.
		ALIGN16 float		fAlpha;			// アルファ値(透過で使用する).
		ALIGN16 D3DXVECTOR2 vUV;			// UV座標.
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
		D3DXVECTOR2	Tex;	//ﾃｸｽﾁｬ座標.
	};
	// 深度(Z)テストON/OFF切替え.
	void SetDeprh( bool flag );

	// 深度(z)テスト設定.
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;

	//↓ｱﾌﾟﾘにひとつ.
	ID3D11Device*			m_pDevice11;	//ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ.
	ID3D11DeviceContext*	m_pContext11;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

	//↓ﾓﾃﾞﾙの種類ごとに用意.
	ID3D11VertexShader*		m_pVertexShader1;	//頂点ｼｪｰﾀﾞ.
	ID3D11VertexShader*		m_pVertexShader2;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*		m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*			m_pConstantBuffer;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.

												//↓ﾓﾃﾞﾙごとに用意.
	ID3D11Buffer*			m_pVertexBuffer1;	//頂点ﾊﾞｯﾌｧ.
	ID3D11Buffer*			m_pVertexBuffer2;	//頂点ﾊﾞｯﾌｧ.

	ID3D11SamplerState*					m_pSampleLinear;	//ｻﾝﾌﾟﾗ:ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.
	vector<ID3D11ShaderResourceView*>	m_pTextures;

	D3DXMATRIX m_mWorld;

	ID3D11BlendState*		m_pAlphaBlend;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ有効.
	ID3D11BlendState*		m_pNoAlphaBlend;	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ無効.


	D3DXVECTOR3		m_vPos;		//位置.
	float			m_Alpha;	// アルファ値.

	vector<SPRITE_STATE>	m_SState;		// スプライト情報.
};

#endif	// #ifndef MULTI_TEXTURE_H.