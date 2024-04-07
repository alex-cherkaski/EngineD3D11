#pragma once
#include "PCH.h"

class CoreObject final
{
public:
	CoreObject() = default;
	CoreObject(const GUID& gpuDataGUID) : m_gpuDataGUID(gpuDataGUID) { }

	//virtual ~CoreObject() = default;

	const GUID& GetGPUDataGUID() const { return m_gpuDataGUID; }
	void SetGPUDataGUID(const GUID& gpuDataGUID);

	XMMATRIX GetWorldMatrix() const;
	void SetPosition(const XMFLOAT3& position) { m_position = position; }
	void SetRotation(float rotation) { m_rotation = rotation; }
	void SetScale(const XMFLOAT3& scale) { m_scale = scale; }

private:
	GUID m_gpuDataGUID = { 0, 0, 0, { 0 } };

	XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
	float m_rotation = 0.0f;
};

