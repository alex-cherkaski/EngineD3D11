#include "PCH.h"
#include "Components/Components.h"
#include "Core/Core.h"
#include "ECS/Registry.h"
#include "Macros.h"
#include "MeshManager/MeshManager.h"
#include "Scene.h"
#include "ShaderManager/ShaderManager.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/GraphicsMeshRenderSystem.h"
#include "Systems/UIRenderSystem.h"
#include "TextureManager/TextureManager.h"
#include "UIManager/UIManager.h"

Scene::Scene(const wchar_t* filePath)
{
	// Create the lookup table that maps node string names to scene parse states.
	ConstructParseStateLookUpTable();

	// Create the actual file stream and xml scene reader.
	CreateSceneReader(filePath);

	// Parse the xml tree and reconstruct the scene.
	ReadSceneFile();
}

void Scene::Update(float deltaTime)
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsUpdate(deltaTime);
}

void Scene::Render()
{
	static Registry& registry = Registry::GetInstanceWrite();
	registry.RunSystemsRender();
}

void Scene::Shutdown()
{
	Registry::GetInstanceWrite().Shutdown();
	EventManager::GetInstanceWrite().Shutdown();

	MeshManager::GetInstanceWrite().Clear();
	ShaderManager::GetInstanceWrite().Clear();
	TextureManager::GetInstanceWrite().Clear();
	UIManager::GetInstanceWrite().Clear();

	Engine::GetInstanceWrite().ResetAverageFPSTracker();
}

void Scene::CreateSceneReader(const wchar_t* filePath)
{
	// Attempt to create the xml file stream for reading.
	const HRESULT createStreamOnFileResult = SHCreateStreamOnFile(
		filePath,									// The path of the xml file to create the file stream for.
		STGM_READ,									// The file stream access mode the the xml file.
		m_sceneReader.XMLFileStream.GetAddressOf()	// Pointer to the resulting IStream interface.
	);

	// Error check xml file stream creation.
	ENGINE_ASSERT_HRESULT(createStreamOnFileResult);

	// Attempt to create the xml reader.
	const HRESULT createXMLReaderResult = CreateXmlReader(
		__uuidof(IXmlReader),							// the uuid of the xml reader interface.
		(void**)m_sceneReader.XMLReader.GetAddressOf(),	// Pointer to the resulting xml reader interface.
		nullptr											// Optional pointer to IMalloc interface.
	);

	// Error check xml reader creation.
	ENGINE_ASSERT_HRESULT(createXMLReaderResult);

	// Attempt to set the data property of the xml reader.
	const HRESULT setPropertyResult = m_sceneReader.XMLReader->SetProperty(
		XmlReaderProperty::XmlReaderProperty_DtdProcessing,	// The property of the xml reader to set.
		DtdProcessing::DtdProcessing_Prohibit				// The value to set the property to.
	);

	// Error check xml property setting.
	ENGINE_ASSERT_HRESULT(setPropertyResult);

	// Attempt to set the file stream of the xml reader.
	const HRESULT setInputResult = m_sceneReader.XMLReader->SetInput(m_sceneReader.XMLFileStream.Get());

	// Error check xml reader input stream setting.
	ENGINE_ASSERT_HRESULT(setInputResult);
}

