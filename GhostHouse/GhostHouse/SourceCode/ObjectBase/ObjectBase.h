#ifndef OBJECT_BASE_H
#define OBJECT_BASE_H

#include "..\Main\Global.h"
#include "..\Resource\SpriteResouce\Sprite\CSpriteUI.h"
#include "..\Resource\SpriteResouce\SpriteResouce.h"

#include "..\Collision\CollisionManager\CollisionManager.h"
#include <unordered_map>
#include <functional>

struct stCAMERA;

// オブジェクトの種類.
enum class enObjectNo
{
	None,

	Signboard,			// 看板.
	Key,				// 鍵.
	// 背景オブジェクト.
	Ground,		// 地面.
	Ground_I,	
	Ground_L,	
	Flor,
	Flor_I_one,
	Flor_I,
	Flor_L_one,
	Flor_L,
	Wall,		// 壁.
	Wall_out,
	Pillar,
	Entrance,
	Entrance_close,
	Roof,
	Roof_L,
	Roof2_I,
	Roof2_L,
	OpenDoor,
	Tree,

	Event,
	Goal,
	TextEvent,
	InvisibleWall,
	InvisibleBox,
	// プレイヤー.
	Sayaka,			// サヤカ.
	Towa,			// トワ.

	// ゴースト.
	GhostA,			// ゴーストA.
	GhostB,			// ゴーストB.
	BigGhost,		// ビッグゴースト.

	// ギミック.
	BookShelf,			// 本棚.
	InvisibleFloor,		// 見えない床.
	SlipThroughFloor,	// 見えない床のすり抜けバージョン.
	InvisibleStartFloor,// 見えない床スタート用.
	InvisibleEndFloor,	// 見えない床エンド用.
	KeyDoor,			// 鍵で開くドア.
	Pitfall,			// 落とし穴.
	CameraUpCollision,	// 見えない床接近時、カメラを上にあげる不可視ボックス.
	Switch,				// スイッチ.
	SwitchDoor,			// スイッチで開くドア.
	SwitchRed,				// スイッチ.
	SwitchDoorRed,			// スイッチで開くドア.
	UpDownFloor,		// 上下床.

	NarrowRoad1,		// 細い道.
	NarrowRoad2,		// 細い道.
	NarrowRoad3,		// 細い道.
	NarrowRoad4,		// 細い道.

	Max,

};

const unordered_map<string, enObjectNo> ObjectList
{
	{ "Ground",					enObjectNo::Ground },
	{ "Ground_I",				enObjectNo::Ground_I },
	{ "Ground_L",				enObjectNo::Ground_L },
	{ "Flor",					enObjectNo::Flor },
	{ "Flor_I_one",				enObjectNo::Flor_I_one },
	{ "Flor_I",					enObjectNo::Flor_I },
	{ "Flor_L_one",				enObjectNo::Flor_L_one },
	{ "Flor_L",					enObjectNo::Flor_L },
	{ "Wall",					enObjectNo::Wall },
	{ "Wall_out",				enObjectNo::Wall_out },
	{ "Entrance",				enObjectNo::Entrance },
	{ "Entrance_close",			enObjectNo::Entrance_close },
	{ "Roof",					enObjectNo::Roof },
	{ "Roof_L",					enObjectNo::Roof_L },
	{ "Roof2_I",				enObjectNo::Roof2_I },
	{ "Roof2_L",				enObjectNo::Roof2_L },
	{ "Pillar",					enObjectNo::Pillar },
	{ "Sayaka_s",				enObjectNo::Sayaka },
	{ "Towa_s",					enObjectNo::Towa },
	{ "GhostA",					enObjectNo::GhostA },
	{ "GhostB_s",				enObjectNo::GhostB },
	{ "BigGhost_s",				enObjectNo::BigGhost },
	{ "BookShelf",				enObjectNo::BookShelf },
	{ "Ground",					enObjectNo::Ground },
	{ "InvisibleFloor",			enObjectNo::InvisibleFloor },
	{ "SlipThroughFloor",		enObjectNo::SlipThroughFloor },
	{ "InvisibleStartFloor",	enObjectNo::InvisibleStartFloor },
	{ "InvisibleEndFloor",		enObjectNo::InvisibleEndFloor },
	{ "Signboard",				enObjectNo::Signboard },
	{ "Key",					enObjectNo::Key },
	{ "KeyDoor",				enObjectNo::KeyDoor },
	{ "Pitfall",				enObjectNo::Pitfall },
	{ "LongCollision",			enObjectNo::CameraUpCollision },
	{ "Switch",					enObjectNo::Switch },
	{ "SwitchDoor",				enObjectNo::SwitchDoor },
	{ "Switch_Red",				enObjectNo::SwitchRed },
	{ "SwitchDoor_Red",			enObjectNo::SwitchDoorRed },
	{ "UpDownFloor",			enObjectNo::UpDownFloor },
	{ "Tree",					enObjectNo::Tree },

	{ "NarrowRoad1",			enObjectNo::NarrowRoad1 },
	{ "NarrowRoad2",			enObjectNo::NarrowRoad2 },
	{ "NarrowRoad3",			enObjectNo::NarrowRoad3 },
	{ "NarrowRoad4",			enObjectNo::NarrowRoad4 },

	{ "Event",					enObjectNo::Event },
	{ "Goal",					enObjectNo::Goal },
	{ "TextEvent",				enObjectNo::TextEvent },
	{ "InvisibleWall",			enObjectNo::InvisibleWall },
	{ "InvisibleBox",			enObjectNo::InvisibleBox },
	{ "OpenDoor",				enObjectNo::OpenDoor },
};


