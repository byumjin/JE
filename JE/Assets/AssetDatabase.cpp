#include "assetdatabase.h"

AssetDatabase * AssetDatabase::instance = nullptr;
ID3D11Device* AssetDatabase::device = nullptr;

//std::vector<WCHAR*> AssetDatabase::m_ListofGeometry;
//std::vector<WCHAR*> AssetDatabase::m_ListofTexture;

AssetDatabase::AssetDatabase()
{
}

AssetDatabase *AssetDatabase::GetInstance()
{
	if (instance == nullptr)
		instance = new AssetDatabase();
	
	return instance;
}
