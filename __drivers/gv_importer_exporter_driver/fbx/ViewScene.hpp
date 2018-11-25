
#include "fbxsdk/include/fbxsdk.h"
#include "fbxsdk/include/fbxfilesdk/fbxfilesdk_nsuse.h"
#include <fbxfilesdk/kfbxio/kfbxiosettings.h>
#include <fbxfilesdk/kfbxio/kfbximporter.h>
#include <fbxfilesdk/kfbxplugins/kfbxsdkmanager.h>
#include <fbxfilesdk/kfbxplugins/kfbxscene.h>
#include <stdio.h>

#include <fbxfilesdk/kfbxplugins/kfbxtexture.h>
//namespace fbx_detail
//{
	FbxManager* gSdkManager;
	FbxImporter*	gImporter;
	FbxScene*		gScene;
	FbxAnimStack* gCurrentAnimationStack; 
	FbxAnimLayer* gCurrentAnimationLayer; 


	// Scene status.
	enum { UNLOADED, MUST_BE_LOADED, MUST_BE_REFRESHED, REFRESHED };
	// Camera status.
	enum { CAMERA_NOTHING, CAMERA_ORBIT, CAMERA_ZOOM, CAMERA_PAN};
	class VSTexture
	{
	public:
		inline VSTexture()
		{
			mW = mH     = 0;
			mImageData  = NULL;
			mRefTexture = NULL;
		}

		~VSTexture()
		{
			delete mImageData;
		}

		unsigned int   mW;
		unsigned int   mH;
		unsigned char* mImageData;
		FbxTexture*   mRefTexture;
	};

	void InitializeSdkObjects	(FbxManager*& pSdkManager, FbxScene*& pScene);
	void DestroySdkObjects		(FbxManager* pSdkManager);
	void CreateAndFillIOSettings(FbxManager* pSdkManager);
	bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
	bool LoadScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename);

	void ConvertNurbsAndPatch	(FbxManager* pSdkManager, FbxScene* pScene);
	void FillCameraArray		(FbxScene* pScene, KArrayTemplate<FbxNode*>& pCameraArray);
	void FillPoseArray			(FbxScene* pScene, KArrayTemplate<FbxPose*>& pPoseArray);
	void LoadSupportedTextures	(FbxScene* pScene, KArrayTemplate<VSTexture*>& pTextureArray);
	void PreparePointCacheData	(FbxScene* pScene);

	void ConvertNurbsAndPatchRecursive	(FbxManager* pSdkManager, 	FbxNode* pNode);
	void FillCameraArrayRecursive		(FbxNode* pNode, KArrayTemplate<FbxNode*>& pCameraArray);
	void LoadSupportedTexturesRecursive	(FbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray);
	void LoadTexture(FbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray);

	FbxAMatrix GetGlobalPosition(FbxNode* pNode,		FbxTime& pTime,		FbxAMatrix* pParentGlobalPosition = NULL);	
	FbxAMatrix GetGlobalPosition(FbxNode* pNode, 		FbxTime& pTime, 		FbxPose* pPose,		FbxAMatrix* pParentGlobalPosition = NULL);
	FbxAMatrix GetPoseMatrix	 (FbxPose* pPose, 		int pNodeIndex);	
	FbxAMatrix GetGeometry		 (FbxNode* pNode);

	#ifdef IOS_REF
		#undef  IOS_REF
		#define IOS_REF (*(pSdkManager->GetIOSettings()))
	#endif

	void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene)
	{
		// The first thing to do is to create the FBX SDK manager which is the 
		// object allocator for almost all the classes in the SDK.
		pSdkManager = FbxManager::Create();

		if (!pSdkManager)
		{
			printf("Unable to create the FBX SDK manager\n");
			exit(0);
		}

		// create an IOSettings object
		FbxIOSettings * ios = FbxIOSettings::Create(pSdkManager, IOSROOT );
		pSdkManager->SetIOSettings(ios);

		// Load plugins from the executable directory
		FbxString lPath = FbxGetApplicationDirectory();
		#if defined(KARCH_ENV_WIN)
				FbxString lExtension = "dll";
		#elif defined(KARCH_ENV_MACOSX)
				FbxString lExtension = "dylib";
		#elif defined(KARCH_ENV_LINUX)
				FbxString lExtension = "so";
		#endif
		pSdkManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

		// Create the entity that will hold the scene.
		pScene = FbxScene::Create(pSdkManager,"");
	}

	void DestroySdkObjects(FbxManager* pSdkManager)
	{
		// Delete the FBX SDK manager. All the objects that have been allocated 
		// using the FBX SDK manager and that haven't been explicitly destroyed 
		// are automatically destroyed at the same time.
		if (pSdkManager) pSdkManager->Destroy();
		pSdkManager = NULL;
	}

	bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat, bool pEmbedMedia)
	{
		int lMajor, lMinor, lRevision;
		bool lStatus = true;

		// Create an exporter.
		FbxExporter* lExporter = FbxExporter::Create(pSdkManager, "");

		if( pFileFormat < 0 || pFileFormat >= pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount() )
		{
			// Write in fall back format if pEmbedMedia is true
			pFileFormat = pSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

			if (!pEmbedMedia)
			{
				//Try to export in ASCII if possible
				int lFormatIndex, lFormatCount = pSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

				for (lFormatIndex=0; lFormatIndex<lFormatCount; lFormatIndex++)
				{
					if (pSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
					{
						FbxString lDesc =pSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
						char *lASCII = "ascii";
						if (lDesc.Find(lASCII)>=0)
						{
							pFileFormat = lFormatIndex;
							break;
						}
					}
				}
			}
		}

		// Set the export states. By default, the export states are always set to 
		// true except for the option eEXPORT_TEXTURE_AS_EMBEDDED. The code below 
		// shows how to change these states.

		IOS_REF.SetBoolProp(EXP_FBX_MATERIAL,        true);
		IOS_REF.SetBoolProp(EXP_FBX_TEXTURE,         true);
		IOS_REF.SetBoolProp(EXP_FBX_EMBEDDED,        pEmbedMedia);
		IOS_REF.SetBoolProp(EXP_FBX_SHAPE,           true);
		IOS_REF.SetBoolProp(EXP_FBX_GOBO,            true);
		IOS_REF.SetBoolProp(EXP_FBX_ANIMATION,       true);
		IOS_REF.SetBoolProp(EXP_FBX_GLOBAL_SETTINGS, true);

		// Initialize the exporter by providing a filename.
		if(lExporter->Initialize(pFilename, pFileFormat, pSdkManager->GetIOSettings()) == false)
		{
			printf("Call to FbxExporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lExporter->GetLastErrorString());
			return false;
		}

		FbxManager::GetFileFormatVersion(lMajor, lMinor, lRevision);
		printf("FBX version number for this version of the FBX SDK is %d.%d.%d\n\n", lMajor, lMinor, lRevision);

		// Export the scene.
		lStatus = lExporter->Export(pScene); 

		// Destroy the exporter.
		lExporter->Destroy();
		return lStatus;
	}

	bool LoadScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename)
	{
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;
		//int lFileFormat = -1;
		int i, lAnimStackCount;
		bool lStatus;
		char lPassword[1024];

		// Get the file version number generate by the FBX SDK.
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		// Create an importer.
		FbxImporter* lImporter = FbxImporter::Create(pSdkManager,"");

		// Initialize the importer by providing a filename.
		const bool lImportStatus = lImporter->Initialize(pFilename, -1, pSdkManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

		if( !lImportStatus )
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

			if (lImporter->GetLastErrorID() == FbxIO::eFILE_VERSION_NOT_SUPPORTED_YET ||
				lImporter->GetLastErrorID() == FbxIO::eFILE_VERSION_NOT_SUPPORTED_ANYMORE)
			{
				printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
			}

			return false;
		}

		printf("FBX version number for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

		if (lImporter->IsFBX())
		{
			printf("FBX version number for file %s is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

			// From this point, it is possible to access animation stack information without
			// the expense of loading the entire file.

			printf("Animation Stack Information\n");

			lAnimStackCount = lImporter->GetAnimStackCount();

			printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
			printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
			printf("\n");

			for(i = 0; i < lAnimStackCount; i++)
			{
				FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

				printf("    Animation Stack %d\n", i);
				printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
				printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

				// Change the value of the import name if the animation stack should be imported 
				// under a different name.
				printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

				// Set the value of the import state to false if the animation stack should be not
				// be imported. 
				printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
				printf("\n");
			}

			// Set the import states. By default, the import states are always set to 
			// true. The code below shows how to change these states.
			IOS_REF.SetBoolProp(IMP_FBX_MATERIAL,        true);
			IOS_REF.SetBoolProp(IMP_FBX_TEXTURE,         true);
			IOS_REF.SetBoolProp(IMP_FBX_LINK,            true);
			IOS_REF.SetBoolProp(IMP_FBX_SHAPE,           true);
			IOS_REF.SetBoolProp(IMP_FBX_GOBO,            true);
			IOS_REF.SetBoolProp(IMP_FBX_ANIMATION,       true);
			IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
		}

		// Import the scene.
		lStatus = lImporter->Import(pScene);

		if(lStatus == false && lImporter->GetLastErrorID() == FbxIO::ePASSWORD_ERROR)
		{
			printf("Please enter password: ");

			lPassword[0] = '\0';

			scanf("%s", lPassword);
			FbxString lString(lPassword);

			IOS_REF.SetStringProp(IMP_FBX_PASSWORD,      lString);
			IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

			lStatus = lImporter->Import(pScene);

			if(lStatus == false && lImporter->GetLastErrorID() == FbxIO::ePASSWORD_ERROR)
			{
				printf("\nPassword is wrong, import aborted.\n");
			}
		}

		// Destroy the importer.
		lImporter->Destroy();

		return lStatus;
	}
	//================================================
	
	//==========================================

	KScopedPtr<FbxString > gWindowMessage;
	KScopedPtr<FbxString > gFileName;

	KArrayTemplate<FbxNode*>  gCameraArray;
	KArrayTemplate<FbxString*>   gAnimStackNameArray;
	KArrayTemplate<FbxPose*>  gPoseArray;
	KArrayTemplate<VSTexture*> gTextureArray;
	FbxTime gPeriod, gStart, gStop, gCurrentTime;
	int gPoseIndex;
	int gX, gY;
	FbxVector4 gCamPosition, gCamCenter;
	double gRoll;
	int gCameraStatus;int gSceneStatus;
	// the animation curves are extracted from

	void ExportNodeRecursive(FbxNode* pNode, 
		FbxTime& pTime, 
		FbxAMatrix& pParentGlobalPosition);
	void ExportNodeRecursiveAtPose(FbxNode* pNode,
		FbxTime& pTime, 
		FbxAMatrix& pParentGlobalPosition,
		FbxPose* pPose);
	void ExportNode(FbxNode* pNode, 
		FbxTime& lTime, 
		FbxAMatrix& pParentGlobalPosition,
		FbxAMatrix& pGlobalPosition,
		FbxPose* pPose = NULL);
	void ExportMarker(FbxAMatrix& pGlobalPosition);
	void ExportSkeleton(FbxNode* pNode, 
		FbxAMatrix& pParentGlobalPosition, 
		FbxAMatrix& pGlobalPosition);
	void ExportMesh(FbxNode* pNode,
		FbxTime& pTime,
		FbxAMatrix& pGlobalPosition,
		FbxPose* pPose);
	void ComputeShapeDeformation(FbxNode* pNode,
		FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray);
	void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, 
		FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray,
		FbxPose* pPose);
	void ReadVertexCacheData(FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray);
	void ExportCamera(FbxNode* pNode, 
		FbxTime& pTime, 
		FbxAMatrix& pGlobalPosition);
	void ExportLight(FbxNode* pNode, 
		FbxTime& pTime, 
		FbxAMatrix& pGlobalPosition);
	void ExportNull(FbxAMatrix& pGlobalPosition);
	void MatrixScale(FbxAMatrix& pMatrix, double pValue);
	void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue);
	void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix);
	void ExportGrid(FbxAMatrix& pGlobalPosition);

	extern FbxAnimLayer* gCurrentAnimationLayer;

	
	// Deform the vertex array with the shapes contained in the mesh.
	void ComputeShapeDeformation(FbxNode* pNode,
		FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray)
	{
		int i, j;
		int lShapeCount = pMesh->GetShapeCount();
		int lVertexCount = pMesh->GetControlPointsCount();

		FbxVector4* lSrcVertexArray = pVertexArray;
		FbxVector4* lDstVertexArray = new FbxVector4[lVertexCount];
		memcpy(lDstVertexArray, pVertexArray, lVertexCount * sizeof(FbxVector4));

		for (i = 0; i < lShapeCount; i++)
		{
			FbxShape* lShape = pMesh->GetShape(i);

			// Get the percentage of influence of the shape.
			FbxAnimCurve* lFCurve = pMesh->GetShapeChannel(i, gCurrentAnimationLayer);
			if (!lFCurve) continue;
			double lWeight = lFCurve->Evaluate(pTime) / 100.0;

			for (j = 0; j < lVertexCount; j++)
			{
				// Add the influence of the shape vertex to the mesh vertex.
				FbxVector4 lInfluence = (lShape->GetControlPoints()[j] - lSrcVertexArray[j]) * lWeight;
				lDstVertexArray[j] += lInfluence;
			}
		}

		memcpy(pVertexArray, lDstVertexArray, lVertexCount * sizeof(FbxVector4));

		delete [] lDstVertexArray;
	}


	// Deform the vertex array with the links contained in the mesh.
	void ComputeClusterDeformation(FbxAMatrix& pGlobalPosition, 
		FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray,
		FbxPose* pPose)
	{
		// All the links must have the same link mode.
		FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

		int i, j;
		int lClusterCount=0;

		int lVertexCount = pMesh->GetControlPointsCount();
		int lSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);

		FbxAMatrix* lClusterDeformation = new FbxAMatrix[lVertexCount];
		memset(lClusterDeformation, 0, lVertexCount * sizeof(FbxAMatrix));
		double* lClusterWeight = new double[lVertexCount];
		memset(lClusterWeight, 0, lVertexCount * sizeof(double));

		if (lClusterMode == FbxCluster::eADDITIVE)
		{
			for (i = 0; i < lVertexCount; i++)
			{
				lClusterDeformation[i].SetIdentity();
			}
		}

		for ( i=0; i<lSkinCount; ++i)
		{
			lClusterCount =( (FbxSkin *)pMesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
			for (j=0; j<lClusterCount; ++j)
			{
				FbxCluster* lCluster =((FbxSkin *) pMesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
				if (!lCluster->GetLink())
					continue;
				FbxAMatrix lReferenceGlobalInitPosition;
				FbxAMatrix lReferenceGlobalCurrentPosition;
				FbxAMatrix lClusterGlobalInitPosition;
				FbxAMatrix lClusterGlobalCurrentPosition;
				FbxAMatrix lReferenceGeometry;
				FbxAMatrix lClusterGeometry;

				FbxAMatrix lClusterRelativeInitPosition;
				FbxAMatrix lClusterRelativeCurrentPositionInverse;
				FbxAMatrix lVertexTransformMatrix;

				if (lClusterMode == FbxLink::eADDITIVE && lCluster->GetAssociateModel())
				{
					lCluster->GetTransformAssociateModelMatrix(lReferenceGlobalInitPosition);
					lReferenceGlobalCurrentPosition = GetGlobalPosition(lCluster->GetAssociateModel(), pTime, pPose);
					// Geometric transform of the model
					lReferenceGeometry = GetGeometry(lCluster->GetAssociateModel());
					lReferenceGlobalCurrentPosition *= lReferenceGeometry;
				}
				else
				{
					lCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
					lReferenceGlobalCurrentPosition = pGlobalPosition;
					// Multiply lReferenceGlobalInitPosition by Geometric Transformation
					lReferenceGeometry = GetGeometry(pMesh->GetNode());
					lReferenceGlobalInitPosition *= lReferenceGeometry;
				}
				// Get the link initial global position and the link current global position.
				lCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
				lClusterGlobalCurrentPosition = GetGlobalPosition(lCluster->GetLink(), pTime, pPose);

				// Compute the initial position of the link relative to the reference.
				lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;

				// Compute the current position of the link relative to the reference.
				lClusterRelativeCurrentPositionInverse = lReferenceGlobalCurrentPosition.Inverse() * lClusterGlobalCurrentPosition;

				// Compute the shift of the link relative to the reference.
				lVertexTransformMatrix = lClusterRelativeCurrentPositionInverse * lClusterRelativeInitPosition;

				int k;
				int lVertexIndexCount = lCluster->GetControlPointIndicesCount();

				for (k = 0; k < lVertexIndexCount; ++k) 
				{            
					int lIndex = lCluster->GetControlPointIndices()[k];

					// Sometimes, the mesh can have less points than at the time of the skinning
					// because a smooth operator was active when skinning but has been deactivated during export.
					if (lIndex >= lVertexCount)
						continue;

					double lWeight = lCluster->GetControlPointWeights()[k];

					if (lWeight == 0.0)
					{
						continue;
					}

					// Compute the influence of the link on the vertex.
					FbxAMatrix lInfluence = lVertexTransformMatrix;
					MatrixScale(lInfluence, lWeight);

					if (lClusterMode == FbxCluster::eADDITIVE)
					{   
						// Multiply with to the product of the deformations on the vertex.
						MatrixAddToDiagonal(lInfluence, 1.0 - lWeight);
						lClusterDeformation[lIndex] = lInfluence * lClusterDeformation[lIndex];

						// Set the link to 1.0 just to know this vertex is influenced by a link.
						lClusterWeight[lIndex] = 1.0;
					}
					else // lLinkMode == FbxLink::eNORMALIZE || lLinkMode == FbxLink::eTOTAL1
					{
						// Add to the sum of the deformations on the vertex.
						MatrixAdd(lClusterDeformation[lIndex], lInfluence);

						// Add to the sum of weights to either normalize or complete the vertex.
						lClusterWeight[lIndex] += lWeight;
					}

				}
			}
		}

		for (i = 0; i < lVertexCount; i++) 
		{
			FbxVector4 lSrcVertex = pVertexArray[i];
			FbxVector4& lDstVertex = pVertexArray[i];
			double lWeight = lClusterWeight[i];

			// Deform the vertex if there was at least a link with an influence on the vertex,
			if (lWeight != 0.0) 
			{
				lDstVertex = lClusterDeformation[i].MultT(lSrcVertex);

				if (lClusterMode == FbxCluster::eNORMALIZE)
				{
					// In the normalized link mode, a vertex is always totally influenced by the links. 
					lDstVertex /= lWeight;
				}
				else if (lClusterMode == FbxCluster::eTOTAL1)
				{
					// In the total 1 link mode, a vertex can be partially influenced by the links. 
					lSrcVertex *= (1.0 - lWeight);
					lDstVertex += lSrcVertex;
				}
			} 
		}

		delete [] lClusterDeformation;
		delete [] lClusterWeight;
	}

	void ReadVertexCacheData(FbxMesh* pMesh, 
		FbxTime& pTime, 
		FbxVector4* pVertexArray)
	{
		FbxVertexCacheDeformer* lDeformer     = static_cast<FbxVertexCacheDeformer*>(pMesh->GetDeformer(0, FbxDeformer::eVERTEX_CACHE));
		FbxCache*               lCache        = lDeformer->GetCache();
		int                      lChannelIndex = -1;
		unsigned int             lVertexCount  = (unsigned int)pMesh->GetControlPointsCount();
		bool                     lReadSucceed  = false;
		double*                  lReadBuf      = new double[3*lVertexCount];

		if (lCache->GetCacheFileFormat() == FbxCache::eMC)
		{
			if ((lChannelIndex = lCache->GetChannelIndex(lDeformer->GetCacheChannel())) > -1)
			{
				lReadSucceed = lCache->Read(lChannelIndex, pTime, lReadBuf, lVertexCount);
			}
		}
		else // ePC2
		{
			lReadSucceed = lCache->Read((unsigned int)pTime.GetFrame(true), lReadBuf, lVertexCount);
		}

		if (lReadSucceed)
		{
			unsigned int lReadBufIndex = 0;

			while (lReadBufIndex < 3*lVertexCount)
			{
				// In statements like "pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex++])", 
				// on Mac platform, "lReadBufIndex++" is evaluated before "lReadBufIndex/3". 
				// So separate them.
				pVertexArray[lReadBufIndex/3].SetAt(0, lReadBuf[lReadBufIndex]); lReadBufIndex++;
				pVertexArray[lReadBufIndex/3].SetAt(1, lReadBuf[lReadBufIndex]); lReadBufIndex++;
				pVertexArray[lReadBufIndex/3].SetAt(2, lReadBuf[lReadBufIndex]); lReadBufIndex++;
			}
		}

		delete [] lReadBuf;
	}


	// Scale all the elements of a matrix.
	void MatrixScale(FbxAMatrix& pMatrix, double pValue)
	{
		int i,j;

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				pMatrix[i][j] *= pValue;
			}
		}
	}


	// Add a value to all the elements in the diagonal of the matrix.
	void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
	{
		pMatrix[0][0] += pValue;
		pMatrix[1][1] += pValue;
		pMatrix[2][2] += pValue;
		pMatrix[3][3] += pValue;
	}


	// Sum two matrices element by element.
	void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
	{
		int i,j;

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				pDstMatrix[i][j] += pSrcMatrix[i][j];
			}
		}
	}

	//===========================================

	// Get the global position.
	// Do not take in account the geometric transform.
	FbxAMatrix GetGlobalPosition(FbxNode* pNode, FbxTime& pTime, FbxAMatrix* pParentGlobalPosition)
	{
		// Ideally this function would use parent global position and local position to
		// compute the global position.
		// Unfortunately the equation 
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs). To compute
		// the parent rotation and scaling is tricky in the RrSs and Rrs cases.
		// This is why GetNodeGlobalTransform() is used: it always computes the right
		// global position.

		return pNode->GetScene()->GetEvaluator()->GetNodeGlobalTransform(pNode, pTime);
	}


	// Get the global position of the node for the current pose.
	// If the specified node is not part of the pose, get its
	// global position at the current time.
	FbxAMatrix GetGlobalPosition(FbxNode* pNode, FbxTime& pTime, FbxPose* pPose, FbxAMatrix* pParentGlobalPosition)
	{
		FbxAMatrix lGlobalPosition;
		bool        lPositionFound = false;

		if (pPose)
		{
			int lNodeIndex = pPose->Find(pNode);

			if (lNodeIndex > -1)
			{
				// The bind pose is always a global matrix.
				// If we have a rest pose, we need to check if it is
				// stored in global or local space.
				if (pPose->IsBindPose() || !pPose->IsLocalMatrix(lNodeIndex))
				{
					lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
				}
				else
				{
					// We have a local matrix, we need to convert it to
					// a global space matrix.
					FbxAMatrix lParentGlobalPosition;

					if (pParentGlobalPosition)
					{
						lParentGlobalPosition = *pParentGlobalPosition;
					}
					else
					{
						if (pNode->GetParent())
						{
							lParentGlobalPosition = GetGlobalPosition(pNode->GetParent(), pTime, pPose);
						}
					}

					FbxAMatrix lLocalPosition = GetPoseMatrix(pPose, lNodeIndex);
					lGlobalPosition = lParentGlobalPosition * lLocalPosition;
				}

				lPositionFound = true;
			}
		}

		if (!lPositionFound)
		{
			// There is no pose entry for that node, get the current global position instead
			lGlobalPosition = GetGlobalPosition(pNode, pTime, pParentGlobalPosition);
		}

		return lGlobalPosition;
	}

	// Get the matrix of the given pose
	FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
	{
		FbxAMatrix lPoseMatrix;
		FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

		memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

		return lPoseMatrix;
	}

	// Get the geometry deformation local to a node. It is never inherited by the
	// children.
	FbxAMatrix GetGeometry(FbxNode* pNode) {
		FbxVector4 lT, lR, lS;
		FbxAMatrix lGeometry;

		lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

		lGeometry.SetT(lT);
		lGeometry.SetR(lR);
		lGeometry.SetS(lS);

		return lGeometry;
	}

	void ConvertNurbsAndPatchRecursive(FbxManager* pSdkManager, 
		FbxNode* pNode);
	void FillCameraArrayRecursive(FbxNode* pNode, 
		KArrayTemplate<FbxNode*>& pCameraArray);
	void LoadSupportedTexturesRecursive(FbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray);
	void LoadTexture(FbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray);


	void ConvertNurbsAndPatch(FbxManager* pSdkManager, FbxScene* pScene)
	{
		ConvertNurbsAndPatchRecursive(pSdkManager, pScene->GetRootNode());
	}


	void ConvertNurbsAndPatchRecursive(FbxManager* pSdkManager, FbxNode* pNode)
	{
		FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

		if (lNodeAttribute)
		{
			if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
			{
				FbxGeometryConverter lConverter(pSdkManager);
				lConverter.TriangulateInPlace(pNode);
			}
		}

		int i, lCount = pNode->GetChildCount();

		for (i = 0; i < lCount; i++)
		{
			ConvertNurbsAndPatchRecursive(pSdkManager, pNode->GetChild(i));
		}
	}


	void FillCameraArray(FbxScene* pScene, KArrayTemplate<FbxNode*>& pCameraArray)
	{
		pCameraArray.Clear();

		FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
	}


	void FillCameraArrayRecursive(FbxNode* pNode, KArrayTemplate<FbxNode*>& pCameraArray)
	{
		int i, lCount;

		if (pNode)
		{
			if (pNode->GetNodeAttribute())
			{
				if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
				{
					pCameraArray.Add(pNode);
				}
			}

			lCount = pNode->GetChildCount();

			for (i = 0; i < lCount; i++)
			{
				FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
			}
		}
	}


	void FillPoseArray(FbxScene* pScene, KArrayTemplate<FbxPose*>& pPoseArray)
	{
		int      i, lPoseCount;

		for (i=0, lPoseCount = pScene->GetPoseCount(); i < lPoseCount; i++)
		{
			pPoseArray.Add(pScene->GetPose(i));
		}
	}

	void LoadSupportedTextures(FbxScene* pScene, KArrayTemplate<VSTexture*>& pTextureArray)
	{
		pTextureArray.Clear();

		LoadSupportedTexturesRecursive(pScene->GetRootNode(), pTextureArray);
	}

	void LoadSupportedTexturesRecursive(FbxNode* pNode, KArrayTemplate<VSTexture*>& pTextureArray)
	{
		if (pNode)
		{
			int i, lCount;
			FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

			if (lNodeAttribute)
			{
				FbxLayerContainer* lLayerContainer = NULL;

				switch (lNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eNurbs:
					lLayerContainer = pNode->GetNurb();
					break;

				case FbxNodeAttribute::ePatch:
					lLayerContainer = pNode->GetPatch();
					break;

				case FbxNodeAttribute::eMesh:
					lLayerContainer = pNode->GetMesh();
					break;
				}

				if (lLayerContainer){
					int lMaterialIndex;
					int lTextureIndex;
					FbxProperty lProperty;
					int lNbTex;
					FbxTexture* lTexture = NULL; 
					FbxSurfaceMaterial *lMaterial = NULL;
					int lNbMat = pNode->GetSrcObjectCount(FbxSurfaceMaterial::ClassId);
					for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++){
						lMaterial = FbxCast <FbxSurfaceMaterial>(pNode->GetSrcObject(FbxSurfaceMaterial::ClassId, lMaterialIndex));
						if(lMaterial){                                                          
							lProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
							if(lProperty.IsValid()){
								lNbTex = lProperty.GetSrcObjectCount(FbxTexture::ClassId);
								for (lTextureIndex = 0; lTextureIndex < lNbTex; lTextureIndex++){
									lTexture = FbxCast <FbxTexture> (lProperty.GetSrcObject(FbxTexture::ClassId, lTextureIndex)); 
									if(lTexture)
										LoadTexture(lTexture, pTextureArray);
								}
							}
						}
					}
				} 

			}

			lCount = pNode->GetChildCount();

			for (i = 0; i < lCount; i++)
			{
				LoadSupportedTexturesRecursive(pNode->GetChild(i), pTextureArray);
			}
		}
	}

	void LoadTexture(FbxTexture* pTexture, KArrayTemplate<VSTexture*>& pTextureArray)
	{
		// First find if the texture is already loaded
		int i, lCount = pTextureArray.GetCount();

		for (i = 0; i < lCount; i++)
		{
			if (pTextureArray[i]->mRefTexture == pTexture) return;
		}

		// Right now, only Targa textures are loaded by this sample
		FbxString lFileName = pTexture->GetFileName();
		/*
		if (lFileName.Right(3).Upper() == "TGA")
		{
			tga_image lTGAImage;

			if (tga_read(&lTGAImage, lFileName.Buffer()) == TGA_NOERR)
			{
				// Make sure the image is left to right
				if (tga_is_right_to_left(&lTGAImage)) tga_flip_horiz(&lTGAImage);

				// Make sure the image is bottom to top
				if (tga_is_top_to_bottom(&lTGAImage)) tga_flip_vert(&lTGAImage);

				// Make the image BGR 24
				tga_convert_depth(&lTGAImage, 24);

				VSTexture* lTexture = new VSTexture;

				lTexture->mW = lTGAImage.width;
				lTexture->mH = lTGAImage.height;
				lTexture->mRefTexture = pTexture;
				lTexture->mImageData  = new unsigned char[lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8];
				memcpy(lTexture->mImageData, lTGAImage.image_data, lTGAImage.width*lTGAImage.height*lTGAImage.pixel_depth/8);

				tga_free_buffers(&lTGAImage);

				pTextureArray.Add(lTexture);
			}
		}*/
	}

	void PreparePointCacheData(FbxScene* pScene)
	{
		// This function show how to cycle thru scene elements in a linear way.
		int lIndex, lNodeCount = FbxGetSrcCount<FbxNode>(pScene);

		for (lIndex=0; lIndex<lNodeCount; lIndex++)
		{
			FbxNode* lNode = FbxGetSrc<FbxNode>(pScene, lIndex);

			if (lNode->GetGeometry()) 
			{
				int i, lVertexCacheDeformerCount = lNode->GetGeometry()->GetDeformerCount(FbxDeformer::eVERTEX_CACHE);

				// There should be a maximum of 1 Vertex Cache Deformer for the moment
				lVertexCacheDeformerCount = lVertexCacheDeformerCount > 0 ? 1 : 0;

				for (i=0; i<lVertexCacheDeformerCount; ++i )
				{
					// Get the Point Cache object
					FbxVertexCacheDeformer* lDeformer = static_cast<FbxVertexCacheDeformer*>(lNode->GetGeometry()->GetDeformer(i, FbxDeformer::eVERTEX_CACHE));
					if( !lDeformer ) continue;
					FbxCache* lCache = lDeformer->GetCache();
					if( !lCache ) continue;

					// Process the point cache data only if the constraint is active
					if (lDeformer->IsActive())
					{
						if (lCache->GetCacheFileFormat() == FbxCache::ePC2)
						{
							// This code show how to convert from PC2 to MC point cache format
							// turn it on if you need it.
#if 0 
							if (!lCache->ConvertFromPC2ToMC(FbxCache::eMC_ONE_FILE, 
								FbxTime::GetFrameRate(pScene->GetGlobalTimeSettings().GetTimeMode())))
							{
								// Conversion failed, retrieve the error here
								FbxString lTheErrorIs = lCache->GetError().GetLastErrorString();
							}
#endif
						}
						else if (lCache->GetCacheFileFormat() == FbxCache::eMC)
						{
							// This code show how to convert from MC to PC2 point cache format
							// turn it on if you need it.
							//#if 0 
							if (!lCache->ConvertFromMCToPC2(FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), 0))
							{
								// Conversion failed, retrieve the error here
								FbxString lTheErrorIs = lCache->GetError().GetLastErrorString();
							}
							//#endif
						}


						// Now open the cache file to read from it
						if (!lCache->OpenFileForRead())
						{
							// Cannot open file 
							FbxString lTheErrorIs = lCache->GetError().GetLastErrorString();

							// Set the deformer inactive so we don't play it back
							lDeformer->SetActive(false);
						}
					}
				}
			}
		}
	}

	//===========================================================
	void AnimStackSelectionCallback( int pItem )
	{
		int lNbAnimStacks = gAnimStackNameArray.GetCount();
		if (!lNbAnimStacks || pItem >= lNbAnimStacks)
		{
			return;
		}

		// select the base layer from the animation stack
		gCurrentAnimationStack = gScene->FindMember(FBX_TYPE(FbxAnimStack), gAnimStackNameArray[pItem]->Buffer());
		if (gCurrentAnimationStack == NULL)
		{
			// this is a problem. The anim stack should be found in the scene!
			return;
		}

		// we assume that the first animation layer connected to the animation stack is the base layer
		// (this is the assumption made in the FBXSDK)
		gCurrentAnimationLayer = gCurrentAnimationStack->GetMember(FBX_TYPE(FbxAnimLayer), 0);
		gScene->GetEvaluator()->SetContext(gCurrentAnimationStack);

		FbxTakeInfo* lCurrentTakeInfo = gScene->GetTakeInfo(*(gAnimStackNameArray[pItem]));
		if (lCurrentTakeInfo)
		{
			gStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			gStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			// Take the time line value
			FbxTimeSpan lTimeLineTimeSpan;
			gScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);

			gStart = lTimeLineTimeSpan.GetStart();
			gStop  = lTimeLineTimeSpan.GetStop();
		}

		gCurrentTime = gStart;

		// Set the scene status flag to refresh 
		// the scene in the next timer callback.
		gSceneStatus = MUST_BE_REFRESHED;
	}



