#include "CDX9Mesh.h"
#include <crtdbg.h>	// _ASSERTマクロで使用.

const char SHADER_NAME[] = "Data\\Shader\\SpotLight.hlsl";

// コンストラクタ.
CDX9Mesh::CDX9Mesh()
	: m_hWnd				( nullptr )
	, m_pDevice9			( nullptr )
	, m_pDevice11			( nullptr )
	, m_pContext11			( nullptr )
	, m_pVertexShader		( nullptr )
	, m_pVertexLayout		( nullptr )
	, m_pPixelShader		( nullptr )
	, m_pCBufferPerMesh		( nullptr )
	, m_pCBufferPerMaterial	( nullptr )
	, m_pCBufferPerFrame	( nullptr )
	, m_pVertexBuffer		( nullptr )
	, m_ppIndexBuffer		( nullptr )
	, m_pSampleLinear		( nullptr )

	, m_pAlphaBlend			( nullptr )
	, m_pNoAlphaBlend		( nullptr )
	, m_CoverageBlend		( nullptr )
	, m_RasterizerWireON	( nullptr )
	, m_RasterizerWireOFF	( nullptr )

	, m_pMaterials			( nullptr )
	, m_pMesh				( nullptr )
	, m_pMeshForRay			( nullptr )
	, m_EnableTexture		( false )
	, m_vPos				( 0.0f, 0.0f, 0.0f )
	, m_vRot				( 0.0f, 0.0f, 0.0f )
	, m_fScale				( 1.0f )
	, m_NumMaterials		(0)
	, m_NumAttr				(0)
	, m_AttrID				()
	, m_fAlpha				( 1.0f )
{
}

