#pragma once
#include "SceneReader.h"

class Scene final
{
	struct NodeValues
	{
		PCWSTR prefix = nullptr;
		PCWSTR localName = nullptr;
		PCWSTR value = nullptr;

		UINT prefixSize = 0;
		UINT localNameSize = 0;
		UINT valueSize = 0;
	};

	enum class ParseCategory
	{
		NONE = 0,
		ParsingMeshes,
		ParsingShaderes,
		ParsingTextures,
		ParsingSystems,
		ParsingEntities,
		ParsingComponents,
	};

	enum class ParseUnit
	{
		NONE = 0,
		ParsingMesh,
		ParsingShader,
		ParsingTexture,
		ParsingSystem,
		ParsingEntity,
		ParsingComponent,
	};

public:
	Scene(const wchar_t* filePath);

	void Update(float deltaTime);
	void Render();

private:
	void CreateSceneReader(const wchar_t* filePath);
	void ReadSceneFile();
	void ReadAttributes();

private:
	SceneReader m_sceneReader;
	ParseCategory m_currentParseCategory = ParseCategory::NONE;
	ParseUnit m_currentPraseUnit = ParseUnit::NONE;
};

