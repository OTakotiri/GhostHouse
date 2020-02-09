#include "DebugText.h"
#include <crtdbg.h>

//ｼｪｰﾀﾞﾌｧｲﾙ名.
const char SHADER_NAME[] = "Data\\Shader\\DebugText.hlsl";

//ｺﾝｽﾄﾗｸﾀ.
CDebugText::CDebugText()
	: m_pDevice11( nullptr )
	, m_pContext11( nullptr )
	, m_pVertexShader( nullptr )
	, m_pVertexLayout( nullptr )
	, m_pPixelShader( nullptr )
	, m_pConstantBuffer( nullptr )
	, m_pVertexBuffer()
	, m_pSampleLinear( nullptr )
	, m_pAsciiTexture( nullptr )
	, m_pAlphaBlend( nullptr )
	, m_pNoAlphaBlend( nullptr )
	, m_fKerning()
	, m_fAlpha( 0.0f )
	, m_fScale( 1.0f )
	, m_vColor( 0.0f, 0.0f, 0.0f, 0.0f )

	, m_vPos()
{
}

//ﾃﾞｽﾄﾗｸﾀ.
CDebugText::~CDebugText()
{
}

//初期化関数.
HRESULT CDebugText::Init(
	ID3D11DeviceContext* pContext11,
	float Scale, D3DXVECTOR4 vColor )
{
	GetInstance()->m_fAlpha = vColor.w;
	GetInstance()->m_vColor = vColor;
	GetInstance()->m_fScale = Scale / 25.0f;

	//95文字分繰り返し.
	for( int i = 0; i < 95; i++ ){
		GetInstance()->m_fKerning[i] = 10.0f;
	}
	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄをｺﾋﾟｰ.
	GetInstance()->m_pContext11 = pContext11;
	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄからﾃﾞﾊﾞｲｽを取得.
	GetInstance()->m_pContext11->GetDevice( &GetInstance()->m_pDevice11 );

	//Windowsｻｲｽﾞ.
	GetInstance()->m_dwWindowHeight = WND_H;
	GetInstance()->m_dwWindowWidth = WND_W;

	//===ﾌｫﾝﾄごとにｸｱｯﾄﾞ(矩形)作成===.
	float left = 0.0f, top = 0.0f, right = 0.0f, bottom = 0.0f;
	int cnt = 0;
	//2重ﾙｰﾌﾟで1文字ずつ指定する.
	for( int k = 0; k < 10; k++ ){
		for( int i = 0; i < 10; i++ ){
			left = k * GetInstance()->WDIMENSION;
			top = i * GetInstance()->DIMENSION;
			right = left + GetInstance()->m_fKerning[cnt];
			bottom = top + GetInstance()->DIMENSION;

			left /= GetInstance()->TEX_DIMENSION;
			top /= GetInstance()->TEX_DIMENSION;
			right /= GetInstance()->TEX_DIMENSION;
			bottom /= GetInstance()->TEX_DIMENSION;

			VERTEX vertices[] =
			{
				D3DXVECTOR3( 0.0f,       GetInstance()->DIMENSION, 0.0f ),	D3DXVECTOR2( left,  bottom ),
				D3DXVECTOR3( 0.0f,       0.0f,      0.0f ),	D3DXVECTOR2( left,  top ),
				D3DXVECTOR3( GetInstance()->WDIMENSION, GetInstance()->DIMENSION, 0.0f ),	D3DXVECTOR2( right, bottom ),
				D3DXVECTOR3( GetInstance()->WDIMENSION, 0.0f,      0.0f ),	D3DXVECTOR2( right, top )
			};
			//最大要素数を算出する.
			UINT uVerMax = sizeof( vertices ) / sizeof( vertices[0] );

			//ﾊﾞｯﾌｧ構造体.
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DEFAULT;	//使用方法(ﾃﾞﾌｫﾙﾄ).
			bd.ByteWidth = sizeof( VERTEX ) * uVerMax;//頂点のｻｲｽﾞ.
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//頂点ﾊﾞｯﾌｧとして扱う.
			bd.CPUAccessFlags = 0;	//CPUからはｱｸｾｽしない.
			bd.MiscFlags = 0;		//その他のﾌﾗｸﾞ(未使用).
			bd.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).

										//ｻﾌﾞﾘｿｰｽﾃﾞｰﾀ構造体.
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;	//板ﾎﾟﾘの頂点をｾｯﾄ.

											//頂点ﾊﾞｯﾌｧの作成.
			if( FAILED( GetInstance()->m_pDevice11->CreateBuffer(
				&bd, &InitData, &GetInstance()->m_pVertexBuffer[cnt] ) ) ){
				_ASSERT_EXPR( false, L"頂点ﾊﾞｯﾌｧ作成失敗" );
				return E_FAIL;
			}
			cnt++;
		}
	}

	//ﾃｸｽﾁｬ用のｻﾝﾌﾟﾗ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;//POINT:高速だが粗い.
	samDesc.AddressU
		= D3D11_TEXTURE_ADDRESS_WRAP;//ﾗｯﾋﾟﾝｸﾞﾓｰﾄﾞ(WRAP:繰り返し).
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//MIRROR: 反転繰り返し.
	//CLAMP : 端の模様を引き伸ばす.
	//BORDER: 別途境界色を決める.
	//ｻﾝﾌﾟﾗ作成.
	if( FAILED( GetInstance()->m_pDevice11->CreateSamplerState(
		&samDesc, &GetInstance()->m_pSampleLinear ) ) )//(out)ｻﾝﾌﾟﾗ.
	{
		_ASSERT_EXPR( false, L"ｻﾝﾌﾟﾗ作成失敗" );
		return E_FAIL;
	}

	//ﾃｸｽﾁｬ作成.
	if( FAILED( D3DX11CreateShaderResourceViewFromFile(
		GetInstance()->m_pDevice11,	//ﾘｿｰｽを使用するﾃﾞﾊﾞｲｽのﾎﾟｲﾝﾀ.
		"Data\\DebugText\\ascii.png",	//ﾌｧｲﾙ名.
		nullptr, nullptr,
		&GetInstance()->m_pAsciiTexture,	//(out)ﾃｸｽﾁｬ.
		nullptr ) ) ){
		_ASSERT_EXPR( false, L"ﾃｸｽﾁｬ作成失敗" );
		return E_FAIL;
	}

	//ｼｪｰﾀﾞ作成.
	if( FAILED( GetInstance()->InitShader() ) ){
		return E_FAIL;
	}
	//ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ作成.
	if( FAILED( GetInstance()->InitBlend() ) ){
		return E_FAIL;
	}

	return S_OK;
}


