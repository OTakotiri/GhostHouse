//ｸﾞﾛｰﾊﾞﾙ変数.
//ﾃｸｽﾁｬは、ﾚｼﾞｽﾀ t(n).
Texture2D		g_Texture : register( t0 );
//ｻﾝﾌﾟﾗは、ﾚｼﾞｽﾀ s(n).
SamplerState	g_samLinear		: register( s0 );
SamplerState	g_samLinear1	: register( s1 );

//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
cbuffer global : register(b0)
{
	matrix	g_mW			: packoffset(c0);	// ﾜｰﾙﾄﾞ行列.
	float	g_fViewPortW	: packoffset(c4);	// ビューポート幅.
	float	g_fViewPortH	: packoffset(c5);	// ビューポート高さ.
	float	g_fAlpha		: packoffset(c6);	// アルファ値.
	float2	g_vUV			: packoffset(c7);	// UV座標.
};

//構造体.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float2	Tex			: TEXCOORD;
};

// 頂点シェーダ.
VS_OUTPUT VS_Main( float4 Pos : POSITION,
				   float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul( Pos, g_mW );

	// スクリーン座標に合わせる計算,
	output.Pos.x = ( output.Pos.x / g_fViewPortW ) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - ( output.Pos.y / g_fViewPortH ) * 2.0f;

	output.Tex = Tex;

	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

VS_OUTPUT VS_Main2( float4 Pos : POSITION,
					float2 Tex : TEXCOORD )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Pos = mul( Pos, g_mW );

	// スクリーン座標に合わせる計算,
	output.Pos.x = ( output.Pos.x / g_fViewPortW ) * 2.0f - 1.0f;
	output.Pos.y = 1.0f - ( output.Pos.y / g_fViewPortH ) * 2.0f;

	output.Tex = Tex;

	// UV座標をずらす.
	output.Tex.x += g_vUV.x;
	output.Tex.y += g_vUV.y;

	return output;
}

// ピクセルシェーダ.
float4 PS_Main( VS_OUTPUT input ) : SV_Target
{
	float4 color = g_Texture.Sample( g_samLinear, input.Tex );
//	color.a *= g_fAlpha;	// アルファ値を掛け合わせる.
	color = color * g_fAlpha + color * ( 1 - color.a );
	
	return color;
}