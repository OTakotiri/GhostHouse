#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "..\SceneBase\SceneBase.h"

using namespace std;

struct Light;
struct stCAMERA;

class CSceneBase;

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	
	// 読み込み関数.
	void Load( HWND hWnd, ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 );
	void LoadRender( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera );
	// 更新関数.
	void Update();
	// 描画関数.
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera );

	// シーンをスタックに入れる.
	void Push( shared_ptr<CSceneBase> pScenebase );
	// シーンをスタックから取り出す.
	void Pop();
	// シーンをスタックから二つ取り出す.
	void TwoPop();
	// シーンをスタックの一番上と入れ替える.
	void Change( shared_ptr<CSceneBase> pScenebase );
	// シーンスタックを入れかえる.
	void Swap( shared_ptr<CSceneBase> pScenebase );
	// シーンスタックの解放.
	void StackRelease();

private:
	stack<shared_ptr<CSceneBase>> m_pStackScene;
};

#endif // #ifndef SCENE_MANAGER_H.