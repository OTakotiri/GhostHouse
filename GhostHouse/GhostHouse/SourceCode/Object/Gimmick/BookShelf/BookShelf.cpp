#include "BookShelf.h"
#include "..\..\..\XInput\XInput.h"
#include "..\..\Resource\MeshResource\MeshResource.h"

CBookShelf::CBookShelf(const stObjectInfo& objInfo)
	: m_fCount(0.0f)
	, m_bFadeInFlag(true)
	, m_isMoveing(false)
	, m_PlayerPosition{ 0.0f, 0.0f, 0.0f }
{
	SetObjectInfo(objInfo);
	m_fScale = MODEL_SCALE;
}

CBookShelf::~CBookShelf()
{}

void CBookShelf::Update(shared_ptr<CObjectBase> pObj)
{
	if (pObj->GetObjectNo() == enObjectNo::Towa) return;
	if (m_pStaticMesh == nullptr)	return;

	if (m_bMoveFlag == true) {
		FadeInOut();
		// フェードインしている状態で回転させる.
		if (m_fCount >= 1.0f) {
			if (m_isMoveing == false) {
				AxisMove();
				m_isMoveing = true;
			}
		}
	}

	Action(pObj);
}

void CBookShelf::Render(D3DXMATRIX & mView, D3DXMATRIX & mProj, Light& stLight, stCAMERA& stCamera)
{
	if (m_pStaticMesh == nullptr)	return;
	if (m_fAlpha <= 0.8f) {
		m_fAlpha = 1.0f;
		if (m_bMoveFlag == true) {
			if (m_fCount >= 0.0f) {
				CSpriteResource::GetSprite("Fade")->SetAlpha(m_fCount);
			}
		}
		return;
	}

	m_pStaticMesh->SetPosition(m_vPosition);	// 座標を描画用にセット.
	m_pStaticMesh->SetRotation(m_vRotation);
	m_pStaticMesh->SetAlpha(1.0f);
	m_pStaticMesh->SetScale(MODEL_SCALE);
	m_pStaticMesh->SetBlend(true);
	m_pStaticMesh->Render(mView, mProj, stLight, stCamera);	// 描画.
	m_pStaticMesh->SetBlend(false);

	// 移動フラグが下りていたら終了.
	if (m_bMoveFlag == false) return;
	// カウントが0より低かったら終了.
	if (m_fCount < 0.0f) return;
	CSpriteResource::GetSprite("Fade")->SetAlpha(m_fCount);
}

void CBookShelf::Collision(shared_ptr<CObjectBase> pObj)
{
}

void CBookShelf::Load(ID3D11Device * pDevice11, ID3D11DeviceContext * pContext11)
{
	if (m_pStaticMesh == nullptr) {
		m_pStaticMesh = CMeshResorce::GetStatic(MAIN_MODEL_NAME);
		if (m_pStaticMesh != nullptr) {
			m_pCollision->Init(m_pStaticMesh->GetMesh());
			m_pCollision->GetSphere()->SetRadius(SPHERE_COLLISION_RADIUS);
		}
	}
}

void CBookShelf::Action(shared_ptr<CObjectBase> pObj)
{
	// 既に移動していたら終了.
	if (m_bMoveFlag == true) return;

	// スフィアの当たり判定にあたっていなければ終了.
	if (m_pCollision->isSphereCollision(pObj->GetSphere()) == false)	return;

	// トワ(兄)がいなければテキストを表示させて終了.
	if (pObj->GetBitFlag(BitFlag::isTowaAfterHelping) == false) return;

	if (CXInput::IsPress(XINPUT_GAMEPAD_B) == true) {
		m_PlayerPosition = pObj->GetPosition();
		m_bMoveFlag = true;
	}
}

void CBookShelf::AxisMove()
{
	float axis = MOVE_POWER;
	if (m_PlayerPosition.x <= m_vPosition.x || m_PlayerPosition.z <= m_vPosition.z) {
		axis = MOVE_POWER;
	}
	else if (m_PlayerPosition.x >= m_vPosition.x || m_PlayerPosition.z >= m_vPosition.z) {
		axis = -MOVE_POWER;
	}
	D3DXVECTOR3 vecAxisX = { axis, 0.0f, 0.0f };
	D3DXMATRIX mYaw;
	//Y軸回転.
	D3DXMatrixRotationY(&mYaw, m_vRotation.y);

	//Z軸ベクトルそのものを回転状態により変換する.
	D3DXVec3TransformCoord(&vecAxisX, &vecAxisX, &mYaw);
	m_vPosition.x = m_vPosition.x + vecAxisX.x;
	m_vPosition.z = m_vPosition.z + vecAxisX.z;
}

void CBookShelf::FadeInOut()
{
	if (m_fCount >= 0.0f) {
		if (m_fCount >= 1.5f) m_bFadeInFlag = false;
		if (m_bFadeInFlag == true) {
			m_fCount += 0.04f;
		}
		else {
			m_fCount -= 0.02f;
		}
	}
}

LPD3DXMESH CBookShelf::GetMeshData()
{
	if (m_pStaticMesh == nullptr) return nullptr;
	return m_pStaticMesh->GetMesh();
}