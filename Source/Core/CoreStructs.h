#pragma once
#include "PCH.h"

struct VertexData
{
	XMFLOAT3 m_position = { NAN, NAN, NAN };
};

struct GPUMeshData
{
	std::wstring VertexShaderPath;
	std::wstring PixelShaderPath;

	std::vector<VertexData> Vertices;
	std::vector<UINT> Indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> PixelBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader = nullptr;

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};
