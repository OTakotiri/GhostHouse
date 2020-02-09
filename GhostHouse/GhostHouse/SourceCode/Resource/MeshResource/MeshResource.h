#ifndef MESH_RESORCE_H
#define MESH_RESORCE_H

//警告についてのｺｰﾄﾞ分析を無効にする. 4005:再定義.
#pragma warning( disable:4005)

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <filesystem>	// C++17 必須.

#include <D3DX11.h>
#include <D3D11.h>
#include <d3dx9.h>

#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include "..\ModelResource\DX9Mesh\CDX9Mesh.h"
#include "..\ModelResource\SkinMesh\CDX9SkinMesh.h"

namespace fs = std::filesystem;

class CMeshResorce
{
	const char* FILE_PATH = "Data\\Model";

	typedef std::unordered_map<std::string, std::shared_ptr<CDX9Mesh>>		static_mesh_map;
	typedef std::unordered_map<std::string, std::shared_ptr<CDX9SkinMesh>>	skin_mesh_map;

public:
	CMeshResorce();
	~CMeshResorce();

	// インスタンスの取得.
	static CMeshResorce* GetInstance();

	// モデルの読み込み.
	static void Load( 
		HWND hWnd, 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9 );
	// スレッドの解放.
	static bool ThreadRelease();

	// スタティックメッシュの読み込み.
	static std::shared_ptr<CDX9Mesh> GetStatic( const std::string& name );
	static std::shared_ptr<CDX9SkinMesh> GetSkin( const std::string& name );

	// スキンメッシュの読み込み.
	static bool GetStatic( std::shared_ptr<CDX9Mesh>& model, const std::string& name );
	static bool GetSkin( std::shared_ptr<CDX9SkinMesh>& model, const std::string& name );

private:
	// モデルの読み込み.
	void ModelLoad( 
		HWND hWnd, 
		ID3D11Device* pDevice11, 
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9 );

private:
	static_mesh_map m_staticMeshList;	// スタティックメッシュのリスト.
	skin_mesh_map	m_skinMeshList;		// スキンメッシュのリスト.

	std::thread		m_thread;	// スレッド.

	bool	m_isLoadEnd;		// ロードが終了したか.
	bool	m_isThreadJoined;	// スレッドが解放されたか.

private:
	// コピー・ムーブコンストラクタ, 代入演算子の削除.
	CMeshResorce( const CMeshResorce & )				= delete;
	CMeshResorce& operator = ( const CMeshResorce & )	= delete;
	CMeshResorce( CMeshResorce && )						= delete;
	CMeshResorce& operator = ( CMeshResorce && )		= delete;
};

#endif	// #ifndef MESH_RESORCE_H.