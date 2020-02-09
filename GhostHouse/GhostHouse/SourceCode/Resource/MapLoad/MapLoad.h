#ifndef MAP_LOAD_H
#define MAP_LOAD_H

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <unordered_map>

#include "..\..\SingletonBase\SingletonBase.h"
#include "..\..\Resource\SpriteResouce\Sprite\CSpriteUI.h"
#include "..\..\ObjectBase\ObjectBase.h"

using namespace std;

class CMapLoad : public CSingletonBase<CMapLoad>
{
	typedef function<shared_ptr<CObjectBase>(const stObjectInfo&)> Init_func;
private:
	enum class enLine : int
	{
		None,

		Index,		// インデックス.
		xPosition,	// x 座標.
		yPosition,	// y 座標.
		zPosition,	// z 座標.

		xRotation,	// x 座標.
		yRotation,	// y 座標.
		zRotation,	// z 座標.

		TipNo,		// チップナンバー.

		Max,
	};

public:
	CMapLoad();
	virtual ~CMapLoad();

	// ファイルの読み込み.
	static void CreateStage( vector<shared_ptr<CObjectBase>> &pvObj );
 	static void FileRead( vector<shared_ptr<CObjectBase>> &pvObj, string fileName );

	// 各ステージ名の読み込み.
	static void StageNameListLoad( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );
	// スレッドの解放.
	static bool ThreadRelease();

	// ステージ名取得関数.
	static string GetStageName( int i ){ return GetInstance()->m_mStageInfoDataList.at(i).first; }
	static shared_ptr<CSpriteUI> GetStageImage( int i ){ return GetInstance()->m_mStageInfoDataList.at(i).second; }
	static pair<string, shared_ptr<CSpriteUI>> GetStageInfoData( int i ){ return GetInstance()->m_mStageInfoDataList.at(i); }
	static void SetStageNum( int i ){ GetInstance()->m_NowStageNum = i; }
	static int GetStageNum(){ return GetInstance()->m_NowStageNum; }
	// ステージの最大数取得関数.
	static int GetMAXStageNumber(){ return GetInstance()->m_mStageInfoDataList.size(); }

	static void SetStageName( string sFileName ){ GetInstance()->m_sFileName = sFileName; }

private:
	// ステージ名検索関数.
	void StageNameListSearch( ID3D11Device* pDevice11,
		ID3D11DeviceContext* pContext11 );

private:
	// ステージ名.
	string			m_sFileName;
	// ステージ名のリスト.
	vector<string>	m_vsStageNameList;
	// ステージの　名前と画像を取得.
	map<int, pair<string, shared_ptr<CSpriteUI>>>	m_mStageInfoDataList;

	unordered_map<string, Init_func> CreateObjList;

	int m_NowStageNum;

	std::thread		m_thread;	// スレッド.

	bool	m_isLoadEnd;		// ロードが終了したか.
	bool	m_isThreadJoined;	// スレッドが解放されたか.
};

#endif	// #ifndef MAP_LOAD_H.