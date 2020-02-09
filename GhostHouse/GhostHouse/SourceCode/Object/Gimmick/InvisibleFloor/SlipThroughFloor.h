#ifndef SLIPTHROUGHFLOOR_H
#define SLIPTHROUGHFLOOR_H

#include "..\..\..\ObjectBase\MapBase\MapBase.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

class CSlipThroughFloor : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "SlipThroughFloor";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
public:
	CSlipThroughFloor(const stObjectInfo& objInfo)
	{
		SetObjectInfo(objInfo);
		m_pCollision = make_shared<CCollisionManager>(&m_vPosition, &m_vRotation.y);
		m_fScale = MODEL_SCALE;
	}
	CSlipThroughFloor() {};
	virtual ~CSlipThroughFloor() {};

	// 更新関数.
	virtual void Update(shared_ptr<CObjectBase> pObj)	override
	{
		if( m_pStaticMesh == nullptr )	return;

		Action( pObj );
	}
	// 描画関数.
	virtual void Render(D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera)	override
	{
		if( m_pStaticMesh == nullptr )	return;
		if( m_bMoveFlag == false )	return;

		m_pStaticMesh->SetPosition( m_vPosition );	// 座標を描画用にセット.
		m_pStaticMesh->SetRotation( m_vRotation );
		m_pStaticMesh->SetAlpha( m_fAlpha );
		m_pStaticMesh->SetScale( MODEL_SCALE );
		m_pStaticMesh->SetWireFlag( true );
		m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
		m_pStaticMesh->SetWireFlag( false );
	}
	// 当たり判定用関数.
	virtual void Collision(shared_ptr<CObjectBase> pObj) override
	{}
	virtual void Load(ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11)	override
	{
		// モデルの読み込み.
		if (m_pStaticMesh == nullptr) {
			m_pStaticMesh = CMeshResorce::GetStatic(MAIN_MODEL_NAME);
			if (m_pStaticMesh != nullptr) {
				m_pCollision->Init(m_pStaticMesh->GetMesh());
			}
		}
	}
	virtual void Action( shared_ptr<CObjectBase> pObj )	override
	{
		if( pObj->GetBitFlag( BitFlag::isCameraMoveUp ) == false ){
			m_bMoveFlag = false;
			return;
		}
		m_bMoveFlag = true;
	}
	LPD3DXMESH GetMeshData() override
	{
		if( m_pStaticMesh == nullptr ) return nullptr;
		return m_pStaticMesh->GetMesh();
	}
private:
	shared_ptr<CDX9Mesh>	m_pStaticMesh;
};

#endif	// #ifndef SLIPTHROUGHFLOOR_H.