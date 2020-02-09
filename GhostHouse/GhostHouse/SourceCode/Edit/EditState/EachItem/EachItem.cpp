#include "EachItem.h"

#include "..\..\DebugMode\DebugText\DebugText.h"
#include "..\..\XInput\XInput.h"

// çXêVä÷êî.
void CEachItem::Update()
{
	if( ( GetAsyncKeyState( 'W' ) & 0x0001 ) || 
		( CXInput::IsPress( XINPUT_GAMEPAD_DPAD_UP ) )){
		m_NowSelectItem--;
		if( m_NowSelectItem < edit::enSelect::bgin ) m_NowSelectItem++;
	}
	if( ( GetAsyncKeyState( 'S' ) & 0x0001 ) || 
		( CXInput::IsPress( XINPUT_GAMEPAD_DPAD_DOWN ) )){
		m_NowSelectItem++;
		if( m_NowSelectItem > edit::enSelect::end ) m_NowSelectItem--;
	}
}

// ï`âÊä÷êî.
void CEachItem::Render()
{
	EachItemNameListRender();
	SelectingCursorRender();
}

void CEachItem::EachItemNameListRender()
{
	D3DXVECTOR3 vPos = 
	{ edit::RENDER_POSITION_X, edit::RENDER_POSITION_Y, 0.0f };
	for( auto& n : edit::EachItemNameList ){
		CDebugText::SetPosition(vPos);
		CDebugText::Render(n);
		vPos.y += CDebugText::GetScale();
	}
}

void CEachItem::SelectingCursorRender()
{
	D3DXVECTOR3 vPos = 
	{
		edit::RENDER_POSITION_X - CDebugText::GetScale(),
		edit::RENDER_POSITION_Y +(CDebugText::GetScale())* static_cast<float>(m_NowSelectItem-1),
		0.0f 
	};
	CDebugText::SetPosition( vPos );
	CDebugText::Render( ">" );
}