/***************************************************************************************************
*	SkinMeshCode Version 2.00
*	LastUpdate	: 2019/10/09.
*		�p�[�T�[�N���X�ƃX�L�����b�V���N���X��ʃt�@�C���ɕ���.
**/
#include "CDX9SkinMesh.h"
#include <crtdbg.h>

//�V�F�[�_��(�f�B���N�g�����܂�)
const char SHADER_NAME[] = "Data\\Shader\\SkinMesh.hlsl";

/******************************************************************************************************************************************
*
*	�ȍ~�A�X�L�����b�V���N���X.
*
**/
//�R���X�g���N�^.
CDX9SkinMesh::CDX9SkinMesh()
	: m_hWnd( nullptr )
	, m_pDevice9( nullptr )
	, m_pDevice11( nullptr )
	, m_pContext11( nullptr )
	, m_pSampleLinear( nullptr )
	, m_pVertexShader( nullptr )
	, m_pPixelShader( nullptr )
	, m_pVertexLayout( nullptr )
	, m_pCBufferPerMesh( nullptr )
	, m_pCBufferPerMaterial( nullptr )
	, m_pCBufferPerFrame( nullptr )
	, m_pCBufferPerBone( nullptr )
	, m_vPos()
	, m_vRot()
	, m_vScale( D3DXVECTOR3( 1.0f, 1.0f, 1.0f ) )
	, m_fAlpha(1.0f)
	, m_mWorld()
	, m_mRotation()
	, m_mView()
	, m_mProj()
//	, m_vLight()
//	, m_vEye()
	, m_dAnimSpeed( 0.0001f )	//��悸�A���̒l.
	, m_dAnimTime()
	, m_pReleaseMaterial( nullptr )
	, m_pD3dxMesh( nullptr )
	, m_FilePath()
	, m_iFrame()
{

}

CDX9SkinMesh::CDX9SkinMesh( 
	HWND hWnd,
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
	: CDX9SkinMesh()
{
	Init( hWnd, pDevice11, pContext11, pDevice9, fileName );
}

//�f�X�g���N�^.
CDX9SkinMesh::~CDX9SkinMesh()
{
	//�������.
	Release();

	//�V�F�[�_��T���v���֌W.
	SAFE_RELEASE( m_pSampleLinear );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pVertexLayout );

	//�R���X�^���g�o�b�t�@�֌W.
	SAFE_RELEASE( m_pCBufferPerBone );
	SAFE_RELEASE( m_pCBufferPerFrame );
	SAFE_RELEASE( m_pCBufferPerMaterial );
	SAFE_RELEASE( m_pCBufferPerMesh );

//	SAFE_DELETE( m_pReleaseMaterial );
	m_pReleaseMaterial = nullptr;
	
	SAFE_RELEASE( m_pD3dxMesh );

	m_pDevice9 = nullptr;
	//Dx11 �f�o�C�X�֌W.
	m_pContext11 = nullptr;
	m_pDevice11 = nullptr;

	m_hWnd = nullptr;
}


//������.
HRESULT CDX9SkinMesh::Init( 
	HWND hWnd,
	ID3D11Device* pDevice11,
	ID3D11DeviceContext* pContext11,
	LPDIRECT3DDEVICE9 pDevice9,
	const char* fileName )
{
	m_hWnd = hWnd;
	m_pDevice11 = pDevice11;
	m_pContext11 = pContext11;
	m_pDevice9 = pDevice9;

	//�V�F�[�_�̍쐬.
	if( FAILED( InitShader() ) ){
		return E_FAIL;
	}
	//���f���ǂݍ���.
	if( FAILED( LoadXMesh( fileName ) ) ){
		return E_FAIL;
	}
	//�u�����h�쐬.
	if( FAILED( InitBlend() )){
		return E_FAIL;
	}

	return S_OK;
}

//�V�F�[�_������.
HRESULT	CDX9SkinMesh::InitShader()
{
	//D3D11�֘A�̏�����
	ID3DBlob *pCompiledShader = nullptr;
	ID3DBlob *pErrors = nullptr;
	UINT	uCompileFlag = 0;
#ifdef _DEBUG
	uCompileFlag = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION;
#endif//#ifdef _DEBUG

	//�u���u����o�[�e�b�N�X�V�F�[�_�[�쐬.
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"VS_Main", "vs_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) ){
		int size = pErrors->GetBufferSize();
		char* ch = (char*)pErrors->GetBufferPointer();
		MessageBox( 0, "hlsl�ǂݍ��ݎ��s", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );

	if( FAILED(
		m_pDevice11->CreateVertexShader( pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), NULL, &m_pVertexShader ) ) ){
		SAFE_RELEASE( pCompiledShader );
		MessageBox( 0, "�o�[�e�b�N�X�V�F�[�_�[�쐬���s", NULL, MB_OK );
		return E_FAIL;
	}
	//���_�C���v�b�g���C�A�E�g���`	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_INDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE_WEIGHT",0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof( layout ) / sizeof( layout[0] );

	//���_�C���v�b�g���C�A�E�g���쐬
	if( FAILED(
		m_pDevice11->CreateInputLayout(
			layout, numElements, pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(), &m_pVertexLayout ) ) ){
		return FALSE;
	}
	//���_�C���v�b�g���C�A�E�g���Z�b�g
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//�u���u����s�N�Z���V�F�[�_�[�쐬
	if( FAILED(
		D3DX11CompileFromFile(
			SHADER_NAME, nullptr, nullptr,
			"PS_Main", "ps_5_0",
			uCompileFlag, 0, nullptr,
			&pCompiledShader, &pErrors, nullptr ) ) ){
		MessageBox( 0, "hlsl�ǂݍ��ݎ��s", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pErrors );
	if( FAILED(
		m_pDevice11->CreatePixelShader(
			pCompiledShader->GetBufferPointer(),
			pCompiledShader->GetBufferSize(),
			NULL, &m_pPixelShader ) ) ){
		SAFE_RELEASE( pCompiledShader );
		MessageBox( 0, "�s�N�Z���V�F�[�_�[�쐬���s", NULL, MB_OK );
		return E_FAIL;
	}
	SAFE_RELEASE( pCompiledShader );

	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED( CreateCBuffer( &m_pCBufferPerMesh, sizeof( CBUFFER_PER_MESH ) ) ) ){
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED( CreateCBuffer( &m_pCBufferPerMaterial, sizeof( CBUFFER_PER_MATERIAL ) ) ) ){
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED( CreateCBuffer( &m_pCBufferPerFrame, sizeof( cbPerObject ) ) ) ){
		return E_FAIL;
	}
	//�R���X�^���g�o�b�t�@(���b�V������).
	if( FAILED( CreateCBuffer( &m_pCBufferPerBone, sizeof( CBUFFER_PER_BONES ) ) ) ){
		return E_FAIL;
	}

	//�e�N�X�`���[�p�T���v���[�쐬.
	if( FAILED( CreateLinearSampler( &m_pSampleLinear ) ) ){
		return E_FAIL;
	}

	return S_OK;
}



