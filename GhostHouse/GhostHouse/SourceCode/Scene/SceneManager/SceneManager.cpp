#include "SceneManager.h"

CSceneManager::CSceneManager()
{
}
CSceneManager::~CSceneManager()
{
}

void CSceneManager::Load( HWND hWnd, ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 )
{
	m_pStackScene.top()->Load( hWnd, pDevice11, pContext11, pDevice9 );
}

void CSceneManager::LoadRender( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	m_pStackScene.top()->Render( mView, mProj, stLight, stCamera );
}

void CSceneManager::Update()
{
	m_pStackScene.top()->Update();
}

void CSceneManager::Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
	Light& stLight, stCAMERA& stCamera )
{
	m_pStackScene.top()->Render( mView, mProj, stLight, stCamera );
}


void CSceneManager::Push( shared_ptr<CSceneBase> pScenebase )
{
	if( pScenebase == nullptr ) return;
	m_pStackScene.push( pScenebase );
}

void CSceneManager::Pop()
{
	m_pStackScene.pop();
}
void CSceneManager::TwoPop()
{
	m_pStackScene.pop();
	m_pStackScene.pop();
}

void CSceneManager::Change( shared_ptr<CSceneBase> pScenebase )
{
	m_pStackScene.pop();
	m_pStackScene.push( pScenebase );
}

void CSceneManager::Swap( shared_ptr<CSceneBase> pScenebase )
{
	stack<shared_ptr<CSceneBase>> temp;
	temp.push( pScenebase );
	m_pStackScene.swap( temp );
}

void CSceneManager::StackRelease()
{
	size_t size_max = m_pStackScene.size();
	for( size_t i = 0; i < size_max; i++  ){
		m_pStackScene.pop();
	}
}
