#pragma once
#include "PCH.h"

struct TransformComponent
{
	XMFLOAT4X4 Transform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
};

struct PhysicsComponent
{
	XMFLOAT3 LinearVelocity = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 AngularVelocity = { 0.0f, 0.0f, 0.0f };
};

struct GraphicsMeshComponent
{
	const wchar_t* MeshName = nullptr;
	const wchar_t* ShaderName = nullptr;
	const wchar_t* TextureName = nullptr;
};

struct GraphicsTextComponent
{

};

