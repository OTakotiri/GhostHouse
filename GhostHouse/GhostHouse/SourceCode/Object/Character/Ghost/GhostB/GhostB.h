#ifndef GHOST_B_H
#define GHOST_B_H

#include "..\..\..\..\ObjectBase\CharacterBase\GhostBase\GhostBase.h"
#include "..\HitoDama\HitoDama.h"
#include "..\..\..\..\Resource\MeshResource\MeshResource.h"

class CDX9SkinMesh;

class CGhostB : public CGhostBase
{
private:
	const char*	MAIN_MODEL_NAME		= "GhostB_s";	// 仕様モデル名.

	// 無くなるかも. 
	const float INIT_POSITION_Y = 0.0f;

	const float MAX_WAIT_DISTANCE	= 3.0f;		// 待機時の上下の距離.
	const float MAX_SEARCH_DISTANCE	= 5.0f;		// サーチ範囲.

	const float MODEL_SCALE				= 0.08f;	// モデルの大きさ.
	const float SPHERE_COLLISION_RADIUS = 1.0f;		// 当たり判定用の半径の大きさ.

public:
	CGhostB( const stObjectInfo& objInfo );
	virtual ~CGhostB();

	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override;
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override;
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override;

	// スタティックメッシュ情報取得関数.
	virtual LPD3DXMESH	GetMeshData() override;
private:
	CGhostB(){};

	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;
	void AnimInit();
	void AnimAdd();

private:
	// ゴーストBが飛ばす人魂クラス.
	unique_ptr<CHitoDama>			m_pHitoDama;

	shared_ptr<CDX9SkinMesh>		m_pSkinMesh;
	shared_ptr<CDX9SkinMesh::CHANGE_TEXTURES>	m_pTexture;

	LPD3DXANIMATIONCONTROLLER		m_pAc;

	double m_AnimSpd;
	double m_AnimSpeed;
	bool m_bChangeTexture;
};

#endif	// #ifndef GHOST_B_H.