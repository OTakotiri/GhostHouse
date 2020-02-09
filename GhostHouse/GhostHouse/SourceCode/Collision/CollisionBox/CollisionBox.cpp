#include "CollisionBox.h"
#include "..\..\Resource\ModelResource\DX9Mesh\CDX9Mesh.h"


CCollisionBox::CCollisionBox()
	: m_pvPosition	( nullptr )
{
}

CCollisionBox::CCollisionBox( D3DXVECTOR3* pvPos )
	: m_pvPosition	( pvPos )
{
}

CCollisionBox::~CCollisionBox()
{
	m_pvPosition	= nullptr;
}

//------------------------------.
// 判定処理.
//------------------------------.
bool CCollisionBox::isCollision( CCollisionBox* collBox )
{
	// 攻撃側に2つの頂点を用意し初期化する.
	D3DXVECTOR3 vecMaxAtk, vecMinAtk;	// 最大,最小頂点.
	vecMaxAtk = m_vPositionMax + *m_pvPosition;
	vecMinAtk = m_vPositionMin + *m_pvPosition;

	// 目標側に2つの頂点を用意し初期化する.
	D3DXVECTOR3 vecMaxTrg, vecMinTrg;	// 最大,最小頂点.
	vecMaxTrg = collBox->GetPositionMax() + collBox->GetPosition();
	vecMinTrg = collBox->GetPositionMin() + collBox->GetPosition();

	// 2つの物体それぞれの最大,最小位置という.
	// 4つの情報があれば、衝突を検出できる.
	if( vecMinAtk.x < vecMaxTrg.x && vecMaxAtk.x > vecMinTrg.x &&
		vecMinAtk.y < vecMaxTrg.y && vecMaxAtk.y > vecMinTrg.y &&
		vecMinAtk.z < vecMaxTrg.z && vecMaxAtk.z > vecMinTrg.z ){
		return true;
	}
	return false;
}

//------------------------------.
// バウンディングボックス作成.
//------------------------------.
HRESULT CCollisionBox::InitBBox( LPD3DXMESH pMesh )
{
	LPDIRECT3DVERTEXBUFFER9 pVB = nullptr;
	VOID* pVertices = nullptr;
	D3DXVECTOR3	Max, Min;

	if( FAILED( pMesh->GetVertexBuffer( &pVB ) ) ){
		_ASSERT_EXPR( false, L"頂点バッファ取得失敗" );
		return E_FAIL;
	}
	// メッシュの頂点バッファをロックする.
	if( FAILED( pVB->Lock( 0, 0, &pVertices, 0 ) ) ){
		_ASSERT_EXPR( false, L"頂点バッファのロック失敗" );
		SAFE_RELEASE( pVB );
		return E_FAIL;
	}
	// メッシュ内の頂点位置の最大と最小を検索する.
	D3DXComputeBoundingBox(
		static_cast<D3DXVECTOR3*>( pVertices ),
		pMesh->GetNumVertices(),
		D3DXGetFVFVertexSize(
			pMesh->GetFVF() ),
		&Min, &Max );	// (out)最小,最大頂点.

	pVB->Unlock();		// アンロック.
	SAFE_RELEASE( pVB );	// 使用済み頂点バッファの解放.

	m_vPositionMin = Min;
	m_vPositionMax = Max;

	m_vLength.x = ( Max.x - Min.x ) / 2.0f;
	m_vLength.y = ( Max.y - Min.y ) / 2.0f;
	m_vLength.z = ( Max.z - Min.z ) / 2.0f;

	return S_OK;
}