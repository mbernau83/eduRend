Texture2D texDiffuse : register(t0);


//Databinding that acts as a global variable inside the HLSL

cbuffer TransformBufferCam : register(b0)
{
	matrix CamModelToWorldMatrix;
	matrix CamWorldToViewMatrix;
	matrix CamProjectionMatrix;
};

cbuffer TransformBufferLight : register(b1)
{
    matrix LightModelToWorldMatrix;
    matrix LightWorldToViewMatrix;
    matrix LightProjectionMatrix;
};

cbuffer MaterialBuffer : register(b2)
{
    float4 ka;
    float4 kd;
    float4 ks;
	
};

struct PSIn
{
	float4 Pos  : SV_Position;
    float3 PosWorld : World_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1

    float3 lightPos = { 0, 6, 0 };

    return float4(lightPos * 0.5 + 0.5, 1);

	////////////////// Phong : vektorkomponenter F6 sida 28
    //phong is Iphong = Iambient + Idiffuse + Ispecular;

	//Ytnomral (N), Ljusvektor (L), L reflekterad i N (R), vy-vektor (V), glans (alpha/A)
    //--saturate function clamps a value between 0 and 1;
    //https://youtu.be/VrF43BT4Yyc

    float3 placeHoderVec3 = { 0, 0, 0, };
    float3 N = input.Normal;

    float3 L = normalize(placeHoderVec3 /*the light vector goes here*/);
    float3 V = normalize(placeHoderVec3 /*the eye vector goes here*/);
    float3 R = -reflect(L, N); //a reflection vector
    float RdotV = saturate(dot(R, V)); //Viewing angle influence on specular highlight

    


	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return float4(input.TexCoord, 0, 1);
}