//ｸﾞﾛｰﾊﾞﾙ変数.

//ﾃｸｽﾁｬは、ﾚｼﾞｽﾀ t(n).
Texture2D		g_Texture : register(t0);
//ｻﾝﾌﾟﾗは、ﾚｼﾞｽﾀ s(n).
SamplerState	g_samLinear : register(s0);

//ｺﾝｽﾀﾝﾄﾊﾞｯﾌｧ.
cbuffer global
{
	matrix	g_mW;		//ﾜｰﾙﾄﾞ行列.
	matrix	g_mWVP;		//ﾜｰﾙﾄﾞ,ﾋﾞｭｰ,ﾌﾟﾛｼﾞｪｸｼｮﾝの合成行列.
	float4	g_vLightDir;//ﾗｲﾄの方向(ﾍﾞｸﾄﾙ).
	float4	g_vColor;	//ﾃﾞｨﾌｭｰｽﾞ色(拡散反射色). 
	float4	g_vCamPos;	//ｶﾒﾗ位置(視点位置).
};

//構造体.
struct VS_OUTPUT
{
	float4	Pos			: SV_Position;
	float4	Color		: COLOR;
	float3	Light		: TEXCOORD0;
	float3	Normal		: TEXCOORD1;
	float3	EyeVector	: TEXCOORD2;
	float2	Tex			: TEXCOORD3;
};

//-------------------------------------------------
//	頂点(ﾊﾞｰﾃｯｸｽ)ｼｪｰﾀﾞ.
//-------------------------------------------------
VS_OUTPUT VS(float4 Pos		: POSITION,	//位置.
			 float4 Normal	: NORMAL,	//法線.
			 float2 Tex		: TEXCOORD )//ﾃｸｽﾁｬ.
{
	VS_OUTPUT output = (VS_OUTPUT)0;	//0で初期化.

	//mul(A,B) : AとBの掛け算.
	output.Pos = mul(Pos, g_mWVP);

	//光沢.
	//板ﾎﾟﾘに法線を掛ける.
	output.Normal = mul(Normal, (float3x3)g_mW);
	output.Light = g_vLightDir;
	float3 PosWorld = mul(Pos, g_mW);
	output.EyeVector = g_vCamPos - PosWorld;

	output.Tex = Tex;

	return output;
}

//*************************************************
//	ﾗｽﾀﾗｲｻﾞ(さわれない).
//*************************************************

//-------------------------------------------------
//	ﾋﾟｸｾﾙｼｪｰﾀﾞ.
//-------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{

	//光沢.
	float3 Normal	= normalize(input.Normal);
	float3 LightDir	= normalize(input.Light);
	float3 ViewDir	= normalize(input.EyeVector);
	float4 NL		= saturate(dot(Normal, LightDir));

	//Reflect:反射.
	// 面の反射値を求める.
	float3 Reflect	= normalize(2 * NL * Normal - LightDir);
	float4 Specular = 2 * pow(saturate(dot(Reflect, ViewDir)), 2);

	float4 Color = g_Texture.Sample(g_samLinear, input.Tex);//色を返す.
//	Color = Color * NL + Specular;
//	Color = (Color * NL + Specular)*g_vColor;
	Color = Color + Color * Specular;

	return Color;
}

//normalize()	: 正規化する.
//saturate()	: 0～1にｸﾗﾝﾌﾟする(強制的に1に納める).
//dot( A, B )	: AとBの内積を求める.
//pow( A, B )	: AのB乗を返す.

//Specular(ｽﾍﾟｷｭﾗ)	: 反射色.
//Ambient(ｱﾝﾋﾞｴﾝﾄ)	: 環境色.
//Diffuse(ﾃﾞｨﾌｭｰｽﾞ)	: 拡散反射色.
//Emissive(ｴﾐｯｼﾌﾞ)	: 自己発光色.

