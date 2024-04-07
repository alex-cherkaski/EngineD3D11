#include "PCH.h"
#include "CoreObject.h"
#include "Macros.h"

void CoreObject::SetGPUDataGUID(const GUID& gpuDataGUID)
{
	ENGINE_ASSERT(GUID_IS_ZERO(m_gpuDataGUID), "Attempting to overwrite non-zero core object GPU data GUID.");
	m_gpuDataGUID = gpuDataGUID;
}

XMMATRIX CoreObject::GetWorldMatrix() const
{
	const XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	const XMMATRIX rotationZ = XMMatrixRotationAxis({ 0.0f, 0.0f, 1.0f }, m_rotation);
	const XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return translationMatrix * rotationZ * scale;
}
