#pragma once

#include <fbxsdk.h>
#include "../../stdafx.h"
#include "D3DUtil.h"
#include "../Shaders/ShaderStructure.h"
#include "../../Assets/Asset.h"
#include "tiny_obj_loader.h"

class Geometry : public Asset
{
public:

	Geometry()
	{
		g_pFbxSdkManager = NULL;

		mVB = NULL; //Vertex buffer
		mIB = NULL; //Index buffer

		mVBforBB = NULL;
		mIBforBB = NULL;

		ZeroMemory(m_strPath, MAX_PATH);

		uSmoothingGroupMax = 0;
	}

	~Geometry()
	{
		if (g_pFbxSdkManager)
			g_pFbxSdkManager->Destroy();

		if (VertexContainer)
		{
			delete[] VertexContainer;
			VertexContainer = NULL;
		}

		ReleaseCOM(mVB);
		ReleaseCOM(mIB);

		ReleaseCOM(mVBforBB);
		ReleaseCOM(mIBforBB);

		mMeshData.Vertices.clear();
		std::vector<Vertex> vVertexClear;
		mMeshData.Vertices.swap(vVertexClear);

		mMeshData.Indices.clear();
		std::vector<UINT> vIndexClear;
		mMeshData.Indices.swap(vIndexClear);

		std::vector<std::vector<UINT>>::iterator OI;
		UINT count = 0;

		for (count = 0, OI = OriginalIndexArray.begin(); count < OriginalIndexArray.size(); count++, OI++)
		{
			OI->clear();;
			std::vector<UINT> vOIClearInner;
			OI->swap(vOIClearInner);
		}

		OriginalIndexArray.clear();
		std::vector<std::vector<UINT>> vOIClear;
		OriginalIndexArray.swap(vOIClear);

		if (mTriangleArray != NULL)
			delete[] mTriangleArray;
		mTriangleArray = NULL;
	}

	/*
	virtual void Import(const char* path)
	{
		LoadObject(path);
	}
	*/

	void LoadFromFilename(ID3D11Device *pd3dDevice, LPCWSTR path);	

	HRESULT LoadObj(const char* path);


	void Calculatehandedness(const XMFLOAT3 &F1, const XMFLOAT3 &F2, XMFLOAT3 &TANGENT, const XMFLOAT3 &BINORMAL);


	//FBX
	HRESULT LoadFbx(const char* path);
	void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
	void LoadUVInformation(FbxMesh* pMesh);
	bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false);
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);


	void GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup);

	//get mesh normals info
	void GetNormals(FbxNode* pNode);
	void GetTBN();

	void BuildBuffers(ID3D11Device *pd3dDevice);
	void BuildVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS);
	void BuildIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS);

	void BuildBoundingBoxBuffers(ID3D11Device *pd3dDevice);
	void BuildBoundingBoxVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS);
	void BuildBoundingBoxIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS);

	UINT GetIndexCount()
	{
		return mTotalIndexCount;
	}

	Triangle* GetTriangleArray();
	void SetTriangleArray(Triangle* p);
	UINT GetTriangleCount();
	void SetTriangleCount(UINT val);
	void SetLayerCount(UINT val);
	UINT GetLayerCount();

	MeshData* GetMeshData();
	void SetMeshData(MeshData paraMeshdata);
	
	void Geometry::CalculateTriangleArray(long vertexCount, const XMFLOAT3 *vertex, const XMFLOAT3 *normal,
		const XMFLOAT2 *texcoord, long triangleCount, const Triangle *triangle);

	ID3D11Buffer* mVB; //Vertex buffer
	ID3D11Buffer* mIB; //Index buffer

	ID3D11Buffer* mVBforBB; //Vertex buffer for BoundingBox
	ID3D11Buffer* mIBforBB; //Index buffer for BoundingBox

	UINT mTotalVertexCount;
	UINT mTotalIndexCount;

	XMFLOAT3 *mvertexArray;
	XMFLOAT2 *mtexcoordArray;
	Triangle *mTriangleArray;

	MeshData mMeshData;

	BoundingBox mAxisAlignedBox;
	char m_strPath[MAX_PATH];

	FbxManager* g_pFbxSdkManager;
	Vertex *VertexContainer;

	std::vector<std::vector<UINT>> OriginalIndexArray;

	UINT mTriangleCount;
	UINT mLayerCount;

	UINT uSmoothingGroupMax;
};