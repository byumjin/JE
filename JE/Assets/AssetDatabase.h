#ifndef ASSETDATABASE_H
#define ASSETDATABASE_H


#include "asset.h"

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <vector>

#include "../Engine/Textures/Texture.h"
#include "../Engine/Common/Geometry.h"

struct AssetData_GeoListInfo
{
	WCHAR GeometryPath[MAX_PATH];
};

struct AssetData_TextureListInfo
{
	WCHAR TexturePath[MAX_PATH];
};

struct AssetData_MaterialListInfo
{
	WCHAR MaterialPath[MAX_PATH];
	WCHAR MaterialTexturePath[MAX_TEXTURE][MAX_PATH];
};

// Basically all heavy things like shaders, meshes or textures
// are cached in memory. They can be instanced independently if
// no caching is desired.
class AssetDatabase
{
private:
	static AssetDatabase * instance;
	static ID3D11Device* device;

	std::unordered_map<std::type_index, std::unordered_map<WCHAR*, Asset*>> assetMap;
	AssetDatabase();

public:

	//GeoList
	std::vector<WCHAR*> m_ListofGeometry;

	//TextureList
	std::vector<WCHAR*> m_ListofTexture;

	//MaterialList
	std::vector<WCHAR*> m_ListofMaterial;

	
	/*
	AssetDatabase(ID3D11Device* pDevice)
	{
		device = pDevice;
	}
	*/

	void Shutdown()
	{
		for (UINT i = 0; i < m_ListofGeometry.size(); i++)
		{
			Geometry* pGeo = FindAsset<Geometry>(m_ListofGeometry.at(i));
			
			delete pGeo;

			delete[] m_ListofGeometry.at(i);
		}

		for (UINT i = 0; i < m_ListofTexture.size(); i++)
		{
			Texture* pTex = FindAsset<Texture>(m_ListofTexture.at(i));
			delete pTex;

			delete[] m_ListofTexture.at(i);
		}

		for (UINT i = 0; i < m_ListofMaterial.size(); i++)
		{
			BasicMaterialEffect_Instance* pMI = FindAsset<BasicMaterialEffect_Instance>(m_ListofMaterial.at(i));
			delete pMI;

			delete[] m_ListofMaterial.at(i);
		}

		assetMap.clear();

		m_ListofGeometry.clear();
		m_ListofTexture.clear();
		m_ListofMaterial.clear();

		//delete instance;
	}

	static void SetDevice(ID3D11Device* pDevice)
	{
		device = pDevice;
	}
	
	static AssetDatabase * GetInstance();

	template <class T>
	T* LoadAsset(WCHAR* id)
	{
		T * t = FindAsset<T>(id);

		if (t != nullptr)
			return t;

		t = new T();
		t->LoadFromFilename(device, id);
		assetMap[typeid(T)][id] = t;
		return t;
	}

	/*
	// A specific method for textures... In the future, this should receive a tuple and make asset accept parameters
	// on initialization
	Texture * LoadTexture(std::string id)
	{
		Texture * t = FindAsset<Texture>(id);

		if (t != nullptr)
			return t;

		t = new Texture();
		t->LoadFromImage(device, id.c_str(), Min_Filter, Mag_Filter, Wrap_s, Wrap_t);
		assetMap[typeid(Texture)][id] = t;
		return t;
	}
	*/

	template<class T>
	T * FindAsset(WCHAR* id)
	{
		std::unordered_map<WCHAR*, Asset*>& idMap = assetMap[typeid(T)];

		if (idMap.find(id) != idMap.end())
			return dynamic_cast<T*>(idMap[id]);

		return nullptr;
	}

};

#endif // ASSETDATABASE_H
