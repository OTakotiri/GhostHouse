#include "FileManager.h"
#include "..\EditImGui\EditImGui.h"

#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx11.h"
#include "..\..\ImGui\imgui_impl_win32.h"

char CFileManager::m_SaveFileName[128] = "input file name";
char CFileManager::m_LoadFileName[128] = "select file name";

CFileManager::CFileManager()
	: m_StageNameList	()
	, m_isFileOperations	( false )
{
}

CFileManager::~CFileManager()
{
}

//-------------------------------.
// Imguiの描画関数.
//-------------------------------.
void CFileManager::ImGuiRender()
{
	GetStageNameList( m_StageNameList );

	if( ImGui::Button( "Stage clear" ) ) m_pTmpStage->clear();

	ImGui::InputText( "save file name", m_SaveFileName, sizeof( m_SaveFileName ) );
	if( ImGui::Button("InputFileName") ) m_isFileOperations = true;
	if( ImGui::Button( "Save" ) ){
		m_Save_hr = Save( *m_pTmpStage );

		ImGui::SameLine();

		if( m_Save_hr == S_OK ){
			ImGui::Text( "Save OK" );
		} else if( m_Save_hr == E_FAIL ){
			ImGui::Text( "Save FALE" );
		} else {
			ImGui::Text( "Save empty" );
		}
	}

	ImGui::Text(""); ImGui::Separator(); ImGui::Text("");

	ImGui::InputText( "load file name", m_LoadFileName, sizeof( m_LoadFileName ) );
	if( ImGui::Button( "Load" ) ){
		m_Load_hr = Load( *m_pTmpStage );

		ImGui::SameLine();

		if( m_Load_hr == S_OK ){
			ImGui::Text( "Save OK" );
		} else if( m_Save_hr == E_FAIL ){
			ImGui::Text( "Save FALE" );
		} else {
			ImGui::Text( "Save empty" );
		}
	}

	ImGui::SetNextTreeNodeOpen( false, ImGuiCond_::ImGuiCond_Once );
	if( ImGui::TreeNode( "Select a saved stage" ) ){
		for( const auto& s : m_StageNameList ){
			if( ImGui::Button( s.c_str() ) ){
				strcpy_s( m_LoadFileName, sizeof( m_LoadFileName ), s.c_str() );
			}
		}
		ImGui::TreePop();
	}

	KeyBoardRender();
}

//-------------------------------.
// 仮想キーボードの表示.
//-------------------------------.
void CFileManager::KeyBoardRender()
{
	if( m_isFileOperations == false ) return;
	ImGui::SetNextWindowSize( ImVec2( 300.0f, 200.0f ), ImGuiCond_::ImGuiCond_Once );
	ImGui::SetNextWindowPos( ImVec2( 100.0f, 400.0f ), ImGuiCond_::ImGuiCond_Once );
	ImGui::GetWindowSize();
	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_NoResize;
	flag |= ImGuiWindowFlags_NoMove;
	flag |= ImGuiWindowFlags_NoScrollbar;
	flag |= ImGuiWindowFlags_NoScrollWithMouse;
	flag |= ImGuiWindowFlags_NoSavedSettings;
	flag |= ImGuiWindowFlags_AlwaysAutoResize;

	bool isOpen = true;
	ImGui::Begin( "KeyBoard", &isOpen, flag );

	static bool isShiftKey = true;
	static std::string name = "";
	ImGui::Text( "%s", name.c_str() );

	const ImVec2 vSize = ImVec2( 35.0f, 35.0f );

	auto keyInput = [&]( const char* key )
	{
		char t[2] = "";
		t[0] = isShiftKey == true ? key[0]+32 : key[0];

		if( ImGui::Button(t,vSize) ) name += t;
	};
	auto keyInputNumber = [&]( const char* key )
	{
		if( ImGui::Button(key, vSize) ) name += key;
	};

	// 一段目の表示.
	keyInputNumber("1");ImGui::SameLine();
	keyInputNumber("2");ImGui::SameLine();
	keyInputNumber("3");ImGui::SameLine();
	keyInputNumber("4");ImGui::SameLine();
	keyInputNumber("5");ImGui::SameLine();
	keyInputNumber("6");ImGui::SameLine();
	keyInputNumber("7");ImGui::SameLine();
	keyInputNumber("8");ImGui::SameLine();
	keyInputNumber("9");ImGui::SameLine();
	keyInputNumber("0");ImGui::SameLine();
	if( ImGui::Button("Back\nSpace",ImVec2( 50.0f, 35.0f )) ){
		if( name.size() != 0 ){
			name.pop_back();
		}
	}

	// 二段目の表示.
	ImGui::Text(" ");ImGui::SameLine();
	keyInput("Q");ImGui::SameLine();
	keyInput("W");ImGui::SameLine();
	keyInput("E");ImGui::SameLine();
	keyInput("R");ImGui::SameLine();
	keyInput("T");ImGui::SameLine();
	keyInput("Y");ImGui::SameLine();
	keyInput("U");ImGui::SameLine();
	keyInput("I");ImGui::SameLine();
	keyInput("O");ImGui::SameLine();
	keyInput("P");

	// 三段目の表示.
	ImGui::Text(" ");ImGui::SameLine();
	ImGui::Text(" ");ImGui::SameLine();
	keyInput("A");ImGui::SameLine();
	keyInput("S");ImGui::SameLine();
	keyInput("D");ImGui::SameLine();
	keyInput("F");ImGui::SameLine();
	keyInput("G");ImGui::SameLine();
	keyInput("H");ImGui::SameLine();
	keyInput("J");ImGui::SameLine();
	keyInput("K");ImGui::SameLine();
	keyInput("L");ImGui::SameLine();
	if( ImGui::Button("Enter",ImVec2( 50.0f, 35.0f )) ){
		if( name.size() < 128 ){
			strcpy_s( m_SaveFileName, sizeof( m_SaveFileName ), name.c_str() );
			m_isFileOperations = false;
		}
	}

	// 四段目の表示.
	if( ImGui::Button("Shift",ImVec2( 50.0f, 35.0f )) ) isShiftKey = !isShiftKey; ImGui::SameLine();
	keyInput("Z");ImGui::SameLine();
	keyInput("X");ImGui::SameLine();
	keyInput("C");ImGui::SameLine();
	keyInput("V");ImGui::SameLine();
	keyInput("B");ImGui::SameLine();
	keyInput("N");ImGui::SameLine();
	keyInput("M");ImGui::SameLine();
	keyInputNumber("_");ImGui::SameLine();

	ImGui::End();
}

