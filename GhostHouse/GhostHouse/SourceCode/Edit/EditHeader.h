#ifndef EDIT_HEADER_H
#define EDIT_HEADER_H

#include <d3dx9math.h>
#include <string>
#include <vector>
#include <memory>

#include "..\ObjectBase\ObjectBase.h"

namespace edit
{
	const float RENDER_POSITION_X = 50.f;
	const float RENDER_POSITION_Y = 30.f;

	const std::string FILE_PATH = "Data\\MapData\\";

	const float MODEL_SCALE = 0.08f;

	// オブジェクト情報.
	struct OBJECT_INFO
	{
		D3DXVECTOR3 vPos;	// 座標情報.
		D3DXVECTOR3 vRot;	// 回転情報.
		std::string Name;	// オブジェクト名.

		OBJECT_INFO()
			: vPos( 0.0f, 0.0f, 0.0f )
			, vRot( 0.0f, 0.0f, 0.0f )
			, Name( "" )
		{}
	};

	// 描画用のオブジェクト構造体.
	struct RENDER_OBJRCT : public edit::OBJECT_INFO
	{
		shared_ptr<CDX9Mesh> pMesh;

		RENDER_OBJRCT()
			: edit::OBJECT_INFO()
			, pMesh( nullptr )
		{
		}

		RENDER_OBJRCT operator = ( const edit::OBJECT_INFO& obj )
		{
			this->vPos = obj.vPos;
			this->vRot = obj.vRot;
			this->Name = obj.Name;
			this->pMesh = nullptr;
			return *this;
		};
		operator OBJECT_INFO() const
		{
			edit::OBJECT_INFO obj;
			obj.vPos = this->vPos;
			obj.vRot = this->vRot;
			obj.Name = this->Name;
			return obj;
		};
	};

	typedef int SelectItem;
	namespace enSelect
	{
		enum Item
		{
			None,

			BackGround,
			Gimmick,
			Player,
			Ghost,
			EditEnd,

			bgin = BackGround,
			end = EditEnd,

			Max,
		};
	};

	typedef int NowChoiceState;
	namespace enNowChoice
	{
		enum State
		{
			None,

			Name,
			Object,
			Decision,

			Max,
		};
	};

	const std::vector<std::string> EachItemNameList =
	{
		"BackGround",
		"Gimmick",
		"Player",
		"Ghost",
		"Save/Load"
	};
};

#endif	// #ifndef EDIT_HEADER_H.