//X�t�@�C������X�L���֘A�̏���ǂݏo���֐�.
HRESULT CDX9SkinMesh::ReadSkinInfo(
	MYMESHCONTAINER* pContainer, MY_SKINVERTEX* pvVB, SKIN_PARTS_MESH* pParts )
{
	//X�t�@�C�����璊�o���ׂ����́A
	//�u���_���Ƃ��ްݲ��ޯ���v�u���_���Ƃ̃{�[���E�F�C�g�v.
	//�u�o�C���h�s��v�u�|�[�Y�s��v��4����.

	int i, k, m, n;	//�e�탋�[�v�ϐ�.
	int iNumVertex = 0;	//���_��.
	int iNumBone = 0;	//�{�[����.

	//���_��.
	iNumVertex = m_pD3dxMesh->GetNumVertices( pContainer );
	//�{�[����.
	iNumBone = m_pD3dxMesh->GetNumBones( pContainer );

	//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�.
	//��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���.
	for( i = 0; i < iNumBone; i++ ){
		//���̃{�[���ɉe�����󂯂钸�_��.
		int iNumIndex
			= m_pD3dxMesh->GetNumBoneVertices( pContainer, i );

		int*	pIndex = new int[iNumIndex]();
		double*	pWeight = new double[iNumIndex]();

		for( k = 0; k < iNumIndex; k++ ){
			pIndex[k]
				= m_pD3dxMesh->GetBoneVerticesIndices( pContainer, i, k );
			pWeight[k]
				= m_pD3dxMesh->GetBoneVerticesWeights( pContainer, i, k );
		}

		//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������.
		for( k = 0; k < iNumIndex; k++ ){
			//X�t�@�C����CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ�.
			//5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi��.

			//�E�F�C�g�̑傫�����Ƀ\�[�g(�o�u���\�[�g).
			for( m = 4; m > 1; m-- ){
				for( n = 1; n < m; n++ ){
					if( pvVB[pIndex[k]].bBoneWeight[n - 1] < pvVB[pIndex[k]].bBoneWeight[n] ){
						float tmp = pvVB[pIndex[k]].bBoneWeight[n - 1];
						pvVB[pIndex[k]].bBoneWeight[n - 1] = pvVB[pIndex[k]].bBoneWeight[n];
						pvVB[pIndex[k]].bBoneWeight[n] = tmp;
						int itmp = pvVB[pIndex[k]].bBoneIndex[n - 1];
						pvVB[pIndex[k]].bBoneIndex[n - 1] = pvVB[pIndex[k]].bBoneIndex[n];
						pvVB[pIndex[k]].bBoneIndex[n] = itmp;
					}
				}
			}
			//�\�[�g��́A�Ō�̗v�f�Ɉ�ԏ������E�F�C�g�������Ă�͂�.
			bool flag = false;
			for( m = 0; m < 4; m++ ){
				if( pvVB[pIndex[k]].bBoneWeight[m] == 0 ){
					flag = true;
					pvVB[pIndex[k]].bBoneIndex[m] = i;
					pvVB[pIndex[k]].bBoneWeight[m] = (float)pWeight[k];
					break;
				}
			}
			if( flag == false ){
				pvVB[pIndex[k]].bBoneIndex[3] = i;
				pvVB[pIndex[k]].bBoneWeight[3] = (float)pWeight[k];
				break;
			}
		}
		//�g���I���΍폜����.
		delete[] pIndex;
		delete[] pWeight;
	}

	//�{�[������.
	pParts->iNumBone = iNumBone;
	pParts->pBoneArray = new BONE[iNumBone]();
	//�|�[�Y�s��𓾂�(�����|�[�Y).
	for( i = 0; i < pParts->iNumBone; i++ ){
		pParts->pBoneArray[i].mBindPose
			= m_pD3dxMesh->GetBindPose( pContainer, i );
	}

	return S_OK;
}

//X����X�L�����b�V�����쐬����.
//	���Ӂj�f�ށiX)�̂ق��́A�O�p�|���S���ɂ��邱��.
HRESULT CDX9SkinMesh::LoadXMesh( const char* fileName )
{
	//�t�@�C�������p�X���Ǝ擾.
	strcpy_s( m_FilePath, sizeof( m_FilePath ), fileName );

	//X�t�@�C���ǂݍ���.
	m_pD3dxMesh = new D3DXPARSER();
	m_pD3dxMesh->LoadMeshFromX( m_pDevice9, fileName );


	//�S�Ẵ��b�V�����쐬����.
	BuildAllMesh( m_pD3dxMesh->m_pFrameRoot );

	return S_OK;
}


//Direct3D�̃C���f�b�N�X�o�b�t�@�[�쐬.
HRESULT CDX9SkinMesh::CreateIndexBuffer( DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer )
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = dwSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if( FAILED( m_pDevice11->CreateBuffer( &bd, &InitData, ppIndexBuffer ) ) ){
		return FALSE;
	}

	return S_OK;
}


//�����_�����O.
void CDX9SkinMesh::Render(
	D3DXMATRIX& mView,
	D3DXMATRIX& mProj,
	Light& stLight,
	const stCAMERA& stCamera,
	bool ChangeTexFlag,
	LPD3DXANIMATIONCONTROLLER pAC )
{
	m_mView = mView;
	m_mProj = mProj;
//	m_vLight = vLight;
//	m_vEye = vEye;

	if( pAC == nullptr ){
		if( m_pD3dxMesh->m_pAnimController ){
			m_pD3dxMesh->m_pAnimController->AdvanceTime( m_dAnimSpeed, NULL );

			m_dAnimTime = m_pD3dxMesh->m_pAnimController->GetTime();
		}
	} else {
		m_pD3dxMesh->m_pAnimController = pAC;
		m_pD3dxMesh->m_pAnimController->AdvanceTime( m_dAnimSpeed, NULL );
	}
	

	D3DXMATRIX m;
	D3DXMatrixIdentity( &m );
	m_pD3dxMesh->UpdateFrameMatrices( m_pD3dxMesh->m_pFrameRoot, &m );
	DrawFrame( m_pD3dxMesh->m_pFrameRoot, stLight, mView, mProj, ChangeTexFlag );
}


