#ifndef CWIPE_EFFECT_UI_H
#define CWIPE_EFFECT_UI_H

#include "..\..\Main\Global.h"

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
class CWipeEffectUI
{
public:
	CWipeEffectUI();	//ｺﾝｽﾄﾗｸﾀ.
	CWipeEffectUI( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );	//ｺﾝｽﾄﾗｸﾀ.

	~CWipeEffectUI();	//ﾃﾞｽﾄﾗｸﾀ.

	//初期化.
	HRESULT Init( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );

	//解放.
	void Release();

	//ﾚﾝﾀﾞﾘﾝｸﾞ用.
	void Render();

	// 円の中心になる相手の座標.
	void SetTargetPos( D3DXVECTOR2 tarpos ){ m_vTargetPos = tarpos; };

	// ワイプエフェクトの開始と終了.
	void SetStart(){ m_isStart = true; }
	void SetEnd(){ m_isEnd = true; }

	bool isActive(){ return m_isRender; }

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

	// 深度テストセット.
	void SetDepth( bool flag );

	void Update();
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
		ALIGN16 float		fRadius;		// 円の半径.
		ALIGN16 D3DXVECTOR2 vTargetPos;		// 標的の座標.
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

	ID3D11BlendState*		m_pAlphaBlend;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ有効.
	ID3D11BlendState*		m_pNoAlphaBlend;	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ無効.

	// 深度(z)テスト設定.
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;

	D3DXVECTOR3		m_vPos;		//位置.
	D3DXVECTOR2		m_vTargetPos;
	float			m_fRadius;

	bool			m_isStart;
	bool			m_isEnd;
	bool			m_isRender;
};


#endif	// #ifndef CWIPE_EFFECT_UI_H.