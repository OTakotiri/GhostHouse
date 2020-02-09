#ifndef GAME_END_SCENE_H
#define GAME_END_SCENE_H

#include "..\SceneBase\SceneBase.h"
#include "..\..\Resource\SpriteResouce\SpriteResouce.h"
#include <thread>

class CGameEndScene : public CSceneBase
{
private:
	// ゲームパッドのスティックの傾きの遊び.
	int STICK_IDLING = 32500;

public:
	CGameEndScene( shared_ptr<CSceneManager> sceneManager );
	virtual ~CGameEndScene();

	void Load( HWND hWnd, ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11, LPDIRECT3DDEVICE9 pDevice9 ) override;
	void Update() override;
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera ) override;

private:
	// 次のシーンへの処理関数.
	void NextScene();

	bool Release();
private:
	// ウィンドウハンドル.
	HWND		m_hWnd;
	std::shared_ptr<CSpriteUI> m_pGameEndImage;
	std::shared_ptr<CSpriteUI> m_pGameEndText;

	bool m_isLoadEnd;	// 読み込みの終了フラグ.
	bool m_isNextScene;	// 次のシーンへのフラグ.

	bool m_isChangeScene;	// 次のシーンへ実際移動する許可のフラグ.

	bool m_bPushDownKey;
	bool m_bPushUpKey;

	float m_TextAlpha;
	float m_AlphaCount;

	bool	m_isEnd;			//スレッドに対して解放許可を出すフラグ.
	std::thread BGMThread;		//BGMを鳴らすスレッド.
	std::thread::id InThreadID;	//BGMスレッドID退避.

	DWORD ThreadExitCode;
	bool	m_bThreadRelease;
};

#endif	// #ifndef GAME_END_SCENE_H.