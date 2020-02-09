#ifndef UP_DOWN_FLOOR_H
#define UP_DOWN_FLOOR_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"

#include "..\..\..\DebugMode\DebugText\DebugText.h"

class CUpDownFloor : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "UpDownFloor";	// 仕様モデル名.
	const float MODEL_SCALE		= 0.08f;
	const float RADIUS			= 0.008f;
	const int	MAX_WAIT_TIME	= 100;				// 待機時間,

public:
	CUpDownFloor( const stObjectInfo& objInfo );
	~CUpDownFloor();
	
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
	// 移動角度(上下).
	float	m_fAngleCount;
	// 待機カウント.
	int		m_iWaitCount;
};

#endif	// #ifndef UP_DOWN_FLOOR_H.