//�S�Ẵ��b�V�����쐬����.
void CDX9SkinMesh::BuildAllMesh( D3DXFRAME* pFrame )
{
	if( pFrame && pFrame->pMeshContainer ){
		CreateAppMeshFromD3DXMesh( pFrame );
	}

	//�ċA�֐�.
	if( pFrame->pFrameSibling != nullptr ){
		BuildAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr ){
		BuildAllMesh( pFrame->pFrameFirstChild );
	}
	
}

//���b�V���쐬.
HRESULT CDX9SkinMesh::CreateAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = (MYFRAME*)p;

	m_pMeshForRay = pFrame->pMeshContainer->MeshData.pMesh;
	//	LPD3DXMESH pD3DXMesh = pFrame->pMeshContainer->MeshData.pMesh;//D3DXү��(��������E�E�E).
	MYMESHCONTAINER* pContainer = (MYMESHCONTAINER*)pFrame->pMeshContainer;

	//�A�v�����b�V��(�E�E�E�����Ƀ��b�V���f�[�^���R�s�[����).
	SKIN_PARTS_MESH* pAppMesh = new SKIN_PARTS_MESH();
	pAppMesh->bTex = false;

	//���O�ɒ��_���A�|���S�������𒲂ׂ�.
	pAppMesh->dwNumVert = m_pD3dxMesh->GetNumVertices( pContainer );
	pAppMesh->dwNumFace = m_pD3dxMesh->GetNumFaces( pContainer );
	pAppMesh->dwNumUV = m_pD3dxMesh->GetNumUVs( pContainer );
	//Direct3D�ł�UV�̐��������_���K�v.
	if( pAppMesh->dwNumVert < pAppMesh->dwNumUV ){
		//���L���_���ŁA���_������Ȃ��Ƃ�.
		MessageBox( NULL,
			"Direct3D�́AUV�̐��������_���K�v�ł�(UV��u���ꏊ���K�v�ł�)�e�N�X�`���͐������\���Ȃ��Ǝv���܂�",
			"Error", MB_OK );
		return E_FAIL;
	}
	//�ꎞ�I�ȃ������m��(���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@).
	MY_SKINVERTEX* pvVB = new MY_SKINVERTEX[pAppMesh->dwNumVert]();
	int* piFaceBuffer = new int[pAppMesh->dwNumFace * 3]();
	//3���_�|���S���Ȃ̂ŁA1�t�F�C�X=3���_(3�C���f�b�N�X).

	//���_�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v = m_pD3dxMesh->GetVertexCoord( pContainer, i );
		pvVB[i].vPos.x = v.x;
		pvVB[i].vPos.y = v.y;
		pvVB[i].vPos.z = v.z;
	}
	//�|���S�����(���_�C���f�b�N�X)�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->dwNumFace * 3; i++ ){
		piFaceBuffer[i] = m_pD3dxMesh->GetIndex( pContainer, i );
	}
	//�@���ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR3 v = m_pD3dxMesh->GetNormal( pContainer, i );
		pvVB[i].vNorm.x = v.x;
		pvVB[i].vNorm.y = v.y;
		pvVB[i].vNorm.z = v.z;
	}
	//�e�N�X�`�����W�ǂݍ���.
	for( DWORD i = 0; i < pAppMesh->dwNumVert; i++ ){
		D3DXVECTOR2 v = m_pD3dxMesh->GetUV( pContainer, i );
		pvVB[i].vTex.x = v.x;
		pvVB[i].vTex.y = v.y;
	}
	//�}�e���A���ǂݍ���.
	pAppMesh->dwNumMaterial = m_pD3dxMesh->GetNumMaterials( pContainer );
	pAppMesh->pMaterial = new MY_SKINMATERIAL[pAppMesh->dwNumMaterial]();

	//�}�e���A���̐������C���f�b�N�X�o�b�t�@���쐬.
	pAppMesh->ppIndexBuffer = new ID3D11Buffer*[pAppMesh->dwNumMaterial]();
	//�|���Z�ł͂Ȃ��uID3D11Buffer*�v�̔z��Ƃ����Ӗ�.
	for( DWORD i = 0; i < pAppMesh->dwNumMaterial; i++ ){
		//����(�A���r�G���g).
		pAppMesh->pMaterial[i].Ka.x = m_pD3dxMesh->GetAmbient( pContainer, i ).y;
		pAppMesh->pMaterial[i].Ka.y = m_pD3dxMesh->GetAmbient( pContainer, i ).z;
		pAppMesh->pMaterial[i].Ka.z = m_pD3dxMesh->GetAmbient( pContainer, i ).w;
		pAppMesh->pMaterial[i].Ka.w = m_pD3dxMesh->GetAmbient( pContainer, i ).x;
		//�g�U���ˌ�(�f�B�t���[�Y).
		pAppMesh->pMaterial[i].Kd.x = m_pD3dxMesh->GetDiffuse( pContainer, i ).y;
		pAppMesh->pMaterial[i].Kd.y = m_pD3dxMesh->GetDiffuse( pContainer, i ).z;
		pAppMesh->pMaterial[i].Kd.z = m_pD3dxMesh->GetDiffuse( pContainer, i ).w;
		pAppMesh->pMaterial[i].Kd.w = m_pD3dxMesh->GetDiffuse( pContainer, i ).x;
		//���ʔ��ˌ�(�X�y�L����).
		pAppMesh->pMaterial[i].Ks.x = m_pD3dxMesh->GetSpecular( pContainer, i ).y;
		pAppMesh->pMaterial[i].Ks.y = m_pD3dxMesh->GetSpecular( pContainer, i ).z;
		pAppMesh->pMaterial[i].Ks.z = m_pD3dxMesh->GetSpecular( pContainer, i ).w;
		pAppMesh->pMaterial[i].Ks.w = m_pD3dxMesh->GetSpecular( pContainer, i ).x;

		//�e�N�X�`��(�f�B�t���[�Y�e�N�X�`���̂�).
		char* name = m_pD3dxMesh->GetTexturePath( pContainer, i );
		if( name ){
			char* ret = strrchr( m_FilePath, '\\' );
			if( ret != NULL ){
				int check = ret - m_FilePath;
				char path[512];
				strcpy_s( path, 512, m_FilePath );
				path[check + 1] = '\0';

				strcat_s( path, sizeof( path ), name );
				strcpy_s( pAppMesh->pMaterial[i].TextureName,
					sizeof( pAppMesh->pMaterial[i].TextureName ),
					path );
			}
		}
		//�e�N�X�`�����쐬.
		if( pAppMesh->pMaterial[i].TextureName[0] != 0
			&& FAILED(
				D3DX11CreateShaderResourceViewFromFileA(
					m_pDevice11, pAppMesh->pMaterial[i].TextureName,
					NULL, NULL, &pAppMesh->pMaterial[i].pTexture, NULL ) ) ){
			MessageBox( NULL, "�e�N�X�`���ǂݍ��ݎ��s",
				"Error", MB_OK );
			return E_FAIL;
		}
		//���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�.
		//����ɃC���f�b�N�X�̌��𒲂ׂ�.
		int iCount = 0;
		int* pIndex = new int[pAppMesh->dwNumFace * 3]();
		//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m��.
		//(�����̂ۂ育�񂮂�[�Ղ͕K������ȉ��ɂȂ�).

		for( DWORD k = 0; k < pAppMesh->dwNumFace; k++ ){
			//���� i==k �Ԗڂ̃|���S���̃}�e���A���ԍ��Ȃ�.
			if( i == m_pD3dxMesh->GeFaceMaterialIndex( pContainer, k ) ){
				//k�Ԗڂ̃|���S������钸�_�̃C���f�b�N�X.
				pIndex[iCount]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 0 );	//1��.
				pIndex[iCount + 1]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 1 );	//2��.
				pIndex[iCount + 2]
					= m_pD3dxMesh->GetFaceVertexIndex( pContainer, k, 2 );	//3��.
				iCount += 3;
			}
		}
		if( iCount > 0 ){
			//�C���f�b�N�X�o�b�t�@�쐬.
			CreateIndexBuffer( iCount * sizeof( int ),
				pIndex, &pAppMesh->ppIndexBuffer[i] );
		} else{
			//������̏����ɕs����o������.
			//�J�E���g����0�ȉ��̏ꍇ�́A�C���f�b�N�X�o�b�t�@�� nullptr �ɂ��Ă���.
			pAppMesh->ppIndexBuffer[i] = nullptr;
		}

		//���̃}�e���A�����̃|���S����.
		pAppMesh->pMaterial[i].dwNumFace = iCount / 3;
		//�s�v�ɂȂ����̂ō폜.
		delete[] pIndex;
	}

	//�X�L����񂠂�H
	if( pContainer->pSkinInfo == nullptr ){
		/*	char strDbg[128];
			sprintf_s( strDbg, "ContainerName:[%s]", pContainer->Name );
			MessageBox( nullptr, strDbg, "Not SkinInfo", MB_OK );*/
		pAppMesh->bEnableBones = false;
	} else{
		//�X�L�����(�W���C���g�A�E�F�C�g)�ǂݍ���.
		ReadSkinInfo( pContainer, pvVB, pAppMesh );
	}

	//�o�[�e�b�N�X�o�b�t�@���쐬.
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( MY_SKINVERTEX ) * ( pAppMesh->dwNumVert );
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pvVB;

	HRESULT hRslt = S_OK;
	if( FAILED(
		m_pDevice11->CreateBuffer(
			&bd, &InitData, &pAppMesh->pVertexBuffer ) ) ){
		hRslt = E_FAIL;
	}

	//�p�[�c���b�V���ɐݒ�.
	pFrame->pPartsMesh = pAppMesh;
	m_pReleaseMaterial = pAppMesh;

	//�ꎞ�I�ȓ��ꕨ�͕s�v�Ȃ�̂ō폜.
	if( piFaceBuffer ){
		delete[] piFaceBuffer;
	}
	if( pvVB ){
		delete[] pvVB;
	}

	return hRslt;
}


