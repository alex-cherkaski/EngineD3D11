#pragma once
#include "SceneReader.h"
#include "ECS/Types.h"

class Scene final
{
	struct ElementValues
	{
		PCWSTR prefix = nullptr;
		PCWSTR localName = nullptr;

		UINT prefixSize = 0;
		UINT localNameSize = 0;
	};

	struct TextValues
	{
		PCWSTR value = nullptr;
		UINT valueSize = 0;
	};

	struct AttributeValues
	{
		PCWSTR prefix = nullptr;
		PCWSTR localName = nullptr;
		PCWSTR value = nullptr;

		UINT prefixSize = 0;
		UINT localNameSize = 0;
		UINT valueSize = 0;
	};

	struct Node
	{
		ElementValues elementValues;
		TextValues textValues;
		std::vector<AttributeValues> attributes;
	};

	enum class ParseState : byte
	{
		NONE = 0,
		Mesh,
		Shader,
		Texture,
		UI,
		System,
		Entity,
		TransformComponent,
		GraphicsMeshComponent,
		PhysicsComponent,
		UIComponent
	};

public:
	Scene(const wchar_t* filePath);

	void Update(float deltaTime);
	void Render();
	void Shutdown();

private:
	void CreateSceneReader(const wchar_t* filePath);
	void ReadSceneFile();
	void ReadAttributes(Node& node);

	void SetParseState(const Node& node);
	void ResetParseState();

	void ConstructParseStateLookUpTable();

	void ProcessNode(Node& node);
	void ProcessMeshNode(const Node& node);
	void ProcessShaderNode(const Node& node);
	void ProcessTextureNode(const Node& node);
	void ProcessUINode(const Node& node);
	void ProcessSystemNode(const Node& node);
	void ProcessEntityNode();

	void ProcessTransfromComponentNode(const Node& node);
	void ProcessGraphicsMeshComponentNode(const Node& node);
	void ProcessPhysicsComponentNode(const Node& node);
	void ProcessUIComponentNode(const Node& node);

private:
	std::stack<Node> m_nodeStack;
	std::unordered_map<size_t, ParseState> m_parseStateLookUpTable;

	SceneReader m_sceneReader;
	ParseState m_parseState = ParseState::NONE;

	size_t m_lastProcessedEntity = 0;
};