//===========================================================
//	HLSLﾌｧｲﾙを読み込みｼｪｰﾀﾞを作成する.
//	HLSL: High Level Shading Language の略.
//===========================================================
HRESULT CDebugText::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//HLSLからﾊﾞｰﾃｯｸｽｼｪｰﾀﾞのﾌﾞﾛﾌﾞを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//ｼｪｰﾀﾞﾌｧｲﾙ名(HLSLﾌｧｲﾙ).
			nullptr,		//ﾏｸﾛ定義の配列へのﾎﾟｲﾝﾀ(未使用).
			nullptr,		//ｲﾝｸﾙｰﾄﾞﾌｧｲﾙを扱うｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			"VS_Main",		//ｼｪｰﾀﾞｴﾝﾄﾘｰﾎﾟｲﾝﾄ関数の名前.
			"vs_5_0",		//ｼｪｰﾀﾞのﾓﾃﾞﾙを指定する文字列(ﾌﾟﾛﾌｧｲﾙ).
			uCompileFlag,	//ｼｪｰﾀﾞｺﾝﾊﾟｲﾙﾌﾗｸﾞ.
			0,				//ｴﾌｪｸﾄｺﾝﾊﾟｲﾙﾌﾗｸﾞ(未使用).
			nullptr,		//ｽﾚｯﾄﾞ ﾎﾟﾝﾌﾟ ｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			&pCompiledShader,//ﾌﾞﾛﾌﾞを格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			&pErrors,		//ｴﾗｰと警告一覧を格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			nullptr ) ) )		//戻り値へのﾎﾟｲﾝﾀ(未使用).
	{
		_ASSERT_EXPR( false, L"hlsl読み込み失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	//上記で作成したﾌﾞﾛﾌﾞから「ﾊﾞｰﾃｯｸｽｼｪｰﾀﾞ」を作成.
	if( FAILED(
		m_pDevice11->CreateVertexShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pVertexShader ) ) )	//(out)ﾊﾞｰﾃｯｸｽｼｪｰﾀﾞ.
	{
		_ASSERT_EXPR( false, L"ﾊﾞｰﾃｯｸｽｼｪｰﾀﾞ作成失敗" );
		return E_FAIL;
	}

	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄを定義.
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{
			"POSITION",						//位置.
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,	//DXGIのﾌｫｰﾏｯﾄ(32bit float型*3).
			0,
			0,								//ﾃﾞｰﾀの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD",						//ﾃｸｽﾁｬ位置.
			0,
			DXGI_FORMAT_R32G32_FLOAT,		//DXGIのﾌｫｰﾏｯﾄ(32bit float型*2).
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,	//ﾃﾞｰﾀの開始位置.
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄの配列要素数を算出.
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄを作成.
	if( FAILED(
		m_pDevice11->CreateInputLayout(
			layout,
			numElements,
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			&m_pVertexLayout ) ) )	//(out)頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄ.
	{
		_ASSERT_EXPR( false, L"頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄ作成失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	//HLSLからﾋﾟｸｾﾙｼｪｰﾀﾞのﾌﾞﾛﾌﾞを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//ｼｪｰﾀﾞﾌｧｲﾙ名(HLSLﾌｧｲﾙ).
			nullptr,		//ﾏｸﾛ定義の配列へのﾎﾟｲﾝﾀ(未使用).
			nullptr,		//ｲﾝｸﾙｰﾄﾞﾌｧｲﾙを扱うｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			"PS_Main",		//ｼｪｰﾀﾞｴﾝﾄﾘｰﾎﾟｲﾝﾄ関数の名前.
			"ps_5_0",		//ｼｪｰﾀﾞのﾓﾃﾞﾙを指定する文字列(ﾌﾟﾛﾌｧｲﾙ).
			uCompileFlag,	//ｼｪｰﾀﾞｺﾝﾊﾟｲﾙﾌﾗｸﾞ.
			0,				//ｴﾌｪｸﾄｺﾝﾊﾟｲﾙﾌﾗｸﾞ(未使用).
			nullptr,		//ｽﾚｯﾄﾞ ﾎﾟﾝﾌﾟ ｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			&pCompiledShader,//ﾌﾞﾛﾌﾞを格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			&pErrors,		//ｴﾗｰと警告一覧を格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			nullptr ) ) )		//戻り値へのﾎﾟｲﾝﾀ(未使用).
	{
		_ASSERT_EXPR( false, L"hlsl読み込み失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	//上記で作成したﾌﾞﾛﾌﾞから「ﾋﾟｸｾﾙｼｪｰﾀﾞ」を作成.
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			nullptr,
			&m_pPixelShader ) ) )	//(out)ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	{
		_ASSERT_EXPR( false, L"ﾋﾟｸｾﾙｼｪｰﾀﾞ作成失敗" );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	//ｺﾝｽﾀﾝﾄ(定数)ﾊﾞｯﾌｧ作成.
	//ｼｪｰﾀﾞに特定の数値を送るﾊﾞｯﾌｧ.
	//ここでは変換行列渡し用.
	//ｼｪｰﾀﾞに World, View, Projection 行列を渡す.
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧを指定.
	cb.ByteWidth = sizeof( SHADER_CONSTANT_BUFFER );//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｻｲｽﾞ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//書き込みでｱｸｾｽ.
	cb.MiscFlags = 0;	//その他のﾌﾗｸﾞ(未使用).
	cb.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).
	cb.Usage = D3D11_USAGE_DYNAMIC;	//使用方法:直接書き込み.

									//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧの作成.
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&cb,
			nullptr,
			&m_pConstantBuffer ) ) ){
		_ASSERT_EXPR( false, L"ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDebugText::InitBlend()
{
	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ用ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ構造体.
	//pngﾌｧｲﾙ内にｱﾙﾌｧ情報があるので、透過するようにﾌﾞﾚﾝﾄﾞｽﾃｰﾄで設定する.
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory( &BlendDesc, sizeof( BlendDesc ) );	//初期化.

	BlendDesc.IndependentBlendEnable
		= false;	//false:RenderTarget[0]のﾒﾝﾊﾞｰのみ使用する.
					//true:RenderTarget[0～7]が使用できる
					// (ﾚﾝﾀﾞｰﾀｰｹﾞｯﾄ毎に独立したﾌﾞﾚﾝﾄﾞ処理).
	BlendDesc.AlphaToCoverageEnable
		= false;	//true:ｱﾙﾌｧﾄｩｶﾊﾞﾚｰｼﾞを使用する.
	BlendDesc.RenderTarget[0].BlendEnable
		= true;	//true:ｱﾙﾌｧﾌﾞﾚﾝﾄﾞを使用する.
	BlendDesc.RenderTarget[0].SrcBlend	//元素材に対する設定.
		= D3D11_BLEND_SRC_ALPHA;			//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞを指定.
	BlendDesc.RenderTarget[0].DestBlend	//重ねる素材に対する設定.
		= D3D11_BLEND_INV_SRC_ALPHA;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞの反転を指定.
	BlendDesc.RenderTarget[0].BlendOp	//ﾌﾞﾚﾝﾄﾞｵﾌﾟｼｮﾝ.
		= D3D11_BLEND_OP_ADD;				//ADD:加算合成.
	BlendDesc.RenderTarget[0].SrcBlendAlpha	//元素材のｱﾙﾌｧに対する指定.
		= D3D11_BLEND_ONE;						//そのまま使用.
	BlendDesc.RenderTarget[0].DestBlendAlpha	//重ねる素材のｱﾙﾌｧに対する設定.
		= D3D11_BLEND_ZERO;							//何もしない。
	BlendDesc.RenderTarget[0].BlendOpAlpha	//ｱﾙﾌｧのﾌﾞﾚﾝﾄﾞｵﾌﾟｼｮﾝ.
		= D3D11_BLEND_OP_ADD;					//ADD:加算合成.
	BlendDesc.RenderTarget[0].RenderTargetWriteMask	//ﾋﾟｸｾﾙ毎の書き込みﾏｽｸ.
		= D3D11_COLOR_WRITE_ENABLE_ALL;			//全ての成分(RGBA)へのﾃﾞｰﾀの格納を許可する.

												//ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ作成.
	if( FAILED(
		m_pDevice11->CreateBlendState(
			&BlendDesc, &m_pAlphaBlend ) ) ){
		_ASSERT_EXPR( false, L"ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ作成失敗" );
		return E_FAIL;
	}

	//ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ作成.
	BlendDesc.RenderTarget[0].BlendEnable
		= false;	//false:ｱﾙﾌｧﾌﾞﾚﾝﾄﾞを使用しない.
	if( FAILED(
		m_pDevice11->CreateBlendState(
			&BlendDesc, &m_pNoAlphaBlend ) ) ){
		_ASSERT_EXPR( false, L"ﾌﾞﾚﾝﾄﾞｽﾃｰﾄ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}

//透過設定の切り替え.
void CDebugText::SetBlend( bool EnableAlpha )
{
	//ﾌﾞﾚﾝﾄﾞｽﾃｰﾄの設定.
	UINT mask = 0xffffffff;	//ﾏｽｸ値.
	if( EnableAlpha == true ){
		m_pContext11->OMSetBlendState(
			m_pAlphaBlend, nullptr, mask );
	} else{
		m_pContext11->OMSetBlendState(
			m_pNoAlphaBlend, nullptr, mask );
	}
}

//ﾚﾝﾀﾞﾘﾝｸﾞ関数.
void CDebugText::Render( const string& stext, const float& fvalue )
{
//#ifdef _DEBUG
	string Text = stext + to_string(fvalue);

	//ﾋﾞｭｰﾄﾗﾝｽﾌｫｰﾑ.
	D3DXVECTOR3 vEye( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vLook( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH(
		&GetInstance()->m_mView, &vEye, &vLook, &vUp );
#if 0
	//ﾌﾟﾛｼﾞｪｸｼｮﾝﾄﾗﾝｽﾌｫｰﾑ(固定).
	D3DMATRIX mOtho = {
		2.0f / static_cast<float>( m_dwWindowWidth ), 0.0f,0.0f,0.0f,
		0.0f, -2.0f / static_cast<float>( m_dwWindowHeight ),0.0f,0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
#else
	D3DXMATRIX mOtho;
	D3DXMatrixIdentity( &mOtho );//単位行列作成.
	mOtho._11 = 2.0f / static_cast<float>( GetInstance()->m_dwWindowWidth );
	mOtho._22 = -2.0f / static_cast<float>( GetInstance()->m_dwWindowHeight );
	mOtho._41 = -1.0f;
	mOtho._42 = 1.0f;
#endif
	GetInstance()->m_mProj = mOtho;

	//ﾌﾟﾘﾐﾃｨﾌﾞ・ﾄﾎﾟﾛｼﾞｰ.
	GetInstance()->m_pContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄをｾｯﾄ.
	GetInstance()->m_pContext11->IASetInputLayout( GetInstance()->m_pVertexLayout );

	//使用するｼｪｰﾀﾞの登録.
	GetInstance()->m_pContext11->VSSetShader( GetInstance()->m_pVertexShader, nullptr, 0 );
	GetInstance()->m_pContext11->PSSetShader( GetInstance()->m_pPixelShader, nullptr, 0 );

	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧを登録.
	GetInstance()->m_pContext11->VSSetConstantBuffers( 0, 1, &GetInstance()->m_pConstantBuffer );
	GetInstance()->m_pContext11->PSSetConstantBuffers( 0, 1, &GetInstance()->m_pConstantBuffer );

	//ｻﾝﾌﾟﾗとﾃｸｽﾁｬをﾋﾟｸｾﾙｼｪｰﾀﾞに渡す.
	GetInstance()->m_pContext11->PSSetSamplers( 0, 1, &GetInstance()->m_pSampleLinear );
	GetInstance()->m_pContext11->PSSetShaderResources( 0, 1, &GetInstance()->m_pAsciiTexture );

	//文字数分ﾙｰﾌﾟ.
	for( int i = 0; i < static_cast<int>(strlen( Text.c_str() )); i++ ){
		char font = Text[i];
		int index = font - 32;	//ﾌｫﾝﾄｲﾝﾃﾞｯｸｽ作成.

								//ﾌｫﾝﾄﾚﾝﾀﾞﾘﾝｸﾞ.
		GetInstance()->RenderFont( index, GetInstance()->m_vPos.x, GetInstance()->m_vPos.y );
		GetInstance()->m_vPos.x += GetInstance()->m_fKerning[index]*GetInstance()->m_fScale;
	}
//#endif	// #ifdef _DEBUG.
}

void CDebugText::Render( const string& stext )
{
//#ifdef _DEBUG
	string Text = stext;

	//ﾋﾞｭｰﾄﾗﾝｽﾌｫｰﾑ.
	D3DXVECTOR3 vEye( 0.0f, 0.0f, -1.0f );
	D3DXVECTOR3 vLook( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH(
		&GetInstance()->m_mView, &vEye, &vLook, &vUp );
#if 0
	//ﾌﾟﾛｼﾞｪｸｼｮﾝﾄﾗﾝｽﾌｫｰﾑ(固定).
	D3DMATRIX mOtho = {
		2.0f / static_cast<float>( m_dwWindowWidth ), 0.0f,0.0f,0.0f,
		0.0f, -2.0f / static_cast<float>( m_dwWindowHeight ),0.0f,0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
#else
	D3DXMATRIX mOtho;
	D3DXMatrixIdentity( &mOtho );//単位行列作成.
	mOtho._11 = 2.0f / static_cast<float>( GetInstance()->m_dwWindowWidth );
	mOtho._22 = -2.0f / static_cast<float>( GetInstance()->m_dwWindowHeight );
	mOtho._41 = -1.0f;
	mOtho._42 = 1.0f;
#endif
	GetInstance()->m_mProj = mOtho;

	//ﾌﾟﾘﾐﾃｨﾌﾞ・ﾄﾎﾟﾛｼﾞｰ.
	GetInstance()->m_pContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄをｾｯﾄ.
	GetInstance()->m_pContext11->IASetInputLayout( GetInstance()->m_pVertexLayout );

	//使用するｼｪｰﾀﾞの登録.
	GetInstance()->m_pContext11->VSSetShader( GetInstance()->m_pVertexShader, nullptr, 0 );
	GetInstance()->m_pContext11->PSSetShader( GetInstance()->m_pPixelShader, nullptr, 0 );

	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧを登録.
	GetInstance()->m_pContext11->VSSetConstantBuffers( 0, 1, &GetInstance()->m_pConstantBuffer );
	GetInstance()->m_pContext11->PSSetConstantBuffers( 0, 1, &GetInstance()->m_pConstantBuffer );

	//ｻﾝﾌﾟﾗとﾃｸｽﾁｬをﾋﾟｸｾﾙｼｪｰﾀﾞに渡す.
	GetInstance()->m_pContext11->PSSetSamplers( 0, 1, &GetInstance()->m_pSampleLinear );
	GetInstance()->m_pContext11->PSSetShaderResources( 0, 1, &GetInstance()->m_pAsciiTexture );

	//文字数分ﾙｰﾌﾟ.
	for( int i = 0; i < static_cast<int>( strlen( Text.c_str() ) ); i++ ){
		char font = Text[i];
		int index = font - 32;	//ﾌｫﾝﾄｲﾝﾃﾞｯｸｽ作成.

								//ﾌｫﾝﾄﾚﾝﾀﾞﾘﾝｸﾞ.
		GetInstance()->RenderFont( index, GetInstance()->m_vPos.x, GetInstance()->m_vPos.y );
		GetInstance()->m_vPos.x += GetInstance()->m_fKerning[index] * GetInstance()->m_fScale;
	}
//#endif	// #ifdef _DEBUG.
}

//ﾌｫﾝﾄﾚﾝﾀﾞﾘﾝｸﾞ関数.
void CDebugText::RenderFont( int FontIndex, float x, float y )
{
	//ﾜｰﾙﾄﾞ変換.
	D3DXMATRIX mWorld, mScale, mTran;
	D3DXMatrixIdentity( &mWorld );
	D3DXMatrixTranslation( &mTran,
		x,  y, -100.0f );

	//拡大縮小行列作成.
	D3DXMatrixScaling(
		&mScale, //(out)
		m_fScale, m_fScale, m_fScale );	// x, y, z.

	mWorld = mScale * mTran;

	//ｼｪｰﾀﾞのｺﾝｽﾀﾝﾄﾊﾞｯﾌｧに各種ﾃﾞｰﾀを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	SHADER_CONSTANT_BUFFER cb;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
								//ﾊﾞｯﾌｧ内のﾃﾞｰﾀの書き換え開始時にmap.
	if( SUCCEEDED(
		m_pContext11->Map( m_pConstantBuffer,
			0, D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) ){
		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝ行列を渡す.
		D3DXMATRIX m = mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose( &m, &m );//行列を転置する.
		cb.mWVP = m;

		//ｶﾗｰ.
		cb.vColor = m_vColor;

		//透過値.
		cb.fAlpha.x = m_fAlpha;

		memcpy_s( pData.pData, pData.RowPitch,
			(void*)( &cb ), sizeof( cb ) );

		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	//頂点ﾊﾞｯﾌｧをｾｯﾄ.
	UINT stride = sizeof( VERTEX );//ﾃﾞｰﾀの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1,
		&m_pVertexBuffer[FontIndex], &stride, &offset );


	//描画.
	SetBlend( true );
	m_pContext11->Draw( 4, 0 );
	SetBlend( false );
}