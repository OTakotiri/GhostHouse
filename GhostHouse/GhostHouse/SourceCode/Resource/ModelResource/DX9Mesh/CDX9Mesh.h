#ifndef CDX9MESH_H
#define CDX9MESH_H

// 警告についてのコード分析を無効にする 4005:再定義.
#pragma warning( disable:4005 )

#include <Windows.h>
#include <string>

#include "..\..\..\Main\Global.h"

#include <d3dx9.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>	// 「D3DX～」の定義使用に必要.
#include <D3D10.h>

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )

// スフィア構造体.
struct SPHERE
{
	D3DXVECTOR3 vCenter;	// 中心.
	float		fRadius;	// 半径.
};


// メッシュデータをファイルから取り出すためだけにDirectX9を使用する.
//※レンダリング(描画)はDirectX11で行う.
class CDX9Mesh
{
public:
	CDX9Mesh(
		HWND hWnd,
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName );
	~CDX9Mesh();// デストラクタ.

	// レンダリング用.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera, bool ChangeTexFlag = false );

	// ブレンドの有効無効.
	void SetBlend( bool EnableAlpha );
	void SetCoverage( bool EnableCoverage );

	// 位置情報を設定.
	void SetPosition( const D3DXVECTOR3& vPos ){ m_vPos = vPos; }
	D3DXVECTOR3 GetPosition(){ return m_vPos; }

	// 回転情報を設定.
	void SetRotation( const D3DXVECTOR3& vRot ){ m_vRot = vRot; }

	// 回転情報を取得.
	D3DXVECTOR3 GetRotation(){ return m_vRot; }

	// 拡縮情報を設定.
	void SetScale( float fScale ) { m_fScale = fScale; }

	// アルファ値の設定.
	void SetAlpha( float fAlpha ) { m_fAlpha = fAlpha; }

	// ワイヤーフレームで表示するか設定.
	void SetWireFlag( bool flag );

	// 裏面描画の設定 : trueで描画しない.
	void SetCullBackFlag( bool flag );

	static D3DXVECTOR2 WorldtoScreenPos( const D3DXMATRIX& mView, 
		const D3DXMATRIX& mProj, const D3DXVECTOR3& pos );

	LPD3DXMESH GetMesh(){ return m_pMeshForRay; }

	SPHERE	m_Sphere;	// スフィア構造体.

	struct CHANGE_TEXTURES
	{
		CHAR szTextureName[512];				//テクスチャファイル名.
		ID3D11ShaderResourceView*	pTexture;	//切り替えるテクスチャ.

		ID3D11ShaderResourceView*	pDefaultTexture;	//切替前のテクスチャアドレス.
		CHAR szTargetTextureName[512];					//切り替られる対象のテクスチャファイル名.
		CHANGE_TEXTURES()
			: szTextureName			()
			, pTexture				( nullptr )
			, pDefaultTexture		( nullptr )
			, szTargetTextureName	()
		{}

	};

	// 外部で読み込んだテクスチャをセットする.
	void SetChangeTextures( shared_ptr<CHANGE_TEXTURES> pCT ){ m_pChangeTextures = pCT; }
protected:
	CDX9Mesh();	// コンストラクタ.


	HRESULT Init(
		HWND hWnd,
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9,
		const char* fileName );
	
	// メッシュ読み込み.
	HRESULT LoadXMesh( const char* filename );
	// 解放関数.
	void Release();
	// ｼｪｰﾀﾞ作成.
	HRESULT InitShader();
	// ブレンド作成.
	HRESULT InitBlend();
	// ワイヤーフレーム描画用の作成.
	HRESULT InitWtire();
	// 裏面描画の設定.
	HRESULT InitCullBack();

	HRESULT InitSphere( CDX9Mesh* pMesh );

	void ChangeTextureRender( int no );

	//======================================
	//	構造体.
	//======================================
	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧのｱﾌﾟﾘ側の定義.
	//※ｼｪｰﾀﾞ内のｺﾝｽﾀﾝﾄﾊﾞｯﾌｧと一致している必要あり.
	struct CBUFFER_PER_MESH
	{
		D3DXMATRIX	mW;			//ワールド行列.
		D3DXMATRIX	mWVP;		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成変換行列.	
								//		D3DXVECTOR4 vLightDir;	//ライト方向.
								//		D3DXVECTOR4 vCamPos;	//カメラ位置(視点位置).
								//		D3DXVECTOR4 vColor;		// アルファ値(透過で使用する).

	};

	struct cbPerObject
	{
		ALIGN16 D3DXMATRIX  WVP;
		ALIGN16 D3DXMATRIX	World;
		ALIGN16 float		fAlpha;
		ALIGN16 Light		light;
	};

