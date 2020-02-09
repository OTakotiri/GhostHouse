#include "CEffect.h"

//’è”éŒ¾.
//•`‰æ—p²Ý½ÀÝ½(½Ìß×²Ä)Å‘å”.
const int RENDER_SPRITE_MAX = 2000;
//´Ìª¸ÄŠÇ——p²Ý½ÀÝ½Å‘å”.
const int EFFECT_INSTANCE_MAX = 1000;

//ºÝ½Ä×¸À.
CEffect::CEffect()
	: m_pManager	( nullptr )
	, m_pRenderer	( nullptr )
	, m_pEffect		( nullptr )
#ifdef ENABLE_XAUDIO2
	, m_pSound(nullptr)
	, m_pXA2(nullptr)
	, m_pXA2Master(nullptr)
#endif//#ifdef ENABLE_XAUDIO2
{
}

CEffect::CEffect( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const std::string& fileName )
	: CEffect	()
{
	if( FAILED( Init( pDevice11, pContext11,  fileName )) ){
		_ASSERT_EXPR( false, L"Effekseer‰Šú‰»Ž¸”s" );
	}
}

//ÃÞ½Ä×¸À.
CEffect::~CEffect()
{
	Destroy();
}

//‰Šú‰».
HRESULT CEffect::Init( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11, const std::string& fileName )
{
	if( FAILED( Create( pDevice11, pContext11 )) ){
		_ASSERT_EXPR( false, L"Effekseer\’zŽ¸”s" );
		return E_FAIL;
	}
	if( FAILED( LoadData( fileName )) ){
		_ASSERT_EXPR( false, L"Effekseer“Ç‚Ýž‚ÝŽ¸”s" );
		return E_FAIL;
	}
	return S_OK;
}

//\’z.
HRESULT CEffect::Create( ID3D11Device* pDevice11, ID3D11DeviceContext* pContext11 )
{
#ifdef ENABLE_XAUDIO2
	//XAudio2‚Ì‰Šú‰»‚ðs‚¤.
	if (FAILED(
		XAudio2Create(&m_pXA2)))
	{
		_ASSERT_EXPR(false, "XAudio2ì¬Ž¸”s");
		return E_FAIL;
	}
	if (FAILED(
		m_pXA2->CreateMasteringVoice(&m_pXA2Master)))
	{
		_ASSERT_EXPR(false, "MasteringVoiceì¬Ž¸”s");
		return E_FAIL;
	}
#endif/#ifdef ENABLE_XAUDIO2
	
	//•`‰æ—p²Ý½ÀÝ½‚Ì¶¬.
	m_pRenderer
		= ::EffekseerRendererDX11::Renderer::Create(
			pDevice11, pContext11, RENDER_SPRITE_MAX);

	//´Ìª¸ÄŠÇ——p²Ý½ÀÝ½‚Ì¶¬.
	m_pManager
		= ::Effekseer::Manager::Create(EFFECT_INSTANCE_MAX);

	//•`‰æ—p²Ý½ÀÝ½‚©‚ç•`‰æ‹@”\‚ðÝ’è.
	m_pManager->SetSpriteRenderer(m_pRenderer->CreateSpriteRenderer());
	m_pManager->SetRibbonRenderer(m_pRenderer->CreateRibbonRenderer());
	m_pManager->SetRingRenderer(m_pRenderer->CreateRingRenderer());
	m_pManager->SetModelRenderer(m_pRenderer->CreateModelRenderer());

	//•`‰æ—p²Ý½ÀÝ½‚©‚çÃ¸½Á¬‚Ì“Ç‚Ýž‚Ý‹@”\‚ðÝ’è.
	//“ÆŽ©Šg’£‰Â”\AŒ»Ý‚ÍÌ§²Ù‚©‚ç“Ç‚Ýž‚ñ‚Å‚¢‚é.
	m_pManager->SetTextureLoader(m_pRenderer->CreateTextureLoader());
	m_pManager->SetModelLoader(m_pRenderer->CreateModelLoader());

#ifdef ENABLE_XAUDIO2
	//‰¹‚ÌÄ¶—p²Ý½ÀÝ½‚Ì¶¬.
	m_pSound = ::EffekseerSound::Sound::Create(m_pXA2, 16, 16);

	//‰¹‚ÌÄ¶—p²Ý½ÀÝ½‚©‚çÄ¶‹@”\‚ðÝ’è.
	m_pManager->SetSoundPlayer(m_pSound->CreateSoundPlayer());

	//‰¹‚ÌÄ¶—p²Ý½ÀÝ½‚©‚ç»³ÝÄÞÃÞ°À‚Ì“Ç‚Ýž‚Ý‹@”\‚ðÝ’è.
	//“ÆŽ©Šg’£‰Â”\AŒ»Ý‚ÍÌ§²Ù‚©‚ç“Ç‚Ýž‚ñ‚Å‚¢‚é.
	m_pManager->SetSoundLoader(m_pSound->CreateSoundLoader());
#endif//#ifdef ENABLE_XAUDIO2

	return S_OK;
}


