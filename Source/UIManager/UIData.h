#pragma once
#include "PCH.h"
#include "TextureManager/TextureData.h"

struct UIVertexAttributes
{
	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT2 Texture = { 0.0f, 0.0f };
};

struct UIMeshData
{
	std::vector<UIVertexAttributes> Vertices;
	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};

struct UIData
{
	std::wstring Text;
	UIMeshData Mesh;
};

