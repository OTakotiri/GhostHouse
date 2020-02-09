#ifndef EACH_OBJECT_H
#define EACH_OBJECT_H

#include "..\..\EditHeader.h"

class CEachObject
{
	typedef std::vector<std::string> ObjectName;
public:
	CEachObject();
	~CEachObject();

	void Update();
	void Render();

	void SetSelectItem( const int& item );
	void ObjectNameSetting();

	std::string GetObjectName() const { return m_SelectObjectList[m_SelectObjectNunber]; }

private:
	void EachObjectNameListRender();
	void SelectingCursorRender();
	void SelectObjectNunberReset( const int& item );

private:
	ObjectName	m_BackGroundNameList;
	ObjectName	m_GimmickNameList;
	ObjectName	m_PlayerNameList;
	ObjectName	m_GhostNameList;

	std::vector<ObjectName*> m_ObjectList;
	ObjectName	m_SelectObjectList;

	int			m_SelectObjectNunber;
	edit::SelectItem	m_NowSelectItem;
	edit::SelectItem	m_OldSelectItem;
};

#endif	// #ifndef EACH_OBJECT_H.