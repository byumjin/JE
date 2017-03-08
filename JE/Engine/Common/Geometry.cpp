#include "Geometry.h"



#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

static bool gVerbose = true;


void Geometry::InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
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

void Geometry::DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

bool Geometry::SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
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

bool Geometry::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
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

HRESULT Geometry::LoadObj(const char* path)
{
	//QString filepath = file;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errors = tinyobj::LoadObj(shapes, materials, path);
	//std::cout << errors << std::endl;

	//VertexContainer[lIndexByPolygonVertex]
	//pTriangleArray[lTrianlgeIndexByPolygonVertex]

	if (errors.size() == 0)
	{
		int min_idx = 0;
		//Read the information from the vector of shape_ts
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			VertexContainer = new Vertex[shapes[i].mesh.positions.size()];

			std::vector<XMFLOAT3> pos, nor;
			std::vector<XMFLOAT2> uv;
			std::vector<float> &positions = shapes[i].mesh.positions;
			std::vector<float> &normals = shapes[i].mesh.normals;
			std::vector<float> &uvs = shapes[i].mesh.texcoords;

			for (unsigned int j = 0; j < positions.size() / 3; j++)
			{
				VertexContainer[j].Position = XMFLOAT3(positions[j * 3], positions[j * 3 + 1], positions[j * 3 + 2]);
			}

			for (unsigned int j = 0; j < normals.size() / 3; j++)
			{
				VertexContainer[j].Normal = XMFLOAT3(normals[j * 3], normals[j * 3 + 1], normals[j * 3 + 2]);
			}

			for (unsigned int j = 0; j < uvs.size() / 2; j++)
			{
				VertexContainer[j].TexCoord = XMFLOAT2(uvs[j * 2], 1.0f - uvs[j * 2 + 1]);
			}

			SetLayerCount(1);

			std::vector<unsigned int> indices = shapes[i].mesh.indices;

			SetTriangleCount(indices.size() / 3);
			
			Triangle* pTriangleArray = new Triangle[GetTriangleCount()];
			SetTriangleArray(pTriangleArray);

			for (unsigned int j = 0; j < indices.size(); j += 3)
			{
				pTriangleArray[j / 3].index[0] = indices[j] + min_idx;
				pTriangleArray[j / 3].index[1] = indices[j+1] + min_idx;
				pTriangleArray[j / 3].index[2] = indices[j+2] + min_idx;				
			}

			for (UINT x = 0; x < indices.size(); x++)
			{
				GetMeshData()->Indices.push_back(indices[x]);
			}			

			for (UINT x = 0; x < shapes[i].mesh.positions.size() / 3; x++)
			{
				GetMeshData()->Vertices.push_back(VertexContainer[x]);
			}

			BoundingBox::CreateFromPoints(mAxisAlignedBox, shapes[i].mesh.positions.size() / 3, &VertexContainer[0].Position, sizeof(Vertex));

			//pTriangleArray[lTrianlgeIndexByPolygonVertex].index[0] = lIndexByPolygonVertex - lPolygonSize + 2;
		}
	}
	else
	{
		//An error loading the OBJ occurred!
		//std::cout << errors << std::endl;
	}

	GetTBN();

	

	if (VertexContainer)
	{
		delete[] VertexContainer;
		VertexContainer = NULL;
	}

	return true;
}

HRESULT Geometry::LoadFbx(const char* path)
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
void Geometry::GetSmoothing(FbxManager* pSdkManager, FbxNode* pNode, bool pCompute, bool pConvertToSmoothingGroup)
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
void Geometry::GetNormals(FbxNode* pNode)
{
	if (!pNode)
		return;

	//get mesh
	FbxMesh* lMesh = pNode->GetMesh();
	if (lMesh)
	{
		int iTrianlgeCount = 0;

		for (int lPolyIndex = 0; lPolyIndex < lMesh->GetPolygonCount(); ++lPolyIndex)
		{
			iTrianlgeCount += lMesh->GetPolygonSize(lPolyIndex) - 2;
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

		delete[] indices;
	}//end if lMesh



	 //recursively traverse each node in the scene
	int i, lCount = pNode->GetChildCount();
	for (i = 0; i < lCount; i++)
	{
		GetNormals(pNode->GetChild(i));
	}
}

void Geometry::LoadUVInformation(FbxMesh* pMesh)
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
	}

	BoundingBox::CreateFromPoints(mAxisAlignedBox, pMesh->GetPolygonVertexCount(), &VertexContainer[0].Position, sizeof(Vertex));
}

