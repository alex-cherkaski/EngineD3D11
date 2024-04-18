#pragma once
#include "PCH.h"

struct VertexAttributes
{
	XMFLOAT3 Position = { NAN, NAN, NAN };
	XMFLOAT3 Normal = { NAN, NAN, NAN };
	XMFLOAT2 Texture = { NAN, NAN };
};

struct GPUTextureData
{
	std::wstring TextureFilePath;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D = nullptr;
	D3D11_TEXTURE2D_DESC Texture2DDescriptor = { 0 };

	bool IsDDS = false;
};

struct GPUModelData
{
	std::wstring VertexShaderPath;
	std::wstring PixelShaderPath;

	std::string Message;

	std::vector<VertexAttributes> Vertices;
	std::vector<UINT> Indices;

	Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> PixelBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader = nullptr;

	std::vector<GPUTextureData> GPUTextureDatas = { };

	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
};
