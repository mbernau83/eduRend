Texture2D texDiffuse : register(t0);

SamplerState texSampler : register(s0);

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


    
    //LAB3
    float2 textureScale2D = 2;
    float2 texCoordinate2D = input.TexCoord * textureScale2D;

    float4 textureColor = texDiffuse.Sample(texSampler, texCoordinate2D);

    //END OF BLOCK


    float3 N = input.Normal;   
   
    float3 L = normalize(LightPosition.xyz - input.PosWorld.xyz); /*the light vector goes here*/
    float LdotN = dot(L, N);
    
    float3 V = normalize(CamPosition.xyz - input.PosWorld.xyz); /*the eye vector goes here*/
    //float3 R = normalize(2 * saturate(dot(L, N)) * N - L); //a reflection vector //;
    float3 R = normalize(-reflect(L, N));
    float RdotV = dot(R, V); //Viewing angle influence on specular highlight. Saturate clamps
    float alpha = 5; //Good range 0-128;
   
    float3 blue = { 0, 0, 1 };
    
    float3 ambient = ka.xyz;
    float3 diffuse = kd.xyz * saturate(LdotN);
    float3 specular = /*ks.xyz **/ pow(saturate(RdotV), alpha);
   
    //float3 phong = ambient;
    //float3 phong = diffuse;
    //float3 phong = specular;
    float3 phong = ambient + diffuse + specular * blue;

    return float4(phong, 1) * textureColor;

    //return float4(input.TexCoord, 0, 1);
    	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
//	return 
}