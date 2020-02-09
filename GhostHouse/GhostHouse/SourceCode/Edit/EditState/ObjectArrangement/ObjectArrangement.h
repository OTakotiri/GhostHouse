#ifndef OBJECT_ARRANGEMENT_H
#define OBJECT_ARRANGEMENT_H

#include "..\..\EditHeader.h"

class CDX9Mesh;
struct Light;
struct stCAMERA;

class CObjectArrangement
{
public:
	CObjectArrangement();
	~CObjectArrangement();

	void Update();
	void Render( 
		D3DXMATRIX& mView, 
		D3DXMATRIX& mProj,
		Light& stLight, 
		stCAMERA& stCamera );

	edit::OBJECT_INFO GetObjectInfo() const { return m_ObjctInfo; }

	void SetPosition( const D3DXVECTOR3& vPos ){ m_ObjctInfo.vPos = vPos; }
	void SetObjectName( const std::string& name )
	{ 
		m_pModel = nullptr;
		m_ObjctInfo.Name = name; 
	}

private:
	bool ModelLoad();

private:
	std::shared_ptr<CDX9Mesh>	m_pModel;
	edit::OBJECT_INFO			m_ObjctInfo;

	bool m_isWierDips;
};

#endif	// #ifndef OBJECT_ARRANGEMENT_H.