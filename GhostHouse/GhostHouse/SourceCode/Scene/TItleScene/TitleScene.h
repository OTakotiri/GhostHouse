#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H

#include "..\SceneBase\SceneBase.h"
#include "..\GameScene\GameScene.h"
#include "..\StageSelect\StageSelect.h"

#include "..\..\UI\TitleUI\TitleUI.h"
#include "..\..\UI\Fade\FadeUI.h"

#include "..\..\Resource\SpriteResouce\SpriteResouce.h"
#include "..\..\Object\Stage\Stage.h"
#include "..\..\Camera\Camera.h"
#include "..\..\XInput\XInput.h"

#include "..\..\DebugMode\DebugText\DebugText.h"

#include <thread>

class CTitleScene : public CSceneBase
{
private:
	// ゲームパッドのスティックの傾きの遊び.
	int STICK_IDLING	= 32500;

public:
	CTitleScene( shared_ptr<CSceneManager> sceneManager );
	virtual ~CTitleScene();

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

	// ステージ.
	unique_ptr<CStage>		m_pSatage;
	// カメラ.
	unique_ptr<CCamera>		m_pCamera;
	// タイトルUI.
	unique_ptr<CTitleUI>	m_pTitleUI;

	bool m_isLoadEnd;	// 読み込みの終了フラグ.
	bool m_isNextScene;	// 次のシーンへのフラグ.
	bool m_isEditNextScene;

	bool m_isChangeScene;	// 次のシーンへ実際移動する許可のフラグ.
	bool m_isEditChangeScene;

	bool m_bPushDownKey;
	bool m_bPushUpKey;

	bool	m_isEnd;			//スレッドに対して解放許可を出すフラグ.
	std::thread BGMThread;		//BGMを鳴らすスレッド.
	std::thread::id InThreadID;	//BGMスレッドID退避.

	DWORD ThreadExitCode;
	bool	m_bThreadRelease;
};

#endif // #ifndef TITLE_SCENE_H.