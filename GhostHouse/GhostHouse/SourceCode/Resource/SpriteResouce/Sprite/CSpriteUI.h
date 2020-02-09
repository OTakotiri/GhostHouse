#ifndef CSPRITE_UI_H
#define CSPRITE_UI_H

#include "..\..\..\Main\Global.h"

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>


//ﾗｲﾌﾞﾗﾘ読み込み.
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx10.lib")	//「D3DX～」の定義使用時に必要.




/**************************************************
*	ｽﾌﾟﾗｲﾄUIｸﾗｽ.
**/
class CSpriteUI
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
		int				FrameNum;	// フレーム数.
	};

	CSpriteUI();	//ｺﾝｽﾄﾗｸﾀ.
	CSpriteUI( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		const char* fileName, const SPRITE_STATE& pSs );	//ｺﾝｽﾄﾗｸﾀ.
	
	~CSpriteUI();	//ﾃﾞｽﾄﾗｸﾀ.

	//初期化.
	HRESULT Init( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, 
		const char* fileName, const SPRITE_STATE& pSs );

	//解放.
	void Release();

	//ﾚﾝﾀﾞﾘﾝｸﾞ用.
	void Render();

	//UV位置変更可能レンダリング用.
	void Render(float UV_x, float UV_y);

	// 深度テストセット.
	void SetDepth( bool flag );


	//位置情報を設定.
	void SetPosition( float x, float y, float z )
	{
		m_SState.vPos.x = x;
		m_SState.vPos.y = y;
		m_SState.vPos.z = z;
	}
	void SetPosition( const D3DXVECTOR3& vPos )
	{
		m_SState.vPos = vPos;
	}
	D3DXVECTOR3 GetPosition(){ return m_SState.vPos; }

	void SetPattern( LONG x, LONG y )
	{
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	void SetAlpha( float fAlpha )
	{ 
		m_Alpha = fAlpha; 
	}
	float GetAlpha() const { return m_Alpha; }

	void SetScale( float fScale ){ m_fScale = fScale; }

	float GetSpriteWidth(){ return m_SState.Base.w; }

private:
	//ｼｪｰﾀﾞ作成.
	HRESULT InitShader();
	//ﾓﾃﾞﾙ作成.
	HRESULT InitModel();
	// テクスチャ作成.
	HRESULT CreateTexture( const char* fileName,
		ID3D11ShaderResourceView** pTexture );

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

	//↓ｱﾌﾟﾘにひとつ.
	ID3D11Device*			m_pDevice11;	//ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ.
	ID3D11DeviceContext*	m_pContext11;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

	//↓ﾓﾃﾞﾙの種類ごとに用意.
	ID3D11VertexShader*		m_pVertexShader;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*		m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*			m_pConstantBuffer;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.

	//↓ﾓﾃﾞﾙごとに用意.
	ID3D11Buffer*			m_pVertexBuffer;	//頂点ﾊﾞｯﾌｧ.

	ID3D11SamplerState*		m_pSampleLinear;	//ｻﾝﾌﾟﾗ:ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.
	ID3D11ShaderResourceView*	m_pTexture;		//ﾃｸｽﾁｬ.

	int			m_AnimCount;	//ｶｳﾝﾀ.
	D3DXVECTOR2	m_UV;		//ﾃｸｽﾁｬUV座標.

	ID3D11BlendState*		m_pAlphaBlend;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ有効.
	ID3D11BlendState*		m_pNoAlphaBlend;	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ無効.

	// 深度(z)テスト設定.
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;


	D3DXVECTOR3		m_vPos;		//位置.
	float			m_fScale;	// スケール.
	float			m_Alpha;	// アルファ値.

	SPRITE_STATE	m_SState;		// スプライト情報.
	POINT			m_PatternNo;	// パターン番号.
};


#endif	// #ifndef CSPRITE_UI_H.