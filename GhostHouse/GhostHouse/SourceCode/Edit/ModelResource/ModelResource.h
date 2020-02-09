#ifndef MODEL_RESOURCE_H
#define MODEL_RESOURCE_H

#include "..\..\SingletonBase\SingletonBase.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"

#include <filesystem>
#include <thread>
#include <unordered_map>

class CDX9Mesh;

namespace fs = std::filesystem;

class CEditModelResource : public std::CSingletonBase<CEditModelResource>
{
	// モデルリストCSVのファイルパス.
	const char* MODEL_PAHT = "Data\\Model";	
	typedef std::unordered_map<std::string, std::shared_ptr<CDX9Mesh>> String_CDX9Mesh_Map;

public:
	CEditModelResource()
		: m_pMeshList			()
		, m_GhostNameList		()
		, m_GimmickNameList		()
		, m_BackGroundNameList	()
		, m_PlayerNameList		()
		, m_pThread				()
		, m_isLoadBegan			( false )
		, m_isThreadUnlocked	( false )
	{}

	~CEditModelResource()
	{
		m_PlayerNameList.clear();
		m_BackGroundNameList.clear();
		m_GimmickNameList.clear();
		m_GhostNameList.clear();
		m_pMeshList.clear();
	}

	// モデル読み込み関数.
	static void Load( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );
	// モデル取得関数.
	static shared_ptr<CDX9Mesh>	GetMesh( const string& modelName );
	// スレッドの解放.
	static bool ThreadRelease();

	static vector<string> GetGhostNameList()		{ return GetInstance()->m_GhostNameList; }
	static vector<string> GetGimmickNameList()		{ return GetInstance()->m_GimmickNameList; }
	static vector<string> GetBackGroundNameList()	{ return GetInstance()->m_BackGroundNameList; }
	static vector<string> GetGPlayerNameList()		{ return GetInstance()->m_PlayerNameList; }

private:
	// モデルの読み込み.
	void ModelLoad( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );
	// オブジェクト名追加.
	void ObjectNamePush( const string& filePaath, const string& objName );
	// 同じオブジェクト名の追加を除外する.
	bool SameNameElimination( const string& objName );


private:
	// メッシュクラス.
	String_CDX9Mesh_Map	m_pMeshList;	
	
	vector<string>	m_GhostNameList;
	vector<string>	m_GimmickNameList;
	vector<string>	m_BackGroundNameList;
	vector<string>	m_PlayerNameList;
	
	thread			m_pThread;

	bool			m_isLoadBegan;
	bool			m_isThreadUnlocked;

private:
	CEditModelResource( const CEditModelResource & )				= delete;
	CEditModelResource& operator = ( const CEditModelResource & )	= delete;
	CEditModelResource( CEditModelResource && )						= delete;
	CEditModelResource& operator = ( CEditModelResource && )		= delete;
};

#endif	// #ifndef MODEL_RESOURCE_H.