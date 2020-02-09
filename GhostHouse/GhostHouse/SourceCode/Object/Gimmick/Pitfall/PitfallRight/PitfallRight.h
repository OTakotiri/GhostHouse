#ifndef PITFALL_RIGHT_H
#define PITFALL_RIGHT_H

#include "..\..\..\..\ObjectBase\GimmickBase\GimmickBase.h"

class CPitfallRight : public CGimmickBase
{
	enum class enPitfallSatate
	{
		NOW_OPEN,
		OPEN,
		NOW_CLOSE,
		CLOSE,
	};
private:
	const int MAX_ANGLE		= 90;
	const int MAX_WAIT_TIME	= 100;
	const int MOVE_SPEED	= 5;

	const float MODEL_SCALE = 0.08f;
public:
	CPitfallRight( stObjectInfo objInfo );
	~CPitfallRight();

	// çXêVä÷êî.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override;
	// ï`âÊä÷êî.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override;
	// ìñÇΩÇËîªíËópä÷êî.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override;
	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override;
private:
	// ìÆçÏä÷êî.
	virtual void Action( shared_ptr<CObjectBase> pObj )	override;

	shared_ptr<CDX9Mesh>	m_pStaticMesh;
	int m_iCount;

	enPitfallSatate	m_enState;
};

#endif	// #ifndef PITFALL_RIGHT_H.