void Geometry::BuildBuffers(ID3D11Device *pd3dDevice)
{

	mTotalVertexCount = mMeshData.Vertices.size();
	mTotalIndexCount = mMeshData.Indices.size();

	std::vector<Vertex> vertices(mTotalVertexCount);

	for (UINT p = 0; p < mTotalVertexCount; p++)
	{
		vertices[p].Position = mMeshData.Vertices[p].Position;
		vertices[p].Normal = mMeshData.Vertices[p].Normal;
		vertices[p].BiNormalV = mMeshData.Vertices[p].BiNormalV;
		vertices[p].TangentU = mMeshData.Vertices[p].TangentU;
		vertices[p].Color = mMeshData.Vertices[p].Color;

		for (int i = 0; i < MAX_UV_SET; i++)
		{
			vertices[p].TexCoordSet[i] = mMeshData.Vertices[p].TexCoordSet[i];
		}

		vertices[p].TexCoord = mMeshData.Vertices[p].TexCoord;
	}

	BuildVertexBuffers(pd3dDevice, &vertices[0], D3D11_USAGE_IMMUTABLE, mTotalVertexCount, D3D11_BIND_VERTEX_BUFFER, 0, 0);

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;

	indices.insert(indices.end(), mMeshData.Indices.begin(), mMeshData.Indices.end());

	BuildIndexBuffers(pd3dDevice, &indices[0], D3D11_USAGE_IMMUTABLE, mTotalIndexCount, D3D11_BIND_INDEX_BUFFER, 0, 0);


}

void Geometry::BuildVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = USAGE;
	vbd.ByteWidth = sizeof(Vertex)* totalVertexCount;
	vbd.BindFlags = BINDFALGS;
	vbd.CPUAccessFlags = CPUACCESSFLAGS;
	vbd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pvertices;
	HRESULT(pd3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));
}

