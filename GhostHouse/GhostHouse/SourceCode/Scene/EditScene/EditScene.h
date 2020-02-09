#ifndef EDIT_SCENE_H
#define EDIT_SCENE_H

#include "..\SceneBase\SceneBase.h"
#include "..\..\Edit\Edit.h"

class CEditScene : public CSceneBase
{
public:
	CEditScene( shared_ptr<CSceneManager> sceneManager );
	~CEditScene();

	// 読込処理関数.
	void Load( 
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11,
		LPDIRECT3DDEVICE9 pDevice9 ) override;
	// 更新処理関数.
	void Update() override;
	// 描画処理関数.
	void Render( 
		D3DXMATRIX& mView, 
		D3DXMATRIX& mProj,
		Light& stLight, 
		stCAMERA& stCamera ) override;
private:
	// ImGuiの作成関数.
	void ImGuiCreate(
		HWND hWnd, 
		ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );
	// ライトの設定.
	void SetingLight( Light& stLight );
	// シーン切り替え関数.
	void ChangeScene();

private:
	unique_ptr<CEdit> m_pEdit;	// エディットクラス.
	bool m_isImGuiCreate;		// ImGuiを作成したか.
	bool m_isLightSet;			// ライトを設定したか.
};

#endif // #ifndef EDIT_SCENE_H.