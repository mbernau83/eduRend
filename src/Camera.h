
//
//  Camera.h
//
//	Basic camera class
//

#pragma once
#define _USE_MATH_DEFINES
#ifndef CAMERA_H
#define CAMERA_H

#include <algorithm>
#include <stdexcept>
#include "vec\vec.h"
#include "vec\mat.h"

using namespace linalg;

class Camera
{
public:
	// Aperture attributes
	float vfov, aspect;	
	
	// Clip planes in view space coordinates
	// Evrything outside of [zNear, zFar] is clipped away on the GPU side
	// zNear should be > 0
	// zFar should depend on the size of the scene
	// This range should be kept as tight as possibly to improve
	// numerical precision in the z-buffer
	float zNear, zFar;	
						
	vec3f position;

	//MouseToCamInput
	float pitch;
	float yaw;
	float roll;
	float velocity;
	const vec4f forwardViewDirection = { 0.0f, 0.0f, -1.0f, 0.0f };

	Camera(
		float vfov,
		float aspect,
		float zNear,
		float zFar):
		vfov(vfov), aspect(aspect), zNear(zNear), zFar(zFar)
	{
		position = {0.0f, 0.0f, 0.0f};
		pitch = 0;
		yaw = 0;
		roll = 0;
		velocity = 5;
	}

	// Move to an absolute position
	//
	void moveTo(const vec3f& p)
	{
		position = p;
	}

	// Move relatively
	//
	void move(const vec3f& v)
	{
		position += v * get_MouseRotation().get_3x3().inverse();
	}

	void Update(float deltaTime, InputHandler* input_handler)
	{
		//Keyboard input WASD expressed as position
		ReadKeyInput(deltaTime, input_handler);

		//Get mouse input dX & dY
		yaw += input_handler->GetMouseDeltaX() * deltaTime *.1f;
		pitch += input_handler->GetMouseDeltaY() * deltaTime * .1f;
		
		if (pitch >= fPI / 2)
			pitch = fPI / 2;
		if (pitch <= -fPI / 2)
			pitch = -fPI / 2;
	}

	// Return World-to-View matrix for this camera
	//
	mat4f get_WorldToViewMatrix()
	{
		// Assuming a camera's position and rotation is defined by matrices T(p) and R,
		// the View-to-World transform is T(p)*R (for a first-person style camera).
		//
		// World-to-View then is the inverse of T(p)*R;
		//		inverse(T(p)*R) = inverse(R)*inverse(T(p)) = transpose(R)*T(-p)
		// Since now there is no rotation, this matrix is simply T(-p)

		mat4f mouse_rotation = get_MouseRotation();
		mouse_rotation.transpose();

		mat4f translation = mat4f::translation(-position);
						
		return  mouse_rotation * translation;
	}

	// Matrix transforming from View space to Clip space
	// In a performance sensitive situation this matrix should be precomputed
	// if possible
	//
	mat4f get_ProjectionMatrix()
	{
		return mat4f::projection(vfov, aspect, zNear, zFar);
	}
	
	//Obtain rotation 
	mat4f get_RotationMatrix(float roll, float yaw, float pitch)
	{
		float r = roll;
		float y = yaw;
		float p = pitch;

		return mat4f::rotation(r, y, p);
	}

	//Get a rotation from mouse input
	mat4f get_MouseRotation()
	{
		return get_RotationMatrix(0, -yaw, -pitch);
	}

	void ReadKeyInput(float deltaTime, InputHandler* input_handler)
	{
		// Basic camera control
		if (input_handler->IsKeyPressed(Keys::Up) || input_handler->IsKeyPressed(Keys::W))
			move({ 0.0f, 0.0f, -velocity * deltaTime });
		if (input_handler->IsKeyPressed(Keys::Down) || input_handler->IsKeyPressed(Keys::S))
			move({ 0.0f, 0.0f, velocity * deltaTime });
		if (input_handler->IsKeyPressed(Keys::Right) || input_handler->IsKeyPressed(Keys::D))
			move({ velocity * deltaTime, 0.0f, 0.0f });
		if (input_handler->IsKeyPressed(Keys::Left) || input_handler->IsKeyPressed(Keys::A))
			move({ -velocity * deltaTime, 0.0f, 0.0f });
	}
};


#endif