
//
//  Camera.h
//
//	Basic camera class
//

#pragma once
#define _USE_MATH_DEFINES
#ifndef CAMERA_H
#define CAMERA_H

#include<math.h>
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
		position += v;
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

		mat4f side_side = get_RotationMatrix(0, yaw, 0);
		mat4f up_down = get_RotationMatrix(0, 0, pitch);
		//up_down.transpose(); //inverts, but I dont like it :-)
		
		return  mat4f::translation(-position) * up_down * side_side;
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
	mat4f get_RotationMatrix(float roll, float yaw, float pitch) //MY CHANGE
	{
		float r = to_radians(roll);
		float y = to_radians(yaw);
		float p = to_radians(pitch);

		return mat4f::rotation(r, y, p);
	}

	//Degrees to rad
	float to_radians(float value)
	{
		return value / 180 * M_PI;
	}
};

#endif