#pragma once
#include "PCH.h"

struct VertexAttributes
{
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Normal = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2 Texture = { 0.0f, 0.0f };
};

struct MeshData
{
	std::vector<VertexAttributes> Vertices;
	std::vector<UINT> Indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer = nullptr;

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

