#ifndef SWTICH_DOOR_RED_H
#define SWTICH_DOOR_RED_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"

class CSwitchDoorRed : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "SwitchDoor_Red";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
public:
	CSwitchDoorRed( const stObjectInfo& objInfo  );
	~CSwitchDoorRed();

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
	void FadeInOut();

	float	m_fCount;
	bool	m_bFadeInFlag;
};

#endif	// SWTICH_DOOR_RED_H.