	struct CBUFFER_PER_MATERIAL
	{
		D3DXVECTOR4 vAmbient;	//アンビエント色(環境色).
		D3DXVECTOR4	vDiffuse;	//ディフーズ色(拡散反射色).
		D3DXVECTOR4 vSpecular;	//スペキュラ色(鏡面反射色).
								//		D3DXVECTOR4 vColor;		// アルファ値(透過で使用する).
	};
	//頂点の構造体.
	struct VERTEX
	{
		D3DXVECTOR3 Pos;	//頂点座標.
		D3DXVECTOR3 Normal;	//法線(陰影計算に必要).
		D3DXVECTOR2	Tex;	//ﾃｸｽﾁｬ座標.
	};
	// マテリアル構造体.
	struct MY_MATERIAL
	{
		D3DXVECTOR4 Ambient;	// アンビエント.
		D3DXVECTOR4 Diffuse;	// ディフーズ.
		D3DXVECTOR4 Specular;	// スペキュラ.
		char szTexTureName[64];	// テクスチャファイル名.
		ID3D11ShaderResourceView* pTexture;	// テクスチャ.
		DWORD dwNumFace;		// その他のマテリアルのポリゴン数.
								// コンストラクタ.
		MY_MATERIAL() 
			: Ambient		()
			, Diffuse		()
			, Specular		()
			, szTexTureName	()
			, pTexture		( nullptr )
			, dwNumFace		(0)
		{}
		// デストラクタ.
		~MY_MATERIAL()
		{
			SAFE_RELEASE( pTexture );
		}
	};

	HWND				m_hWnd;		// ウィンドウハンドル.
	// Dx9
	LPDIRECT3DDEVICE9	m_pDevice9;	// Dx9デバイスオブジェクト.
	
	// Dx11
	ID3D11Device*			m_pDevice11;	//ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ.
	ID3D11DeviceContext*	m_pContext11;	//ﾃﾞﾊﾞｲｽｺﾝﾃｷｽﾄ.

											//↓ﾓﾃﾞﾙの種類ごとに用意.
	ID3D11VertexShader*		m_pVertexShader;	//頂点ｼｪｰﾀﾞ.
	ID3D11InputLayout*		m_pVertexLayout;	//頂点ﾚｲｱｳﾄ.
	ID3D11PixelShader*		m_pPixelShader;		//ﾋﾟｸｾﾙｼｪｰﾀﾞ.
	ID3D11Buffer*			m_pCBufferPerMesh;		//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ(メッシュ毎).
	ID3D11Buffer*			m_pCBufferPerMaterial;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ(マテリアル毎).

	ID3D11Buffer*			m_pCBufferPerFrame;	//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ(マテリアル毎).

	ID3D11Buffer*			m_pVertexBuffer;	// 頂点バッファ.
	ID3D11Buffer**			m_ppIndexBuffer;	// インデックスバッファ.
	ID3D11SamplerState*		m_pSampleLinear;	//ｻﾝﾌﾟﾗ:ﾃｸｽﾁｬに各種ﾌｨﾙﾀをかける.

	ID3D11BlendState*		m_pAlphaBlend;		//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ有効.
	ID3D11BlendState*		m_pNoAlphaBlend;	//ｱﾙﾌｧﾌﾞﾚﾝﾄﾞ無効.
	ID3D11BlendState*		m_CoverageBlend;	//ｱﾙﾌｧﾄｩｶﾊﾞﾚｰｼﾞ有効.

	ID3D11RasterizerState*	m_RasterizerWireON;
	ID3D11RasterizerState*	m_RasterizerWireOFF;

	ID3D11RasterizerState*	m_RasterizerCullBackON;
	ID3D11RasterizerState*	m_RasterizerCullBackOFF;


	LPD3DXMESH	m_pMesh;			// メッシュオブジェクト.
	LPD3DXMESH	m_pMeshForRay;		// レイとメッシュ用.
	DWORD		m_NumMaterials;		// マテリアル数.

	MY_MATERIAL*	m_pMaterials;	// マテリアル構造体.
	DWORD			m_NumAttr;		// 属性数.
	DWORD			m_AttrID[300];	// 属性ID ※300属性まで.

	bool			m_EnableTexture;// テクスチャあり.

	shared_ptr<CHANGE_TEXTURES> m_pChangeTextures;	// マルチテクスチャ用.

private:
	Light light;

	float			m_fScale;		// 拡縮値( x, y, z等倍 ).
	D3DXVECTOR3		m_vRot;			// 回転値( x, y z ).
									// ※	 x=Pitch, y=Yaw, z=Roll.
	D3DXVECTOR3		m_vPos;			// 位置( x, y, z ).
	float			m_fAlpha;		// アルファ値.


};
#endif // #ifndef CDX9MESH_H.