void Geometry::BuildIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = USAGE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = BINDFALGS;
	ibd.CPUAccessFlags = CPUACCESSFLAGS;
	ibd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pindices;
	HRESULT(pd3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Geometry::BuildBoundingBoxBuffers(ID3D11Device *pd3dDevice)
{
	XMFLOAT3 Center = mAxisAlignedBox.Center;
	XMFLOAT3 Extents = mAxisAlignedBox.Extents;
	Vertex BoundingBoxVertice[8];

	BoundingBoxVertice[0].Position = XMFLOAT3(Center.x + Extents.x, Center.y + Extents.y, Center.z + Extents.z);
	BoundingBoxVertice[1].Position = XMFLOAT3(Center.x + Extents.x, Center.y - Extents.y, Center.z + Extents.z);
	BoundingBoxVertice[2].Position = XMFLOAT3(Center.x + Extents.x, Center.y - Extents.y, Center.z - Extents.z);
	BoundingBoxVertice[3].Position = XMFLOAT3(Center.x + Extents.x, Center.y + Extents.y, Center.z - Extents.z);

	BoundingBoxVertice[4].Position = XMFLOAT3(Center.x - Extents.x, Center.y + Extents.y, Center.z + Extents.z);
	BoundingBoxVertice[5].Position = XMFLOAT3(Center.x - Extents.x, Center.y - Extents.y, Center.z + Extents.z);
	BoundingBoxVertice[6].Position = XMFLOAT3(Center.x - Extents.x, Center.y - Extents.y, Center.z - Extents.z);
	BoundingBoxVertice[7].Position = XMFLOAT3(Center.x - Extents.x, Center.y + Extents.y, Center.z - Extents.z);

	UINT BoundingBoxIndices[24];

	BoundingBoxIndices[0] = 0; BoundingBoxIndices[1] = 1;
	BoundingBoxIndices[2] = 1; BoundingBoxIndices[3] = 2;
	BoundingBoxIndices[4] = 2; BoundingBoxIndices[5] = 3;
	BoundingBoxIndices[6] = 3; BoundingBoxIndices[7] = 0;

	BoundingBoxIndices[8] = 4; BoundingBoxIndices[9] = 5;
	BoundingBoxIndices[10] = 5; BoundingBoxIndices[11] = 6;
	BoundingBoxIndices[12] = 6; BoundingBoxIndices[13] = 7;
	BoundingBoxIndices[14] = 7; BoundingBoxIndices[15] = 4;

	BoundingBoxIndices[16] = 0; BoundingBoxIndices[17] = 4;
	BoundingBoxIndices[18] = 1; BoundingBoxIndices[19] = 5;
	BoundingBoxIndices[20] = 2; BoundingBoxIndices[21] = 6;
	BoundingBoxIndices[22] = 3; BoundingBoxIndices[23] = 7;

	BuildBoundingBoxVertexBuffers(pd3dDevice, &BoundingBoxVertice[0], D3D11_USAGE_IMMUTABLE, 8, D3D11_BIND_VERTEX_BUFFER, 0, 0/*, mVBforBB*/);
	BuildBoundingBoxIndexBuffers(pd3dDevice, &BoundingBoxIndices[0], D3D11_USAGE_IMMUTABLE, 24, D3D11_BIND_INDEX_BUFFER, 0, 0/*, mIBforBB*/);
}

void Geometry::BuildBoundingBoxVertexBuffers(ID3D11Device *pd3dDevice, Vertex *pvertices, D3D11_USAGE USAGE, UINT totalVertexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = USAGE;
	vbd.ByteWidth = sizeof(Vertex)* totalVertexCount;
	vbd.BindFlags = BINDFALGS;
	vbd.CPUAccessFlags = CPUACCESSFLAGS;
	vbd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pvertices;
	HRESULT(pd3dDevice->CreateBuffer(&vbd, &vinitData, &mVBforBB));
}

void Geometry::BuildBoundingBoxIndexBuffers(ID3D11Device *pd3dDevice, UINT *pindices, D3D11_USAGE USAGE, UINT totalIndexCount, UINT BINDFALGS, UINT CPUACCESSFLAGS, UINT MISCFLAGS)
{
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = USAGE;
	ibd.ByteWidth = sizeof(UINT)* totalIndexCount;
	ibd.BindFlags = BINDFALGS;
	ibd.CPUAccessFlags = CPUACCESSFLAGS;
	ibd.MiscFlags = MISCFLAGS;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pindices;
	HRESULT(pd3dDevice->CreateBuffer(&ibd, &iinitData, &mIBforBB));
}

Triangle* Geometry::GetTriangleArray()
{
	return mTriangleArray;
}

void Geometry::SetTriangleArray(Triangle* p)
{
	mTriangleArray = p;
}

void Geometry::SetLayerCount(UINT val)
{
	mLayerCount = val;
}

UINT Geometry::GetLayerCount()
{
	return mLayerCount;
}

MeshData* Geometry::GetMeshData()
{
	return &mMeshData;
}

void Geometry::SetMeshData(MeshData paraMeshdata)
{
	mMeshData = paraMeshdata;
}

void Geometry::GetTBN()
{
	mvertexArray = new XMFLOAT3[mMeshData.Vertices.size()];
	XMFLOAT3 *mnormalArray = new XMFLOAT3[mMeshData.Vertices.size()];
	mtexcoordArray = new XMFLOAT2[mMeshData.Vertices.size()];

	for (UINT i = 0; i < mMeshData.Vertices.size(); i++)
	{
		mvertexArray[i] = mMeshData.Vertices.at(i).Position;
		mnormalArray[i] = mMeshData.Vertices.at(i).Normal;
		mtexcoordArray[i] = mMeshData.Vertices.at(i).TexCoord;
	}

	CalculateTriangleArray(mMeshData.Vertices.size(), mvertexArray, mnormalArray, mtexcoordArray, mTriangleCount, mTriangleArray);

	if (mvertexArray != NULL)
		delete[] mvertexArray;
	mvertexArray = NULL;

	if (mtexcoordArray != NULL)
		delete[] mtexcoordArray;
	mtexcoordArray = NULL;

	if (mnormalArray != NULL)
		delete[] mnormalArray;
	mnormalArray = NULL;

	if (mTriangleArray != NULL)
		delete[] mTriangleArray;
	mTriangleArray = NULL;

}

void Geometry::CalculateTriangleArray(long vertexCount, const XMFLOAT3 *vertex, const XMFLOAT3 *normal, const XMFLOAT2 *texcoord, long triangleCount, const Triangle *triangle)
{
	XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 3];
	XMFLOAT3 *tan2 = tan1 + vertexCount;
	XMFLOAT3 *tan3 = tan2 + vertexCount;
	ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 3);

	for (long a = 0; a < triangleCount; a++)
	{
		long i1 = triangle->index[0];
		long i2 = triangle->index[1];
		long i3 = triangle->index[2];

		const XMFLOAT3& p0 = vertex[i1];
		const XMFLOAT3& p1 = vertex[i2];
		const XMFLOAT3& p2 = vertex[i3];

		const XMFLOAT2& w0 = texcoord[i1];
		const XMFLOAT2& w1 = texcoord[i2];
		const XMFLOAT2& w2 = texcoord[i3];

		float e0x = p1.x - p0.x;
		float e1x = p2.x - p0.x;

		float e0y = p1.y - p0.y;
		float e1y = p2.y - p0.y;

		float e0z = p1.z - p0.z;
		float e1z = p2.z - p0.z;

		float u0 = w1.x - w0.x;
		float u1 = w2.x - w0.x;

		float v0 = w1.y - w0.y;
		float v1 = w2.y - w0.y;

		float dino = u0 * v1 - v0 * u1;

		if (dino != 0.0f)
		{
			float r = 1.0f / (dino);
			XMFLOAT3 TANGENT((v1 * e0x - v0 * e1x) * r,	(v1 * e0y - v0 * e1y) * r, (v1 * e0z - v0 * e1z) * r);
			XMFLOAT3 BINORMAL((u0 * e1x - u1 * e0x) * r, (u0 * e1y - u1 * e0y) * r,	(u0 * e1z - u1 * e0z) * r);


			TANGENT = MathHelper::XMFLOAT3_NORMALIZE(TANGENT);
			BINORMAL = MathHelper::XMFLOAT3_NORMALIZE(BINORMAL);

			//XMFLOAT3 NORMAL;

			// Calculate handedness
			XMFLOAT3 fFaceNormal;
			fFaceNormal = MathHelper::XMFLOAT3_CROSS(MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(e0x, e0y, e0z)), MathHelper::XMFLOAT3_NORMALIZE(XMFLOAT3(e1x, e1y, e1z)));

			fFaceNormal = MathHelper::XMFLOAT3_NORMALIZE(fFaceNormal);

			//U flip
			if (MathHelper::XMFLOAT3_DOT(MathHelper::XMFLOAT3_CROSS(TANGENT, BINORMAL), fFaceNormal) < 0.0f)
			{
				TANGENT = XMFLOAT3(-TANGENT.x, -TANGENT.y, -TANGENT.z);
			}

			//NORMAL = normal[i1]; //MathHelper::XMFLOAT3_CROSS(TANGENT, BINORMAL);

			//TANGENT
			tan1[i1].x += TANGENT.x;
			tan1[i1].y += TANGENT.y;
			tan1[i1].z += TANGENT.z;

			tan1[i2].x += TANGENT.x;
			tan1[i2].y += TANGENT.y;
			tan1[i2].z += TANGENT.z;

			tan1[i3].x += TANGENT.x;
			tan1[i3].y += TANGENT.y;
			tan1[i3].z += TANGENT.z;

			//BINORMAL
			tan2[i1].x += BINORMAL.x;
			tan2[i1].y += BINORMAL.y;
			tan2[i1].z += BINORMAL.z;

			tan2[i2].x += BINORMAL.x;
			tan2[i2].y += BINORMAL.y;
			tan2[i2].z += BINORMAL.z;

			tan2[i3].x += BINORMAL.x;
			tan2[i3].y += BINORMAL.y;
			tan2[i3].z += BINORMAL.z;

			//NORMAL

			tan3[i1].x += normal[i1].x;
			tan3[i1].y += normal[i1].y;
			tan3[i1].z += normal[i1].z;

			tan3[i2].x += normal[i2].x;
			tan3[i2].y += normal[i2].y;
			tan3[i2].z += normal[i2].z;

			tan3[i3].x += normal[i3].x;
			tan3[i3].y += normal[i3].y;
			tan3[i3].z += normal[i3].z;

			/*
			tan3[i1].x += NORMAL.x;
			tan3[i1].y += NORMAL.y;
			tan3[i1].z += NORMAL.z;

			tan3[i2].x += NORMAL.x;
			tan3[i2].y += NORMAL.y;
			tan3[i2].z += NORMAL.z;

			tan3[i3].x += NORMAL.x;
			tan3[i3].y += NORMAL.y;
			tan3[i3].z += NORMAL.z;
			*/
		}
		else
		{
			int x = 9;
		}

		triangle++;
	}

	//Soomthing
	for (UINT k = 0; k < uSmoothingGroupMax + 1; k++)
	{
		for (UINT i = 0; i < OriginalIndexArray.size(); i++)
		{
			XMFLOAT3 TotalTANGENT = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 TotalBINORMAL = XMFLOAT3(0.0f, 0.0f, 0.0f);
			XMFLOAT3 TotalNORMAL = XMFLOAT3(0.0f, 0.0f, 0.0f);

			//check, if a vertex has same smoothing group 
			for (UINT j = 0; j < OriginalIndexArray.at(i).size(); j++)
			{
				if (GetMeshData()->Vertices[OriginalIndexArray.at(i).at(j)].SmoothingID == k)
				{
					TotalTANGENT.x += tan1[OriginalIndexArray.at(i).at(j)].x;
					TotalTANGENT.y += tan1[OriginalIndexArray.at(i).at(j)].y;
					TotalTANGENT.z += tan1[OriginalIndexArray.at(i).at(j)].z;

					TotalBINORMAL.x += tan2[OriginalIndexArray.at(i).at(j)].x;
					TotalBINORMAL.y += tan2[OriginalIndexArray.at(i).at(j)].y;
					TotalBINORMAL.z += tan2[OriginalIndexArray.at(i).at(j)].z;

					TotalNORMAL.x += tan3[OriginalIndexArray.at(i).at(j)].x;
					TotalNORMAL.y += tan3[OriginalIndexArray.at(i).at(j)].y;
					TotalNORMAL.z += tan3[OriginalIndexArray.at(i).at(j)].z;
				}
			}

			for (UINT j = 0; j < OriginalIndexArray.at(i).size(); j++)
			{
				if (GetMeshData()->Vertices[OriginalIndexArray.at(i).at(j)].SmoothingID == k)
				{
					tan1[OriginalIndexArray.at(i).at(j)] = TotalTANGENT;
					tan2[OriginalIndexArray.at(i).at(j)] = TotalBINORMAL;
					tan3[OriginalIndexArray.at(i).at(j)] = TotalNORMAL;
				}
			}
		}
	}
	
	for (long a = 0; a < vertexCount; a++)
	{
		XMVECTOR t = MathHelper::XMFLOAT3_TO_XMVECTOR(tan1[a], 1.0f);
		t = XMVector3Normalize(t);

		XMVECTOR n = MathHelper::XMFLOAT3_TO_XMVECTOR(tan3[a], 1.0f);
		n = XMVector3Normalize(n);

		XMVECTOR b = MathHelper::XMFLOAT3_TO_XMVECTOR(tan2[a], 1.0f);
		b = XMVector3Normalize(b);

		// Gram-Schmidt orthogonalize
		//t = XMVector3Normalize(t - XMVector3Dot(t, n)*n);
		b = XMVector3Cross(n, t);

		XMFLOAT3 ftempTangent = MathHelper::XMVECTOR_TO_XMFLOAT3(t);
		mMeshData.Vertices.at(a).TangentU = ftempTangent;

		XMFLOAT3 ftempBi = MathHelper::XMVECTOR_TO_XMFLOAT3(b);
		mMeshData.Vertices.at(a).BiNormalV = ftempBi;

		XMFLOAT3 ftempNormal = MathHelper::XMVECTOR_TO_XMFLOAT3(n);
		mMeshData.Vertices.at(a).Normal = ftempNormal;
	}

	delete[] tan1;
	tan1 = NULL;
}

UINT Geometry::GetTriangleCount()
{
	return mTriangleCount;
}

void Geometry::SetTriangleCount(UINT val)
{
	mTriangleCount = val;
}

void Geometry::LoadFromFilename(ID3D11Device *pd3dDevice, LPCWSTR path)
{
	//FBX
	WCHAR Buffer[MAX_PATH];
	wcsncpy_s(Buffer, path, MAX_PATH);

	char * tempPath = ConvertWCtoC(Buffer);

	char * ext = strchr(tempPath, '.');

	if (strcmp(ext, ".fbx") == 0 || strcmp(ext, ".FBX") == 0)
	{
		LoadFbx(tempPath);
	}
	else if (strcmp(ext, ".obj") == 0)
	{
		LoadObj(tempPath);
	}

	strcpy_s(m_strPath, tempPath);

	delete[] tempPath;

	BuildBuffers(pd3dDevice);
	BuildBoundingBoxBuffers(pd3dDevice);
	//CreateMaterials(GetLayerCount());
	//IniTextureListManager();
}