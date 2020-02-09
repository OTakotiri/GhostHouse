#include "Sphere.h"

// シェーダファイル名(ディレクトリも含む).
const char SHADER_NAME[] = "Data\\Shader\\Ray.hlsl";

CSphere::CSphere()
	: m_pDevice11		( nullptr )
	, m_pContext11		( nullptr )
	, m_pVertexShader	( nullptr )
	, m_pVertexLayout	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pConstantBuffer	( nullptr )
	, g_pIndexBuffer	( nullptr )
	, m_pVertexBuffer	( nullptr )
	, m_pAlphaBlend		( nullptr )
	, m_pNoAlphaBlend	( nullptr )
	, m_vPosition		()
	, m_fYaw			( 0.0f )
	, m_fScale			( 1.0f )
{
}

CSphere::~CSphere()
{
	SAFE_RELEASE( m_pNoAlphaBlend );
	SAFE_RELEASE( m_pAlphaBlend );
	SAFE_RELEASE( g_pIndexBuffer );
	SAFE_RELEASE( m_pConstantBuffer );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pVertexLayout );
	SAFE_RELEASE( m_pVertexShader );

	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
}

//------------------------.
// 作成関数.
//------------------------.
HRESULT CSphere::Init( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const float& fRadius )
{
	m_pDevice11 = pDevice11;
	m_pContext11 = pContext11;

	if( FAILED( InitShader() ) ){
		return E_FAIL;
	}
	if( FAILED( InitSphere( fRadius ) ) ){
		return E_FAIL;
	}
	if( FAILED( InitBlend() ) ){
		return E_FAIL;
	}
	return S_OK;
}

//------------------------.
// シェーダー作成.
//------------------------.
HRESULT CSphere::InitShader()
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
	cb.ByteWidth = sizeof( C_BUFFER );//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｻｲｽﾞ.
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