CDX9Mesh::CDX9Mesh(
	HWND hWnd,
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
	: CDX9Mesh()
{
	Init( hWnd,  pDevice11, pContext11, pDevice9, fileName );
}
// デストラクタ.
CDX9Mesh::~CDX9Mesh()
{
	// 解放処理.
	Release();
}

HRESULT CDX9Mesh::Init(
	HWND hWnd,
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
{
	m_hWnd			= hWnd;
	m_pDevice11		= pDevice11;
	m_pContext11	= pContext11;
	m_pDevice9		= pDevice9;

	if( FAILED( LoadXMesh( fileName ))){
		return E_FAIL;
	}
	if( FAILED( InitShader() )){
		return E_FAIL;
	}
	if( FAILED( InitBlend() )){
		return E_FAIL;
	}
	if( FAILED( InitWtire() )){
		return E_FAIL;
	}
	if( FAILED( InitCullBack() ) ){
		return E_FAIL;
	}
	
	return S_OK;
}

// メッシュ読み込み.
HRESULT CDX9Mesh::LoadXMesh( const char* filename )
{

	// マテリアルバッファ.
	LPD3DXBUFFER pD3DXMtrlBuffer = nullptr;

	// Xファイルのロード.
	if ( FAILED( D3DXLoadMeshFromXA(
		filename,			// ファイル名.
		D3DXMESH_SYSTEMMEM	// システムメモリに読み込み.
		| D3DXMESH_32BIT,	
		m_pDevice9,			// 32bit.			
		nullptr,
		&pD3DXMtrlBuffer,	// (out)マテリアル情報.
		nullptr,
		&m_NumMaterials,	// (out)マテリアル数.
		&m_pMesh )))		// (out)メッシュオブジェクト.
	{
		// 何のファイルファイルが読み込めてないか　修正必要.
		_ASSERT_EXPR( false, L"Xファイル読み込み失敗" );
		return E_FAIL;
	}

	//Xﾌｧｲﾙのﾛｰﾄﾞ(レイとの判定用に別設定で読み込む).
	if( FAILED( D3DXLoadMeshFromXA(
		filename,	//ﾌｧｲﾙ名.
		D3DXMESH_SYSTEMMEM,	//ｼｽﾃﾑﾒﾓﾘに読み込み.
		m_pDevice9, nullptr,
		&pD3DXMtrlBuffer,	//(out)ﾏﾃﾘｱﾙ情報.
		nullptr,
		&m_NumMaterials,	//(out)ﾏﾃﾘｱﾙ数.
		&m_pMeshForRay ) ) )//(out)ﾒｯｼｭｵﾌﾞｼﾞｪｸﾄ.
	{
		_ASSERT_EXPR( false, L"Xﾌｧｲﾙ読込失敗" );
		return E_FAIL;
	}


	D3D11_BUFFER_DESC		bd;			// Dx11バッファ構造体.
	D3D11_SUBRESOURCE_DATA	InitData;	// 初期化データ.

										// 読み込んだ情報から必要な情報を抜き出す.
	D3DXMATERIAL *d3dxMaterials
		= static_cast<D3DXMATERIAL*>(pD3DXMtrlBuffer->GetBufferPointer());
	// マテリアル数分の領域を確保.
	m_pMaterials = new MY_MATERIAL[m_NumMaterials]();
	m_ppIndexBuffer = new ID3D11Buffer*[m_NumMaterials]();

	// マテリアル数分繰り返し.
	for( DWORD No = 0; No < m_NumMaterials; No++ )
	{
		// インデックスバッファの初期化.
		m_ppIndexBuffer[No] = nullptr;

		// マテリアル情報のコピー.
		// アンビエント.
		m_pMaterials[No].Ambient.x = d3dxMaterials[No].MatD3D.Ambient.r;
		m_pMaterials[No].Ambient.y = d3dxMaterials[No].MatD3D.Ambient.g;
		m_pMaterials[No].Ambient.z = d3dxMaterials[No].MatD3D.Ambient.b;
		m_pMaterials[No].Ambient.w = d3dxMaterials[No].MatD3D.Ambient.a;
		// ディフーズ.
		m_pMaterials[No].Diffuse.x = d3dxMaterials[No].MatD3D.Diffuse.r;
		m_pMaterials[No].Diffuse.y = d3dxMaterials[No].MatD3D.Diffuse.g;
		m_pMaterials[No].Diffuse.z = d3dxMaterials[No].MatD3D.Diffuse.b;
		m_pMaterials[No].Diffuse.w = d3dxMaterials[No].MatD3D.Diffuse.a;
		// スペキュラ.
		m_pMaterials[No].Specular.x = d3dxMaterials[No].MatD3D.Specular.r;
		m_pMaterials[No].Specular.y = d3dxMaterials[No].MatD3D.Specular.g;
		m_pMaterials[No].Specular.z = d3dxMaterials[No].MatD3D.Specular.b;
		m_pMaterials[No].Specular.w = d3dxMaterials[No].MatD3D.Specular.a;

		// (その面に)テクスチャが貼られているか？.
		if ( d3dxMaterials[No].pTextureFilename != nullptr &&
			lstrlen(d3dxMaterials[No].pTextureFilename) > 0 )
		{
			// テクスチャにフラグを立てる.
			m_EnableTexture = true;

			char path[128] = "";
			int path_count = lstrlen( filename );
			for( int k = path_count; k >= 0; k-- ){
				if( filename[k] == '\\' ){
					for( int j = 0; j <= k; j++ ){
						path[j] = filename[j];
					}
					path[k + 1] = '\0';
					break;
				}
			}
			// パスとテクスチャファイル名を連結.
			strcat_s( path, sizeof(path), d3dxMaterials[No].pTextureFilename );

			// テクスチャファイル名をコピー.
			strcpy_s( 
				m_pMaterials[No].szTexTureName,
				sizeof( m_pMaterials[No].szTexTureName ), path );
			//------------------------------------------------------------.
			// テクスチャ作成.
			if( FAILED( D3DX11CreateShaderResourceViewFromFileA(
				m_pDevice11, 
				m_pMaterials[No].szTexTureName,	// テクスチャファイル名.
				nullptr, 
				nullptr, 
				&m_pMaterials[No].pTexture,		// (out)テクスチャオブジェクト.
				nullptr )))
			{
				_ASSERT_EXPR( false, L"テクスチャ作成失敗" );
				return E_FAIL;
			}
		}
	}


	//--------------------------------------------------------
	// インデックスバッファ作成.
	//--------------------------------------------------------
	// メッシュの属性情報を得る.
	// 属性情報でインデックスバッファから細かいマテリアルごとのインデックスバッファを分離できる.
	D3DXATTRIBUTERANGE* pAttrTable = nullptr;

	// メッシュの面及び頂点の順番変更を制御し、パフォーマンスを最適化する.
	// D3DXMESHOPT_COMPACT : 面の順番を変更し、使用されていない頂点と面を削除する.
	// D3DXMESHOPT_ATTRSORT : パフォーマンスを上げるため、面の順番を変更して最適化を行う.
	m_pMesh->OptimizeInplace( 
		D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT, 
		nullptr, nullptr, nullptr, nullptr );
	// 属性テーブルの取得.
	m_pMesh->GetAttributeTable( nullptr, &m_NumAttr );
	pAttrTable = new D3DXATTRIBUTERANGE[m_NumAttr];
	if( FAILED( m_pMesh->GetAttributeTable( pAttrTable, &m_NumAttr )))
	{
		_ASSERT_EXPR( false, L"属性テーブル取得失敗" );
		return E_FAIL;
	}

	// 同じくLockしないと取り出せない.
	int*	pIndex = nullptr;
	m_pMesh->LockIndexBuffer(
		D3DLOCK_READONLY, (void**)&pIndex );
	// 属性ごとのインデックスバッファを作成.
	for( DWORD No = 0; No < m_NumAttr; No++ ){
		m_AttrID[No] = pAttrTable[No].AttribId;
		// Dx9のインデックスバッファからの情報で、Dx11のインデックスバッファを作成,
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.ByteWidth = sizeof( int )*pAttrTable[No].FaceCount * 3;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		// 大きいインデックスバッファ内のオフセット(x3する).
		InitData.pSysMem = &pIndex[pAttrTable[No].FaceStart*3];

		if( FAILED( m_pDevice11->CreateBuffer(
			&bd, &InitData, &m_ppIndexBuffer[No] ))){
			_ASSERT_EXPR( false, L"インデックスバッファ作成失敗" );
			return E_FAIL;
		}
		// 面の数をコピー.
		m_pMaterials[m_AttrID[No]].dwNumFace = pAttrTable[No].FaceCount;
	}
	// 属性テーブルの削除.
	delete[] pAttrTable;
	// 使用済みのインデックスバッファの解放.
	m_pMesh->UnlockIndexBuffer();

	// 不要になったマテリアルバッファを解放.
	SAFE_RELEASE( pD3DXMtrlBuffer );


	//---------------------------------------------------.
	// 頂点バッファの作成.
	//---------------------------------------------------.
	// Dx9の場合、mapではなくLockで頂点バッファからデータを取り出す.
	LPDIRECT3DVERTEXBUFFER9	pVB = nullptr;
	m_pMesh->GetVertexBuffer( &pVB );
	DWORD dwStride	= m_pMesh->GetNumBytesPerVertex();	// 頂点バイト数.
	BYTE* pVertices	= nullptr;
	VERTEX* pVertex	= nullptr;
	if( SUCCEEDED(
		pVB->Lock( 0, 0, (VOID**)&pVertices, 0 )))
	{
		pVertex = (VERTEX*)pVertices;
		// Dx9の頂点バッファからの情報で、Dx11頂点バッファを作成.
		bd.Usage		= D3D11_USAGE_DEFAULT;
		// 頂点を格納するのに必要なバイト数.
		bd.ByteWidth	= m_pMesh->GetNumBytesPerVertex()*m_pMesh->GetNumVertices();
		bd.BindFlags	= D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags	= 0;
		bd.MiscFlags	= 0;
		InitData.pSysMem = pVertex;
		if ( FAILED( m_pDevice11->CreateBuffer( 
			&bd, &InitData, &m_pVertexBuffer )))
		{
			_ASSERT_EXPR( false, L"頂点バッファ作成失敗" );
			return E_FAIL;
		}
		pVB->Unlock();
	}
	SAFE_RELEASE( pVB );	// 頂点バッファ解放.


							//ﾃｸｽﾁｬ用のｻﾝﾌﾟﾗ構造体.
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//ﾘﾆｱﾌｨﾙﾀ(線形補間).
													 //POINT:高速だが粗い.
	samDesc.AddressU
		= D3D11_TEXTURE_ADDRESS_WRAP;//ﾗｯﾋﾟﾝｸﾞﾓｰﾄﾞ(WRAP:繰り返し).
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//MIRROR: 反転繰り返し.
	//CLAMP : 端の模様を引き伸ばす.
	//BORDER: 別途境界色を決める.
	//ｻﾝﾌﾟﾗ作成.
	if( FAILED( m_pDevice11->CreateSamplerState(
		&samDesc, &m_pSampleLinear ) ) )//(out)ｻﾝﾌﾟﾗ.
	{
		_ASSERT_EXPR( false, L"ｻﾝﾌﾟﾗ作成失敗" );
		return E_FAIL;
	}

	return S_OK;
}

// 解放関数.
void CDX9Mesh::Release()
{

	if( m_ppIndexBuffer != nullptr ){
		for( int No = m_NumMaterials-1; No >= 0; No-- ){
			if( m_ppIndexBuffer[No] != nullptr ){
				SAFE_RELEASE( m_ppIndexBuffer[No] );
			}
		}
		delete[] m_ppIndexBuffer;
		m_ppIndexBuffer = nullptr;
	}
	// マテリアルの解放.
	SAFE_DELETE_ARY	( m_pMaterials);

	SAFE_RELEASE	( m_pMeshForRay );
	SAFE_RELEASE	( m_pMesh );

	SAFE_RELEASE	( m_pAlphaBlend );
	SAFE_RELEASE	( m_pNoAlphaBlend );
	SAFE_RELEASE	( m_CoverageBlend );

	SAFE_RELEASE	( m_RasterizerWireON );
	SAFE_RELEASE	( m_RasterizerWireOFF );

	SAFE_RELEASE	( m_pSampleLinear );
	SAFE_RELEASE	( m_pVertexBuffer );
	SAFE_RELEASE	( m_pCBufferPerMaterial );
	SAFE_RELEASE	( m_pCBufferPerMesh );
	SAFE_RELEASE	( m_pPixelShader );
	SAFE_RELEASE	( m_pVertexLayout );
	SAFE_RELEASE	( m_pVertexShader );
	
	m_pDevice9 = nullptr;
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;
	m_hWnd = nullptr;
}

//===========================================================
//	HLSLﾌｧｲﾙを読み込みｼｪｰﾀﾞを作成する.
//	HLSL: High Level Shading Language の略.
//===========================================================
HRESULT CDX9Mesh::InitShader()
{
	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrors = nullptr;
	UINT uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag =
		D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG


	std::string sVS_Name = "VS";
	//if( m_EnableTexture == true ){
	//	sVS_Name += "Main";
	//} else{
	//	sVS_Name += "NoTex";
	//}
	//HLSLからﾊﾞｰﾃｯｸｽｼｪｰﾀﾞのﾌﾞﾛﾌﾞを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,		//ｼｪｰﾀﾞﾌｧｲﾙ名(HLSLﾌｧｲﾙ).
			nullptr,			//ﾏｸﾛ定義の配列へのﾎﾟｲﾝﾀ(未使用).
			nullptr,			//ｲﾝｸﾙｰﾄﾞﾌｧｲﾙを扱うｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			sVS_Name.c_str(),	//ｼｪｰﾀﾞｴﾝﾄﾘｰﾎﾟｲﾝﾄ関数の名前.
			"vs_5_0",			//ｼｪｰﾀﾞのﾓﾃﾞﾙを指定する文字列(ﾌﾟﾛﾌｧｲﾙ).
			uCompileFlag,		//ｼｪｰﾀﾞｺﾝﾊﾟｲﾙﾌﾗｸﾞ.
			0,					//ｴﾌｪｸﾄｺﾝﾊﾟｲﾙﾌﾗｸﾞ(未使用).
			nullptr,			//ｽﾚｯﾄﾞ ﾎﾟﾝﾌﾟ ｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			&pCompiledShader,	//ﾌﾞﾛﾌﾞを格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			&pErrors,			//ｴﾗｰと警告一覧を格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
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
	D3D11_INPUT_ELEMENT_DESC layout[3];
	//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄの配列要素数を算出.
	UINT numElements = 0;
	if( m_EnableTexture == true ){
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",	0 ,DXGI_FORMAT_R32G32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof(tmp) / sizeof(tmp[0]);	// 要素数算出.
		memcpy_s( layout, sizeof(layout), tmp, 
			sizeof(D3D11_INPUT_ELEMENT_DESC) * numElements );
	} else {
		D3D11_INPUT_ELEMENT_DESC tmp[] =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		numElements = sizeof( tmp ) / sizeof( tmp[0] );	// 要素数算出.
		memcpy_s( layout, sizeof( layout ), tmp, 
			sizeof( D3D11_INPUT_ELEMENT_DESC ) * numElements );
	}

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

	std::string sPS_Name = "PS";
	//if( m_EnableTexture == true ){
	//	sPS_Name += "Main";
	//} else{
	//	sPS_Name += "NoTex";
	//}
	//HLSLからﾋﾟｸｾﾙｼｪｰﾀﾞのﾌﾞﾛﾌﾞを作成.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME,	//ｼｪｰﾀﾞﾌｧｲﾙ名(HLSLﾌｧｲﾙ).
			nullptr,			//ﾏｸﾛ定義の配列へのﾎﾟｲﾝﾀ(未使用).
			nullptr,			//ｲﾝｸﾙｰﾄﾞﾌｧｲﾙを扱うｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			sPS_Name.c_str(),	//ｼｪｰﾀﾞｴﾝﾄﾘｰﾎﾟｲﾝﾄ関数の名前.
			"ps_5_0",			//ｼｪｰﾀﾞのﾓﾃﾞﾙを指定する文字列(ﾌﾟﾛﾌｧｲﾙ).
			uCompileFlag,		//ｼｪｰﾀﾞｺﾝﾊﾟｲﾙﾌﾗｸﾞ.
			0,					//ｴﾌｪｸﾄｺﾝﾊﾟｲﾙﾌﾗｸﾞ(未使用).
			nullptr,			//ｽﾚｯﾄﾞ ﾎﾟﾝﾌﾟ ｲﾝﾀｰﾌｪｲｽへのﾎﾟｲﾝﾀ(未使用).
			&pCompiledShader,	//ﾌﾞﾛﾌﾞを格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
			&pErrors,			//ｴﾗｰと警告一覧を格納するﾒﾓﾘへのﾎﾟｲﾝﾀ.
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
	cb.ByteWidth = sizeof( CBUFFER_PER_MESH );	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｻｲｽﾞ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//書き込みでｱｸｾｽ.
	cb.MiscFlags = 0;							//その他のﾌﾗｸﾞ(未使用).
	cb.StructureByteStride = 0;					//構造体のｻｲｽﾞ(未使用).
	cb.Usage = D3D11_USAGE_DYNAMIC;				//使用方法:直接書き込み.

												//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧの作成.
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&cb, nullptr, &m_pCBufferPerMesh )))
	{
		_ASSERT_EXPR( false, L"ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	// コンスタントバッファ マテリアル用.
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧを指定.
	cb.ByteWidth = sizeof( CBUFFER_PER_MATERIAL );//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｻｲｽﾞ.
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//書き込みでｱｸｾｽ.
	cb.MiscFlags = 0;	//その他のﾌﾗｸﾞ(未使用).
	cb.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).
	cb.Usage = D3D11_USAGE_DYNAMIC;	//使用方法:直接書き込み.

									//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧの作成.
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&cb, nullptr, &m_pCBufferPerMaterial )))
	{
		_ASSERT_EXPR( false, L"ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}

	D3D11_BUFFER_DESC cbf;
	// コンスタントバッファ マテリアル用.
	cbf.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧを指定.
	cbf.ByteWidth = sizeof( cbPerObject );//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｻｲｽﾞ.
	cbf.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	//書き込みでｱｸｾｽ.
	cbf.MiscFlags = 0;	//その他のﾌﾗｸﾞ(未使用).
	cbf.StructureByteStride = 0;	//構造体のｻｲｽﾞ(未使用).
	cbf.Usage = D3D11_USAGE_DYNAMIC;	//使用方法:直接書き込み.

										//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧの作成.
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&cbf, nullptr, &m_pCBufferPerFrame ) ) ){
		_ASSERT_EXPR( false, L"ｺﾝｽﾀﾝﾄFﾊﾞｯﾌｧ作成失敗" );
		return E_FAIL;
	}


	return S_OK;
}

