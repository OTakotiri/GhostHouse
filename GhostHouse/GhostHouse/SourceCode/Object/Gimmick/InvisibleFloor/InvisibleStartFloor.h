#ifndef INVISIBLE_START_FLOOR_H
#define INVISIBLE_START_FLOOR_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"
#include "..\..\..\DebugMode\Sphere\Sphere.h"

class CInvisibleStartFloor : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "InvisibleStartFloor";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
	const float MODEL_ALPHA = 1.0f;
	const float COLLISION_RADIUS = 0.1f;
public:
	CInvisibleStartFloor( const stObjectInfo& objInfo );
	~CInvisibleStartFloor();

	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override;
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override;
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override;
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override;

	virtual LPD3DXMESH GetMeshData() override;
private:
	CInvisibleStartFloor();
	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;

	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	int m_State;
	bool m_isNowHit;
	bool m_isOlsHit;
};

#endif	// #ifndef INVISIBLE_START_FLOOR_H.