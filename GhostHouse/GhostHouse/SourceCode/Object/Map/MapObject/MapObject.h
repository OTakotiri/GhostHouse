#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

#include "..\..\..\ObjectBase\MapBase\MapBase.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

class CMapObject : public CMapBase
{
	const float MODEL_ALPHA = 1.0f;
	const float MODEL_SCALE = 0.08f;
public:
	CMapObject( const stObjectInfo& objInfo )
	{
		SetObjectInfo( objInfo );
		m_pCollision = make_shared<CCollisionManager>( &m_vPosition, &m_vRotation.y );
		m_fScale = MODEL_SCALE;
	}
	virtual ~CMapObject()
	{
	};

	// 更新関数.
	virtual void Update( shared_ptr<CObjectBase> pObj )	override
	{
	}
	// 描画関数.
	virtual void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera )	override
	{
		if( m_pStaticMesh == nullptr )	return;

		m_pStaticMesh->SetPosition( m_vPosition );	// 座標情報を描画用にセット.
		m_pStaticMesh->SetRotation( m_vRotation );	// 回転情報を描画用にセット.
		m_pStaticMesh->SetAlpha( MODEL_ALPHA );		// アルファ値をセット.
		m_pStaticMesh->SetScale( MODEL_SCALE );		// サイズのセット.
		m_pStaticMesh->SetCoverage( true );
		m_pStaticMesh->SetCullBackFlag( true );
		m_pStaticMesh->Render( mView, mProj, stLight, stCamera );	// 描画.
		m_pStaticMesh->SetCullBackFlag( false );
		m_pStaticMesh->SetCoverage( false );
	}
	// 当たり判定用関数.
	virtual void Collision( shared_ptr<CObjectBase> pObj ) override
	{
	}

	virtual void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )	override
	{
		// モデルの読み込み.
		if( m_pStaticMesh == nullptr ){
			m_pStaticMesh = CMeshResorce::GetStatic( m_ObjName );
			if( m_pStaticMesh != nullptr ){
				m_pCollision->Init( m_pStaticMesh->GetMesh() );
			}
		}
	}
	virtual LPD3DXMESH GetMeshData() override
	{
		if( m_pStaticMesh == nullptr ) return nullptr;
		return m_pStaticMesh->GetMesh();
	}
private:
	shared_ptr<CDX9Mesh>	m_pStaticMesh;
};

#endif	// #ifndef MAP_OBJECT_H.