// レンダリング用.
// ※DirectX内のレンダリング関数.
//   最終的に画面に出力するのはMainクラスのレンダリング関数がやる.
void CDX9Mesh::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera, bool ChangeTexFlag )
{
	//　ワールド行列, スケール行列, 回転行列, 平行移動行列.
	D3DXMATRIX mWorld, mScale, mRot, mTran;
	D3DXMATRIX mYaw, mPitch, mRoll;

	//拡大縮小行列作成.
	D3DXMatrixScaling(
		&mScale, //(out)
		m_fScale, m_fScale, m_fScale );	// x, y, z.

	// Y軸回転行列作成.
	D3DXMatrixRotationY( &mYaw,		m_vRot.y );	// 単純にY軸回転.
												// X軸回転行列作成.
	D3DXMatrixRotationX( &mPitch,	m_vRot.x );	// 単純にX軸回転.
												// Z軸回転行列作成.
	D3DXMatrixRotationZ( &mRoll,	m_vRot.z );	// 単純にZ軸回転.

												// 平行移動行列.
	D3DXMatrixTranslation(
		&mTran,	//(out).
		m_vPos.x, m_vPos.y, m_vPos.z );	// x, y, z座標.

										// 回転行列を作成.
										//mRot = mYaw * mPitch * mRoll;
	mRot = mYaw * mPitch * mRoll;
	// ワールド行列作成.
	//    ※   拡縮 * 回転 * 移動	※順番がとても大切 !!!.
	mWorld = mScale * mRot * mTran;

	//使用するｼｪｰﾀﾞのｾｯﾄ.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );	//頂点ｼｪｰﾀﾞ.
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );	//ﾋﾟｸｾﾙｼｪｰﾀﾞ.

	static float rng = 0.0f;
	if( GetAsyncKeyState('K') & 0x8000 ){
		rng += 0.001f;
	}

	//ｼｪｰﾀﾞのｺﾝｽﾀﾝﾄﾊﾞｯﾌｧに各種ﾃﾞｰﾀを渡す.
	D3D11_MAPPED_SUBRESOURCE pData;
	//ﾊﾞｯﾌｧ内のﾃﾞｰﾀの書き換え開始時にMap.
	if( SUCCEEDED( m_pContext11->Map(
		m_pCBufferPerFrame, 0,
		D3D11_MAP_WRITE_DISCARD,
		0, &pData ) ) )
	{
		cbPerObject cb;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
		//ワールド行列を渡す.
		cb.World = mWorld;
		D3DXMatrixTranspose( &cb.World, &cb.World );

		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝ行列を渡す.
		D3DXMATRIX mWVP = mWorld * mView * mProj;
		D3DXMatrixTranspose( &mWVP, &mWVP );//行列を転置する.
		//※行列の計算方法がDirectXとGPUで異なるため転置が必要.
		cb.WVP = mWVP;

		cb.light.Rot = stLight.Rot;
		D3DXMatrixTranspose( &cb.light.Rot, &cb.light.Rot );

		//cb.light.pos = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
		cb.light.dir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

		D3DXVec3TransformNormal( &cb.light.dir, &cb.light.dir, &cb.light.Rot );

		cb.light.range = stLight.range;
		cb.light.cone = stLight.cone;					//値がでかいとなんか照らす円が小さくなります.
		cb.light.att = stLight.att;
		cb.light.ambient = stLight.ambient;
		cb.light.diffuse = stLight.diffuse;

		cb.light.pos.x = stLight.pos.x;
		cb.light.pos.y = stLight.pos.y;
		cb.light.pos.z = stLight.pos.z;

		cb.fAlpha = m_fAlpha;

		//cb.light.dir.x = 10 - stLight.pos.x;
		//cb.light.dir.y = 10 - stLight.pos.y;
		//cb.light.dir.z = 10 - stLight.pos.z;

		memcpy_s(
			pData.pData,	//ｺﾋﾟｰ先のﾊﾞｯﾌｧ.
			pData.RowPitch,	//ｺﾋﾟｰ先のﾊﾞｯﾌｧｻｲｽﾞ.
			(void*)( &cb ),	//ｺﾋﾟｰ元のﾊﾞｯﾌｧ.
			sizeof( cb ) );	//ｺﾋﾟｰ元のﾊﾞｯﾌｧｻｲｽﾞ.

		//ﾊﾞｯﾌｧ内のﾃﾞｰﾀの書き換え終了時にUnmap.
		m_pContext11->Unmap( m_pCBufferPerFrame, 0 );
	}
	//このｺﾝｽﾀﾝﾄﾊﾞｯﾌｧをどのｼｪｰﾀﾞで使用するか？.
	m_pContext11->VSSetConstantBuffers(
		0, 1, &m_pCBufferPerFrame );	//頂点ｼｪｰﾀﾞ.
	m_pContext11->PSSetConstantBuffers(
		0, 1, &m_pCBufferPerFrame );	//ﾋﾟｸｾﾙｼｪｰﾀﾞ.

										//頂点ｲﾝﾌﾟｯﾄﾚｲｱｳﾄをｾｯﾄ.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//ﾌﾟﾘﾐﾃｨﾌﾞ・ﾄﾎﾟﾛｼﾞｰをｾｯﾄ.
	m_pContext11->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// 頂点バッファをセット.
	UINT stride = m_pMesh->GetNumBytesPerVertex();
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(
		0, 1, &m_pVertexBuffer, &stride, &offset );

	// 属性の数だけ、それぞれの属性のインデックスバッファを描画.
	for( DWORD No = 0; No < m_NumAttr; No++ ){
		// 使用されていないマテリアル対策.
		if( m_pMaterials[m_AttrID[No]].dwNumFace == 0 ){
			continue;
		}
		// インデックスバッファをセット.
		m_pContext11->IASetIndexBuffer(
			m_ppIndexBuffer[No], DXGI_FORMAT_R32_UINT, 0 );
		// マテリアルの各要素をシェーダに渡す.
		D3D11_MAPPED_SUBRESOURCE pDataMat;
		if( SUCCEEDED(
			m_pContext11->Map( m_pCBufferPerMaterial, 
				0, D3D11_MAP_WRITE_DISCARD, 0, &pDataMat )))
		{
			CBUFFER_PER_MATERIAL cb;
			// アンビエント,ディフーズ,スペキュラをシェーダに渡す.
			cb.vAmbient = m_pMaterials[m_AttrID[No]].Ambient;
			cb.vDiffuse = m_pMaterials[m_AttrID[No]].Diffuse;
			cb.vSpecular = m_pMaterials[m_AttrID[No]].Specular;

			//			cb.vColor.w = 0.1f;

			memcpy_s( pDataMat.pData, pDataMat.RowPitch,
				(void*)&cb, sizeof(cb) );
			m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
		}
		// このコンスタントバッファをどのシェーダで使うか？.
		m_pContext11->VSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );
		m_pContext11->PSSetConstantBuffers( 1, 1, &m_pCBufferPerMaterial );

		// テクスチャをシェーダに渡す.
		if( m_pMaterials[m_AttrID[No]].pTexture != nullptr ){
			// テクスチャがあるとき.
			m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );
			if( ChangeTexFlag == false ){
				// 元に読み込んでいたテクスチャを表示.
				m_pContext11->PSSetShaderResources( 0, 1, &m_pMaterials[m_AttrID[No]].pTexture );
			} else {
				// 切り替えたテクスチャを表示.
				ChangeTextureRender( No );
			}
		} else {
			// テクスチャがないとき.
			ID3D11ShaderResourceView* pNoThing[1] = {0};
			m_pContext11->PSSetShaderResources( 0, 1, pNoThing );
		}
		// プリミティブ(ポリゴン)をレンダリング.
		m_pContext11->DrawIndexed( m_pMaterials[m_AttrID[No]].dwNumFace * 3, 0, 0 );
	}
}

