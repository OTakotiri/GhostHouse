#ifndef GHOST_A_H
#define GHOST_A_H

#include "..\..\..\..\ObjectBase\CharacterBase\GhostBase\GhostBase.h"
#include "..\..\..\..\Resource\MeshResource\MeshResource.h"
class CGhostA : public CGhostBase
{
private:
	const char*	MAIN_MODEL_NAME		= "GhostA";	// 仕様モデル名.

	// 無くなるかも. 
	const float INIT_POSITION_Y		= 0.0f;		

	const float MOVE_SPEED				= 0.03f;	// 移動速度.
	const float MAX_WAIT_DISTANCE		= 3.0f;		// 待機時の上下の距離.
	const float MIN_SEARCH_DISTANCE		= 5.0f;		// 追尾前のサーチ範囲.
	const float MAX_SEARCH_DISTANCE		= 6.0f;		// 追尾後のサーチ範囲.
	const float MODEL_SCALE				= 0.08f;	// モデルの大きさ.
	const float SPHERE_COLLISION_RADIUS = 1.0f;		// 当たり判定用の半径の大きさ.

public:
	CGhostA( const stObjectInfo& objInfo );
	virtual ~CGhostA();

	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override;
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override;
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override;
	virtual LPD3DXMESH	GetMeshData() override;

private:
	CGhostA(){};

	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;
	// 索敵範囲の当たり判定処理.
	bool SearchCollision( shared_ptr<CObjectBase> pObj );

	shared_ptr<CDX9Mesh>	m_pStaticMesh;

	shared_ptr<CDX9Mesh::CHANGE_TEXTURES>	m_pTexture;

	bool m_bChangeTexture;
};

#endif	// #ifndef GHOST_A_H.