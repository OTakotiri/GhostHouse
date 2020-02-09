#ifndef GLOBAL_H
#define GLOBAL_H

// 警告についてのコード分析を無効にする 4005:再定義.
#pragma warning( disable:4005 )

#include <Windows.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include "MyMacro.h"

#include <d3dx9.h>
#include <D3DX11.h>
#include <D3D11.h>
#include <D3DX10.h>	// 「D3DX～」の定義使用に必要.
#include <D3D10.h>

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx10.lib" )

using namespace std;

//=================================================
//	定数.
//=================================================
//#define WND_W 1280	//←定数宣言で#defineは使わない.
const int WND_W = 1280;	//ｳｨﾝﾄﾞｳの幅.
const int WND_H = 720;	//ｳｨﾝﾄﾞｳの高さ.

struct stCAMERA
{
	D3DXVECTOR3 Pos;	// 位置.
	D3DXVECTOR3 vLook;	// 注視位置.
	D3DXVECTOR3 vRot;	// 回転.
	float		fDegree;// 角度.
	float		fYaw;	// Y軸回転.
	float		fLength;//レンジ(距離).
	D3DXMATRIX	mRot;	// 回転行列.
	D3DXVECTOR3 InvisibleCenter;	//見えない床の中心.
	D3DXVECTOR3 InvisibleCollisionPos;	//見えない床ギミック作動のための当たり判定の位置.
};

struct Light
{
	Light()
	{
		ZeroMemory( this, sizeof( Light ) );
	}
	D3DXVECTOR3 pos;
	float range;
	D3DXVECTOR3 dir;
	float cone;
	D3DXVECTOR3 att;
	D3DXVECTOR4 ambient;
	D3DXVECTOR4 diffuse;
	D3DXMATRIX Rot;
	D3DXVECTOR4 Color;
};

#endif	// #ifndef GLOBAL_H.