#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "..\EditHeader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <filesystem>

namespace fs = std::filesystem;

class CFileManager
{
public:
	CFileManager();
	~CFileManager();

	// Imguiの描画関数.
	void ImGuiRender();
	// マップの保存.
	HRESULT Save( const std::vector<edit::RENDER_OBJRCT>& objInfo );
	// マップの読み込み.
	HRESULT Load( std::vector<edit::RENDER_OBJRCT>& objInfo );
	// ステージ名の取得関数.
	HRESULT GetStageNameList( std::vector<std::string>& stageNameList );
	// 仮ステージの設定関数.
	void SetTmpStage( std::vector<edit::RENDER_OBJRCT>* pobj ){ m_pTmpStage = pobj; }

private:
	// ファイルの行の各値の取得.
	void FieldRead( edit::RENDER_OBJRCT& tmpObj, const std::string& field, const int& count );
	// 仮想キーボードの表示.
	void KeyBoardRender();

private:
	static char m_SaveFileName[128];	// セーブ時のファイル名.
	static char m_LoadFileName[128];	// ロード時のファイル名.

	std::vector<std::string> m_StageNameList;		// ステージ名のリスト.
	std::vector<edit::RENDER_OBJRCT>*	m_pTmpStage;// 仮ステージ.

	HRESULT	m_Save_hr;	// セーブ時のハンドル.
	HRESULT	m_Load_hr;	// ロード時のハンドル.

	bool m_isFileOperations; // 仮想キーの描画フラグ.
};

#endif	// #ifndef FILE_MANAGER_H.
