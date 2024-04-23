#pragma once
#include "PCH.h"

struct TextureData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D = nullptr; // Debug info.
	std::wstring TextureFilePath; // Debug info.
	D3D11_TEXTURE2D_DESC Texture2DDescriptor = { 0 }; // Debug info.

	bool IsDDS = false;
};