//-------------------------------.
// マップの保存.
//-------------------------------.
HRESULT CFileManager::Save( const std::vector<edit::RENDER_OBJRCT>& objInfo )
{
	std::string directory_path = edit::FILE_PATH + m_SaveFileName;
	int err = _mkdir( directory_path.c_str() );

	std::ofstream outFile;
	std::string filePath = directory_path + "\\" + m_SaveFileName + ".csv";
	outFile.open( filePath, std::ios::trunc );
	
	int oneLineSkip = 0;
	int Index = 1;
	for( auto obj : objInfo ){
		if( oneLineSkip == 0 ){
			outFile << "Index, " "pos_x," "pos_y," "pos_z,";
			outFile << "rot_x," "rot_y," "rot_z," "ObjName" << std::endl;
			oneLineSkip++;
		}

		outFile << Index++ << ",";
		outFile << obj.vPos.x << ",";
		outFile << obj.vPos.y << ",";
		outFile << obj.vPos.z << ",";

		outFile << obj.vRot.x << ",";
		outFile << obj.vRot.y << ",";
		outFile << obj.vRot.z << ",";

		outFile << obj.Name << std::endl;
	}
	outFile.close();

	return S_OK;
}

//-------------------------------.
// マップの読み込み.
//-------------------------------.
HRESULT CFileManager::Load( std::vector<edit::RENDER_OBJRCT>& objInfo )
{
	objInfo.clear();
	
	std::string filePath = edit::FILE_PATH + m_LoadFileName + "\\" + m_LoadFileName + ".csv";
	std::ifstream inFile( filePath );
	std::string lineString;

	int oneLineSkip = 0;
	while( std::getline( inFile, lineString ) ){
		if( oneLineSkip == 0 ){
			oneLineSkip++;
			continue;
		}
		edit::RENDER_OBJRCT tmpObj = {};
		std::istringstream stream( lineString );
		std::string field = "";
		int count = -1;
		while( std::getline( stream, field, ',' ) ){
			FieldRead( tmpObj, field, count );
			count++;
		}
		objInfo.emplace_back( tmpObj );
	}
	inFile.close();

	return S_OK;
}

//-------------------------------.
// ステージ名の取得関数.
//-------------------------------.
HRESULT CFileManager::GetStageNameList( std::vector<std::string>& stageNameList )
{
	stageNameList.clear();
	auto enumerateStage = [&]( const fs::directory_entry& entry )
	{
		stageNameList.emplace_back( entry.path().filename().string() );
	};

	try {
		fs::directory_iterator dir_it( "Data\\MapData\\" ), end_it;
		std::for_each( dir_it, end_it, enumerateStage );
	} catch( fs::filesystem_error& e ){
		std::string Message = e.what();
		_ASSERT_EXPR( false, L"ディレクトリ取得失敗" );
	}
	return S_OK;
}

//-------------------------------.
// ファイルの行の各値の取得.
//-------------------------------.
void CFileManager::FieldRead( edit::RENDER_OBJRCT& tmpObj, const std::string& field, const int& count )
{
	switch( count ){
		case 0:
			tmpObj.vPos.x = std::stof( field );
			break;
		case 1:
			tmpObj.vPos.y = std::stof( field );
			break;
		case 2:
			tmpObj.vPos.z = std::stof( field );
			break;
		case 3:
			tmpObj.vRot.x = std::stof( field );
			break;
		case 4:
			tmpObj.vRot.y = std::stof( field );
			break;
		case 5:
			tmpObj.vRot.z = std::stof( field );
			break;
		case 6:
			tmpObj.Name = field;
			break;
		default:
			break;
	}
}