#ifndef SCENE_BASE_H
#define SCENE_BASE_H

// 警告についてのコード分析を無効にする 4005:再定義.
#pragma warning( disable:4005 )
#pragma warning(disable: 4819)

#include <Windows.h>
#include <string>
#include <stack>
#include <memory>

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

#include "..\SceneManager\SceneManager.h"

using namespace std;

struct Light;
struct stCAMERA;

class CSceneManager;

class CSceneBase
{
public:
	CSceneBase( shared_ptr<CSceneManager> sceneManager );
	virtual ~CSceneBase(){};
	
	// 読み込み関数.
	virtual void Load( HWND hWnd, ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 ) = 0;
	// 更新関数.
	virtual void Update() = 0;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera ) = 0;

protected:
	shared_ptr<CSceneManager> m_pSceneManager;
};

#endif // #ifndef SCENE_BASE_H.