//}
/*


bool			init_fbx_sdk()
{
	// Initialize the scene status flag.
	gSceneStatus = UNLOADED;

	// Create the FBX SDK manager which is the object allocator for almost 
	// all the classes in the SDK and create the scene.
	InitializeSdkObjects(gSdkManager, gScene);

	gFileName.Reset(new FbxString());
	*gFileName = *m_file_name;

	if (gSdkManager)
	{
		// Create the importer.
		int lFileFormat = -1;
		gImporter = FbxImporter::Create(gSdkManager,"");
		if (!gSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(*gFileName, lFileFormat) )
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			lFileFormat = gSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription( "FBX binary (*.fbx)" );;
		}

		// Initialize the importer by providing a filename.
		if(gImporter->Initialize(gFileName->Buffer(), lFileFormat) == true)
		{
			gSceneStatus = MUST_BE_LOADED;
		}
		else
		{
			GVM_ERROR( "Unable to open file "<<m_file_name);
			return false;
		}
	}
	else
	{
		GVM_ERROR( "Unable to create the FBX SDK manager "<<m_file_name);
		return false;
	}
	// When gPoseIndex is not -1, draw the scene at that pose
	gPoseIndex = -1;
	return true;
}

bool			import_fbx_file()
{
	if (gSceneStatus == MUST_BE_LOADED)
	{
		if(gImporter->Import(gScene) == true)
		{
			// Set the scene status flag to refresh 
			// the scene in the first timer callback.
			gSceneStatus = MUST_BE_REFRESHED;

			// Convert Axis System to what is used in this example, if needed
			FbxAxisSystem SceneAxisSystem = gScene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurAxisSystem(FbxAxisSystem::YAxis, FbxAxisSystem::ParityOdd, FbxAxisSystem::RightHanded);
			if( SceneAxisSystem != OurAxisSystem )
			{
				OurAxisSystem.ConvertScene(gScene);
			}

			// Convert Unit System to what is used in this example, if needed
			FbxSystemUnit SceneSystemUnit = gScene->GetGlobalSettings().GetSystemUnit();
			if( SceneSystemUnit.GetScaleFactor() != 1.0 )
			{
				FbxSystemUnit OurSystemUnit(1.0);
				OurSystemUnit.ConvertScene(gScene);
			}

			// Nurbs and patch attribute types are not supported yet.
			// Convert them into mesh node attributes to have them drawn.
			ConvertNurbsAndPatch(gSdkManager, gScene);

			// Convert any .PC2 point cache data into the .MC format for 
			// vertex cache deformer playback.
			PreparePointCacheData(gScene);

			// Get the list of all the cameras in the scene.
			FillCameraArray(gScene, gCameraArray);

			// Get the list of all the animation stack.
			gScene->FillAnimStackNameArray(gAnimStackNameArray);

			// Get the list of pose in the scene
			FillPoseArray(gScene, gPoseArray);

			// Load the texture data in memory (for supported formats)
			LoadSupportedTextures(gScene, gTextureArray);

			// Initialize the frame period.
			gPeriod.SetTime(0, 0, 0, 1, 0, gScene->GetGlobalSettings().GetTimeMode());
		}
		else
		{
			// Import failed, set the scene status flag accordingly.
			GVM_ERROR( "Unable to open file "<<m_file_name);
			return false;
		}

		// Destroy the importer to release the file.
		gImporter->Destroy();
	}
	AnimStackSelectionCallback(0);
	return true;
}
void			on_exit()
{
	// are automatically destroyed at the same time.
	if (gSdkManager) gSdkManager->Destroy();
	gSdkManager = NULL;

	// Delete the array used to associate menu ids with animation stack names.
	FbxSdkDeleteAndClear(gAnimStackNameArray);

	// Delete any texture allocated
	FbxSdkDeleteAndClear(gTextureArray);
}


void			get_export_extension	( gvt_array<gv_id  > & result )	
{

};
gv_class_info*	get_export_source_class	()								
{
return NULL;
};
bool			do_export				( const gv_string_tmp & file_name,  gv_object * source )
{
return false;
};

protected:
gv_model * m_result_model;

// Export the scene at a given time for the current animation stack.
void ExportScene(FbxScene* pScene, 
FbxTime& pTime)
{
FbxAMatrix lDummyGlobalPosition;

int i, lCount = pScene->GetRootNode()->GetChildCount();

for (i = 0; i < lCount; i++)
{
ExportNodeRecursive(pScene->GetRootNode()->GetChild(i), pTime, lDummyGlobalPosition);
}

//ExportGrid(lDummyGlobalPosition);
}

// Export the scene at a given pose. The elements not part of the pose
// will be drawn at the given time for the current animation stack.
void ExportSceneAtPose(FbxScene* pScene, FbxTime& pTime, int pPoseIndex)
{
FbxAMatrix lDummyGlobalPosition;
FbxPose*   lPose = pScene->GetPose(pPoseIndex);

int i, lCount = pScene->GetRootNode()->GetChildCount();

for (i = 0; i < lCount; i++)
{
ExportNodeRecursiveAtPose(pScene->GetRootNode()->GetChild(i), pTime, lDummyGlobalPosition, lPose);
}
}

// Export recursively each node of the scene. To avoid recomputing 
// uselessly the global positions, the global position of each 
// node is passed to it's children while browsing the node tree.
void ExportNodeRecursive(FbxNode* pNode, 
FbxTime& pTime, 
FbxAMatrix& pParentGlobalPosition)
{
// Compute the node's global position.
FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, &pParentGlobalPosition);

// Geometry offset.
// it is not inherited by the children.
FbxAMatrix lGeometryOffset = GetGeometry(pNode);
FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

ExportNode(pNode, pTime, pParentGlobalPosition, lGlobalOffPosition,NULL);

int i, lCount = pNode->GetChildCount();

for (i = 0; i < lCount; i++)
{
ExportNodeRecursive(pNode->GetChild(i), pTime, lGlobalPosition);
}
}

// Export recursively each node of the scene. To avoid recomputing 
// uselessly the global positions, the global position of each 
// node is passed to it's children while browsing the node tree.
// If the node is part of the given pose for the current scene,
// it will be drawn at the position specified in the pose, Otherwise
// it will be drawn at the given time.
void ExportNodeRecursiveAtPose(FbxNode* pNode, 
FbxTime& pTime, 
FbxAMatrix& pParentGlobalPosition,
FbxPose* pPose=NULL)
{
FbxAMatrix lGlobalPosition;

lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);

// Geometry offset.
// it is not inherited by the children.
FbxAMatrix lGeometryOffset = GetGeometry(pNode);
FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;

ExportNode(pNode, pTime, pParentGlobalPosition, lGlobalOffPosition, pPose);

int i, lCount = pNode->GetChildCount();

for (i = 0; i < lCount; i++)
{
ExportNodeRecursiveAtPose(pNode->GetChild(i), pTime, lGlobalPosition, pPose);
}
}

// Export the node following the content of it's node attribute.
void ExportNode(FbxNode* pNode, 
FbxTime& pTime, 
FbxAMatrix& pParentGlobalPosition,
FbxAMatrix& pGlobalPosition,
FbxPose* pPose)
{
FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();



if (lNodeAttribute)
{
GVM_DEBUG_LOG( debug ,"import from fbx node "<< pNode->GetName());

if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
{
ExportMarker(pGlobalPosition);
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
{
ExportSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
{
ExportMesh(pNode, pTime, pGlobalPosition, pPose);
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs)
{
// Not supported yet. 
// Should have been converted into a mesh in function ConvertNurbsAndPatch().
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
{
// Not supported yet. 
// Should have been converted into a mesh in function ConvertNurbsAndPatch().
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
{
ExportCamera(pNode, pTime, pGlobalPosition);
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eLight)
{
ExportLight(pNode, pTime, pGlobalPosition);
}
else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNULL)
{
ExportNull(pGlobalPosition);
}

}
else
{
ExportNull(pGlobalPosition);
}
}


// Export a small box where the node is located.
void ExportMarker(FbxAMatrix& pGlobalPosition)
{
//GlExportMarker(pGlobalPosition);  
}


// Export a limb between the node and its parent.
void ExportSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition)
{
FbxSkeleton* lSkeleton = (FbxSkeleton*) pNode->GetNodeAttribute();

// Only draw the skeleton if it's a limb node and if 
// the parent also has an attribute of type skeleton.
if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLIMB_NODE &&
pNode->GetParent() &&
pNode->GetParent()->GetNodeAttribute() &&
pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
{
//GlExportLimbNode(pParentGlobalPosition, pGlobalPosition); 
}
}


// Export the vertices of a mesh.
void ExportMesh(FbxNode* pNode, FbxTime& pTime, FbxAMatrix& pGlobalPosition, FbxPose* pPose)
{
FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute();
int is_skeletal_mesh= (lMesh->GetDeformerCount(FbxDeformer::eSkin) ) ;
gv_skeletal_mesh * my_skeletal_mesh=NULL; 
gv_static_mesh   * my_static_mesh=NULL;

int lClusterCount = 0;
int lSkinCount= 0;
int lVertexCount = lMesh->GetControlPointsCount();
// No vertex to draw.
if (lVertexCount == 0)
{
return;
}
my_static_mesh=m_result_model->get_sandbox()->create_object<gv_static_mesh>( gv_id(pNode->GetName()),m_result_model);
gv_vertex_buffer * my_vertex_buffer=m_result_model->get_sandbox()->create_object<gv_vertex_buffer>	(my_static_mesh);
gv_index_buffer  * my_index_buffer =m_result_model->get_sandbox()->create_object<gv_index_buffer>	(my_static_mesh);
my_static_mesh->m_vertex_buffer=my_vertex_buffer;
my_static_mesh->m_index_buffer=my_index_buffer;
if (is_skeletal_mesh )
{
gv_string_tmp name("sk_");;
name<<pNode->GetName();
my_skeletal_mesh=m_result_model->get_sandbox()->create_object<gv_skeletal_mesh>( gv_id(*name),m_result_model);
my_skeletal_mesh->m_t_pose_mesh=my_static_mesh;
my_static_mesh->set_owner(my_skeletal_mesh);
}

FbxVector4* lVertexArray = lMesh->GetControlPoints();
my_vertex_buffer->m_raw_pos.resize(lVertexCount); 
for ( int i=0; i< lVertexCount; i++ ) 
{
my_vertex_buffer->m_raw_pos[i].x=(gv_float)lVertexArray[i][0];
my_vertex_buffer->m_raw_pos[i].y=(gv_float)lVertexArray[i][1];
my_vertex_buffer->m_raw_pos[i].z=(gv_float)lVertexArray[i][2];
}

//we need to get the number of clusters
lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
for( int i=0; i< lSkinCount; i++)
lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
if (lClusterCount)
{
// Deform the vertex array with the links.
//ComputeClusterDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
//export_bones();
}

FbxMesh* pMesh=lMesh;
FbxLayerElementArrayTemplate<FbxVector2>* lUVArray = NULL;    
pMesh->GetTextureUV(&lUVArray, FbxLayerElement::eDIFFUSE_TEXTURES); 

FbxLayerElement::EMappingMode lMappingMode = FbxLayerElement::eNone;
VSTexture*                     lTexture     = NULL;

if(pMesh->GetLayer(0) && pMesh->GetLayer(0)->GetUVs())	lMappingMode = pMesh->GetLayer(0)->GetUVs()->GetMappingMode();
// Find the texture data
{
FbxTexture* lCurrentTexture           = NULL;
FbxLayerElementTexture* lTextureLayer = NULL;
FbxSurfaceMaterial* lSurfaceMaterial= FbxCast <FbxSurfaceMaterial>(pMesh->GetNode()->GetSrcObject(FbxSurfaceMaterial::ClassId, 0));
if(lSurfaceMaterial)
{
FbxProperty lProperty;
lProperty = lSurfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
if(lProperty.IsValid())
{   
lCurrentTexture = FbxCast <FbxTexture>(lProperty.GetSrcObject(FbxTexture::ClassId, 0)); 
gv_string_tmp tex_name=lCurrentTexture->GetFileName();
my_static_mesh->m_diffuse_texture=m_result_model->get_sandbox()->create_object<gv_texture> ( my_static_mesh);
my_static_mesh->m_diffuse_texture->set_file_name(*tex_name);
}
}
}


int lPolygonIndex;
int lPolygonCount = pMesh->GetPolygonCount();

for (lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
{
int lVerticeIndex;
int lVerticeCount = pMesh->GetPolygonSize(lPolygonIndex);
GV_ASSERT(lVerticeCount==3); //otherwise we need to make a fan for polygon
for (lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
{
int lCurrentUVIndex;

if (lMappingMode == FbxLayerElement::eByPolygonVertex)
{
lCurrentUVIndex = pMesh->GetTextureUVIndex(lPolygonIndex, lVerticeIndex);
}
else // FbxLayerElement::eByControlPoint
{
lCurrentUVIndex = pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex);
}
//if(lUVArray)			glTexCoord2dv(lUVArray->GetAt(lCurrentUVIndex).mData);

//glVertex3dv((GLdouble *)pVertexArray[pMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
}


}

}

// Export an oriented camera box where the node is located.
void ExportCamera(FbxNode* pNode, 
FbxTime& pTime, 
FbxAMatrix& pGlobalPosition)
{
FbxAMatrix lCameraGlobalPosition;
FbxVector4 lCameraPosition, lCameraDefaultDirection, lCameraInterestPosition;

lCameraPosition = pGlobalPosition.GetT();

// By default, FBX cameras point towards the X positive axis.
FbxVector4 lXPositiveAxis(1.0, 0.0, 0.0);
lCameraDefaultDirection = lCameraPosition + lXPositiveAxis;

lCameraGlobalPosition = pGlobalPosition;

// If the camera is linked to an interest, get the interest position.
if (pNode->GetTarget())
{
lCameraInterestPosition = GetGlobalPosition(pNode->GetTarget(), pTime).GetT();

// Compute the required rotation to make the camera point to it's interest.
FbxVector4 lCameraDirection;
FbxVector4::AxisAlignmentInEulerAngle(lCameraPosition, 
lCameraDefaultDirection, 
lCameraInterestPosition, 
lCameraDirection);

// Must override the camera rotation 
// to make it point to it's interest.
lCameraGlobalPosition.SetR(lCameraDirection);
}

// Get the camera roll.
FbxCamera* cam = pNode->GetCamera();
double lRoll = 0;

if (cam)
{
lRoll = cam->Roll.Get();
FbxAnimCurve* fc = cam->Roll.GetCurve(gCurrentAnimationLayer);
if (fc) fc->Evaluate(pTime);
}
//GlExportCamera(lCameraGlobalPosition, lRoll);
}


// Export a colored sphere or cone where the node is located.
void ExportLight(FbxNode* pNode, 
FbxTime& pTime, 
FbxAMatrix& pGlobalPosition)
{
FbxLight* lLight = (FbxLight*) pNode->GetNodeAttribute();
FbxAMatrix lLightRotation, lLightGlobalPosition;
FbxColor lColor(1,1,1);
double lConeAngle = 0.0;

// Must rotate the light's global position because 
// FBX lights point towards the Y negative axis.
FbxVector4 lYNegativeAxis(-90.0, 0.0, 0.0);
lLightRotation.SetR(lYNegativeAxis);
lLightGlobalPosition = pGlobalPosition * lLightRotation;

// Get the light color.
FbxAnimCurve* fc;

fc = lLight->Color.GetCurve(gCurrentAnimationLayer, KFCURVENODE_COLOR_RED);
if (fc) lColor.mRed = fc->Evaluate(pTime);
fc = lLight->Color.GetCurve(gCurrentAnimationLayer, KFCURVENODE_COLOR_GREEN);
if (fc) lColor.mGreen = fc->Evaluate(pTime);
fc = lLight->Color.GetCurve(gCurrentAnimationLayer, KFCURVENODE_COLOR_BLUE);
if (fc) lColor.mBlue = fc->Evaluate(pTime);

// The cone angle is only relevant if the light is a spot.
if (lLight->LightType.Get() == FbxLight::eSPOT)
{
fc = lLight->ConeAngle.GetCurve(gCurrentAnimationLayer);
if (fc) lConeAngle = fc->Evaluate(pTime);
}

//GlExportLight(lLightGlobalPosition, 
//	lLight, 
//	lColor, 
//	lConeAngle);
}

// Export a cross hair where the node is located.
void ExportNull(FbxAMatrix& pGlobalPosition)
{
//GlExportCrossHair(pGlobalPosition);
}

*/