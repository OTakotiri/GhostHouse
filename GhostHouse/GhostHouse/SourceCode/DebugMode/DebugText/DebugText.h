#ifndef TEXT_SPRITE_H
#define TEXT_SPRITE_H

//警告についてのｺｰﾄﾞ分析を無効にする.4005:再定義.
#pragma warning(disable:4005)
#include <Windows.h>

#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>	//「D3DX～」の定義使用時に必要.
#include <D3D10.h>

#include "..\..\Main\Global.h"
#include "..\..\SingletonBase\SingletonBase.h"

//ﾗｲﾌﾞﾗﾘ読み込み.
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx10.lib")	//「D3DX～」の定義使用時に必要.


// デバッグテキストクラス.
class CDebugText : CSingletonBase<CDebugText>
{
public:
	const float DIMENSION		= 12.0f;
	const float WDIMENSION		= 10.0f;
	const float TEX_DIMENSION	= 128.0f;

	//======================================
	//	構造体.
	//======================================
	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義.
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct SHADER_CONSTANT_BUFFER
	{
		D3DXMATRIX	mWVP;		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成変換行列.
		D3DXVECTOR4 vColor;		// カラー(RGBAの型に合わせる).
		D3DXVECTOR4 fAlpha;		// アルファ値(x のみ使用).
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
		D3DXVECTOR2	Tex;	//ﾃｸｽﾁｬ座標.
	};

	CDebugText();	// コンストラクタ
	~CDebugText();	// デストラクタ.

					// 初期化関数.
	static HRESULT Init( 
		ID3D11DeviceContext* pContext11,
		float Scale, 
		D3DXVECTOR4 vColor );
	static void SetPosition( const D3DXVECTOR3& vPos ){ GetInstance()->m_vPos = vPos; }

	// レンダリング関数.
	static void Render( const string& stext, const float& fvalue );
	static void Render( const string& stext );
	
	static float GetScale(){ return GetInstance()->m_fScale*10.0f; };
private:
	//ｼｪｰﾀﾞ作成.
	HRESULT InitShader();

	// ブレンドステート作成.
	HRESULT InitBlend();
	// アルファブレンド切替設定.
	void SetBlend( bool EnableAlpha );

	// フォントレンダリング関数.
	void RenderFont( int FontIndex, float x, float y );

	ID3D11Device*			m_pDevice11;	//ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ.
	ID3D11DeviceContext*	m_pContext11;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

	ID3D11VertexShader*		m_pVertexShader;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*		m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*			m_pConstantBuffer;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.

	ID3D11Buffer*			m_pVertexBuffer[100];	//頂点ﾊﾞｯﾌｧ.

	ID3D11SamplerState*			m_pSampleLinear;	//ｻﾝﾌﾟﾗ:ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.
	ID3D11ShaderResourceView*	m_pAsciiTexture;	//ﾃｸｽﾁｬ.


	ID3D11BlendState *m_pAlphaBlend;	// アルファブレンド有効.
	ID3D11BlendState *m_pNoAlphaBlend;	// アルファブレンド無効.

	DWORD	m_dwWindowWidth;	// ウィンドウ幅.	
	DWORD	m_dwWindowHeight;	// ウィンドウ高さ.

	float		m_fKerning[100];	// カーニング.
	float		m_fAlpha;			// 透過値.
	float		m_fScale;			// 拡縮値(25pixelを基準25pixel = 1.0f).
	D3DXVECTOR4	m_vColor;			// 色.

	D3DXVECTOR3 m_vPos;	// 位置.
	D3DXMATRIX	m_mView;// ビュー行列.
	D3DXMATRIX	m_mProj;// プロジェクション行列.

};

#endif // #ifndef CDEBUG_TEXT_H.