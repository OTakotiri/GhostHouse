//ｸﾞﾛｰﾊﾞﾙ変数.

//ﾃｸｽﾁｬは ﾚｼﾞｽﾀ t(n).
Texture2D		g_Texture	: register(t0);
//ｻﾝﾌﾟﾗは ﾚｼﾞｽﾀ s(n).
SamplerState	g_samLinear	: register(s0);

//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
cbuffer global
{
	matrix	g_mW;	//ﾜｰﾙﾄﾞ行列.
	matrix	g_mWVP;	//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成行列.
	float4	g_vLightDir;	//ﾗｲﾄの方向ﾍﾞｸﾄﾙ.
	float4	g_vDiffuse;		//ﾃﾞｨﾌｭｰｽﾞ色(拡散反射色).
	float4	g_vSpecular;	//ｽﾍﾟｷｭﾗ色(鏡面反射色).
	float4	g_vCamPos;		//ｶﾒﾗ位置(視点位置).
};

//頂点ｼｪｰﾀﾞの出力ﾊﾟﾗﾒｰﾀ.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float4	PosLocal	: POSITION;
	float4	Normal		: NORMAL;
	float2	Tex			: TEXCOORD;
};

//-------------------------------------------------
//	頂点(ﾊﾞｰﾃｯｸｽ)ｼｪｰﾀﾞ.
//-------------------------------------------------
VS_OUTPUT VS_Main(float4 Pos : POSITION,
	float4 Norm : NORMAL,
	float2 Tex : TEXCOORD)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, g_mWVP);
	output.PosLocal = Pos;
	output.Normal = Norm;
	output.Tex = Tex;

	return output;
}

//*************************************************
//	ﾗｽﾀﾗｲｻﾞ(ｱﾌﾟﾘ側で設定する項目。ここでは触れない).
//*************************************************

//-------------------------------------------------
//	ﾋﾟｸｾﾙｼｪｰﾀﾞ.
//-------------------------------------------------
float4 PS_Main(VS_OUTPUT input) : SV_Target
{
	float3 PosWorld = mul(input.PosLocal, g_mW);
	float3 EyeVector = g_vCamPos - PosWorld;

	float3 Normal = normalize(mul(input.Normal, (float3x3)g_mW));
	float3 LightDir = normalize(g_vLightDir);
	float3 ViewDir = normalize(EyeVector);
	float4 NL = saturate(dot(Normal, LightDir));

	float3 Reflect = normalize(2 * NL*Normal - LightDir);
	float4 Specular = pow(saturate(dot(Reflect, ViewDir)), 1);

	float4 Color =
		g_Texture.Sample(g_samLinear, input.Tex) / 2.0f +
		(g_vDiffuse * NL + Specular * g_vSpecular) / 2.0f;

	return Color;
}