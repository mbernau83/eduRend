
#include <math.h>
#include "Scene.h"

Scene::Scene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	dxdevice(dxdevice),
	dxdevice_context(dxdevice_context),
	window_width(window_width),
	window_height(window_height)
{ }



void Scene::WindowResize(
	int window_width,
	int window_height)
{
	this->window_width = window_width;
	this->window_height = window_height;
}

OurTestScene::OurTestScene(
	ID3D11Device* dxdevice,
	ID3D11DeviceContext* dxdevice_context,
	int window_width,
	int window_height) :
	Scene(dxdevice, dxdevice_context, window_width, window_height)
{
	InitTransformationBuffer();
	InitTransformBufferCamAndLight();
	// + init other CBuffers
}

//
// Called once at initialization
//
void OurTestScene::Init()
{
	camera = new Camera(
		45.0f * fTO_RAD,		// field-of-view (radians)
		(float)window_width / window_height,	// aspect ratio
		1.0f,					// z-near plane (everything closer will be clipped/removed)
		500.0f);				// z-far plane (everything further will be clipped/removed)

	// Move camera to (0,0,5)
	camera->moveTo({ 0, 0, 5 });

	// Create objects
	//quad = new QuadModel(dxdevice, dxdevice_context);

	sponza = new OBJModel("assets/crytek-sponza/sponza.obj", dxdevice, dxdevice_context);
	sun = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);
	earth = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);
	moon = new OBJModel("assets/sphere/sphere.obj", dxdevice, dxdevice_context);
	plane = new OBJModel("assets/Trojan/Trojan.obj", dxdevice, dxdevice_context);
	light = new PointLight;
	light->position = vec3f(0, -15, 0);

	//Lab3
	//floor = new QuadModel("assets/textures/yroadcrossing.png", dxdevice, dxdevice_context);

	totalTime = 0;
	earthAngle = 0;
	moonAngle = 0;

	const char* cube_filenames[6] =
	{
		"assets/cubemaps/debug_cubemap/debug_posx.png",
		"assets/cubemaps/debug_cubemap/debug_negx.png",
		"assets/cubemaps/debug_cubemap/debug_posy.png",
		"assets/cubemaps/debug_cubemap/debug_negy.png",
		"assets/cubemaps/debug_cubemap/debug_posz.png",
		"assets/cubemaps/debug_cubemap/debug_negz.png",
	};

	cube = new Cube("assets/textures/yroadcrossing.png", cube_filenames, dxdevice, dxdevice_context);

}

//
// Called every frame
// dt (seconds) is time elapsed since the previous frame
//
void OurTestScene::Update(
	float dt,
	InputHandler* input_handler)
{
	//Total game time
	totalTime += dt;

	camera->Update(dt, input_handler);

	// Now set/update object transformations
	// This can be done using any sequence of transformation matrices,
	// but the T*R*S order is most common; i.e. scale, then rotate, and then translate.
	// If no transformation is desired, an identity matrix can be obtained 
	// via e.g. Mquad = linalg::mat4f_identity; 

	// Quad model-to-world transformation
	Mquad = mat4f::translation(0, 0, 0) *			// No translation
		mat4f::rotation(-angle, 0.0f, 1.0f, 0.0f) *	// Rotate continuously around the y-axis
		mat4f::scaling(1.5, 1.5, 1.5);				// Scale uniformly to 150%

	// Sponza model-to-world transformation
	Msponza = mat4f::translation(0, -5, 0) *		 // Move down 5 units
		mat4f::rotation(fPI / 2, 0.0f, 1.0f, 0.0f) * // Rotate pi/2 radians (90 degrees) around y
		mat4f::scaling(0.05f);						 // The scene is quite large so scale it down to 5%

		// Sphere (moon) model-to-world transformation M1 * M2 * M3
	Mcube = mat4f::translation(0, 1, 2) *
		mat4f::rotation(0, 0, 0) *
		mat4f::scaling(1.5f);

	// Plane model-to-world transformation
	MmyPlane = mat4f::translation(0, 3, 0) *
		mat4f::rotation(0, totalTime, 0) *
		mat4f::scaling(.3f);

	// Sphere (sun) model-to-world transformation
	Msun = mat4f::translation(0, 2, 0) *
		mat4f::rotation(0, totalTime, 0) *
		mat4f::scaling(.3f);

	std::cout << std::sin(totalTime * 10) << std::endl;

	// Sphere (earth) model-to-world transformation M1 * M2
	Mearth = Msun * (mat4f::translation(0, 0, 3) *
		mat4f::rotation(0, -1 * std::abs(6 * totalTime), 0) *
		mat4f::scaling(.5));

	// Sphere (moon) model-to-world transformation M1 * M2 * M3
	Mmoon = Mearth * mat4f::translation(0, 0, 2) *
		mat4f::rotation(0, 0, 0) *
		mat4f::scaling(.5f);

	// Camera model-to-world
	Mcamera = mat4f::translation(camera->position) *
		mat4f::rotation(0, 0, 0) *
		mat4f::scaling(1.0f);

	Mfloor = mat4f::translation(0, -1.f, 0) *
		mat4f::rotation(PI / 2, -1.0f, 0.0f, 0.0f) *
		mat4f::scaling(100.5, 100.5, 100.5);


	// Light model-to-world //Perhaps not needed

	//Mlight = mat4f::translation(light->position) *
	//	mat4f::rotation(0, 0, 0) *
	//	mat4f::scaling(1.0f);


	// Increment the rotation angle.
	angle += angle_vel * dt;

	// Print fps
	fps_cooldown -= dt;
	if (fps_cooldown < 0.0)
	{
		std::cout << "fps " << (int)(1.0f / dt) << std::endl;
		// printf("fps %i\n", (int)(1.0f / dt));
		fps_cooldown = 2.0;
	}
}

