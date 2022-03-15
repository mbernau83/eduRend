
cbuffer TransformationBuffer : register(b0)
{
	matrix ModelToWorldMatrix;
	matrix WorldToViewMatrix;
	matrix ProjectionMatrix;
};

struct VSIn
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
};

struct PSIn
{
	float4 Pos  : SV_Position;
    float3 PosWorld : World_Position;
	float3 Normal : NORMAL;
	float2 TexCoord : TEX;
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

//-----------------------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------------------

PSIn VS_main(VSIn input)
{
	PSIn output = (PSIn)0;
	


	// Model->View transformation
	matrix MV = mul(WorldToViewMatrix, ModelToWorldMatrix);

	// Model->View->Projection (clip space) transformation
	// SV_Position expects the output position to be in clip space
	matrix MVP = mul(ProjectionMatrix, MV);
	
	// Perform transformations and send to output
	
    output.Pos = mul(MVP, float4(input.Pos, 1));
	output.TexCoord = input.TexCoord;
 
	output.Normal = normalize( mul(ModelToWorldMatrix, float4(input.Normal,0)).xyz );
    output.Binormal = normalize(mul(ModelToWorldMatrix, float4(input.Binormal, 0)).xyz);
    output.Tangent = normalize(mul(ModelToWorldMatrix, float4(input.Tangent, 0)).xyz);

	////// Testing Computation for position in world space (Currently just a guess)
	//Tänkte att output.Normal redan är en beräkning för model to world och att vi 
	//redan har en position i output position som vi kan återanvända. Är dock inte säker.

    output.PosWorld = ModelToWorldMatrix._m03_m13_m23; //mul(ModelToWorldMatrix, float4(pos.xyz, 0)).xyz;
	
		
	return output;
}