#ifndef MAIN_H
#define MAIN_H

#include "MyMacro.h"
#include "Global.h"

// シーンクラス.
#include "..\Scene\SceneManager\SceneManager.h"
#include "..\Scene\TItleScene\TitleScene.h"

#include "..\DebugMode\DebugText\DebugText.h"

//=================================================
//	定数.
//=================================================
const char WND_TITLE[] = "オバケノイエ";
const char APP_NAME[]  = "GhostHouse";


/**************************************************
*	メインクラス.
**/
class CMain
{
public:
	CMain();	// コンストラクタ.
	~CMain();	// デストラクタ.

	// ウィンドウ初期化関数.
	HRESULT InitWindow(
		HINSTANCE hInstance,
		INT x, INT y, INT width, INT height,
		const char* WindowName );

	// ウィンドウ関数(メッセージ毎の処理).
	LRESULT MsgProc(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam);

	HRESULT Init();
	void Destroy();

	// メッセージループ.
	void Loop();
	// アプリーケーション処理.
	void AppMain();

//----------------------------
// ここから3D関係.
	// DirectX初期化.
	HRESULT InitD3D11();
	HRESULT InitDx9();

//----------------------------

private:
	HWND	m_hWnd;	// ウィンドウハンドル.

	// 更新関数.
	void Update();
	// 描画関数.
	void Render();

	// バックバッファクリア関数.
	void ClearBackBuffer();
	// カメラ関数.
	void Camera();
	// プロジェクション関数.
	void Proj();
	// カメラコントロール.
	void CameraControl();

//----------------------------
// ここから3D関係.
	//======================================
	//	構造体.
	//======================================
	// カメラ情報.
	

	// メッシュの読み込み関数.
	HRESULT LoadMesh();

	// 深度(Z)テストON/OFF切替え.
	void SetDeprh( bool flag );

	//↓アプリにひとつ.
	ID3D11Device*			m_pDevice11;	// デバイスオブジェクト.
	ID3D11DeviceContext*	m_pContext11;	// デバイスコンテキスト.
	LPDIRECT3DDEVICE9		m_pDevice9;		// Dx9ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄ.
	IDXGISwapChain*			m_pSwapChain;	// スワップチェーン.
	ID3D11RenderTargetView*	m_pBackBuffer_TexRTV;	// レンダーターゲットビュー.
	ID3D11Texture2D*		m_pBackBuffer_DSTex;	// デプスステンシル用テクスチャ.
	ID3D11DepthStencilView*	m_pBackBuffer_DSTexDSV;	// デプスステンシルビュー.
	
	// 深度(z)テスト設定.
	ID3D11DepthStencilState*	m_pDepthStencilState;
	ID3D11DepthStencilState*	m_pDepthStencilStateOff;

	stCAMERA		m_Camera;	// カメラ位置.
	D3DXVECTOR3		m_vLight;	// ライトの方向.
	D3DXMATRIX		m_mView;	// ビュー行列.
	D3DXMATRIX		m_mProj;	// プロジェクション行列.

	Light			m_Light;
	
//----------------------------

	// シーン管理クラス.
	shared_ptr<CSceneManager> pSceneManager;
	bool	m_isLoadCheck;
};

#endif//#ifndef MAIN_H
