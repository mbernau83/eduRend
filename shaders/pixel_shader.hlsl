Texture2D texDiffuse : register(t0);


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

struct PSIn
{
	float4 Pos  : SV_Position;
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

    return float4(float3(0, 6, 0) * 0.5 + 0.5, 1);

	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return float4(input.TexCoord, 0, 1);
}