void CDX9Mesh::ChangeTextureRender( int no )
{
	int str_length = lstrlen( m_pMaterials[m_AttrID[no]].szTexTureName );
	int str_no = 0;
	bool change_flag = false;
	for( str_no = str_length; 0 < str_no; str_no-- ){
		if( m_pMaterials[m_AttrID[no]].szTexTureName[str_no] == '\\' ){
			str_no++;	//パスの区切りまで来ているので一つ戻す.
			change_flag = true;
			break;
		}
	}
	if( change_flag == true ){
		//対象テクスチャ名なのか判定.
		if( strcmp(
			&m_pMaterials[m_AttrID[no]].szTexTureName[str_no],
			m_pChangeTextures->szTargetTextureName ) == 0 ){
			//別のテクスチャを設定.
			m_pContext11->PSSetShaderResources( 0, 1, &m_pChangeTextures->pTexture );
		} else{
			m_pContext11->PSSetShaderResources( 0, 1, &m_pMaterials[m_AttrID[no]].pTexture );
		}
	}
}

// スフィア作成.
HRESULT CDX9Mesh::InitSphere( CDX9Mesh* pMesh )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;	// 頂点バッファ.
	void* pVertices = nullptr;	// 頂点.
	D3DXVECTOR3 vCenter;		// 中心.
	float fRadius;

	// 頂点バッファを取得.
	if( FAILED(
		pMesh->GetMesh()->GetVertexBuffer( &pVB ) ) ){
		return E_FAIL;
	}
	// メッシュの頂点バッファをロックする.
	if( FAILED(
		pVB->Lock( 0, 0, &pVertices, 0 ) ) ){
		SAFE_RELEASE( pVB );
		return E_FAIL;
	}
	// メッシュの外接円の中心と半径を計算する.
	D3DXComputeBoundingSphere(
		static_cast<D3DXVECTOR3*>( pVertices ),
		pMesh->GetMesh()->GetNumVertices(),					// 頂点の数.
		D3DXGetFVFVertexSize( pMesh->GetMesh()->GetFVF() ),	// 頂点の情報.
		&vCenter,	// (out)中心座標.
		&fRadius );	// (out)半径.

					// アンロック.
	pVB->Unlock();
	SAFE_RELEASE( pVB );

	// 中心と半径を構造体に設定.
	pMesh->m_Sphere.vCenter = vCenter;
	pMesh->m_Sphere.fRadius = fRadius * 0.07f;

	return S_OK;
}