//�{�[�������̃|�[�Y�ʒu�ɃZ�b�g����֐�.
void CDX9SkinMesh::SetNewPoseMatrices(
	SKIN_PARTS_MESH* pParts, int frame, MYMESHCONTAINER* pContainer )
{
	//�]�ރt���[����Update���邱��.
	//���Ȃ��ƍs�񂪍X�V����Ȃ�.
	//m_pD3dxMesh->UpdateFrameMatrices(
	// m_pD3dxMesh->m_pFrameRoot)�������_�����O���Ɏ��s���邱��.

	//�܂��A�A�j���[�V�������ԂɌ��������s����X�V����̂�D3DXMESH�ł�
	//�A�j���[�V�����R���g���[����(����)����Ă������̂Ȃ̂ŁA
	//�A�j���[�V�����R���g���[�����g���ăA�j����i�s�����邱�Ƃ��K�v.
	//m_pD3dxMesh->m_pAnimController->AdvanceTime(...)��.
	//�����_�����O���Ɏ��s���邱��.

	if( pParts->iNumBone <= 0 ){
		//�{�[���� 0�@�ȉ��̏ꍇ.
	}

	for( int i = 0; i < pParts->iNumBone; i++ ){
		pParts->pBoneArray[i].mNewPose
			= m_pD3dxMesh->GetNewPose( pContainer, i );
	}
}


//����(���݂�)�|�[�Y�s���Ԃ��֐�.
D3DXMATRIX CDX9SkinMesh::GetCurrentPoseMatrix( SKIN_PARTS_MESH* pParts, int index )
{
	D3DXMATRIX ret =
		pParts->pBoneArray[index].mBindPose * pParts->pBoneArray[index].mNewPose;
	return ret;
}


//�t���[���̕`��.
VOID CDX9SkinMesh::DrawFrame( LPD3DXFRAME p, Light& stLight,
	D3DXMATRIX& mView, D3DXMATRIX& mProj, bool ChangeTexFlag )
{
	MYFRAME*			pFrame = (MYFRAME*)p;
	SKIN_PARTS_MESH*	pPartsMesh = pFrame->pPartsMesh;
	MYMESHCONTAINER*	pContainer = (MYMESHCONTAINER*)pFrame->pMeshContainer;

	if( pPartsMesh != nullptr ){
		DrawPartsMesh(
			pPartsMesh,
			pFrame->CombinedTransformationMatrix,
			pContainer, stLight, mView,  mProj, ChangeTexFlag );
	}

	//�ċA�֐�.
	//(�Z��)
	if( pFrame->pFrameSibling != nullptr ){
		DrawFrame( pFrame->pFrameSibling, stLight, mView, mProj, ChangeTexFlag );
	}
	//(�e�q)
	if( pFrame->pFrameFirstChild != nullptr ){
		DrawFrame( pFrame->pFrameFirstChild, stLight, mView, mProj, ChangeTexFlag );
	}
}

