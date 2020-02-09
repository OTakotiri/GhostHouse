#ifndef KEY_H
#define KEY_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"
#include "..\..\..\Resource\EffectResource\EffectResource.h"

class CKey : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "Key";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
	const float SPHERE_COLLISION_RADIUS = 1.0f;		// 当たり判定用の半径の大きさ.
	const D3DXVECTOR3 WND_OUT = { 100.0f, -100.0f, 100.0f };
public:
	CKey( const stObjectInfo& objInfo );
	~CKey();

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
	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;

private:
	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	shared_ptr<CEffect>		m_pEffect;
	CEffect::Handle			m_Handle;
};

#endif	// #ifndef KEY_H.