#include "ModelResource.h"


void CEditModelResource::Load( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
{
	GetInstance()->ModelLoad( hWnd, pDevice11, pContext11, pDevice9 );
}

shared_ptr<CDX9Mesh> CEditModelResource::GetMesh( const std::string& modelName )
{
	for( auto& m : GetInstance()->m_pMeshList ){
		if( m.first == modelName ) return m.second;
	}
	return nullptr;
}

void CEditModelResource::ModelLoad( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
{
	auto threadload = [&]( HWND hWnd, ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
	{
		auto modelSearch = [&]( const fs::directory_entry& entry )
		{
			string exe = entry.path().extension().string();
			string filepath = entry.path().string();
			string filename = entry.path().stem().string();

			if( exe != ".X" && exe != ".x" ) return;
			if( SameNameElimination(filename) == true ) return;

			ObjectNamePush( filepath, filename );
			m_pMeshList[filename] = make_shared<CDX9Mesh>( hWnd, pDevice11, pContext11, pDevice9, filepath.c_str() );
		};

		try {
			fs::recursive_directory_iterator dir_it(MODEL_PAHT), end_it;
			std::for_each(dir_it, end_it, modelSearch );

		} catch( fs::filesystem_error& e ) {
			std::string Message = e.what();
			_ASSERT_EXPR(false, L"ディレクトリ取得失敗");
		}

		m_isLoadBegan = true;
	};
	int i = thread::hardware_concurrency();
	m_pThread = thread( threadload, hWnd, pDevice11, pContext11, pDevice9 );
}

bool CEditModelResource::ThreadRelease()
{
	if( GetInstance()->m_isThreadUnlocked	== true )	return true;
	if( GetInstance()->m_isLoadBegan		== false )	return false;

	GetInstance()->m_pThread.join();
	GetInstance()->m_isThreadUnlocked = true;
	return true;
}

void CEditModelResource::ObjectNamePush( const string& filePaath, const string& objName )
{
	auto filenameFind = []( const string& filepath, const string& name ) -> bool
	{
		return filepath.find(name) != string::npos ? true : false;
	};

	if( filenameFind( filePaath, "Ghost" ) == true ){
		m_GhostNameList.emplace_back(objName);

	} else if( filenameFind( filePaath, "Gimmick" ) == true ){
		m_GimmickNameList.emplace_back(objName);

	} else if( filenameFind( filePaath, "BackGround" ) == true ){
		m_BackGroundNameList.emplace_back(objName);

	} else if( filenameFind( filePaath, "Player" ) == true ){
		m_PlayerNameList.emplace_back(objName);
	}
}

bool CEditModelResource::SameNameElimination( const string& objName )
{
	auto nameSearch = [&objName]( const vector<string>& nameList ) -> bool
	{
		for( auto& n : nameList ){
			if( n == objName ) return true;
		}
		return false;
	};

	if( nameSearch(m_GhostNameList)		== true ) return true;
	if( nameSearch(m_GimmickNameList)	== true ) return true;
	if( nameSearch(m_BackGroundNameList)== true ) return true;
	if( nameSearch(m_PlayerNameList)	== true ) return true;
	return false;
}