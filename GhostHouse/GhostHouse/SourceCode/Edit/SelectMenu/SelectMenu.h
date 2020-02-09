#ifndef SELECT_MENU_H
#define SELECT_MENU_H

#include "..\EditHeader.h"

class CFileManager;

class CSelectMenu
{
	typedef std::vector<std::string> ObjectName;
public:
	CSelectMenu();
	~CSelectMenu();

	// 描画関数.
	void Render();

	// オブジェクト名の取得.
	std::string GetObjectName() const { return m_SelectOjectName; }
	// メニューの表示に戻る.
	void BackMenu(){ m_isChoice = true; }
	// 仮ステージの設定関数.
	void SetTmpStage( std::vector<edit::RENDER_OBJRCT>* pobj );
	// 現在オブジェクト選択状態か.
	bool IsChoice() const { return m_isChoice; }

private:
	// ウィンドウの設定.
	void SettingWindow();
	// オブジェクトのタブの表示.
	void ObjectTabRender();
	// オブジェクトリストの表示.
	bool ObjectListRender( const std::string& objectListName, const int& index );
	// オブジェクトの選択.
	bool SelectObject( const std::string& objectName );
	// オブジェクト名のリストを取得.
	void GetObjectListName();

private:
	// 各オブジェクト名のリスト.
	ObjectName	m_BackGroundNameList;
	ObjectName	m_GimmickNameList;
	ObjectName	m_PlayerNameList;
	ObjectName	m_GhostNameList;

	std::vector<ObjectName*> m_ObjectList;	// 上のオブジェクト名を保持するためのリスト.

	std::unique_ptr<CFileManager>	m_pFileManager;

	std::string m_SelectOjectName;	// 選択オブジェクト名.
	bool m_isChoice;
};

#endif	// #ifndef SELECT_MENU_H.