//------------------------.
// スフィア作成.
//------------------------.
HRESULT CSphere::InitSphere( const float& fRadius )
{
	const int U_MAX = 30;
	const int V_MAX = 15;

	// 頂点作成.
	VERTEX vertices[480];
	WORD vertex_num = U_MAX * ( V_MAX + 1 );
	for( int v = 0; v <= V_MAX; v++ ){
		for( int u = 0; u < U_MAX; u++ ){
			float theta = D3DXToRadian( 180.0f * v / V_MAX );
			float phi = D3DXToRadian( 360.0f * u / U_MAX );
			float x = sin( theta ) * cos( phi ) * fRadius;
			float y = cos( theta ) * fRadius;
			float z = sin( theta ) * sin( phi )* fRadius;
			vertices[U_MAX * v + u].Pos = D3DXVECTOR3( x, y, z );
		}
	}

	// インデックス作成.
	WORD g_IndexList[930];
	WORD index_num = 2 * V_MAX * ( U_MAX + 1 );
	int i = 0;
	for( int v = 0; v < V_MAX; v++ ){
		for( int u = 0; u <= U_MAX; u++ ){
			if( u == U_MAX ){
				g_IndexList[i++] = v * U_MAX;
				g_IndexList[i++] = ( v + 1 ) * U_MAX;
			} else{
				g_IndexList[i++] = ( v * U_MAX ) + u;
				g_IndexList[i++] = g_IndexList[i - 1] + U_MAX;
			}
		}
	}

	//最大要素数を算出する.
	UINT uVerMax = sizeof( vertices ) / sizeof( vertices[0] );

	//ﾊﾞｯﾌｧ構造体.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;	//使用方法(ﾃﾞﾌｫﾙﾄ).
	bd.ByteWidth = sizeof( VERTEX ) * vertex_num;//頂点のｻｲｽﾞ.
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//頂点ﾊﾞｯﾌｧとして扱う.
	bd.CPUAccessFlags = 0;	//CPUからはｱｸｾｽしない.
	bd.MiscFlags = 0;		//その他のﾌﾗｸﾞ(未使用).
	bd.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).

	//ｻﾌﾞﾘｿｰｽﾃﾞｰﾀ構造体.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;	//板ﾎﾟﾘの頂点をｾｯﾄ.

	//頂点ﾊﾞｯﾌｧの作成.
	if( FAILED( m_pDevice11->CreateBuffer(
		&bd, &InitData, &m_pVertexBuffer ) ) ){
		_ASSERT_EXPR( false, L"頂点ﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	D3D11_BUFFER_DESC id;
	id.Usage = D3D11_USAGE_DEFAULT;
	id.ByteWidth = sizeof( WORD ) * index_num;
	id.BindFlags = D3D11_BIND_INDEX_BUFFER;
	id.CPUAccessFlags = 0;	//CPUからはｱｸｾｽしない.
	id.MiscFlags = 0;		//その他のﾌﾗｸﾞ(未使用).
	id.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).

	D3D11_SUBRESOURCE_DATA IndexData;
	IndexData.pSysMem = g_IndexList;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;
	if( FAILED( m_pDevice11->CreateBuffer(
		&id, &IndexData, &g_pIndexBuffer ) ) ){
		_ASSERT_EXPR( false, L"インデックスﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}

//------------------------.
// 描画関数.
//------------------------.
void CSphere::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj )
{
	if( m_pVertexShader == nullptr ) return;
	//ﾜｰﾙﾄﾞ行列.
	D3DXMATRIX	mWorld;

	// 回転行列,　移動行列.
	D3DXMATRIX mRot, mTran, mScale;

	// 回転行列.
	D3DXMatrixRotationY( &mRot, m_fYaw );

	//平行移動.
	D3DXMatrixTranslation( &mTran,
		m_vPosition.x, m_vPosition.y, m_vPosition.z );

	//拡大縮小行列作成.
	D3DXMatrixScaling(
		&mScale, //(out)
		m_fScale, m_fScale, m_fScale );	// x, y, z.

	//使用するｼｪｰﾀﾞのｾｯﾄ.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );//頂点ｼｪｰﾀﾞ.
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );//ﾋﾟｸｾﾙｼｪｰﾀﾞ.

	// ワールド行列
	mWorld = mScale * mRot * mTran;

	//ｼｪｰﾀﾞのｺﾝｽﾀﾝﾄﾊﾞｯﾌｧに各種ﾃﾞｰﾀを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	C_BUFFER cb;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
	//ﾊﾞｯﾌｧ内のﾃﾞｰﾀの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pConstantBuffer, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData ) ) ){
		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝ行列を渡す.
		D3DXMATRIX mWVP = mWorld * mView * mProj;
		D3DXMatrixTranspose( &mWVP, &mWVP );//行列を転置する.
										  //※行列の計算方法がDirectXとGPUで異なるため転置が必要.

		cb.mWVP = mWVP;

		//色の設定.
		cb.vColor = D3DXVECTOR4( 0.0f, 0.5f, 0.5f, 0.2f );


		memcpy_s(
			pData.pData,	//ｺﾋﾟｰ先のﾊﾞｯﾌｧ.
			pData.RowPitch,	//ｺﾋﾟｰ先のﾊﾞｯﾌｧｻｲｽﾞ.
			(void*)( &cb ),	//ｺﾋﾟｰ元のﾊﾞｯﾌｧ.
			sizeof( cb ) );	//ｺﾋﾟｰ元のﾊﾞｯﾌｧｻｲｽﾞ.

							//ﾊﾞｯﾌｧ内のﾃﾞｰﾀの書き換え終了時にUnmap.
		m_pContext11->Unmap( m_pConstantBuffer, 0 );
	}

	//このｺﾝｽﾀﾝﾄﾊﾞｯﾌｧをどのｼｪｰﾀﾞで使用するか？.
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	//頂点ｼｪｰﾀﾞ.
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pConstantBuffer );	//ﾋﾟｸｾﾙｼｪｰﾀﾞ.

	//頂点ﾊﾞｯﾌｧをｾｯﾄ.
	UINT stride = sizeof( VERTEX );//ﾃﾞｰﾀの間隔.
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers( 0, 1,
		&m_pVertexBuffer, &stride, &offset );

	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄをｾｯﾄ.
	m_pContext11->IASetInputLayout( m_pVertexLayout );
	m_pContext11->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

	//ﾌﾟﾘﾐﾃｨﾌﾞ・ﾄﾎﾟﾛｼﾞｰをｾｯﾄ.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	SetBlend( true );
	//ﾌﾟﾘﾐﾃｨﾌﾞをﾚﾝﾀﾞﾘﾝｸﾞ(描画).
	m_pContext11->DrawIndexed( 930, 0, 0 );
	SetBlend( false );
}

//------------------------.
// ブレンド作成.
//------------------------.
HRESULT CSphere::InitBlend()
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

//------------------------.
// ブレンドの有効無効.
//------------------------.
void CSphere::SetBlend( bool EnableAlpha )
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