#ifndef CAMERAUP_COLLISION_H
#define CAMERAUP_COLLISION_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"
#include "..\..\..\DebugMode\Sphere\Sphere.h"

class CCameraUpCollision : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "LongCollision";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
	const float MODEL_ALPHA = 0.6f;
public:
	CCameraUpCollision(const stObjectInfo& objInfo);
	~CCameraUpCollision();

	// 更新関数.
	virtual void Update(shared_ptr<CObjectBase> pObj)	override;
	// 描画関数.
	virtual void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera)	override;
	// 当たり判定用関数.
	virtual void Collision(shared_ptr<CObjectBase> pObj) override;
	virtual void Load(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11)	override;

	virtual LPD3DXMESH GetMeshData() override;
private:
	CCameraUpCollision();
	// 動作関数.
	virtual void Action(shared_ptr<CObjectBase> pObj)	override;

	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	int m_State;
	bool m_isNowHit;
	bool m_isOlsHit;

	unique_ptr<CSphere>	m_pDebugSphere;
	
};

#endif	// #ifndef CAMERAUP_COLLISION_H.