//�p�[�c���b�V����`��.
void CDX9SkinMesh::DrawPartsMesh( SKIN_PARTS_MESH* pMesh, D3DXMATRIX World, MYMESHCONTAINER* pContainer,
	Light& stLight, D3DXMATRIX& mView, D3DXMATRIX& mProj, bool ChangeTexFlag )
{
	D3D11_MAPPED_SUBRESOURCE pData;

	//�g�p����V�F�[�_�̃Z�b�g.
	m_pContext11->VSSetShader( m_pVertexShader, nullptr, 0 );
	m_pContext11->PSSetShader( m_pPixelShader, nullptr, 0 );


	D3DXMATRIX	mScale, mYaw, mPitch, mRoll, mTran;
	//�g�k.
	D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );
	D3DXMatrixRotationY( &mYaw, m_vRot.y );		//Y����].
	D3DXMatrixRotationX( &mPitch, m_vRot.x );	//X����].
	D3DXMatrixRotationZ( &mRoll, m_vRot.z );	//Z����].

	//=================================================================//

	//��]�s��(����)
	m_mRotation = mYaw * mPitch * mRoll;
	//���s�ړ�.
	D3DXMatrixTranslation( &mTran, m_vPos.x, m_vPos.y, m_vPos.z );
	//���[���h�s��.
	m_mWorld = mScale * m_mRotation * mTran;


	//�A�j���[�V�����t���[����i�߂� �X�L�����X�V.
	m_iFrame++;
	if( m_iFrame >= 3600 ){
		m_iFrame = 0;
	}
	SetNewPoseMatrices( pMesh, m_iFrame, pContainer );

	//------------------------------------------------.
	//	�R���X�^���g�o�b�t�@�ɏ��𑗂�(�{�[��).
	//------------------------------------------------.
	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerBone, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) ){
		CBUFFER_PER_BONES cb;
		for( int i = 0; i < pMesh->iNumBone; i++ ){
			D3DXMATRIX mat = GetCurrentPoseMatrix( pMesh, i );
			D3DXMatrixTranspose( &mat, &mat );
			cb.mBone[i] = mat;
		}
		memcpy_s( pData.pData, pData.RowPitch, (void*)&cb, sizeof( cb ) );
		m_pContext11->Unmap( m_pCBufferPerBone, 0 );
	}
	m_pContext11->VSSetConstantBuffers( 4, 1, &m_pCBufferPerBone );
	m_pContext11->PSSetConstantBuffers( 4, 1, &m_pCBufferPerBone );

	//�o�[�e�b�N�X�o�b�t�@���Z�b�g.
	UINT stride = sizeof( MY_SKINVERTEX );
	UINT offset = 0;
	m_pContext11->IASetVertexBuffers(
		0, 1, &pMesh->pVertexBuffer, &stride, &offset );

	//���_�C���v�b�g���C�A�E�g���Z�b�g.
	m_pContext11->IASetInputLayout( m_pVertexLayout );

	//�v���~�e�B�u�E�g�|���W�[���Z�b�g.
	m_pContext11->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	//------------------------------------------------.
	//	�R���X�^���g�o�b�t�@�ɏ���ݒ�(�t���[������).
	//------------------------------------------------.
	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerFrame, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pData ) ) ){
		cbPerObject cb;	//�ݽ����ޯ̧.
						//���[���h�s���n��.
		cb.World = m_mWorld;
		D3DXMatrixTranspose( &cb.World, &cb.World );

		//ܰ���,�ޭ�,��ۼު���ݍs���n��.
		D3DXMATRIX mWVP = m_mWorld * mView * mProj;
		D3DXMatrixTranspose( &mWVP, &mWVP );//�s���]�u����.
											//���s��̌v�Z���@��DirectX��GPU�ňقȂ邽�ߓ]�u���K�v.
		cb.WVP = mWVP;

		cb.light.Rot = stLight.Rot;
		D3DXMatrixTranspose( &cb.light.Rot, &cb.light.Rot );
		cb.light.dir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

		D3DXVec3TransformNormal( &cb.light.dir, &cb.light.dir, &cb.light.Rot );

		cb.light.range = stLight.range;
		cb.light.cone = stLight.cone;					//�l���ł����ƂȂ񂩏Ƃ炷�~���������Ȃ�܂�.
		cb.light.att = stLight.att;
		cb.light.ambient = stLight.ambient;
		cb.light.diffuse = stLight.diffuse;

		cb.light.pos.x = stLight.pos.x;
		cb.light.pos.y = stLight.pos.y;
		cb.light.pos.z = stLight.pos.z;

		cb.fAlpha = m_fAlpha;

		memcpy_s(
			pData.pData,	//��߰����ޯ̧.
			pData.RowPitch,	//��߰����ޯ̧����.
			(void*)( &cb ),	//��߰�����ޯ̧.
			sizeof( cb ) );	//��߰�����ޯ̧����.

		//�ޯ̧�����ް��̏��������I������Unmap.
		m_pContext11->Unmap( m_pCBufferPerFrame, 0 );
	}
	m_pContext11->VSSetConstantBuffers( 0, 1, &m_pCBufferPerFrame );
	m_pContext11->PSSetConstantBuffers( 0, 1, &m_pCBufferPerFrame );


	//------------------------------------------------.
	//	�R���X�^���g�o�b�t�@�ɏ���ݒ�(���b�V������).
	//------------------------------------------------.
	D3D11_MAPPED_SUBRESOURCE pDat;
	if( SUCCEEDED(
		m_pContext11->Map(
			m_pCBufferPerMesh, 0,
			D3D11_MAP_WRITE_DISCARD, 0, &pDat ) ) ){
		CBUFFER_PER_MESH cb;

		cb.mW = m_mWorld;
		D3DXMatrixTranspose( &cb.mW, &cb.mW );
		cb.mWVP = m_mWorld * m_mView * m_mProj;
		D3DXMatrixTranspose( &cb.mWVP, &cb.mWVP );

		memcpy_s( pDat.pData, pDat.RowPitch, (void*)&cb, sizeof( cb ) );
		m_pContext11->Unmap( m_pCBufferPerMesh, 0 );
	}
	m_pContext11->VSSetConstantBuffers( 1, 1, &m_pCBufferPerMesh );
	m_pContext11->PSSetConstantBuffers( 1, 1, &m_pCBufferPerMesh );


	//�}�e���A���̐������A
	//���ꂼ��̃}�e���A���̃C���f�b�N�X�o�b�t�@��`��.
	for( DWORD i = 0; i < pMesh->dwNumMaterial; i++ ){
		//�g�p����Ă��Ȃ��}�e���A���΍�.
		if( pMesh->pMaterial[i].dwNumFace == 0 ){
			continue;
		}
		//�C���f�b�N�X�o�b�t�@���Z�b�g.
		stride = sizeof( int );
		offset = 0;
		m_pContext11->IASetIndexBuffer(
			pMesh->ppIndexBuffer[i],
			DXGI_FORMAT_R32_UINT, 0 );

		//------------------------------------------------.
		//	�}�e���A���̊e�v�f�ƕϊ��s����V�F�[�_�ɓn��.
		//------------------------------------------------.
		D3D11_MAPPED_SUBRESOURCE pDat;
		if( SUCCEEDED(
			m_pContext11->Map(
				m_pCBufferPerMaterial, 0,
				D3D11_MAP_WRITE_DISCARD, 0, &pDat ) ) ){
			CBUFFER_PER_MATERIAL cb;

			cb.vAmbient = pMesh->pMaterial[i].Ka;
			cb.vDiffuse = pMesh->pMaterial[i].Kd;
			cb.vSpecular = pMesh->pMaterial[i].Ks;

			memcpy_s( pDat.pData, pDat.RowPitch, (void*)&cb, sizeof( cb ) );
			m_pContext11->Unmap( m_pCBufferPerMaterial, 0 );
		}
		m_pContext11->VSSetConstantBuffers( 2, 1, &m_pCBufferPerMaterial );
		m_pContext11->PSSetConstantBuffers( 2, 1, &m_pCBufferPerMaterial );

		//�e�N�X�`�����V�F�[�_�ɓn��.
		if( pMesh->pMaterial[i].TextureName != nullptr ){
			m_pContext11->PSSetSamplers( 0, 1, &m_pSampleLinear );

			if( ChangeTexFlag == false ){
				m_pContext11->PSSetShaderResources( 0, 1, &pMesh->pMaterial[i].pTexture );
			} else {
				ChangeTextureRender( pMesh, i );
			}
		} else {
			ID3D11ShaderResourceView* Nothing[1] = { 0 };
			m_pContext11->PSSetShaderResources( 0, 1, Nothing );
		}
		//Draw.
		m_pContext11->DrawIndexed( pMesh->pMaterial[i].dwNumFace * 3, 0, 0 );
	}
}

