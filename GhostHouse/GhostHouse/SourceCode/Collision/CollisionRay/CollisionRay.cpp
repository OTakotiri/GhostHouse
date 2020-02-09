#include "CollisionRay.h"

#include "..\..\ObjectBase\ObjectBase.h"

CCollisionRay::CCollisionRay( float* pfYaw )
	: m_pfYaw	( pfYaw )
{
}

CCollisionRay::~CCollisionRay()
{
	m_pfYaw = nullptr;
}


// レイとメッシュの当たり判定.
bool CCollisionRay::Intersect( shared_ptr<CObjectBase> pTargetObj,
	float* pfDistance,			// (out)距離.
	D3DXVECTOR3* pvIntersect, 
	D3DXVECTOR3 endpos )	// (out)交差座標.
{
	D3DXMATRIXA16 mAttackRot;	// 回転行列.

	// 回転行列を計算.
	D3DXMatrixRotationY( &mAttackRot, *m_pfYaw );

	// 軸ベクトルを用意.
	D3DXVECTOR3 vecAxisZ;
	// Zベクトルそのものを現在の回転情報により変換する.
	D3DXVec3TransformCoord( &vecAxisZ, &m_vAxis, &mAttackRot );

	// レイの位置.
	D3DXVECTOR3 vecStart, vecEnd;
	vecStart = vecEnd = m_vRay;

	// 自キャラの座標に回転座標を合成する.
	if( endpos == D3DXVECTOR3(0.0f, 0.0f, 0.0f) ){
		vecEnd += vecAxisZ * 1.0f;
	} else {
		vecEnd = endpos;
	}

	// 対象が動いている物体でも、対象のワールド行列の.
	// 逆行列を用いれば正しくレイが当たる.
	// 対象の. ﾜｰﾙﾄﾞ行列、ｽｹｰﾙ行列、回転行列、平行移動行列.
	D3DXMATRIX mWorld, mScale, mRotYPR, mTran;

	//拡大縮小行列作成.
	D3DXMatrixScaling(
		&mScale,	//(out)計算結果.
		pTargetObj->GetScale(),
		pTargetObj->GetScale(),
		pTargetObj->GetScale() );	//x,y,zそれぞれの拡縮値.

	D3DXMATRIX mYaw, mPitch, mRoll;
	//Y軸回転行列作成.
	D3DXMatrixRotationY( &mYaw, pTargetObj->GetRotation().y );
	//X軸回転行列作成.
	D3DXMatrixRotationX( &mPitch, pTargetObj->GetRotation().x );
	//Z軸回転行列作成.
	D3DXMatrixRotationZ( &mRoll, pTargetObj->GetRotation().z );
	//回転行列を作成.
	mRotYPR = mYaw * mPitch * mRoll;

	//平行移動行列作成.
	D3DXMatrixTranslation(
		&mTran,	//(out)計算結果.
		pTargetObj->GetPosition().x, pTargetObj->GetPosition().y, pTargetObj->GetPosition().z );	//x,y,z座標.


	//ﾜｰﾙﾄﾞ行列作成.
	mWorld = mScale * mRotYPR * mTran;
	D3DXMATRIX mBackWorld;
	// 逆行列を求める.
	D3DXMatrixInverse( &mBackWorld, nullptr, &mWorld );
	D3DXVec3TransformCoord( &vecStart, &vecStart, &mBackWorld );
	D3DXVec3TransformCoord( &vecEnd, &vecEnd, &mBackWorld );

	// 距離.
	D3DXVECTOR3 vecDistance;
	vecDistance = vecEnd - vecStart;	// 距離を求める.

	BOOL bHit = FALSE;	// 命中フラグ.

	DWORD dwIndex = 0;	// インデックス番号.
	D3DXVECTOR3 vVertex[3];	// 頂点座標.
	FLOAT U = 0.0F, V = 0.0F;	// (out)重心ヒット座標.

	// レイとメッシュの交差を調べる.
	D3DXIntersect(
		pTargetObj->GetMeshData(),	// 対象メッシュ,
		&vecStart,				// 開始位置.
		&vecDistance,			// レイの距離.
		&bHit,					// (out)判定結果.
		&dwIndex,				// (out)bHItがTRUE時に.
								// レイの始点に最も近くの面のインデックス値へのポインタ.
		&U, &V,					// (out)重心ヒット座標.
		pfDistance,				// (out)ターゲットとの距離.
		nullptr, nullptr );

	if( bHit == TRUE ){
		// 命中した時.
		FindVerticesOnPoly(
			pTargetObj->GetMeshData(), dwIndex, vVertex );
		// 重心座標から交点を算出.
		// ローカル交点は、 v0 + U * (v1-v0) + V * (v2-v0)で求まる.
		*pvIntersect =
			vVertex[0]
			+ U * ( vVertex[1] - vVertex[0] )
			+ V * ( vVertex[2] - vVertex[0] );

		D3DXVec3TransformCoord( pvIntersect, pvIntersect, &mWorld );

		return true;
	}
	return false;
}

// 交差位置のポリゴンの頂点を見つける.
HRESULT CCollisionRay::FindVerticesOnPoly(
	LPD3DXMESH pMeshForRay, DWORD dwPolyIndex,
	D3DXVECTOR3* pVecVertices )	// (out)頂点座標(ローカル座標),
{
	// 頂点ごとのバイト数を取得.
	DWORD dwStrid = pMeshForRay->GetNumBytesPerVertex();
	// 頂点数を取得.
	DWORD dwVertexAmt = pMeshForRay->GetNumVertices();
	// 面数を取得.
	DWORD dwPolyAmt = pMeshForRay->GetNumFaces();

	WORD* pwPoly = nullptr;

	// インデックスバッファをロック(読み込みモード).
	pMeshForRay->LockIndexBuffer(
		D3DLOCK_READONLY, (VOID**)&pwPoly );
	BYTE* pbVertices = nullptr;	// 頂点(バイト数).
	FLOAT* pfVerTices = nullptr;	// 頂点(FLOAT型).
	LPDIRECT3DVERTEXBUFFER9 VB = nullptr;	// 頂点バッファ.
	pMeshForRay->GetVertexBuffer( &VB );	// 頂点情報の取得.

	// 頂点バッファのロック.
	if( SUCCEEDED(
		VB->Lock( 0, 0, (VOID**)&pbVertices, 0 ) ) ){
		for( int i = 0; i < 3; i++ ){
			// ポリゴンの頂点の3つ目を取得.
			pfVerTices
				= (FLOAT*)( &pbVertices[dwStrid*pwPoly[dwPolyIndex * 3 + i]] );
			pVecVertices[i].x = pfVerTices[0];
			pVecVertices[i].y = pfVerTices[1];
			pVecVertices[i].z = pfVerTices[2];
		}

		pMeshForRay->UnlockIndexBuffer();
		VB->Unlock();
	}
	SAFE_RELEASE( VB );

	return S_OK;
}