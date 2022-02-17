
//
//  Model.h
//
//  Carl Johan Gribel 2016, cjgribel@gmail.com
//

#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "stdafx.h"
#include <vector>
#include "vec\vec.h"
#include "vec\mat.h"
#include "ShaderBuffers.h"
#include "Drawcall.h"
#include "OBJLoader.h"
#include "Texture.h"

using namespace linalg;

class Model
{

protected:
	// Pointers to the current device and device context
	ID3D11Device* const			dxdevice;
	ID3D11DeviceContext* const	dxdevice_context;

	// Pointers to the class' vertex & index arrays
	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* index_buffer = nullptr;
	ID3D11Buffer* material_Buffer = nullptr;

	// [X] Extend the Model.h with a cbuff pointer.
	Material material; //To F12, no functionality. Is struct, has values to be passed on.
	// [X]add a method that updates	the cbuffer from a Material object
	// [X]The cbuffer may be created e.g. in the constructor of Model.
	// [ ]Updates and binds are made from within the Render() - method of Quad, Cubeand OBJModel, respectively.

public:

	/////////////////////////////////////////////////////////////////////
	// Translpant from OBJ-model class to enable same functionality in other
	// derived classes of Model
	/////////////////////////////////////////////////////////////////////

	// index ranges, representing drawcalls, within an index array
	struct IndexRange
	{
		unsigned int start;
		unsigned int size;
		unsigned ofs;
		int mtl_index;
	};

	std::vector<IndexRange> index_ranges;
	std::vector<Material> materials;

	void append_materials(const std::vector<Material>& mtl_vec)
	{
		materials.insert(materials.end(), mtl_vec.begin(), mtl_vec.end());
	}
	/////////////////////////////////////////////////////////////////////

	struct MaterialBuffer
	{
		//Extend as appropriate
		//Vec4f to stay in 16-unit alignment.

		vec4f ka;
		vec4f kd;
		vec4f ks;
	};

	Model(
		ID3D11Device* dxdevice, 
		ID3D11DeviceContext* dxdevice_context) 
		:	dxdevice(dxdevice),
			dxdevice_context(dxdevice_context)
	{ 
		InitMaterialBuffer();
	}

	void InitMaterialBuffer()
	{
		HRESULT hr;
		D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
		MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		MatrixBuffer_desc.ByteWidth = sizeof(MaterialBuffer);
		MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		MatrixBuffer_desc.MiscFlags = 0;
		MatrixBuffer_desc.StructureByteStride = 0;
		ASSERT(hr = dxdevice->CreateBuffer(&MatrixBuffer_desc, nullptr, &material_Buffer));
	}

	//Uppdatering per objekt, inte per frame Föreläsning 6 s.24
	void UpdateMaterialBuffer(
		vec4f ka,
		vec4f kd,
		vec4f ks) const
{
		// Map the resource buffer, obtain a pointer and then write our matrices to it
		D3D11_MAPPED_SUBRESOURCE resource;
		dxdevice_context->Map(material_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		MaterialBuffer* material_Buffer_ = (MaterialBuffer*)resource.pData;
		material_Buffer_->ka = ka;
		material_Buffer_->kd = kd;
		material_Buffer_->ks = ks;
		dxdevice_context->Unmap(material_Buffer, 0);
}

	//
	//Abstract render
	//
	virtual void Render() const = 0;

	//
	// Destructor
	//
	virtual ~Model()
	{ 
		SAFE_RELEASE(vertex_buffer);
		SAFE_RELEASE(index_buffer);
		SAFE_RELEASE(material_Buffer);
	}
};

class QuadModel : public Model
{
	unsigned nbr_indices = 0;

public:

	QuadModel(
		ID3D11Device* dx3ddevice,
		ID3D11DeviceContext* dx3ddevice_context);

	virtual void Render() const;

	~QuadModel() { }
};

class Cube : public Model
{
	unsigned nbr_indices = 0;

public:

	Cube(
		ID3D11Device* dx3ddevice,
		ID3D11DeviceContext* dx3ddevice_context);

	virtual void Render() const;

	~Cube() { }
};

class OBJModel : public Model
{

public:

	OBJModel(
		const std::string& objfile,
		ID3D11Device* dxdevice,
		ID3D11DeviceContext* dxdevice_context);

	virtual void Render() const;

	~OBJModel();
};

#endif