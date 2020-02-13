#ifndef SWITCH_RED_H
#define SWITCH_RED_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"

class CSwitchRed : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "Switch_Red";	// 仕様モデル名.
	const float MODEL_SCALE = 0.16f;
private:
	float INIT_POSITION_Y;
public:
	CSwitchRed( const stObjectInfo& objInfo );
	~CSwitchRed();

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

	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	bool m_bisPush;
	bool m_isSayaka;
	bool m_isTowa;
};

#endif	// #ifndef SWITCH_RED_H.