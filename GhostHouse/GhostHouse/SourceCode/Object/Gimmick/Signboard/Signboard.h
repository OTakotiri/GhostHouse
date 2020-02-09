#ifndef SINGNBOARD_H
#define SINGNBOARD_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"
#include "MultiTexture/MultiTexture.h"

class CSignboard : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "Signboard";	// 仕様モデル名.
	const float MODEL_SCALE		= 0.08f;

	const float MOVE_DISTANCE	= 0.03f;
public:
	CSignboard( const stObjectInfo& objInfo );
	~CSignboard();

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
	CSignboard();
	// 動作関数.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;

	void FloatingMove();

	shared_ptr<CDX9Mesh>		m_pStaticMesh;

	float m_MoveDistance;
};

#endif	// #ifndef SINGNBOARD_H.