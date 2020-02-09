#ifndef BIG_GHOST_H
#define BIG_GHOST_H

#include "..\..\..\..\ObjectBase\CharacterBase\GhostBase\GhostBase.h"


class CDX9SkinMesh;
class CDX9Mesh;

class CBigGhost : public CGhostBase
{
private:
	const float MOVE_SPEED				= 0.05f;	// 移動速度.
	const float MAX_WAIT_DISTANCE		= 3.0f;		// 待機時の上下の距離.
	const float MAX_ALPHA				= 2.0f;		// 最大アルファ値.
	const float ATTENUATION_VALUE_ALPH	= 0.02f;	// アルファの減衰値.
	const float MODEL_SCALE				= 0.16f;	// モデルの大きさ.
	const double ANIM_SPEED				= 0.02;
	const char*	MAIN_MODEL_NAME			= "BigGhost_s";	// 仕様モデル名.

public:
	CBigGhost( const stObjectInfo& objInfo );
	~CBigGhost();

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
	CBigGhost(){};
	shared_ptr<CDX9SkinMesh>	m_pSkinMesh;
	shared_ptr<CDX9Mesh>		m_pCollisionBoxMesh;

	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;
	// フェード処理.
	virtual float FadeChange() override;

	bool m_bEndFlag;

	double m_bAnimSpeed;
};

#endif	// #ifndef BIG_GHOST_H.