//”jŠü.
void CEffect::Destroy()
{
	//´Ìª¸ÄÃÞ°À‚Ì‰ð•ú.
	ReleaseData();

	//æ‚É´Ìª¸ÄŠÇ——p²Ý½ÀÝ½‚ð”jŠü.
	m_pManager->Destroy();

#ifdef ENABLE_XAUDIO2
	//ŽŸ‚É‰¹‚ÌÄ¶—p²Ý½ÀÝ½‚ð”jŠü.
	m_pSound->Destroy();
#endif//#ifdef ENABLE_XAUDIO2

	//ŽŸ‚É•`‰æ—p²Ý½ÀÝ½‚ð”jŠü.
	m_pRenderer->Destroy();

#ifdef ENABLE_XAUDIO2
	//XAudio2‚Ì‰ð•ú.
	if (m_pXA2Master != nullptr) {
		m_pXA2Master->DestroyVoice();
		m_pXA2Master = nullptr;
	}
	if( m_pXA2 != nullptr ){
		m_pXA2->Release();
		m_pXA2 = nullptr;
	}
#endif//#ifdef ENABLE_XAUDIO2
}


//ÃÞ°À“Ç‚Ýž‚Ý.
HRESULT CEffect::LoadData( const std::string& fileName )
{
	// •¶Žš•ÏŠ·.
	const size_t charSize = fileName.length() + 1;	// “ü—Í•¶Žš‚ÌƒTƒCƒY+1‚ðŽæ“¾.
	wchar_t* FileName = nullptr;
	FileName = new wchar_t[charSize];				// wchar_tŒ^‚ð"charSize"•ªŠm•Û.
	size_t ret;										// •ÏŠ·Œã‚Ìwchar_t‚ÌƒTƒCƒYŽæ“¾—p.
	mbstowcs_s( &ret, FileName, charSize, fileName.c_str(), _TRUNCATE );

	//´Ìª¸Ä‚Ì“Ç‚Ýž‚Ý.
	m_pEffect = Effekseer::Effect::Create( m_pManager, (const EFK_CHAR*)FileName );

	if (m_pEffect == nullptr) {
		_ASSERT_EXPR(false, L"´Ìª¸Ä“Ç‚Ýž‚ÝŽ¸”s");
		return E_FAIL;
	}

	delete[] FileName;

	return S_OK;
}

//ÃÞ°À‰ð•ú.
void CEffect::ReleaseData()
{
	//´Ìª¸Ä‚Ì‰ð•ú.
	if( m_pEffect != nullptr ){
		m_pEffect->Release();
		m_pEffect = nullptr;
	}
}

//•`‰æŠÖ”.
void CEffect::Render( 
	D3DXMATRIX& mView,
	D3DXMATRIX& mProj,
	const Effekseer::Handle& eHandle )
{
	if( eHandle == -1 ) return;
	//ËÞ­°s—ñ‚ðÝ’è.
	SetViewMatrix(mView);

	//ÌßÛ¼Þª¸¼®Ýs—ñ‚ðÝ’è.
	SetProjectionMatrix(mProj);

	//´Ìª¸Ä‚ÌXVˆ—.
	m_pManager->BeginUpdate();
	m_pManager->UpdateHandle( eHandle );
	m_pManager->EndUpdate();

	//´Ìª¸Ä‚Ì•`‰æŠJŽnˆ—.
	m_pRenderer->BeginRendering();
	m_pManager->DrawHandle( eHandle );
	m_pRenderer->EndRendering();
}


//DirectX D3DXVECTOR3 -> Effekseer Vector3 ‚É•ÏŠ·‚·‚é.
Effekseer::Vector3D	CEffect::ConvertToVec3Efk( const D3DXVECTOR3& SrcVec3Dx ) const
{
	return Effekseer::Vector3D(
		SrcVec3Dx.x, SrcVec3Dx.y, SrcVec3Dx.z);
}

//Effekseer Vector3 -> DirectX D3DXVECTOR3 ‚É•ÏŠ·‚·‚é.
D3DXVECTOR3 CEffect::ConvertToVec3Dx( const Effekseer::Vector3D& SrcVec3Efk) const
{
	return D3DXVECTOR3(SrcVec3Efk.X, SrcVec3Efk.Y, SrcVec3Efk.Z);
}

//DirectX D3DXMATRIX -> Effekseer Matrix44 ‚É•ÏŠ·‚·‚é.
Effekseer::Matrix44 CEffect::ConvertToMatEfk( const D3DXMATRIX & SrcMatDx) const
{
	Effekseer::Matrix44 DestMatEfk;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DestMatEfk.Values[i][j] = SrcMatDx.m[i][j];
		}
	}

	return DestMatEfk;
}

//Effekseer Matrix44 -> DirectX D3DXMATRIX ‚É•ÏŠ·‚·‚é.
D3DXMATRIX CEffect::ConvertToMatDx( const Effekseer::Matrix44& SrcMatEfk ) const
{
	D3DXMATRIX DestMatDx;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			DestMatDx.m[i][j] = SrcMatEfk.Values[i][j];
		}
	}

	return DestMatDx;
}
//ËÞ­°s—ñ‚ðÝ’è.
void CEffect::SetViewMatrix(D3DXMATRIX& mView)
{
	Effekseer::Matrix44 mViewEfk = ConvertToMatEfk(mView);

	//¶Ò×s—ñ‚ðÝ’è.
	m_pRenderer->SetCameraMatrix(mViewEfk);
}

//ÌßÛ¼Þª¸¼®Ýs—ñ‚ðÝ’è.
void CEffect::SetProjectionMatrix(D3DXMATRIX& mProj)
{
	Effekseer::Matrix44 mProjEfk = ConvertToMatEfk(mProj);

	//ÌßÛ¼Þª¸¼®Ýs—ñ‚ðÝ’è.
	m_pRenderer->SetProjectionMatrix(mProjEfk);
}