//
// Called every frame, after update
//
void OurTestScene::Render()
{
	// Bind transformation_buffer to slot b0 of the VS
	dxdevice_context->VSSetConstantBuffers(0, 1, &transformation_buffer);
	dxdevice_context->PSSetConstantBuffers(0, 1, &camAndLightBuffer);
	dxdevice_context->PSSetSamplers(0, 1, &sampler);


	// Obtain the matrices needed for rendering from the camera

	//ORIGNINAL
	Mview = camera->get_WorldToViewMatrix();

	Mproj = camera->get_ProjectionMatrix();

	// Load matrices + the Cube's transformation to the device and render it
	UpdateTransformationBuffer(Mcube, Mview, Mproj);
	cube->Render();

	// Load matrices + the Cube's transformation to the device and render it
	UpdateTransformationBuffer(Msun, Mview, Mproj);
	sun->Render();

	// Load matrices + the Cube's transformation to the device and render it
	UpdateTransformationBuffer(Mearth, Mview, Mproj);
	earth->Render();

	// Load matrices + the Cube's transformation to the device and render it
	UpdateTransformationBuffer(Mmoon, Mview, Mproj);
	moon->Render();

	// Load matrices + the Cube's transformation to the device and render it
	UpdateTransformationBuffer(MmyPlane, Mview, Mproj);
	plane->Render();


	//LAB 3
	// UpdateTransformationBuffer(Mfloor, Mview, Mproj);
	//floor->Render();


	//// Load matrices + the Quad's transformation to the device and render it
	//UpdateTransformationBuffer(Mquad, Mview, Mproj);
	//quad->Render();

	// Load matrices + Sponza's transformation to the device and render it
	UpdateTransformationBuffer(Msponza, Mview, Mproj);
	sponza->Render();

	UpdateTransformBufferCamAndLight(camera->position, light->position);

	//UpdateTransformBufferLight(Mlight, Mview, Mproj);

	//Hacky solution - why is this supposed to be done, because this cant be good
	InitSampler();
}

void OurTestScene::Release()
{
	SAFE_DELETE(cube);
	SAFE_DELETE(quad);
	SAFE_DELETE(sponza);
	SAFE_DELETE(sun);
	SAFE_DELETE(earth);
	SAFE_DELETE(moon);
	SAFE_DELETE(plane);
	SAFE_DELETE(camera);
	SAFE_DELETE(light);

	SAFE_RELEASE(transformation_buffer);
	SAFE_RELEASE(camAndLightBuffer);
	SAFE_RELEASE(sampler);

	//SAFE_RELEASE(transformation_buffer);
	// + release other CBuffers
}

void OurTestScene::WindowResize(
	int window_width,
	int window_height)
{
	if (camera)
		camera->aspect = float(window_width) / window_height;

	Scene::WindowResize(window_width, window_height);
}

void OurTestScene::InitSampler()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sampler_desc = { };
	//sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;				
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_desc.MinLOD = 0.0f;
	sampler_desc.MaxLOD = FLT_MAX;
	ASSERT(hr = dxdevice->CreateSamplerState(&sampler_desc, &sampler));
}

void OurTestScene::InitTransformationBuffer()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(TransformationBuffer);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &transformation_buffer));
}

void OurTestScene::InitTransformBufferCamAndLight()
{
	HRESULT hr;
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(TransformBufferCamLight);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &camAndLightBuffer));
}

void OurTestScene::UpdateTransformationBuffer(
	mat4f ModelToWorldMatrix,
	mat4f WorldToViewMatrix,
	mat4f ProjectionMatrix)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(transformation_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformationBuffer* matrix_buffer_ = (TransformationBuffer*)resource.pData;
	matrix_buffer_->ModelToWorldMatrix = ModelToWorldMatrix;
	matrix_buffer_->WorldToViewMatrix = WorldToViewMatrix;
	matrix_buffer_->ProjectionMatrix = ProjectionMatrix;
	dxdevice_context->Unmap(transformation_buffer, 0);
}

void OurTestScene::UpdateTransformBufferCamAndLight(
	vec3f CamPosition,
	vec3f LightPosition)
{
	// Map the resource buffer, obtain a pointer and then write our matrices to it
	D3D11_MAPPED_SUBRESOURCE resource;
	dxdevice_context->Map(camAndLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	TransformBufferCamLight* matrix_buffer_ = (TransformBufferCamLight*)resource.pData;
	matrix_buffer_->CamPosition = vec4f(CamPosition, 0);
	matrix_buffer_->LightPosition = vec4f(LightPosition, 0);
	dxdevice_context->Unmap(camAndLightBuffer, 0);
}


