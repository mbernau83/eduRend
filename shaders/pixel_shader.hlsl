Texture2D texDiffuse : register(t0);
Texture2D texNormal : register(t1);
TextureCube texCube : register(t2);

SamplerState texSampler : register(s0);
SamplerState cubeSampler : register(s1);

//Databinding that acts as a global variable inside the HLSL

cbuffer TransformBufferCam : register(b0)
{
    float4 CamPosition;
    float4 LightPosition;
};


cbuffer MaterialBuffer : register(b1)
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
    float3 Binormal : BINORMAL;
    float3 Tangent : TANGENT;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1

    //return float4(lightVector * 0.5 + 0.5, 1);

	////////////////// Phong : vektorkomponenter F6 sida 28
    //phong is Iphong = Iambient + Idiffuse + Ispecular;

	//Ytnomral (N), Ljusvektor (L), L reflekterad i N (R), vy-vektor (V), glans (alpha/A)
    //--saturate function clamps a value between 0 and 1;

    //LAB4

    float3 tangent = normalize(input.Tangent);
    float3 binormal = normalize(input.Binormal);

    float3x3 TBN = transpose(float3x3(tangent, binormal, input.Normal));
    float4 normalT = texNormal.Sample(texSampler, input.TexCoord);
    float3 newTNormal = mul(TBN, (normalT * 2 - 1).xyz);
    
    //LAB3
    float2 textureScale2D = 1;
    float2 texCoordinate2D = input.TexCoord * textureScale2D;

    float4 textureColor = texDiffuse.Sample(texSampler, texCoordinate2D);

    //END OF BLOCK


    float3 N = newTNormal; //input.Normal;   
   
    float3 L = normalize(LightPosition.xyz - input.PosWorld.xyz); /*the light vector goes here*/
    float LdotN = dot(L, N);
    
    float3 V = normalize(CamPosition.xyz - input.PosWorld.xyz); /*the eye vector goes here*/
    //float3 R = normalize(2 * saturate(dot(L, N)) * N - L); //a reflection vector //;
    float3 R = normalize(-reflect(L, N));
    float RdotV = dot(R, V); //Viewing angle influence on specular highlight. Saturate clamps
    float alpha = 5; //Good range 0-128;
   
    //float4 blue = { 0, 0, 1, 0 };

       
    //LAB5

    bool isSkybox = false;

    float3 viewReflect = reflect(V, N);
    float4 cubeReflection = texCube.Sample(cubeSampler, viewReflect);
    //return cubeReflection;

    
    float4 ambient = float4(ka.xyz, 0);
    float4 diffuse = float4(kd.xyz * saturate(LdotN), 0);
    float4 specular = float4(ks.xyz * pow(saturate(RdotV), alpha), 0);
   
    //float3 phong = ambient;
    //float3 phong = diffuse;
    //float3 phong = specular;
    float4 phong = ambient + diffuse + specular * cubeReflection;


    return phong * textureColor;

    //return float4(input.Binormal * 0.5 + 0.5, 0); //phong * textureColor;

    //return float4(input.TexCoord, 0, 1);
    	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return 
}