void CDX9SkinMesh::ChangeTextureRender( SKIN_PARTS_MESH* pMesh, int no )
{
	int str_length = lstrlen( pMesh->pMaterial[no].TextureName );
	int str_no = 0;
	bool change_flag = false;
	for( str_no = str_length; 0 < str_no; str_no-- ){
		if( pMesh->pMaterial[no].TextureName[str_no] == '\\' ){
			str_no++;	//�p�X�̋�؂�܂ŗ��Ă���̂ň�߂�.
			change_flag = true;
			break;
		}
	}
	if( change_flag == true ){
		//�Ώۃe�N�X�`�����Ȃ̂�����.
		if( strcmp(
			&pMesh->pMaterial[no].TextureName[str_no],
			m_pChangeTextures->szTargetTextureName ) == 0 ){
			//�ʂ̃e�N�X�`����ݒ�.
			m_pContext11->PSSetShaderResources( 0, 1, &m_pChangeTextures->pTexture );
		} else{
			m_pContext11->PSSetShaderResources( 0, 1, &pMesh->pMaterial[no].pTexture );
		}
	}
}

//����֐�.
HRESULT CDX9SkinMesh::Release()
{
	if( m_pD3dxMesh != nullptr ){
		//�S�Ẵ��b�V���폜.
		DestroyAllMesh( m_pD3dxMesh->m_pFrameRoot );

		//�p�[�T�[�N���X�������.
		m_pD3dxMesh->Release();
		SAFE_DELETE( m_pD3dxMesh );
	}

	return S_OK;
}


//�S�Ẵ��b�V�����폜.
void CDX9SkinMesh::DestroyAllMesh( D3DXFRAME* pFrame )
{
	if( ( pFrame != nullptr ) && ( pFrame->pMeshContainer != nullptr ) ){
		DestroyAppMeshFromD3DXMesh( pFrame );
	}

	//�ċA�֐�.
	if( pFrame->pFrameSibling != nullptr ){
		DestroyAllMesh( pFrame->pFrameSibling );
	}
	if( pFrame->pFrameFirstChild != nullptr ){
		DestroyAllMesh( pFrame->pFrameFirstChild );
	}
}


