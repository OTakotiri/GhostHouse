#include "EachObject.h"
#include "..\..\ModelResource\ModelResource.h"
#include "..\..\DebugMode\DebugText\DebugText.h"
#include "..\..\XInput\XInput.h"

CEachObject::CEachObject()
	: m_BackGroundNameList()
	, m_GimmickNameList()
	, m_PlayerNameList()
	, m_GhostNameList()
	, m_ObjectList()
	, m_SelectObjectList()
	, m_SelectObjectNunber( 0 )
	, m_NowSelectItem( 0 )
	, m_OldSelectItem( 0 )
{
	m_ObjectList.emplace_back( &m_BackGroundNameList );
	m_ObjectList.emplace_back( &m_PlayerNameList );
	m_ObjectList.emplace_back( &m_GhostNameList );
	m_ObjectList.emplace_back( &m_GimmickNameList );
}

CEachObject::~CEachObject()
{
	m_SelectObjectList.clear();
	m_ObjectList.clear();
	m_PlayerNameList.clear();
	m_BackGroundNameList.clear();
	m_GimmickNameList.clear();
	m_GhostNameList.clear();
}

void CEachObject::Update()
{
	if( m_SelectObjectList.empty() == true ) return;

	if( ( GetAsyncKeyState( 'W' ) & 0x0001 ) || 
		( CXInput::IsPress( XINPUT_GAMEPAD_DPAD_UP ) )){
		m_SelectObjectNunber--;
		if( m_SelectObjectNunber < 0 ) m_SelectObjectNunber++;
	}
	if( ( GetAsyncKeyState( 'S' ) & 0x0001 ) || 
		( CXInput::IsPress( XINPUT_GAMEPAD_DPAD_DOWN ) )){
		m_SelectObjectNunber++;
		if( m_SelectObjectNunber >= (int)m_SelectObjectList.size() ) m_SelectObjectNunber--;
	}
}

void CEachObject::Render()
{
	EachObjectNameListRender();
	SelectingCursorRender();
}

void CEachObject::SetSelectItem( const int& item )
{
	if( item >= (int)m_ObjectList.size() ){
		m_SelectObjectList.clear();
		return;
	}
	SelectObjectNunberReset( item );
	m_SelectObjectList = *m_ObjectList[item];
}

void CEachObject::EachObjectNameListRender()
{
	if( m_SelectObjectList.empty() == true ) return;

	D3DXVECTOR3 vPos = 
	{ 
		( edit::RENDER_POSITION_X * 6.0f ), 
		edit::RENDER_POSITION_Y, 
		0.0f 
	};
	for( auto& list : m_SelectObjectList ){
		CDebugText::SetPosition(vPos);
		CDebugText::Render(list);
		vPos.y += CDebugText::GetScale();
	}
}

void CEachObject::SelectingCursorRender()
{
	if( m_SelectObjectList.empty() == true ) return;

	D3DXVECTOR3 vPos = 
	{
		( edit::RENDER_POSITION_X * 6.0f ) - CDebugText::GetScale(),
		edit::RENDER_POSITION_Y + (CDebugText::GetScale())* static_cast<float>(m_SelectObjectNunber),
		0.0f 
	};
	CDebugText::SetPosition( vPos );
	CDebugText::Render( ">" );
}

void CEachObject::ObjectNameSetting()
{
	m_GhostNameList			= CEditModelResource::GetGhostNameList();
	m_BackGroundNameList	= CEditModelResource::GetBackGroundNameList();
	m_GimmickNameList		= CEditModelResource::GetGimmickNameList();
	m_PlayerNameList		= CEditModelResource::GetGPlayerNameList();
}

void CEachObject::SelectObjectNunberReset( const int& item )
{
	m_NowSelectItem = item;
	if( m_NowSelectItem != m_OldSelectItem ){
		m_SelectObjectNunber = 0;
		m_OldSelectItem = item;
	}
}