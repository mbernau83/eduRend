
#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "stdafx.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// New files
// Material
// Texture <- stb

// TEMP


class Scene
{
protected:
	ID3D11Device*			dxdevice;
	ID3D11DeviceContext*	dxdevice_context;
	int						window_width;
	int						window_height;

public:

	Scene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	virtual void Init() = 0;

	virtual void Update(
		float dt,
		InputHandler* input_handler) = 0;
	
	virtual void Render() = 0;
	
	virtual void Release() = 0;

	virtual void WindowResize(
		int window_width,
		int window_height);
};

class OurTestScene : public Scene
{
	//
	// Constant buffers (CBuffers) for data that is sent to shaders
	//

	// CBuffer for transformation matrices
	ID3D11Buffer* transformation_buffer = nullptr;
	ID3D11Buffer* camAndLightBuffer = nullptr;


	// + other CBuffers

	// 
	// CBuffer client-side definitions
	// These must match the corresponding shader definitions 
	//

	struct TransformationBuffer
	{
		mat4f ModelToWorldMatrix;
		mat4f WorldToViewMatrix;
		mat4f ProjectionMatrix;
	};

	struct TransformBufferCamLight //TODO : Update from render
	{
		vec4f CamPosition;
		vec4f LightPosition;
	};

	struct PointLight
	{
		vec3f position;
		mat4f translation;
		mat4f rotation;
		mat4f scale;
	};

	//
	// Scene content
	//
	Camera* camera;

	Cube* cube;
	QuadModel* quad;
	OBJModel* sponza;
	OBJModel* sun;
	OBJModel* earth;
	OBJModel* moon;
	OBJModel* plane;
	PointLight* light;



	// Model-to-world transformation matrices
	mat4f Msponza;
	mat4f Mquad;
	mat4f Msun;
	mat4f Mearth;
	mat4f Mmoon;
	mat4f MmyPlane;
	mat4f Mcamera;
	//mat4f Mlight; //Perhaps not needed

	// World-to-view matrix
	mat4f Mview;
	// Projection matrix
	mat4f Mproj;

	// Misc
	float angle = 0;			// A per-frame updated rotation angle (radians)...
	float angle_vel = fPI / 2;	// ...and its velocity (radians/sec)
	float camera_vel = 5.0f;	// Camera movement velocity in units/s
	float fps_cooldown = 0;
	float earthAngle;
	float moonAngle;
	float totalTime;

	void InitTransformationBuffer();

	void InitTransformBufferCamAndLight();

	void UpdateTransformationBuffer(
		mat4f ModelToWorldMatrix,
		mat4f WorldToViewMatrix,
		mat4f ProjectionMatrix);

	void UpdateTransformBufferCamAndLight(
		vec3f CamPosition,
		vec3f LightPosition);

	//void UpdateTransformationBufferPhong(
	//	mat4f ModelToWorldMatrix,
	//	mat4f WorldToViewMatrix,
	//	mat4f ProjectionMatrix);

public:
	OurTestScene(
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context,
		int window_width,
		int window_height);

	void Init() override;

	void Update(
		float dt,
		InputHandler* input_handler) override;

	void Render() override;

	void Release() override;

	void WindowResize(
		int window_width,
		int window_height) override;
};

#endif