//���b�V���̍폜.
HRESULT CDX9SkinMesh::DestroyAppMeshFromD3DXMesh( LPD3DXFRAME p )
{
	MYFRAME* pFrame = (MYFRAME*)p;

	MYMESHCONTAINER* pMeshContainerTmp = (MYMESHCONTAINER*)pFrame->pMeshContainer;

	//MYMESHCONTAINER�̒��g�̉��.
	if( pMeshContainerTmp != nullptr ){
		if( pMeshContainerTmp->pBoneBuffer != nullptr ){
			pMeshContainerTmp->pBoneBuffer->Release();
			pMeshContainerTmp->pBoneBuffer = nullptr;
		}

		if( pMeshContainerTmp->pBoneOffsetMatrices != nullptr ){
			delete pMeshContainerTmp->pBoneOffsetMatrices;
			pMeshContainerTmp->pBoneOffsetMatrices = nullptr;
		}

		if( pMeshContainerTmp->ppBoneMatrix != nullptr ){
			int iMax = static_cast<int>( pMeshContainerTmp->pSkinInfo->GetNumBones() );

			for( int i = iMax - 1; i >= 0; i-- ){
				if( pMeshContainerTmp->ppBoneMatrix[i] != nullptr ){
					pMeshContainerTmp->ppBoneMatrix[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppBoneMatrix;
			pMeshContainerTmp->ppBoneMatrix = nullptr;
		}

		if( pMeshContainerTmp->ppTextures != nullptr ){
			int iMax = static_cast<int>( pMeshContainerTmp->NumMaterials );

			for( int i = iMax - 1; i >= 0; i-- ){
				if( pMeshContainerTmp->ppTextures[i] != nullptr ){
					pMeshContainerTmp->ppTextures[i]->Release();
					pMeshContainerTmp->ppTextures[i] = nullptr;
				}
			}

			delete[] pMeshContainerTmp->ppTextures;
			pMeshContainerTmp->ppTextures = nullptr;
		}
	}

	pMeshContainerTmp = nullptr;

	//MYMESHCONTAINER�������.

	//LPD3DXMESHCONTAINER�̉��.
	if( pFrame->pMeshContainer->pAdjacency != nullptr ){
		delete[] pFrame->pMeshContainer->pAdjacency;
		pFrame->pMeshContainer->pAdjacency = nullptr;
	}

	if( pFrame->pMeshContainer->pEffects != nullptr ){
		if( pFrame->pMeshContainer->pEffects->pDefaults != nullptr ){
			if( pFrame->pMeshContainer->pEffects->pDefaults->pParamName != nullptr ){
				delete pFrame->pMeshContainer->pEffects->pDefaults->pParamName;
				pFrame->pMeshContainer->pEffects->pDefaults->pParamName = nullptr;
			}

			if( pFrame->pMeshContainer->pEffects->pDefaults->pValue != nullptr ){
				delete pFrame->pMeshContainer->pEffects->pDefaults->pValue;
				pFrame->pMeshContainer->pEffects->pDefaults->pValue = nullptr;
			}

			delete pFrame->pMeshContainer->pEffects->pDefaults;
			pFrame->pMeshContainer->pEffects->pDefaults = nullptr;
		}

		if( pFrame->pMeshContainer->pEffects->pEffectFilename != nullptr ){
			delete pFrame->pMeshContainer->pEffects->pEffectFilename;
			pFrame->pMeshContainer->pEffects->pEffectFilename = nullptr;
		}

		delete pFrame->pMeshContainer->pEffects;
		pFrame->pMeshContainer->pEffects = nullptr;
	}

	if( pFrame->pMeshContainer->pMaterials != nullptr ){
		int iMax = static_cast<int>( pFrame->pMeshContainer->NumMaterials );

		for( int i = iMax - 1; i >= 0; i-- ){
			delete[] pFrame->pMeshContainer->pMaterials[i].pTextureFilename;
			pFrame->pMeshContainer->pMaterials[i].pTextureFilename = nullptr;
		}

		delete[] pFrame->pMeshContainer->pMaterials;
		pFrame->pMeshContainer->pMaterials = nullptr;
	}

	if( pFrame->pMeshContainer->pNextMeshContainer != nullptr ){
		//���̃��b�V���R���e�i�[�̃|�C���^�[�����̂��Ƃ�����.
		//new�ō���邱�Ƃ͂Ȃ��͂��Ȃ̂ŁA����ōs����Ǝv��.
		pFrame->pMeshContainer->pNextMeshContainer = nullptr;
	}

	if( pFrame->pMeshContainer->pSkinInfo != nullptr ){
		pFrame->pMeshContainer->pSkinInfo->Release();
		pFrame->pMeshContainer->pSkinInfo = nullptr;
	}

	//LPD3DXMESHCONTAINER�̉������.

	//MYFRAME�̉��.

	if( pFrame->pPartsMesh != nullptr ){
		//�{�[�����̉��.
		if( pFrame->pPartsMesh->pBoneArray ){
			delete[] pFrame->pPartsMesh->pBoneArray;
			pFrame->pPartsMesh->pBoneArray = nullptr;
		}

		if( pFrame->pPartsMesh->pMaterial != nullptr ){
			int iMax = static_cast<int>( pFrame->pPartsMesh->dwNumMaterial );

			for( int i = iMax - 1; i >= 0; i-- ){
				if( pFrame->pPartsMesh->pMaterial[i].pTexture ){
					pFrame->pPartsMesh->pMaterial[i].pTexture->Release();
					pFrame->pPartsMesh->pMaterial[i].pTexture = nullptr;
				}
			}

			delete[] pFrame->pPartsMesh->pMaterial;
			pFrame->pPartsMesh->pMaterial = nullptr;
		}


		if( pFrame->pPartsMesh->ppIndexBuffer ){
			//�C���f�b�N�X�o�b�t�@���.
			for( DWORD i = 0; i < pFrame->pPartsMesh->dwNumMaterial; i++ ){
				if( pFrame->pPartsMesh->ppIndexBuffer[i] != nullptr ){
					pFrame->pPartsMesh->ppIndexBuffer[i]->Release();
					pFrame->pPartsMesh->ppIndexBuffer[i] = nullptr;
				}
			}
			delete[] pFrame->pPartsMesh->ppIndexBuffer;
		}

		//���_�o�b�t�@�J��.
		pFrame->pPartsMesh->pVertexBuffer->Release();
		pFrame->pPartsMesh->pVertexBuffer = nullptr;
	}

	//�p�[�c�}�e���A���J��.
	delete[] pFrame->pPartsMesh;
	pFrame->pPartsMesh = nullptr;

	//SKIN_PARTS_MESH�������.

	//MYFRAME��SKIN_PARTS_MESH�ȊO�̃����o�[�|�C���^�[�ϐ��͕ʂ̊֐��ŉ������Ă��܂���.

	return S_OK;
}



//�A�j���[�V�����Z�b�g�̐؂�ւ�.
void CDX9SkinMesh::ChangeAnimSet( int index, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr )	return;
	m_pD3dxMesh->ChangeAnimSet( index, pAC );
}


//�A�j���[�V�����Z�b�g�̐؂�ւ�(�J�n�t���[���w��\��).
void CDX9SkinMesh::ChangeAnimSet_StartPos( int index, double dStartFramePos, LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh == nullptr )	return;
	m_pD3dxMesh->ChangeAnimSet_StartPos( index, dStartFramePos, pAC );
}


//�A�j���[�V������~���Ԃ��擾.
double CDX9SkinMesh::GetAnimPeriod( int index )
{
	if( m_pD3dxMesh == nullptr ){
		return 0.0f;
	}
	return m_pD3dxMesh->GetAnimPeriod( index );
}


//�A�j���[�V���������擾.
int CDX9SkinMesh::GetAnimMax( LPD3DXANIMATIONCONTROLLER pAC )
{
	if( m_pD3dxMesh != nullptr ){
		return m_pD3dxMesh->GetAnimMax( pAC );
	}
	return -1;
}


//�w�肵���{�[�����(�s��)���擾����֐�.
bool CDX9SkinMesh::GetMatrixFromBone( const char* sBoneName, D3DXMATRIX* pOutMat )
{
	if( m_pD3dxMesh != nullptr ){
		if( m_pD3dxMesh->GetMatrixFromBone( sBoneName, pOutMat ) ){
			return true;
		}
	}
	return false;
}
//�w�肵���{�[�����(���W)���擾����֐�.
bool CDX9SkinMesh::GetPosFromBone( const char* sBoneName, D3DXVECTOR3* pOutPos )
{
	if( m_pD3dxMesh != nullptr ){
		D3DXVECTOR3 tmpPos;
		if( m_pD3dxMesh->GetPosFromBone( sBoneName, &tmpPos ) ){
			D3DXMATRIX mWorld, mScale, mTran;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );
			D3DXMatrixRotationY( &mYaw, m_vRot.y );
			D3DXMatrixRotationX( &mPitch, m_vRot.x );
			D3DXMatrixRotationZ( &mRoll, m_vRot.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_vPos.x;
			pOutPos->y = mWorld._42 + m_vPos.y;
			pOutPos->z = mWorld._43 + m_vPos.z;

			return true;
		}
	}
	return false;
}

bool CDX9SkinMesh::GetDeviaPosFromBone( const char* sBoneName, D3DXVECTOR3* pOutPos, D3DXVECTOR3 vSpecifiedPos )
{
	if( m_pD3dxMesh != nullptr ){
		D3DXMATRIX mtmp;
		if( m_pD3dxMesh->GetMatrixFromBone( sBoneName, &mtmp ) ){
			D3DXMATRIX mWorld, mScale, mTran, mDevia;
			D3DXMATRIX mRot, mYaw, mPitch, mRoll;
			//�����炭�{�[���̏����̌��������ʒu�ɂȂ��Ă���͂��ł�.
			D3DXMatrixTranslation( &mDevia, vSpecifiedPos.x, vSpecifiedPos.y, vSpecifiedPos.z );//���炵���������̍s����쐬.
			D3DXMatrixMultiply( &mtmp, &mDevia, &mtmp );//�{�[���ʒu�s��Ƃ��炵���������s����|�����킹��.
			D3DXVECTOR3 tmpPos = D3DXVECTOR3( mtmp._41, mtmp._42, mtmp._43 );//�ʒu�̂ݎ擾.

			D3DXMatrixScaling( &mScale, m_vScale.x, m_vScale.y, m_vScale.z );
			D3DXMatrixRotationY( &mYaw, m_vRot.y );
			D3DXMatrixRotationX( &mPitch, m_vRot.x );
			D3DXMatrixRotationZ( &mRoll, m_vRot.z );
			D3DXMatrixTranslation( &mTran, tmpPos.x, tmpPos.y, tmpPos.z );

			mRot = mYaw * mPitch * mRoll;
			mWorld = mTran * mScale * mRot;

			pOutPos->x = mWorld._41 + m_vPos.x;
			pOutPos->y = mWorld._42 + m_vPos.y;
			pOutPos->z = mWorld._43 + m_vPos.z;

			return true;
		}
	}
	return false;
}


//�R���X�^���g�o�b�t�@�쐬�֐�.
HRESULT CDX9SkinMesh::CreateCBuffer(
	ID3D11Buffer** pConstantBuffer,
	UINT size )
{
	D3D11_BUFFER_DESC cb;

	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = size;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;
	if( FAILED(
		m_pDevice11->CreateBuffer( &cb, NULL, pConstantBuffer ) ) ){
		return E_FAIL;
	}
	return S_OK;
}

//�T���v���[�쐬�֐�.
HRESULT CDX9SkinMesh::CreateLinearSampler( ID3D11SamplerState** pSampler )
{
	//�e�N�X�`���[�p�T���v���[�쐬.
	D3D11_SAMPLER_DESC SamDesc;
	ZeroMemory( &SamDesc, sizeof( D3D11_SAMPLER_DESC ) );

	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	if( FAILED(
		m_pDevice11->CreateSamplerState( &SamDesc, &m_pSampleLinear ) ) ){
		return E_FAIL;
	}
	return S_OK;
}


HRESULT CDX9SkinMesh::InitBlend()
{
	//��̧�����ޗp�����޽ðč\����.
	//pnģ�ٓ��ɱ�̧��񂪂���̂ŁA���߂���悤�������޽ðĂŐݒ肷��.
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory( &BlendDesc, sizeof( BlendDesc ) );	//������.

	BlendDesc.IndependentBlendEnable
		= false;	//false:RenderTarget[0]�����ް�̂ݎg�p����.
					//true:RenderTarget[0�`7]���g�p�ł���
					// (���ް���ޯĖ��ɓƗ����������ޏ���).
	BlendDesc.AlphaToCoverageEnable
		= true;		//true:��̧ĩ���ڰ�ނ��g�p����.
	BlendDesc.RenderTarget[0].BlendEnable
		= true;		//true:��̧�����ނ��g�p����.
	BlendDesc.RenderTarget[0].SrcBlend	//���f�ނɑ΂���ݒ�.
		= D3D11_BLEND_SRC_ALPHA;			//��̧�����ނ��w��.
	BlendDesc.RenderTarget[0].DestBlend	//�d�˂�f�ނɑ΂���ݒ�.
		= D3D11_BLEND_INV_SRC_ALPHA;		//��̧�����ނ̔��]���w��.
	BlendDesc.RenderTarget[0].BlendOp	//�����޵�߼��.
		= D3D11_BLEND_OP_ADD;				//ADD:���Z����.
	BlendDesc.RenderTarget[0].SrcBlendAlpha	//���f�ނ̱�̧�ɑ΂���w��.
		= D3D11_BLEND_ONE;						//���̂܂܎g�p.
	BlendDesc.RenderTarget[0].DestBlendAlpha	//�d�˂�f�ނ̱�̧�ɑ΂���ݒ�.
		= D3D11_BLEND_ZERO;							//�������Ȃ��B
	BlendDesc.RenderTarget[0].BlendOpAlpha	//��̧�������޵�߼��.
		= D3D11_BLEND_OP_ADD;					//ADD:���Z����.
	BlendDesc.RenderTarget[0].RenderTargetWriteMask	//�߸�ٖ��̏�������Ͻ�.
		= D3D11_COLOR_WRITE_ENABLE_ALL;			//�S�Ă̐���(RGBA)�ւ��ް��̊i�[��������.

	//�����޽ðč쐬.
	if( FAILED(
		m_pDevice11->CreateBlendState(
			&BlendDesc, &m_pAlphaBlend ) ) ){
		_ASSERT_EXPR( false, L"�����޽ðč쐬���s" );
		return E_FAIL;
	}

	//�����޽ðč쐬.
	BlendDesc.RenderTarget[0].BlendEnable
		= false;	//false:��̧�����ނ��g�p���Ȃ�.
	if( FAILED(
		m_pDevice11->CreateBlendState(
			&BlendDesc, &m_pNoAlphaBlend ) ) ){
		_ASSERT_EXPR( false, L"�����޽ðč쐬���s" );
		return E_FAIL;
	}

	return S_OK;
}

//���ߐݒ�̐؂�ւ�.
void CDX9SkinMesh::SetBlend( bool EnableAlpha )
{
	//�����޽ðĂ̐ݒ�.
	UINT mask = 0xffffffff;	//Ͻ��l.
	if( EnableAlpha == true ){
		m_pContext11->OMSetBlendState(
			m_pAlphaBlend, nullptr, mask );
	} else{
		m_pContext11->OMSetBlendState(
			m_pNoAlphaBlend, nullptr, mask );
	}
}