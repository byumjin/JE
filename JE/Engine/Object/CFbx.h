/*
#ifndef CFBX_H
#define CFBX_H

#include <fbxsdk.h>
#include "Object.h"
#include "../Common/Effect/Effects.h"

class CFbx : public Object
{
public:
	CFbx();
	virtual ~CFbx();

	virtual void Shutdown();


	virtual void Import(const char* path)
	{
		LoadObject(path);		
	}

	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
	
	
	
	void LoadUVInformation(FbxMesh* pMesh);


	bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false);
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

	
	void GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup);

	//get mesh normals info
	void GetNormals(FbxNode* pNode);

	
	HRESULT LoadObject(const char* path);

	virtual void Draw(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc);

private:
	FbxManager* g_pFbxSdkManager;
	Vertex *VertexContainer;
	
};

class CFbxManger
{
public:
	CFbxManger();
	~CFbxManger();

	HRESULT LoadObject(const char* path);

	//HRESULT LoadObject(const char* path);

	std::vector<CFbx> mFbx_List;

private:
};



#endif
*/