void Scene::ReadSceneFile()
{
	// While we are not done parsing the xml file...
	while (!m_sceneReader.XMLReader->IsEOF())
	{
		// Attempt to read the next xml node - not entire element - but node.
		XmlNodeType nodeType;
		const HRESULT readResult = m_sceneReader.XMLReader->Read(&nodeType);
		ENGINE_ASSERT_HRESULT(readResult);

		switch (nodeType)
		{
		case XmlNodeType_None:
			break;

		case XmlNodeType_Element:
		{
			// Construct a new node on the stack, and retrieve its element values member for updating.
			m_nodeStack.emplace();
			Node& node = m_nodeStack.top();
			ElementValues& elementValues = node.elementValues;

			// Get the element prefix and local name.
			const HRESULT getPrefixResult = m_sceneReader.XMLReader->GetPrefix(&elementValues.prefix, &elementValues.prefixSize);
			ENGINE_ASSERT_HRESULT(getPrefixResult);
			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&elementValues.localName, &elementValues.localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);

			// Process all attribute for this opening element.
			ReadAttributes(node);
			SetParseState(node);
			break;
		}

		case XmlNodeType_Attribute:
			break;

		case XmlNodeType_Text:
		{
			Node& node = m_nodeStack.top();
			TextValues& textValues = node.textValues;

			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&textValues.value, &textValues.valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_CDATA:
		{
			//const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			//ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_ProcessingInstruction:
		{
			//const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			//ENGINE_ASSERT_HRESULT(getLocalNameResult);
			//const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			//ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_Comment:
		{
			//const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			//ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_DocumentType:
			break;

		case XmlNodeType_Whitespace:
		{
			//const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			//ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_EndElement:
		{
			Node& node = m_nodeStack.top();
			ProcessNode(node);
			m_nodeStack.pop();
			ResetParseState();

			//const HRESULT getPrefixResult = m_sceneReader.XMLReader->GetPrefix(&prefix, &prefixSize);
			//ENGINE_ASSERT_HRESULT(getPrefixResult);
			//const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			//ENGINE_ASSERT_HRESULT(getLocalNameResult);
			break;
		}

		case XmlNodeType_XmlDeclaration:
			break;

		//case _XmlNodeType_Last:
		//	break;

		default:
			break;
		}
	}
}

void Scene::ReadAttributes(Node& node)
{
	// Move the xml reader to the first attribute, and begins reading its values.
	HRESULT moveToNextAttribute = m_sceneReader.XMLReader->MoveToFirstAttribute();
	ENGINE_ASSERT_HRESULT(moveToNextAttribute);

	// For the next valid attribute...
	while (moveToNextAttribute == S_OK)
	{
		// If the node is not a document type definition node.
		if (!m_sceneReader.XMLReader->IsDefault())
		{
			// Initialize a default attribute values struct.
			AttributeValues attributeValues = { };

			// Get the attribute's prefix and prefix size.
			const HRESULT getPrefixSize = m_sceneReader.XMLReader->GetPrefix(&attributeValues.prefix, &attributeValues.prefixSize);
			ENGINE_ASSERT_HRESULT(getPrefixSize);

			// Get the attribute's local name and local name size.
			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&attributeValues.localName, &attributeValues.localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);

			// Get the attribute's  value and value size.
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&attributeValues.value, &attributeValues.valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);

			// Add the attribute to the list of attributes for this element.
			node.attributes.push_back(attributeValues);
		}

		// Move the reader to the next attribute.
		moveToNextAttribute = m_sceneReader.XMLReader->MoveToNextAttribute();
		ENGINE_ASSERT_HRESULT(moveToNextAttribute);
	}
}

void Scene::SetParseState(const Node& node)
{
	// Hash function from element name.
	std::hash<std::wstring> hasher;

	// If a mapping exists between the element name and the parse state, update the parse state.
	const auto constIterator = m_parseStateLookUpTable.find(hasher(node.elementValues.localName));
	if (constIterator != m_parseStateLookUpTable.cend())
	{
		m_parseState = constIterator->second;
	}
}

void Scene::ResetParseState()
{
	m_parseState = ParseState::NONE; // Put parse state in node instead. Its being reset on exit from node.
}

void Scene::ConstructParseStateLookUpTable()
{
	// Hash function for hashing element names as standard library wide strings.
	std::hash<std::wstring> hasher;

	// Build look up table to map element names to parse states.
	m_parseStateLookUpTable[hasher(L"Mesh")] = ParseState::Mesh;
	m_parseStateLookUpTable[hasher(L"Shader")] = ParseState::Shader;
	m_parseStateLookUpTable[hasher(L"Texture")] = ParseState::Texture;
	m_parseStateLookUpTable[hasher(L"UI")] = ParseState::UI;
	m_parseStateLookUpTable[hasher(L"System")] = ParseState::System;
	m_parseStateLookUpTable[hasher(L"Entity")] = ParseState::Entity;
	m_parseStateLookUpTable[hasher(L"TransformComponent")] = ParseState::TransformComponent;
	m_parseStateLookUpTable[hasher(L"GraphicsMeshComponent")] = ParseState::GraphicsMeshComponent;
	m_parseStateLookUpTable[hasher(L"PhysicsComponent")] = ParseState::PhysicsComponent;
	m_parseStateLookUpTable[hasher(L"UIComponent")] = ParseState::UIComponent;
}

void Scene::ProcessNode(Node& node)
{
	switch (m_parseState)
	{
	case Scene::ParseState::NONE:
		break;
	case Scene::ParseState::Mesh:
		ProcessMeshNode(node);
		break;
	case Scene::ParseState::Shader:
		ProcessShaderNode(node);
		break;
	case Scene::ParseState::Texture:
		ProcessTextureNode(node);
		break;
	case Scene::ParseState::UI:
		ProcessUINode(node);
		break;
	case Scene::ParseState::System:
		ProcessSystemNode(node);
		break;
	case Scene::ParseState::Entity:
		ProcessEntityNode();
		break;
	case ParseState::TransformComponent:
		ProcessTransfromComponentNode(node);
		break;
	case ParseState::GraphicsMeshComponent:
		ProcessGraphicsMeshComponentNode(node);
		break;
	case ParseState::PhysicsComponent:
		ProcessPhysicsComponentNode(node);
		break;
	case ParseState::UIComponent:
		ProcessUIComponentNode(node);
		break;
	default:
		break;
	}
}

void Scene::ProcessMeshNode(const Node& node)
{
	// Retrieve the mesh manager to load the mesh.
	MeshManager& meshManager = MeshManager::GetInstanceWrite();

	// Retrieve the relevant mesh creation parameters from the xml element node.
	const wchar_t* meshName = node.attributes[0].value;
	const wchar_t* meshPath = node.textValues.value;
	const bool meshOpenGLFlag = _wcsicmp(node.attributes[1].value, L"True") == 0;

	// Attempt to create the mesh.
	meshManager.CreateMeshData(meshName, meshPath, meshOpenGLFlag);
}

void Scene::ProcessShaderNode(const Node& node)
{
	// Retrieve the shader manager to load the shader.
	ShaderManager& shaderManager = ShaderManager::GetInstanceWrite();

	// Retrieve shader relevant loading parameters from the xml element node.
	const wchar_t* shaderName = node.attributes[0].value;
	const wchar_t* shaderPath = node.textValues.value;

	// Attempt to create either a mesh shader, or a ui shader depending on the second xml element attribute.
	if (_wcsicmp(node.attributes[1].value, L"False") == 0)
		shaderManager.CreateMeshShaderData(shaderName, shaderPath, shaderPath);
	else
		shaderManager.CreateUIShaderData(shaderName, shaderPath, shaderPath);
}

void Scene::ProcessTextureNode(const Node& node)
{
	// Retrieve the texture manager to load the texture.
	TextureManager& textureManager = TextureManager::GetInstanceWrite();

	// Retrieve texture relevant loading parameters from the xml element node.
	const wchar_t* textureName = node.attributes[0].value;
	const wchar_t* texturePath = node.textValues.value;

	// Attempt to create the texture.
	textureManager.CreateTextureData(textureName, texturePath);
}

void Scene::ProcessUINode(const Node& node)
{
	// Retrieve the ui manager to create a new ui element mesh.
	UIManager& uiManager = UIManager::GetInstanceWrite();

	// Retrieve the ui element identifying name and initial text.
	const wchar_t* uiName = node.attributes[0].value;
	const wchar_t* uiText = node.textValues.value;

	// Attempt to contruct the ui element mesh and texture.
	uiManager.CreateUIData(uiName, uiText);
}

void Scene::ProcessSystemNode(const Node& node)
{
	// Retrieve the registry to create the requested systems.
	Registry& registry = Registry::GetInstanceWrite();
	
	// Attempt to find and create the requested system.
	if (_wcsicmp(node.textValues.value, L"GraphicsMeshRenderSystem") == 0)
		registry.AddSystem<GraphicsMeshRenderSystem>();
	else if (_wcsicmp(node.textValues.value, L"PhysicsSystem") == 0)
		registry.AddSystem<PhysicsSystem>();
	else if (_wcsicmp(node.textValues.value, L"UIRenderSystem") == 0)
		registry.AddSystem<UIRenderSystem>();
}

void Scene::ProcessEntityNode()
{
	// Retrieve the registry to create the requested entity.
	Registry& registry = Registry::GetInstanceWrite();

	// Create the entity.
	m_lastProcessedEntity = registry.CreateEntity();
}

void Scene::ProcessTransfromComponentNode(const Node& node)
{
	// Initialize the new transform component.
	TransformComponent transformComponent = { };

	// Retrieve the SIMD entity transform matrix.
	XMMATRIX transform = XMLoadFloat4x4(&transformComponent.Transform);

	// Retrieve the local space scale factors.
	const float xScale = std::stof(node.attributes[6].value);
	const float yScale = std::stof(node.attributes[7].value);
	const float zScale = std::stof(node.attributes[8].value);

	// Construct the entity local space scale matrix.
	const XMMATRIX scaleMatrix = XMMatrixScaling(xScale, yScale, zScale);

	// Retrieve the entity local space rotations.
	const float xRotation = std::stof(node.attributes[3].value);
	const float yRotation = std::stof(node.attributes[4].value);
	const float zRotation = std::stof(node.attributes[5].value);

	// Construct the entity local space rotation matrix.
	const XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(xRotation),
		XMConvertToRadians(yRotation),
		XMConvertToRadians(zRotation)
	);

	// Retrieve the entity world position.
	const float xTranslation = std::stof(node.attributes[0].value);
	const float yTranslation = std::stof(node.attributes[1].value);
	const float zTranslation = std::stof(node.attributes[2].value);

	// Construct the entity world translation matrix.
	const XMMATRIX translationMatrix = XMMatrixTranslation(xTranslation, yTranslation, zTranslation);

	// Apply the transformations to the entity transform.
	transform *= scaleMatrix * rotationMatrix * translationMatrix;
	XMStoreFloat4x4(&transformComponent.Transform, transform);

	// Add the transform component to the entity.
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddComponent<TransformComponent>(m_lastProcessedEntity, transformComponent);
}

void Scene::ProcessGraphicsMeshComponentNode(const Node& node)
{
	// Initialize the new graphics mesh component.
	GraphicsMeshComponent graphicsMeshComponent = { };

	// Fill the graphics mesh component data fields.
	graphicsMeshComponent.MeshName = node.attributes[0].value;
	graphicsMeshComponent.ShaderName = node.attributes[1].value;

	// The second attribute contains the texture name, but is optional for 3D meshes.
	if (node.attributes.size() > 2)
	{
		graphicsMeshComponent.TextureName = node.attributes[2].value;
	}

	// The third attribute is an auxiliary texture for blending, but is also optional.
	if (node.attributes.size() > 3)
	{
		graphicsMeshComponent.BlendTextureName = node.attributes[3].value;
	}

	// Add the graphics mesh component to the entity.
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddComponent<GraphicsMeshComponent>(m_lastProcessedEntity, graphicsMeshComponent);
}

void Scene::ProcessPhysicsComponentNode(const Node& node)
{
	// Initialize the new physics component.
	PhysicsComponent physicsComponent = { };

	// Set the physics component's linear velocity.
	physicsComponent.LinearVelocity.x = std::stof(node.attributes[0].value);
	physicsComponent.LinearVelocity.y = std::stof(node.attributes[1].value);
	physicsComponent.LinearVelocity.z = std::stof(node.attributes[2].value);

	// Set the physics component's angular velocity.
	physicsComponent.AngularVelocity.x = std::stof(node.attributes[3].value);
	physicsComponent.AngularVelocity.y = std::stof(node.attributes[4].value);
	physicsComponent.AngularVelocity.z = std::stof(node.attributes[5].value);

	// Add the graphics mesh component to the entity.
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddComponent<PhysicsComponent>(m_lastProcessedEntity, physicsComponent);
}

void Scene::ProcessUIComponentNode(const Node& node)
{
	// Initialize the new ui component.
	UIComponent uiComponent = { };

	// Initialize the ui data relevant parameters.
	uiComponent.ShaderName = node.attributes[0].value;
	uiComponent.TextureName = node.attributes[1].value;
	uiComponent.UIName = node.attributes[2].value;

	// Add the UI component to the entity.
	Registry& registry = Registry::GetInstanceWrite();
	registry.AddComponent<UIComponent>(m_lastProcessedEntity, uiComponent);
}
