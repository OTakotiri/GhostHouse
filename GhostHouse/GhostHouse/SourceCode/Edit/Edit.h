#ifndef EDIT_H
#define EDIT_H

#include "EditHeader.h"
#include "EditState/EachItem/EachItem.h"
#include "EditState/EachObject/EachObject.h"
#include "EditState/ObjectArrangement/ObjectArrangement.h"

#include "SelectMenu/SelectMenu.h"

#include "FileManager/FileManager.h"
#include "EditImGui/EditImGui.h"
#include "Player/Player.h"
#include "Ground/Ground.h"

class CEdit
{
public:
	CEdit();
	~CEdit();

	// 各オブジェクトの読み込み.
	void Load( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 );
	// 更新処理.
	void Updete();
	// 描画処理.
	void Render( D3DXMATRIX& mView, D3DXMATRIX& mProj,
		Light& stLight, stCAMERA& stCamera );

	// オブジェクトを置く処理.
	void PutObject();
	// オブジェクトを消す処理.
	void DeleteObject();

	// エディット終了処理.
	bool IsEditEnd(){ return m_isEditEnd; }

private:
	// 操作処理.
	void Control();

	// 仮ステージの描画.
	void TmpStageRender( 
		D3DXMATRIX& mView,
		D3DXMATRIX& mProj,
		Light& stLight,
		stCAMERA& stCamera );

	// 選択オブジェクトの更新.
	void ObjArrangementUpdate();
	// 選択オブジェクトの描画.
	void ObjArrangementRender( 
		D3DXMATRIX& mView, 
		D3DXMATRIX& mProj,
		Light& stLight, 
		stCAMERA& stCamera );


private:
	std::vector<edit::RENDER_OBJRCT>	m_pTmpStage;	// 仮ステージ.

	unique_ptr<CObjectArrangement>	m_pObjArrangement;
	unique_ptr<CSelectMenu>			m_pSelectMenu;
	unique_ptr<CEditImGui>			m_pEditImGui;
	shared_ptr<CPlayer>				m_pPlayer;
	shared_ptr<CGround>				m_pGround;

	bool m_isEditEnd;
	edit::RENDER_OBJRCT m_TmpObj;
};

#endif	// #ifndef EDIT_H.