#ifndef HITO_DAMA_H
#define HITO_DAMA_H

#include "..\..\..\..\ObjectBase\CharacterBase\GhostBase\GhostBase.h"

class CHitoDama : public CGhostBase
{
private:
	const char*		MAIN_MODEL_NAME = "HitoDama";	// 仕様モデル名.

	const float		MOVE_SPEED		= 0.04f;	// 移動速度.
	const float		MAX_SHOT_COUNT	= 40.0f;	// ショットカウント最大数.
	const float		MODEL_SCALE		= 0.08f;	// モデルの大きさ.
	const float		SPHERE_COLLISION_RADIUS = 0.05f;		// 当たり判定用の半径の大きさ.
	const D3DXVECTOR3 WND_OUT_PISITION = { 100.0f, -100.0f, 100.0f };
public:
	CHitoDama();
	virtual ~CHitoDama();

	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override;
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override;
	// ロード関数.
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override;
	// 弾のセット.
	void SetShot( const D3DXVECTOR3& vPos );

	bool IsAction(){ return m_bMoveFlag; }

	void GhostBLink( CObjectBase* pObj ){ m_pGhostB = pObj; }

	virtual LPD3DXMESH GetMeshData() override;

private:
	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;
	// 待機関数.
	virtual void IdleState()	override;

	// 当たり処理.
	void HitProc( shared_ptr<CObjectBase> pObj );

	shared_ptr<CDX9Mesh>	m_pStaticMesh;

	CObjectBase* m_pGhostB;

	// ショットカウント.
	float	m_fShotCount;
	float	m_fScaleChangeCount;

	D3DXVECTOR3 m_vPos;
	float		m_fColl_y;
};

#endif	// #ifndef HITO_DAMA_H.