// オブジェクトの情報を読み込むための構造体.
struct stObjectInfo
{
	D3DXVECTOR3	vPos;		// 座標情報.
	D3DXVECTOR3	vRot;		// 回転情報.
	enObjectNo	enObjNo;	// オブジェクト番号.
	string		ObjName;
	float		fScale;		// モデルの大きさ.
};

namespace BitFlag
{
	enum enFlagType : unsigned int
	{
		Init			= 0,		// 初期化用.

		isHit				= 1 << 0,	// 当たったか.
		isInvincible		= 1 << 1,	// 無敵か.
		isHavKey			= 1 << 2,	// 鍵を持っているか.
		isKeyDoorOpen		= 1 << 3,	// 鍵付きドアが開いているかどうか.
		isMoveSwitch		= 1 << 4,	// スイッチが動いているか.	
		isMoveSwitchRed		= 1 << 5,	// スイッチが動いているか.	
		isMovement			= 1 << 6,	// 対象が動いているか.
		isHitSignboard		= 1 << 7,	// 看板に触れているか.
		isCameraMoveUp		= 1 << 8,	// カメラが動いているか.
		isDead				= 1 << 9,	// 死亡したかどうか.
		isNextScene			= 1 << 10,	// 次のシーンに行けるか.
		isNextText			= 1 << 11,	// テキストを送る.
		isGoal				= 1 << 12,	// ゴールしたかどうか.
		isSignBoardRender	= 1 << 13,	// 看板が表示できるか.
		isStopCamera		= 1 << 14,	// カメラが動かせない状態.
		isNowFrontHouse		= 1 << 15,	// 家の外のエリアにいる状態.
		isNowTowaControl	= 1 << 16,	// トワ操作中.
		isTowaAfterHelping	= 1 << 17,	// トワを助けたか.
	};
};

class CDX9SkinMesh;
class CDX9Mesh;

class CObjectBase
{
protected:
	CObjectBase()
		: m_vPosition		( 0.0f, 0.0f, 0.0f )
		, m_vRotation		( 0.0f, 0.0f, 0.0f )
		, m_fScale			( 0.0f )
		, m_LifePoint		( 0 )
		, m_fAlpha			( 1.0f )
		, m_isGamePause		( false )
		, m_enObjctNo		( enObjectNo::None )
		, m_ObjName			("")
		, m_CameraState		( 0 )
		, m_cBitFlag		( BitFlag::Init )
	{};
	virtual ~CObjectBase(){};

	void SetObjectInfo( const stObjectInfo& objInfo )
	{
		// 座標情報のセット.
		m_vPosition = D3DXVECTOR3( objInfo.vPos.x, objInfo.vPos.y, objInfo.vPos.z );

		// 回転情報のセット.
		m_vRotation = D3DXVECTOR3( objInfo.vRot.x, objInfo.vRot.y, objInfo.vRot.z );

		// オブジェクト番号のセット.
		m_enObjctNo = ObjectList.at(objInfo.ObjName);
		// オブジェクト名のセット.
		m_ObjName	= objInfo.ObjName;
	}
public:
	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj ) = 0;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj, Light& stLight, stCAMERA& stCamera ) = 0;
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) = 0;
	// 読み込み関数.
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 ) = 0;

	int			m_CameraState;
protected:
	D3DXVECTOR3	m_vPosition;	// 座標.
	D3DXVECTOR3	m_vRotation;	// 回転.
	float		m_fScale;		// モデルの大きさ.
	int			m_LifePoint;	// ライフポイント.
	float		m_fAlpha;		// モデルの透過値.
	bool		m_isGamePause;	// ゲームがポーズ状態か.
	enObjectNo	m_enObjctNo;	// オブジェクト番号.
	string		m_ObjName;		// オブジェクト名.

public:
	// 当たり判定クラス : スフィア, ボックス.
	shared_ptr<CCollisionManager>	m_pCollision;

	// スタティックメッシュ情報取得関数.
	virtual LPD3DXMESH	GetMeshData() = 0;

	// 座標情報取得関数.
	const D3DXVECTOR3 GetPosition() const { return m_vPosition; }
	void SetPosition( const D3DXVECTOR3& pos ){ m_vPosition = pos; }
	// 回転情報取得関数.
	const D3DXVECTOR3 GetRotation() const { return m_vRotation; }
	void SetRotation( const D3DXVECTOR3& rot ){ m_vRotation = rot; }
	// 拡縮情報取得関数.
	const float GetScale() const { return m_fScale; }

	// オブジェクト番号取得関数.
	enObjectNo	GetObjectNo() const { return m_enObjctNo; }

	// バウンディングボックス情報取得関数.
	CCollisionBox* GetBox() const { return m_pCollision->GetBox(); }
	// バウンディングスフィア情報取得関数.
	CCollisionSphere* GetSphere() const { return m_pCollision->GetSphere(); }

	int GetLifePoint() const { return m_LifePoint; }

	void SetAlpha( const float& alpha ){ m_fAlpha = alpha; }

	void SetGamePause( const bool& isPause ){ m_isGamePause = isPause; }

	// ビットフラグの真偽を取得.
	bool GetBitFlag( const BitFlag::enFlagType& bitType ){ return m_cBitFlag & bitType ? true : false; }
	// フラグを立てる.
	void BitFlagON( const BitFlag::enFlagType& bitType ){ m_cBitFlag |= bitType; }
	// フラグを下す.
	void BitFlagOFF( const BitFlag::enFlagType& bitType ){ m_cBitFlag &= ~bitType;}

private:
	int m_cBitFlag;	// 各フラグをビットで制御.
};

#endif // #ifndef OBJECT_BASE_H.