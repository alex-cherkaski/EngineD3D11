#pragma once

struct ShaderData
{
	std::wstring VertexShaderPath;
	std::wstring PixelShaderPath;

	Microsoft::WRL::ComPtr<ID3DBlob> VertexBlob = nullptr; // Debug info???
	Microsoft::WRL::ComPtr<ID3D11VertexShader> VertexShader = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> PixelBlob = nullptr; // Debug info???
	Microsoft::WRL::ComPtr<ID3D11PixelShader> PixelShader = nullptr;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayout = nullptr;
};

