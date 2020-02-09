#ifndef STAGE_H
#define STAGE_H

#include "..\..\Resource\MapLoad\MapLoad.h"
#include "..\Gimmick\Signboard\Signboard.h"
#include "..\..\DebugMode\DebugText\DebugText.h"
#include "..\..\Object\Character\Player\Manager\PlayerManager.h"
#include "..\..\UI\Tutorial\TutorialText.h"
#include "..\..\Object\ObjectManager\ObjectManager.h"

class CStage
{
public:
	enum class MapScene
	{
		None = -1,

		In_the_forest = 0,		// 森の中.
		In_front_house,			// 家の前.
		In_the_house_F1,		// 家の中.
		In_the_house_F2,		// 家の中.
		In_the_house_B1_Area1,	// 家の地下.
		In_the_house_B1_Area2,	// 家の地下エリア2.
		In_the_house_F1_end,	// 家の中.

		Max,
	};

	const float WINDOW_OUT_SIZE = 200.0f;
public:
	CStage();
	~CStage();

	// 読み込み関数.
	void Load(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11);
	// 更新関数.
	void Updata();
	// 描画関数.
	void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera);

	// コンテニューの際ステージのリセット.
	void ContinueStageReset();

	// 描画可能か.
	bool isRender();
	// ゴール判定関数.
	bool isGaol();
	// サヤカやられ判定関数.
	bool isSayakaDead();
	// サヤカのHPが0ならtrue.
	bool isSayakaLifePointZero();

	// ポーズしているかどうか設定.
	void SetPause(const bool& isPause) { m_isPause = isPause; }

	D3DXVECTOR3 GetPlayerPosition()
	{
		if (m_pPlayerManager->GetSayaka() == nullptr) return { 0.0f, 0.0f, 0.0f };
		return m_pPlayerManager->GetSayaka()->GetPosition();
	}

	void SetCameraOFF()
	{
		if (m_pPlayerManager->GetSayaka() == nullptr) return;
		m_pPlayerManager->GetSayaka()->BitFlagOFF(BitFlag::isStopCamera);
	}
private:
	// マップの読み込み.
	bool LoadMap();
	// マップオブジェクトの読み込み.
	void LoadMapObject(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11);
	// プレイヤーの作成.
	void PlayerInit(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11);
	// 次のマップに変更.
	void ChangeNextMap();

	// タイトル時のライト座標の初期化.
	void InitLightPosition(Light& stLight);

	// 背景の描画.
	void BackGroundRender(D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera);

	// テキストを次に進める当たり判定の個数,位置を特定.
	void GetNextTextColPosition();
	// テキストを表示.
	void DispText(shared_ptr<CObjectBase> pObj);
	// テキストを次の文に変更.
	void ChangeText();

	// 三次元座標の二点間距離を測る.
	float GetTwoDistanceXYZ(const D3DXVECTOR3 & vMyPos, const D3DXVECTOR3 & vtargetPos);


	void InitTextNum(int Num);
private:
	int init_Map;
	int m_MapSize;
	bool m_isLightSet;
	bool m_bOneSound;
	bool m_bOneCall;
	bool m_bStartGame;
	bool m_isPause;
	bool m_isRender;
	bool m_isStopCamera;

	size_t		m_TextStartNum;
	size_t		m_TextNum;
	vector<int>	m_OldTextNum;
	vector<int>	m_vTextColNum;

	MapScene m_NowMapScene;

	shared_ptr<CDX9Mesh>			m_pBackGround;
	unique_ptr<CPlayerManager>		m_pPlayerManager;
	unique_ptr<CObjectManager>		m_pObjectManager;
	vector<shared_ptr<CObjectBase>> m_pMap;
	vector<vector<shared_ptr<CObjectBase>>>	m_pMapList;		// マップ.
	vector<D3DXVECTOR3>				m_vTextColPostion;

	unique_ptr<CMultiTexture>		m_pPaper;
	unique_ptr<CTutorialText>		m_pText;
};

#endif	// #ifndef STAGE_H.