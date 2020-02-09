#ifndef EDIT_IMGUI_H
#define EDIT_IMGUI_H

#include "..\EditHeader.h"

class CFileManager;

class CEditImGui
{
	const float IMGUI_WND_W = 350.0f;
	const float IMGUI_WND_H = 300.0f;
	const float IMGUI_WND_X = 850.0f;
	const float IMGUI_WND_Y = 350.0f;
public:
	CEditImGui();
	~CEditImGui();

	// 描画関数.
	void Render( bool& isEditEnd );
	// オブジェクトのカウント数設定.
	void SetObjectCount( const int& size ){ m_ObjectCount = size; }

private:
	// 操作説明描画関数.
	void ManipulationDescriptionRender( bool& isEditEnd );
	// エディット終了ウィンドウ描画関数.
	void EditEndPopRender( bool& isEditEnd );

private:
	int m_ObjectCount;	// オブジェクト数.

	std::vector<std::string> m_pStageNameList;	// ステージ名リスト.

	bool m_isEndPopRender;	// 終了ウィンドウ表示フラグ.
};

#endif	// #ifndef EDIT_IMGUI_H.