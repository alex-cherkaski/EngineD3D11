#include "PCH.h"
#include "Scene.h"
#include "Macros.h"

Scene::Scene(const wchar_t* filePath)
{
	CreateSceneReader(filePath);
	ReadSceneFile();
}

void Scene::Update(float deltaTime)
{
}

void Scene::Render()
{
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
	while (!m_sceneReader.XMLReader->IsEOF())
	{
		PCWSTR prefix = nullptr;
		PCWSTR localName = nullptr;
		PCWSTR value = nullptr;

		UINT prefixSize = 0;
		UINT localNameSize = 0;
		UINT valueSize = 0;

		XmlNodeType nodeType;
		const HRESULT readResult = m_sceneReader.XMLReader->Read(&nodeType);
		ENGINE_ASSERT_HRESULT(readResult);

		switch (nodeType)
		{
		case XmlNodeType_None:
			break;

		case XmlNodeType_Element:
		{
			const HRESULT getPrefixResult = m_sceneReader.XMLReader->GetPrefix(&prefix, &prefixSize);
			ENGINE_ASSERT_HRESULT(getPrefixResult);
			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);
			ReadAttributes();
			break;
		}

		case XmlNodeType_Attribute:
			break;

		case XmlNodeType_Text:
		{
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_CDATA:
		{
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_ProcessingInstruction:
		{
			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_Comment:
		{
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_DocumentType:
			break;

		case XmlNodeType_Whitespace:
		{
			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
			break;
		}

		case XmlNodeType_EndElement:
		{
			const HRESULT getPrefixResult = m_sceneReader.XMLReader->GetPrefix(&prefix, &prefixSize);
			ENGINE_ASSERT_HRESULT(getPrefixResult);
			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);
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

void Scene::ReadAttributes()
{
	HRESULT moveToNextAttribute = m_sceneReader.XMLReader->MoveToFirstAttribute();
	while (moveToNextAttribute == S_OK)
	{
		if (!m_sceneReader.XMLReader->IsDefault())
		{
			PCWSTR prefix = nullptr;
			PCWSTR localName = nullptr;
			PCWSTR value = nullptr;

			UINT prefixSize = 0;
			UINT localNameSize = 0;
			UINT valueSize = 0;

			const HRESULT getPrefixSize = m_sceneReader.XMLReader->GetPrefix(&prefix, &prefixSize);
			ENGINE_ASSERT_HRESULT(getPrefixSize);

			const HRESULT getLocalNameResult = m_sceneReader.XMLReader->GetLocalName(&localName, &localNameSize);
			ENGINE_ASSERT_HRESULT(getLocalNameResult);

			const HRESULT getValueResult = m_sceneReader.XMLReader->GetValue(&value, &valueSize);
			ENGINE_ASSERT_HRESULT(getValueResult);
		}

		moveToNextAttribute = m_sceneReader.XMLReader->MoveToNextAttribute();
	}
}
