/*
#include "CFbx.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

static bool gVerbose = true;

CFbx::CFbx()
{
	g_pFbxSdkManager = NULL;
}

CFbx::~CFbx()
{
	Shutdown();		
}

 void CFbx::Shutdown()
{
	Object::Shutdown();

	if (g_pFbxSdkManager)
		g_pFbxSdkManager->Destroy();

	if (VertexContainer)
	{
		delete[] VertexContainer;
		VertexContainer = NULL;
	}
}

void CFbx::Draw(ID3D11DeviceContext* pd3dImmediateContext, ID3DX11EffectTechnique* pTech, D3DX11_TECHNIQUE_DESC &techDesc)
{
	Object::Draw(pd3dImmediateContext, pTech, techDesc);
}

void CFbx::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

void CFbx::DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

bool CFbx::SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
{
	int lMajor, lMinor, lRevision;
	bool lStatus = true;

	// Create an exporter.
	FbxExporter* lExporter = FbxExporter::Create(pManager, "");

	if (pFileFormat < 0 || pFileFormat >= pManager->GetIOPluginRegistry()->GetWriterFormatCount())
	{
		// Write in fall back format in less no ASCII format found
		pFileFormat = pManager->GetIOPluginRegistry()->GetNativeWriterFormat();

		//Try to export in ASCII if possible
		int lFormatIndex, lFormatCount = pManager->GetIOPluginRegistry()->GetWriterFormatCount();

		for (lFormatIndex = 0; lFormatIndex < lFormatCount; lFormatIndex++)
		{
			if (pManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
			{
				FbxString lDesc = pManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
				const char *lASCII = "ascii";
				if (lDesc.Find(lASCII) >= 0)
				{
					pFileFormat = lFormatIndex;
					break;
				}
			}
		}
	}

	// Set the export states. By default, the export states are always set to 
	// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
	// shows how to change these states.
	IOS_REF.SetBoolProp(EXP_FBX_MATERIAL, true);
	IOS_REF.SetBoolProp(EXP_FBX_TEXTURE, true);
	IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED, pEmbedMedia);
	IOS_REF.SetBoolProp(EXP_FBX_SHAPE, true);
	IOS_REF.SetBoolProp(EXP_FBX_GOBO, true);
	IOS_REF.SetBoolProp(EXP_FBX_ANIMATION, true);
	IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

	// Initialize the exporter by providing a filename.
	if (lExporter->Initialize(pFilename, pFileFormat, pManager->GetIOSettings()) == false)
	{
		FBXSDK_printf("Call to FbxExporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		return false;
	}

	FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
	FBXSDK_printf("FBX file format version %d.%d.%d\n\n", lMajor, lMinor, lRevision);

	// Export the scene.
	lStatus = lExporter->Export(pScene);

	// Destroy the exporter.
	lExporter->Destroy();
	return lStatus;
}

bool CFbx::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());		

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

HRESULT CFbx::LoadObject(const char* path)
{
	//int len = strlen(path);
	strcpy_s(m_strPath, path);
	//m_strPath[len-1] = '\0';

	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;


	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Load the scene.
	
	FBXSDK_printf("\n\nFile: %s\n\n", path);
	lResult = LoadScene(lSdkManager, lScene, path);

	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while loading the scene...");
	}
	else
	{
		if (!lScene)
		{
			FBX_ASSERT_NOW("null scene");
		}

		//get root node of the fbx scene
		FbxNode* lRootNode = lScene->GetRootNode();

		//get normals info, if there're mesh in the scene
		GetNormals(lRootNode);

		//set me true to compute smoothing info from normals
		bool lComputeFromNormals = false;
		//set me true to convert hard/soft edges info to smoothing groups info
		bool lConvertToSmoothingGroup = false;
		//get smoothing info, if there're mesh in the scene
		GetSmoothing(lSdkManager, lRootNode, lComputeFromNormals, lConvertToSmoothingGroup);



		//Get the first node in the scene
		FbxNode* lNodeOfInterest = lScene->GetRootNode()->GetChild(0);
		if (lNodeOfInterest)
		{
			FbxMesh* lMeshOFInterest = lNodeOfInterest->GetMesh();
			if (lMeshOFInterest)
			{
				//first, load the UV information and display them
				LoadUVInformation(lMeshOFInterest);			
			}
		}

		GetTBN();
	}

	//Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);
	
	if (VertexContainer)
	{
		delete[] VertexContainer;
		VertexContainer = NULL;
	}

	return true;
}
//get mesh smoothing info
//set pCompute true to compute smoothing from normals by default 
//set pConvertToSmoothingGroup true to convert hard/soft edge info to smoothing group info by default
void CFbx::GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup)
{
	if (!pNode || !pSdkManager)
		return;

	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();
	if (lMesh)
	{
		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

		//if there's no smoothing info in fbx file, but you still want to get smoothing info.
		//please compute smoothing info from normals.
		//Another case to recompute smoothing info from normals is:
		//If users edit normals manually in 3ds Max or Maya and export the scene to FBX with smoothing info,
		//The smoothing info may NOT match with normals.
		//the mesh called "fbx_customNormals" in Normals.fbx is the case. All edges are hard, but normals actually represent the "soft" looking.
		//Generally, the normals in fbx file holds the smoothing result you'd like to get.
		//If you want to get correct smoothing info(smoothing group or hard/soft edges) which match with normals,
		//please drop the original smoothing info of fbx file, and recompute smoothing info from normals.
		//if you want to get soft/hard edge info, please call FbxGeometryConverter::ComputeEdgeSmoothingFromNormals().
		//if you want to get smoothing group info, please get soft/hard edge info first by ComputeEdgeSmoothingFromNormals() 
		//And then call FbxGeometryConverter::ComputePolygonSmoothingFromEdgeSmoothing().
		if (pCompute)
		{
			FbxGeometryConverter lGeometryConverter(pSdkManager);
			lGeometryConverter.ComputeEdgeSmoothingFromNormals(lMesh);
			//convert soft/hard edge info to smoothing group info
			if (pConvertToSmoothingGroup)
				lGeometryConverter.ComputePolygonSmoothingFromEdgeSmoothing(lMesh);
		}

		//if there is smoothing groups info in your fbx file, but you want to get hard/soft edges info
		//please use following code:
		//FbxGeometryConverter lGeometryConverter(lSdkManager);
		//lGeometryConverter.ComputeEdgeSmoothingFromPolygonSmoothing(lMesh);

		//get smoothing info
		FbxGeometryElementSmoothing* lSmoothingElement = lMesh->GetElementSmoothing();
		if (lSmoothingElement)
		{
			//mapping mode is by edge. The mesh usually come from Maya, because Maya can set hard/soft edges.
			//we can get smoothing info(which edges are soft, which edges are hard) by retrieving each edge. 
			if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByEdge)
			{
				//Let's get smoothing of each edge, since the mapping mode of smoothing element is by edge
				for (int lEdgeIndex = 0; lEdgeIndex < lMesh->GetMeshEdgeCount(); lEdgeIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as edge index.
					//get smoothing by the index of edge
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lEdgeIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lEdgeIndex);

					//Got smoothing of each vertex.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("hard/soft value for edge[%d]: %d \n", lEdgeIndex, lSmoothingFlag);
					//add your custom code here, to output smoothing or get them into a list, such as KArrayTemplate<int>
					//. . .
				}//end for lEdgeIndex
			}//end eByEdge
			 //mapping mode is by polygon. The mesh usually come from 3ds Max, because 3ds Max can set smoothing groups for polygon.
			 //we can get smoothing info(smoothing group ID for each polygon) by retrieving each polygon. 
			else if (lSmoothingElement->GetMappingMode() == FbxGeometryElement::eByPolygon)
			{
				int lIndexByPolygonVertex = 0;

				//Let's get smoothing of each polygon, since the mapping mode of smoothing element is by polygon.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					int lSmoothingIndex = 0;
					//reference mode is direct, the smoothing index is same as polygon index.
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lSmoothingIndex = lPolygonIndex;

					//reference mode is index-to-direct, get smoothing by the index-to-direct
					if (lSmoothingElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						lSmoothingIndex = lSmoothingElement->GetIndexArray().GetAt(lPolygonIndex);

					//Got smoothing of each polygon.
					int lSmoothingFlag = lSmoothingElement->GetDirectArray().GetAt(lSmoothingIndex);
					if (gVerbose) FBXSDK_printf("smoothing group ID for polygon[%d]: %d \n", lPolygonIndex, lSmoothingFlag);
					//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<int>
					//. . .

					//get polygon size, you know how many vertices in current polygon.
					int lPolygonSize = lMesh->GetPolygonSize(lPolygonIndex);

					//retrieve each vertex of current polygon.
					for (int i = 0; i < lPolygonSize; i++)
					{
						VertexContainer[lIndexByPolygonVertex].SmoothingID = (UINT)lSmoothingFlag;

						if (VertexContainer[lIndexByPolygonVertex].SmoothingID > uSmoothingGroupMax)
						{
							uSmoothingGroupMax = VertexContainer[lIndexByPolygonVertex].SmoothingID;
						}

						lIndexByPolygonVertex++;
					}
				}//end for lPolygonIndex //PolygonCount

			}//end eByPolygonVertex
		}//end if lSmoothingElement
	}//end if lMesh

	 //recursively traverse each node in the scene
	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetSmoothing(pSdkManager, pNode->GetChild(i), pCompute, pConvertToSmoothingGroup);
	}
}



//get mesh normals info
void CFbx::GetNormals(FbxNode* pNode)
{
	if (!pNode)
		return;



	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();
	if (lMesh)
	{
		int iTrianlgeCount = 0;

		{	
			for (int lPolyIndex = 0; lPolyIndex < lMesh->GetPolygonCount(); ++lPolyIndex)
			{
				iTrianlgeCount += lMesh->GetPolygonSize(lPolyIndex) - 2;
			}
		}

		for (int i = 0; i < lMesh->GetControlPointsCount(); i++)
		{
			std::vector<UINT> temp;
			OriginalIndexArray.push_back(temp);
		}

		
		SetTriangleCount(iTrianlgeCount);
		Triangle* pTriangleArray = new Triangle[GetTriangleCount()];
		SetTriangleArray(pTriangleArray);

		VertexContainer = new Vertex[lMesh->GetPolygonVertexCount()];


		//print mesh node name
		FBXSDK_printf("current mesh node: %s\n", pNode->GetName());

		//get the normal element
		FbxGeometryElementNormal* lNormalElement = lMesh->GetElementNormal();
		if (lNormalElement)
		{
			//mapping mode is by control points. The mesh should be smooth and soft.
			//we can get normals by retrieving each control point
			if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
			{				
				int lIndexByPolygonVertex = 0;
				int lTrianlgeIndexByPolygonVertex = 0;

				//Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					//get polygon size, you know how many vertices in current polygon.
					int lPolygonSize = lMesh->GetPolygonSize(lPolygonIndex);

					//retrieve each vertex of current polygon.
					for (int i = 0; i < lPolygonSize; i++)
					{
						{
							int lNormalIndex = 0;
							//reference mode is direct, the normal index is same as lIndexByPolygonVertex.
							if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
								lNormalIndex = lIndexByPolygonVertex;

							//reference mode is index-to-direct, get normals by the index-to-direct
							if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
								lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

							//Got normals of each polygon-vertex.
							FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
							if (gVerbose) FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n",
								lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
							//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
							//. . .

							VertexContainer[lIndexByPolygonVertex].Normal = XMFLOAT3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]);
							VertexContainer[lIndexByPolygonVertex].OriginalIndex = lMesh->GetPolygonVertex(lPolygonIndex, i);
							OriginalIndexArray.at(VertexContainer[lIndexByPolygonVertex].OriginalIndex).push_back(lIndexByPolygonVertex);

						}

						int iControlPointIndex = lMesh->GetPolygonVertex(lPolygonIndex, i);
						FbxVector4* pVertices = lMesh->GetControlPoints();

						VertexContainer[lIndexByPolygonVertex].Position.x = (float)pVertices[iControlPointIndex].mData[0];
						VertexContainer[lIndexByPolygonVertex].Position.y = (float)pVertices[iControlPointIndex].mData[1];
						VertexContainer[lIndexByPolygonVertex].Position.z = (float)pVertices[iControlPointIndex].mData[2];

						lIndexByPolygonVertex++;

					}//end for i //lPolygonSize

					for (int i = lPolygonSize - 2; i > 0; i--)
					{
						pTriangleArray[lTrianlgeIndexByPolygonVertex].PolygonIndex = lPolygonIndex;

						if (lPolygonSize == 3)
						{
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 2;
						}
						else if (lPolygonSize == 4)
						{
							if (i == 2)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
							else if (i == 1)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
						}
						else if (lPolygonSize == 5)
						{
							if (i == 3)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
							else if (i == 2)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 4;
							}
							else if (i == 1)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 3;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 4;
							}
						}

						lTrianlgeIndexByPolygonVertex++;
					}

				}//end for lPolygonIndex //PolygonCount
				
			}//end eByControlPoint
			//mapping mode is by polygon-vertex.
			//we can get normals by retrieving polygon-vertex.
			else if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
			{
				int lIndexByPolygonVertex = 0;
				int lTrianlgeIndexByPolygonVertex = 0;

				//Let's get normals of each polygon, since the mapping mode of normal element is by polygon-vertex.
				for (int lPolygonIndex = 0; lPolygonIndex < lMesh->GetPolygonCount(); lPolygonIndex++)
				{
					//get polygon size, you know how many vertices in current polygon.
					int lPolygonSize = lMesh->GetPolygonSize(lPolygonIndex);

					//retrieve each vertex of current polygon.
					for (int i = 0; i < lPolygonSize; i++)
					{
						{
							int lNormalIndex = 0;
							//reference mode is direct, the normal index is same as lIndexByPolygonVertex.
							if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
								lNormalIndex = lIndexByPolygonVertex;

							//reference mode is index-to-direct, get normals by the index-to-direct
							if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
								lNormalIndex = lNormalElement->GetIndexArray().GetAt(lIndexByPolygonVertex);

							//Got normals of each polygon-vertex.
							FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
							if (gVerbose) FBXSDK_printf("normals for polygon[%d]vertex[%d]: %f %f %f %f \n",
								lPolygonIndex, i, lNormal[0], lNormal[1], lNormal[2], lNormal[3]);
							//add your custom code here, to output normals or get them into a list, such as KArrayTemplate<FbxVector4>
							//. . .

							VertexContainer[lIndexByPolygonVertex].Normal = XMFLOAT3((float)lNormal[0], (float)lNormal[1], (float)lNormal[2]);
							VertexContainer[lIndexByPolygonVertex].OriginalIndex = lMesh->GetPolygonVertex(lPolygonIndex, i);
							OriginalIndexArray.at(VertexContainer[lIndexByPolygonVertex].OriginalIndex).push_back(lIndexByPolygonVertex);

						}

						int iControlPointIndex = lMesh->GetPolygonVertex(lPolygonIndex, i);
						FbxVector4* pVertices = lMesh->GetControlPoints();
						
						VertexContainer[lIndexByPolygonVertex].Position.x = (float)pVertices[iControlPointIndex].mData[0];
						VertexContainer[lIndexByPolygonVertex].Position.y = (float)pVertices[iControlPointIndex].mData[1];
						VertexContainer[lIndexByPolygonVertex].Position.z = (float)pVertices[iControlPointIndex].mData[2];
						
						lIndexByPolygonVertex++;

					}//end for i //lPolygonSize
					
					for (int i = lPolygonSize - 2; i > 0; i--)
					{
						pTriangleArray[lTrianlgeIndexByPolygonVertex].PolygonIndex = lPolygonIndex;

						if (lPolygonSize == 3)
						{
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
							pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 2;
						}
						else if (lPolygonSize == 4)
						{
							if (i == 2)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
							else if (i == 1)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
						}
						else if (lPolygonSize == 5)
						{
							if (i == 3)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 1;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 3;
							}
							else if (i == 2)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 4;
							}
							else if (i == 1)
							{
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize + 2;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[1] = lIndexByPolygonVertex - lPolygonSize + 3;
								pTriangleArray[lTrianlgeIndexByPolygonVertex].index[2] = lIndexByPolygonVertex - lPolygonSize + 4;
							}
						}

						lTrianlgeIndexByPolygonVertex++;
					}

				}//end for lPolygonIndex //PolygonCount
				
			}//end eByPolygonVertex

			
		}//end if lNormalElement

	
		//ComputeBoundingAxisAlignedBoxFromPoints(&mAxisAlignedBox, pMesh->GetPolygonVertexCount() + iQuadPoly * 2 + iPentaPoly * 4, &VertexContainer[0].Position, sizeof(Vertex));



		UINT *indices = new UINT[GetTriangleCount() * 3];

		for (UINT i = 0; i < GetTriangleCount(); i++)
		{
			indices[i * 3] = pTriangleArray[i].index[0];
			indices[i * 3 + 1] = pTriangleArray[i].index[1];
			indices[i * 3 + 2] = pTriangleArray[i].index[2];
		}

		GetMeshData()->Indices.assign(&indices[0], &indices[GetTriangleCount() * 3]);

		//delete[] VertexContainer;
		delete[] indices;

		//VertexContainer = NULL;
		//indices = NULL;

	}//end if lMesh



	//recursively traverse each node in the scene
	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetNormals(pNode->GetChild(i));
	}
}

void CFbx::LoadUVInformation(FbxMesh* pMesh)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	SetLayerCount(lUVSetNameList.GetCount());

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
					VertexContainer[lPolyVertIndex].UVSetCount = lUVSetNameList.GetCount();
					VertexContainer[lPolyVertIndex].TexCoordSet[lUVSetIndex] = XMFLOAT2((float)lUVValue.mData[0], 1.0f - (float)lUVValue.mData[1]);					

					//0 is Standard
					VertexContainer[lPolyVertIndex].TexCoord = VertexContainer[lPolyVertIndex].TexCoordSet[0];

					//lPolyIndexCounter++;
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						VertexContainer[lPolyIndexCounter].UVSetCount = lUVSetNameList.GetCount();
						VertexContainer[lPolyIndexCounter].TexCoordSet[lUVSetIndex] = XMFLOAT2((float)lUVValue.mData[0], 1.0f - (float)lUVValue.mData[1]);
												
						//0 is Standard
						VertexContainer[lPolyIndexCounter].TexCoord = VertexContainer[lPolyIndexCounter].TexCoordSet[0];

						lPolyIndexCounter++;
					}
				}
			}
		}
	}


	for (int i = 0; i < pMesh->GetPolygonVertexCount(); i++)
	{
		GetMeshData()->Vertices.push_back(VertexContainer[i]);

		//DeepCopy
		
	}

	BoundingBox::CreateFromPoints(mAxisAlignedBox, pMesh->GetPolygonVertexCount(), &VertexContainer[0].Position, sizeof(Vertex));
}


CFbxManger::CFbxManger()
{
	//mUsize = 0;
}
CFbxManger::~CFbxManger()
{
	mFbx_List.clear();
}
HRESULT CFbxManger::LoadObject(const char* path)
{
	//CFbx *mCFbx = new CFbx;
	CFbx mCFbx;
	HR(mCFbx.LoadObject(path));
	mFbx_List.push_back(mCFbx);

	//delete mCFbx;
	
	

	return true;
}
*/