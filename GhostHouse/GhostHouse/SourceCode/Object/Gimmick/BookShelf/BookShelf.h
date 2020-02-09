#ifndef BOOK_SHELF_H
#define BOOK_SHELF_H

#include "..\..\..\ObjectBase\GimmickBase\GimmickBase.h"

class CBookShelf : public CGimmickBase
{
	const char*	MAIN_MODEL_NAME = "BookShelf";	// 仕様モデル名.
	const float MODEL_SCALE = 0.08f;
	const float SPHERE_COLLISION_RADIUS = 1.0f;		// 当たり判定用の半径の大きさ.
	const float MOVE_POWER = 2.0f;
public:
	CBookShelf(const stObjectInfo& objInfo);
	~CBookShelf();

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
	CBookShelf() {};

	// 動作関数.
	virtual void Action(shared_ptr<CObjectBase> pObj)	override;
	void AxisMove();
	void FadeInOut();

private:
	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	float	m_fCount;
	bool	m_bFadeInFlag;
	bool	m_isMoveing;
	D3DXVECTOR3 m_PlayerPosition;
};

#endif	// #ifndef BOOK_SHELF_H.