HRESULT CDX9Mesh::InitBlend()
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
		= false;		//true:ｱﾙﾌｧﾄｩｶﾊﾞﾚｰｼﾞを使用する.
	BlendDesc.RenderTarget[0].BlendEnable
		= true;		//true:ｱﾙﾌｧﾌﾞﾚﾝﾄﾞを使用する.
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

	BlendDesc.AlphaToCoverageEnable
		= true;		//true:ｱﾙﾌｧﾄｩｶﾊﾞﾚｰｼﾞを使用する.
	if( FAILED(
		m_pDevice11->CreateBlendState(
			&BlendDesc, &m_CoverageBlend ) ) ){
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
void CDX9Mesh::SetBlend( bool EnableAlpha )
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

void CDX9Mesh::SetCoverage( bool EnableCoverage )
{
	UINT mask = 0xffffffff;	//ﾏｽｸ値.
	if( EnableCoverage == true ){
		m_pContext11->OMSetBlendState(
			m_CoverageBlend, nullptr, mask );
	} else{
		m_pContext11->OMSetBlendState(
			m_pNoAlphaBlend, nullptr, mask );
	}
}

HRESULT CDX9Mesh::InitWtire()
{
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory( &rdc, sizeof( rdc ) );
	rdc.FillMode = D3D11_FILL_SOLID;//塗りつぶし(ｿﾘｯﾄﾞ).
	rdc.CullMode = D3D11_CULL_NONE;	//D3D11_CULL_BACK:背面を描画しない.
									//D3D11_CULL_FRONT:正面を描画しない.
									//D3D11_CULL_NONE:ｶﾘﾝｸﾞを切る(正背面を描画する).
	rdc.FrontCounterClockwise = FALSE;//ﾎﾟﾘｺﾞﾝの表裏を決定するﾌﾗｸﾞ.
									  //TRUE:左回りなら前向き。右回りなら後ろ向き。
									  //FALSE:逆になる.
	rdc.DepthClipEnable = FALSE;	//距離についてのｸﾘｯﾋﾟﾝｸﾞ有効.

	m_pDevice11->CreateRasterizerState( &rdc, &m_RasterizerWireOFF );
	rdc.FillMode = D3D11_FILL_WIREFRAME;
	m_pDevice11->CreateRasterizerState( &rdc, &m_RasterizerWireON );

	m_pContext11->RSSetState( m_RasterizerWireOFF );

	return S_OK;
}

void CDX9Mesh::SetWireFlag( bool flag )
{
	if( flag == true ){
		m_pContext11->RSSetState( m_RasterizerWireON );
	} else{
		m_pContext11->RSSetState( m_RasterizerWireOFF );
	}
}

HRESULT CDX9Mesh::InitCullBack()
{
	D3D11_RASTERIZER_DESC rdc;
	ZeroMemory( &rdc, sizeof( rdc ) );
	rdc.FillMode = D3D11_FILL_SOLID;//塗りつぶし(ｿﾘｯﾄﾞ).
	rdc.CullMode = D3D11_CULL_NONE;	//D3D11_CULL_BACK:背面を描画しない.
									//D3D11_CULL_FRONT:正面を描画しない.
									//D3D11_CULL_NONE:ｶﾘﾝｸﾞを切る(正背面を描画する).
	rdc.FrontCounterClockwise = FALSE;//ﾎﾟﾘｺﾞﾝの表裏を決定するﾌﾗｸﾞ.
									  //TRUE:左回りなら前向き。右回りなら後ろ向き。
									  //FALSE:逆になる.
	rdc.DepthClipEnable = FALSE;	//距離についてのｸﾘｯﾋﾟﾝｸﾞ有効.

	m_pDevice11->CreateRasterizerState( &rdc, &m_RasterizerCullBackOFF );
	rdc.CullMode = D3D11_CULL_BACK;
	m_pDevice11->CreateRasterizerState( &rdc, &m_RasterizerCullBackON );

	m_pContext11->RSSetState( m_RasterizerWireOFF );

	return S_OK;
}

void CDX9Mesh::SetCullBackFlag( bool flag )
{
	if( flag == true ){
		m_pContext11->RSSetState( m_RasterizerCullBackON );
	} else{
		m_pContext11->RSSetState( m_RasterizerCullBackOFF );
	}
}

D3DXVECTOR2 CDX9Mesh::WorldtoScreenPos( const D3DXMATRIX& mView,
	const D3DXMATRIX& mProj, const D3DXVECTOR3& pos )
{
	D3DXVECTOR3 screenOutPos;

	// ワールド変換行列.
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity( &mWorld );	// 単位行列作成.

	// ビューポート行列.
	D3DXMATRIX mViewport;
	D3DXMatrixIdentity( &mViewport );	// 単位行列作成.
	mViewport._11 = static_cast<float>( WND_W ) / 2.0f;
	mViewport._22 = static_cast<float>( WND_H ) /-2.0f;
	mViewport._41 = static_cast<float>( WND_W ) / 2.0f;
	mViewport._42 = static_cast<float>( WND_H ) / 2.0f;

	// ワールド, ビュー, プロジェ, ビューポートの計算.
	D3DXMATRIX m = mWorld * mView * mProj * mViewport;
	// D3DXVECTOR3と掛け合わせ.
	D3DXVec3TransformCoord( &screenOutPos, &pos, &m );

	// 計算結果のx, y座標を返す. 
	return D3DXVECTOR2( screenOutPos.x, screenOutPos.y );
}