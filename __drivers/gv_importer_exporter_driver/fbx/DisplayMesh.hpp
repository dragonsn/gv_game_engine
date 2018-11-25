/**************************************************************************************

 Copyright (C) 2001 - 2009 Autodesk, Inc. and/or its licensors.
 All Rights Reserved.

 The coded instructions, statements, computer programs, and/or related material 
 (collectively the "Data") in these files contain unpublished information 
 proprietary to Autodesk, Inc. and/or its licensors, which is protected by 
 Canada and United States of America federal copyright law and by international 
 treaties. 
 
 The Data may not be disclosed or distributed to third parties, in whole or in
 part, without the prior written consent of Autodesk, Inc. ("Autodesk").

 THE DATA IS PROVIDED "AS IS" AND WITHOUT WARRANTY.
 ALL WARRANTIES ARE EXPRESSLY EXCLUDED AND DISCLAIMED. AUTODESK MAKES NO
 WARRANTY OF ANY KIND WITH RESPECT TO THE DATA, EXPRESS, IMPLIED OR ARISING
 BY CUSTOM OR TRADE USAGE, AND DISCLAIMS ANY IMPLIED WARRANTIES OF TITLE, 
 NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE OR USE. 
 WITHOUT LIMITING THE FOREGOING, AUTODESK DOES NOT WARRANT THAT THE OPERATION
 OF THE DATA WILL BE UNINTERRUPTED OR ERROR FREE. 
 
 IN NO EVENT SHALL AUTODESK, ITS AFFILIATES, PARENT COMPANIES, LICENSORS
 OR SUPPLIERS ("AUTODESK GROUP") BE LIABLE FOR ANY LOSSES, DAMAGES OR EXPENSES
 OF ANY KIND (INCLUDING WITHOUT LIMITATION PUNITIVE OR MULTIPLE DAMAGES OR OTHER
 SPECIAL, DIRECT, INDIRECT, EXEMPLARY, INCIDENTAL, LOSS OF PROFITS, REVENUE
 OR DATA, COST OF COVER OR CONSEQUENTIAL LOSSES OR DAMAGES OF ANY KIND),
 HOWEVER CAUSED, AND REGARDLESS OF THE THEORY OF LIABILITY, WHETHER DERIVED
 FROM CONTRACT, TORT (INCLUDING, BUT NOT LIMITED TO, NEGLIGENCE), OR OTHERWISE,
 ARISING OUT OF OR RELATING TO THE DATA OR ITS USE OR ANY OTHER PERFORMANCE,
 WHETHER OR NOT AUTODESK HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH LOSS
 OR DAMAGE. 

**************************************************************************************/


#define MAT_HEADER_LENGTH 200

void DisplayControlsPoints(FbxMesh* pMesh);
void DisplayPolygons(FbxMesh* pMesh, gv_vertex_buffer & vb_pos, gv_vertex_buffer & vb_mesh,gv_index_buffer & ib_mesh , gv_static_mesh * mesh);
void DisplayMaterialMapping(FbxMesh* pMesh, gv_static_mesh * my_mesh);
void DisplayTextureMapping(FbxMesh* pMesh);
void DisplayTextureNames( FbxProperty &pProperty, FbxString& pConnectionString );
void DisplayMaterialConnections(FbxMesh* pMesh);
void DisplayMaterialTextureConnections( FbxSurfaceMaterial* pMaterial, 
                                       char * header, int pMatId, int l );


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

gv_bone	*	find_bone (const gv_id & name , gvt_array<gv_bone>& bones)
{
	for ( int i=0; i< bones.size(); i++)
	{
		if (bones[i].m_name==name) return &bones[i];
	}
	return NULL;
}

void update_bone_hierachy_depth(gv_bone * pbone, gvt_array<gv_bone>& bones)
{
	if (!pbone) return; 
	gv_bone * pbone_p=find_bone( pbone->m_parent_id,bones);
	if (!pbone_p) 
	{
		pbone->m_hierachy_depth=0; 
	}
	else
	{
		update_bone_hierachy_depth(pbone_p, bones);
		pbone->m_hierachy_depth=pbone_p->m_hierachy_depth+1;
	}
	return; 
}

void update_bone_hierachy (  gvt_array<gv_bone>& bones)
{
	for ( int i=0 ; i< bones.size() ; i++ )
	{
		if (!find_bone(bones[i].m_parent_id,  bones)   && bones[i].m_parent_id !="RootNode" )
		{
			FbxNode* pNode =(FbxNode*)bones[i].m_user_data; 
			pNode=pNode->GetParent();
			gv_bone * pbone=bones.add_dummy();
			pbone->m_name= (char *) pNode->GetName();
			pbone->m_user_data = (gv_int_ptr)pNode ;
			pbone->m_parent_id=(char *)pNode->GetParent()->GetName();
			pbone->m_parent_idx=-1;
			copy(pbone->m_tm, pNode->EvaluateGlobalTransform()); //bones[i].m_tm; 
			pbone->m_matrix_model_to_bone=bones[i].m_matrix_model_to_bone;
		}
	}
	for ( int i=0 ; i< bones.size() ; i++ )
	{
		if (!bones[i].m_hierachy_depth) update_bone_hierachy_depth(&bones[i],bones);
	}
	bones.sort();
}

FbxAMatrix ComputeTotalMatrix(FbxNode* Node)
{
	FbxAMatrix Geometry;
	FbxVector4 Translation, Rotation, Scaling;
	Translation = Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	Rotation = Node->GetGeometricRotation(FbxNode::eSourcePivot);
	Scaling = Node->GetGeometricScaling(FbxNode::eSourcePivot);
	Geometry.SetT(Translation);
	Geometry.SetR(Rotation);
	Geometry.SetS(Scaling);

	//For Single Matrix situation, obtain transfrom matrix from eDESTINATION_SET, which include pivot offsets and pre/post rotations.
	FbxAMatrix& GlobalTransform = gScene->GetAnimationEvaluator()->GetNodeGlobalTransform(Node);

	FbxAMatrix TotalMatrix;
	TotalMatrix = GlobalTransform * Geometry;

	return TotalMatrix;
}

void DisplayMesh(FbxNode* pNode)
{
	FbxTime poseTime = FBXSDK_TIME_INFINITE;
	poseTime = 0;
	
    FbxMesh* lMesh = (FbxMesh*) pNode->GetNodeAttribute ();
	FbxMesh* pMesh=lMesh;
	gv_model * pmodel=the_model; 
	gv_string_tmp name="MESH_"; 
	name<<(char*)pNode->GetName();

	gv_static_mesh *pgv_mesh=pmodel->get_sandbox()->create_object<gv_static_mesh>(gv_object_name(*name), pmodel ); 
	gv_vertex_buffer * pvb=pmodel->get_sandbox()->create_object<gv_vertex_buffer>(pgv_mesh); 
	gv_index_buffer  * pib=pmodel->get_sandbox()->create_object<gv_index_buffer> (pgv_mesh);
	pgv_mesh->m_vertex_buffer=pvb;
	pgv_mesh->m_index_buffer=pib;
	gv_skeletal_mesh *  pskeletal_mesh =NULL; ; 

	gv_vertex_buffer vb_pos;		//only hold the pos and blend weight.
	gv_vertex_buffer &vb_mesh=*pvb;	//hold the whole vertex data.	
	gv_index_buffer  &ib_mesh=*pib; 
	gvt_hash_map<FbxNode * , gv_bone *> bone_map;

	int   lControlPointsCount = pMesh->GetControlPointsCount();
	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	int i, j;

	vb_pos.m_raw_pos.reserve(lControlPointsCount);
	for (i = 0; i < lControlPointsCount; i++)
	{
		gv_vector3 v3; 
		v3.x =(gv_float)lControlPoints[i][0]; 
		v3.y =(gv_float)lControlPoints[i][1]; 
		v3.z =(gv_float)lControlPoints[i][2]; 
		vb_pos.m_raw_pos.push_back(v3);

	} 

	FbxGeometry* pGeometry=pMesh; 
	int lSkinCount=0;
	int lClusterCount=0;
	FbxCluster* lCluster;

	lSkinCount=pGeometry->GetDeformerCount(FbxDeformer::eSkin);
	if (lSkinCount)  
	{
		vb_pos.m_raw_blend_index.resize(lControlPointsCount); 
		vb_pos.m_raw_blend_index=gv_vector4i(-1,-1,-1,-1); 
		vb_pos.m_raw_blend_weight.resize(lControlPointsCount); 
		vb_pos.m_raw_blend_weight=gv_vector4(0,0,0,0);
		gv_string_tmp name="SK_MESH_"; 
		name<<(char*)pNode->GetName();
		pskeletal_mesh = pmodel->get_sandbox()->create_object<gv_skeletal_mesh>(gv_object_name(*name), pmodel);
		pgv_mesh->set_owner(pskeletal_mesh);
		pskeletal_mesh->m_t_pose_mesh=pgv_mesh;
		pskeletal_mesh->m_skeletal=pmodel->get_sandbox()->create_object<gv_skeletal>(pskeletal_mesh);
	}
	
	
	for(i=0; i!=lSkinCount; ++i)
	{
		if (i==1 ) 
		{//gv_temp only support 1 skin now.
			break;
		};
		gvt_array<gv_bone>& bones=pskeletal_mesh->m_skeletal->m_bones;

		gv_bone bone; 
		lClusterCount = ((FbxSkin *) pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
		FbxCluster::ELinkMode lClusterMode = ((FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();
		FbxAMatrix MeshMatrix = ComputeTotalMatrix(pMesh->GetNode());
		for (j = 0; j != lClusterCount; ++j)
		{
			lCluster=((FbxSkin *) pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
			if(lCluster->GetLink() == NULL)
			{
				continue;
			}
			FbxNode* Link = lCluster->GetLink();

			bone.m_name= (char *) lCluster->GetLink()->GetName();
			bone.m_user_data=(gv_int_ptr)lCluster->GetLink();
			bone.m_parent_id=(char *)lCluster->GetLink()->GetParent()->GetName();
			bone.m_parent_idx=-1;

			int k, lIndexCount = lCluster->GetControlPointIndicesCount();
			int* lIndices = lCluster->GetControlPointIndices();
			double* lWeights = lCluster->GetControlPointWeights();
			for(k = 0; k < lIndexCount; k++)
			{
				vb_pos.set_blending_info(lIndices[k],j ,(gv_float) lWeights[k]);
			}
		
			FbxAMatrix lReferenceGlobalInitPosition;
			FbxAMatrix lReferenceGlobalCurrentPosition;
			FbxAMatrix lClusterGlobalInitPosition;
			FbxAMatrix lClusterGlobalCurrentPosition;
			FbxAMatrix lReferenceGeometry;
			FbxAMatrix lClusterGeometry;
			FbxAMatrix lClusterRelativeInitPosition;
			FbxAMatrix lClusterRelativeCurrentPositionInverse;
			FbxAMatrix lVertexTransformMatrix;

			if (lClusterMode == FbxCluster::eAdditive  && lCluster->GetAssociateModel())
			{
				lCluster->GetTransformAssociateModelMatrix(lReferenceGlobalInitPosition);
				//lReferenceGlobalCurrentPosition = GetGlobalPosition(lCluster->GetAssociateModel(), pTime, pPose);
				lReferenceGlobalCurrentPosition = gScene->GetAnimationEvaluator()->GetNodeGlobalTransform(lCluster->GetAssociateModel(), poseTime);
				// Geometric transform of the model
				lReferenceGeometry = GetGeometry(lCluster->GetAssociateModel());
				lReferenceGlobalCurrentPosition *= lReferenceGeometry;
			}
			else
			{
				lCluster->GetTransformMatrix(lReferenceGlobalInitPosition);
				lReferenceGlobalCurrentPosition = MeshMatrix;
				// Multiply lReferenceGlobalInitPosition by Geometric Transformation
				lReferenceGeometry = GetGeometry(pMesh->GetNode());
				lReferenceGlobalInitPosition *= lReferenceGeometry;
			}
			// Get the link initial global position and the link current global position.
			lCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);
			lClusterGlobalCurrentPosition = Link->GetScene()->GetAnimationEvaluator()->GetNodeGlobalTransform(Link, poseTime);
			// Compute the initial position of the link relative to the reference.
			lClusterRelativeInitPosition = lClusterGlobalInitPosition.Inverse();
			gv_matrix44 world_to_bone;
			copy(world_to_bone, lClusterRelativeInitPosition);
			gv_matrix44 model_world;
			copy(model_world, lReferenceGlobalInitPosition);
			pskeletal_mesh->m_skeletal->m_root_tm=bone.m_matrix_model_to_bone;
			bone.m_matrix_model_to_bone=model_world*world_to_bone;
			copy(bone.m_tm,lClusterGlobalInitPosition);
			bones.push_back(bone);
		}

		update_bone_hierachy(bones);
		pskeletal_mesh->m_skeletal->update_bone_parent_with_name();
		for ( int s=0 ; s<bones.size() ; s++)
		{
			bone_map[(FbxNode *) (bones[s].m_user_data)]=&bones[s];
		}

		for (int s = 0; s < lControlPointsCount; s++)
		{
			for ( int t=0; t< 4; t++ )
			{
				if (vb_pos.m_raw_blend_index[s][t]==-1) continue;
				int bone= vb_pos.m_raw_blend_index[s][t];
				lCluster = ((FbxSkin *)pGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(bone);
				FbxNode * pnode = lCluster->GetLink();
				gv_bone * pbone=bone_map[pnode];
				vb_pos.m_raw_blend_index[s][t]=bones.index(pbone);
			}
		} 

	}
	//===================================================================================
	bool with_skin = false; 
	if (lSkinCount)  with_skin = true;
    DisplayMaterial(lMesh,with_skin);
	//DisplayMaterialMapping(lMesh,pgv_mesh);
	DisplayTexture(lMesh);
	DisplayMaterialConnections(lMesh);
	DisplayPolygons(lMesh,vb_pos, vb_mesh , ib_mesh, pgv_mesh);
	

	vb_pos.normalize_blending_weight();

	//vb_pos.export_object_xml(the_xml);
	//vb_mesh.export_object_xml(the_xml);
	//ib_mesh.export_object_xml(the_xml);
	pgv_mesh->merge_optimize_vertex();

	if (pskeletal_mesh )
		pmodel->m_skeletal_mesh.push_back(pskeletal_mesh);
	else
		pmodel->m_static_mesh.push_back( pgv_mesh);

  //  
    //DisplayLink(lMesh);
    //DisplayShape(lMesh);
}

/*
void DisplayControlsPoints(FbxMesh* pMesh)
{
    int i, lControlPointsCount = pMesh->GetControlPointsCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints();

    DisplayString("    Control Points");

    for (i = 0; i < lControlPointsCount; i++)
    {
        DisplayInt("        Control Point ", i);
        Display3DVector("            Coordinates: ", lControlPoints[i]);

        for (int j = 0; j < pMesh->GetLayerCount(); j++)
        {
            FbxLayerElementNormal* leNormals = pMesh->GetLayer(j)->GetNormals();
            if (leNormals)
            {
                if (leNormals->GetMappingMode() == FbxLayerElement::eByControlPoint)
                {
                    char header[100];
                    sprintf(header, "            Normal Vector (on layer %d): ", j); 
                    if (leNormals->GetReferenceMode() == FbxLayerElement::eDirect)
                        Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
                }
            }
        }
    }

    DisplayString("");
}

*/
void DisplayPolygons(FbxMesh* pMesh, gv_vertex_buffer & vb_pos, gv_vertex_buffer & vb_mesh,gv_index_buffer & ib_mesh , gv_static_mesh * my_mesh)
{
    int i, j, lPolygonCount = pMesh->GetPolygonCount();
    FbxVector4* lControlPoints = pMesh->GetControlPoints(); 
    int vertexId = 0;
	if (!lPolygonCount) return;
	gv_material * last_material=NULL;
	gv_mesh_segment * last_seg=NULL;
    for (i = 0; i < lPolygonCount; i++)
    {
        int l;
		gv_material * cu_material=s_polygon_material_map[i]; 
		GV_ASSERT(cu_material);//?could this happen?
		if (cu_material!=last_material)
		{
#pragma GV_REMINDER("[PITFALL]never use a pointer to point to array element!!!!")
			gv_mesh_segment seg;
			seg.m_start_index=ib_mesh.m_raw_index_buffer.size();
			seg.m_material=cu_material;
			last_material=cu_material;
			my_mesh->m_segments.push_back(seg);
			if (my_mesh->m_segments.size()>1 )
			{
				last_seg=&my_mesh->m_segments[my_mesh->m_segments.size()-2];
				last_seg->m_index_size=seg.m_start_index-last_seg->m_start_index;
			}
		}
		/*
        for (l = 0; l < pMesh->GetLayerCount(); l++)
        {
            if (l=1 ) break; // only support 1 layer currently . 

			FbxLayerElementPolygonGroup* lePolgrp = pMesh->GetLayer(l)->GetPolygonGroups();
            if (lePolgrp)
            {
                switch (lePolgrp->GetMappingMode())
                {
                case FbxLayerElement::eByPolygon:
                    if (lePolgrp->GetReferenceMode() == FbxLayerElement::eIndex)
                    {
                        sprintf(header, "        Assigned to group (on layer %d): ", l); 
                        int polyGroupId = lePolgrp->GetIndexArray().GetAt(i);
                        DisplayInt(header, polyGroupId);
                        break;
                    }
                default:
                    // any other mapping modes don't make sense
                    DisplayString("        \"unsupported group assignment\"");
                    break;
                }
            }
        }*/

        int lPolygonSize	= pMesh->GetPolygonSize(i);
		gv_int start_index	=vb_mesh.m_raw_pos.size();
        for (j = 0; j < lPolygonSize; j++)
        {
            int lControlPointIndex = pMesh->GetPolygonVertex(i, j);
			int s=vb_mesh.m_raw_pos.size(); 
			if (j>=3) 
			{
				//a new triangle form the fan!
				ib_mesh.m_raw_index_buffer.push_back(start_index);
				ib_mesh.m_raw_index_buffer.push_back(s-1);
				ib_mesh.m_raw_index_buffer.push_back(s);
			}
			else
				ib_mesh.m_raw_index_buffer.push_back(s);

			vb_mesh.m_raw_pos.push_back( vb_pos.m_raw_pos[lControlPointIndex] ); 
			if (vb_pos.m_raw_blend_index.size() )
			{
				vb_mesh.m_raw_blend_index.push_back( vb_pos.m_raw_blend_index[lControlPointIndex] ); 
				vb_mesh.m_raw_blend_weight.push_back( vb_pos.m_raw_blend_weight[lControlPointIndex] ); 
			}
			 
            for (l = 0; l < pMesh->GetLayerCount(); l++)
            {
                if (l==4 ) break; // only support 4 layer currently . 
				/*
				FbxLayerElementVertexColor* leVtxc = pMesh->GetLayer(l)->GetVertexColors();
                if (leVtxc)
                {
                    switch (leVtxc->GetMappingMode())
                    {
                    case FbxLayerElement::eByControlPoint:
                        switch (leVtxc->GetReferenceMode())
                        {
                        case FbxLayerElement::eDirect:
                            DisplayColor(header, leVtxc->GetDirectArray().GetAt(lControlPointIndex));
                            break;
                        case FbxLayerElement::eIndexToDirect:
                            {
                                int id = leVtxc->GetIndexArray().GetAt(lControlPointIndex);
                                DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                            }
                            break;
                        default:
                            break; // other reference modes not shown here!
                        }
                        break;

                    case FbxLayerElement::eByPolygonVertex:
                        {
                            switch (leVtxc->GetReferenceMode())
                            {
                            case FbxLayerElement::eDirect:
                                DisplayColor(header, leVtxc->GetDirectArray().GetAt(vertexId));
                                break;
                            case FbxLayerElement::eIndexToDirect:
                                {
                                    int id = leVtxc->GetIndexArray().GetAt(vertexId);
                                    DisplayColor(header, leVtxc->GetDirectArray().GetAt(id));
                                }
                                break;
                            default:
                                break; // other reference modes not shown here!
                            }
                        }
                        break;

                    case FbxLayerElement::eByPolygon: // doesn't make much sense for UVs
                    case FbxLayerElement::eAllSame:   // doesn't make much sense for UVs
                    case FbxLayerElement::eNone:       // doesn't make much sense for UVs
                        break;
                    }
                }*/

                FbxLayerElementUV* leUV = pMesh->GetLayer(l)->GetUVs();
				gv_vector2 uv; 
                if (leUV)
                {
                    switch (leUV->GetMappingMode())
                    {
                    case FbxLayerElement::eByControlPoint:
                        switch (leUV->GetReferenceMode())
                        {
                        case FbxLayerElement::eDirect:
							copy (uv,  leUV->GetDirectArray().GetAt(lControlPointIndex));
                            break;
                        case FbxLayerElement::eIndexToDirect:
                            {
                                int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
								copy (uv,  leUV->GetDirectArray().GetAt(id));
                            }
                            break;
                        default:
                            break; // other reference modes not shown here!
                        }
                        break;

                    case FbxLayerElement::eByPolygonVertex:
                        {
                            int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
                            switch (leUV->GetReferenceMode())
                            {
                            case FbxLayerElement::eDirect:
                            case FbxLayerElement::eIndexToDirect:
                                {
									copy (uv,  leUV->GetDirectArray().GetAt(lTextureUVIndex));
                                }
                                break;
                            default:
                                break; // other reference modes not shown here!
                            }
                        }
                        break;

                    case FbxLayerElement::eByPolygon: // doesn't make much sense for UVs
                    case FbxLayerElement::eAllSame:   // doesn't make much sense for UVs
                    case FbxLayerElement::eNone:       // doesn't make much sense for UVs
                        break;
                    }

					gvt_array< gv_vector2> * pc=&vb_mesh.m_raw_texcoord0;
					pc[l].push_back(uv);
                }


				FbxLayerElementNormal* leNormal = pMesh->GetLayer(l)->GetNormals();
				gv_vector3 normal(0,1,0); 
				if (leNormal)
				{
					if(leNormal->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
					{
						switch (leNormal->GetReferenceMode())
						{
						case FbxLayerElement::eDirect:
							copy(normal, leNormal->GetDirectArray().GetAt(vertexId));
							break;
						case FbxLayerElement::eIndexToDirect:
							{
								int id = leNormal->GetIndexArray().GetAt(vertexId);
								copy(normal, leNormal->GetDirectArray().GetAt(id));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					vb_mesh.m_raw_normal.push_back(normal);
				}

				FbxLayerElementTangent* leTangent = pMesh->GetLayer(l)->GetTangents();
				if (leTangent)
				{
					
					gv_vector3 tangent(1,0,0); 
					if(leTangent->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
					{
						switch (leTangent->GetReferenceMode())
						{
						case FbxLayerElement::eDirect:
							copy(tangent, leTangent->GetDirectArray().GetAt(vertexId));
							break;
						case FbxLayerElement::eIndexToDirect:
							{
								int id = leTangent->GetIndexArray().GetAt(vertexId);
								copy(tangent, leTangent->GetDirectArray().GetAt(id));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					vb_mesh.m_raw_tangent.push_back(tangent);
				}

				FbxLayerElementBinormal* leBinormal = pMesh->GetLayer(l)->GetBinormals();
				if (leBinormal)
				{
					gv_vector3 binormal(0,0,1); 
					if(leBinormal->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
					{
						switch (leBinormal->GetReferenceMode())
						{
						case FbxLayerElement::eDirect:
							copy(binormal, leBinormal->GetDirectArray().GetAt(vertexId));
							break;
						case FbxLayerElement::eIndexToDirect:
							{
								int id = leBinormal->GetIndexArray().GetAt(vertexId);
								copy(binormal, leBinormal->GetDirectArray().GetAt(id));
							}
							break;
						default:
							break; // other reference modes not shown here!
						}
					}
					vb_mesh.m_raw_binormal.push_back(binormal);
				}
            } // for layer
            vertexId++;
        } // for polygonSize

    } // for polygonCount

	last_seg=my_mesh->m_segments.last();
	if (last_seg )
	{
		last_seg->m_index_size=ib_mesh.m_raw_index_buffer.size()-last_seg->m_start_index;
	}
	s_polygon_material_map.clear();
}

void DisplayTextureNames( FbxProperty &pProperty, FbxString& pConnectionString )
{
    int lLayeredTextureCount = pProperty.GetSrcObjectCount(FbxLayeredTexture::ClassId);
    if(lLayeredTextureCount > 0)
    {
        for(int j=0; j<lLayeredTextureCount; ++j)
        {
            FbxLayeredTexture *lLayeredTexture = FbxCast <FbxLayeredTexture>(pProperty.GetSrcObject(FbxLayeredTexture::ClassId, j));
            int lNbTextures = lLayeredTexture->GetSrcObjectCount(FbxTexture::ClassId);
            pConnectionString += " Texture ";

            for(int k =0; k<lNbTextures; ++k)
            {
                //lConnectionString += k;
                pConnectionString += "\"";
                pConnectionString += (char*)lLayeredTexture->GetName();
                pConnectionString += "\"";
                pConnectionString += " ";
            }
            pConnectionString += "of ";
            pConnectionString += pProperty.GetName();
            pConnectionString += " on layer ";
            pConnectionString += j;
        }
        pConnectionString += " |";
    }
    else
    {
        //no layered texture simply get on the property
        int lNbTextures = pProperty.GetSrcObjectCount(FbxTexture::ClassId);

        if(lNbTextures > 0)
        {
            pConnectionString += " Texture ";
            pConnectionString += " ";

            for(int j =0; j<lNbTextures; ++j)
            {
                FbxTexture* lTexture = FbxCast <FbxTexture> (pProperty.GetSrcObject(FbxTexture::ClassId,j));
                if(lTexture)
                {
                    pConnectionString += "\"";
                    pConnectionString += (char*)lTexture->GetName();
                    pConnectionString += "\"";
                    pConnectionString += " ";
                }
            }
            pConnectionString += "of ";
            pConnectionString += pProperty.GetName();
            pConnectionString += " |";
        }
    }
}

void DisplayMaterialTextureConnections( FbxSurfaceMaterial* pMaterial, char * header, int pMatId, int l )
{
    FbxString lConnectionString = "            Material %d (on layer %d) -- ";
    //Show all the textures

    FbxProperty lProperty;
    //Diffuse Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
    DisplayTextureNames(lProperty, lConnectionString);

    //DiffuseFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Emissive Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissive);
    DisplayTextureNames(lProperty, lConnectionString);

    //EmissiveFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);
    DisplayTextureNames(lProperty, lConnectionString);


    //Ambient Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
    DisplayTextureNames(lProperty, lConnectionString); 

    //AmbientFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sAmbientFactor);
    DisplayTextureNames(lProperty, lConnectionString);          

    //Specular Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
    DisplayTextureNames(lProperty, lConnectionString);  

    //SpecularFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sSpecularFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Shininess Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    DisplayTextureNames(lProperty, lConnectionString);

    //Bump Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sBump);
    DisplayTextureNames(lProperty, lConnectionString);

    //Normal Map Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
    DisplayTextureNames(lProperty, lConnectionString);

    //Transparent Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparentColor);
    DisplayTextureNames(lProperty, lConnectionString);

    //TransparencyFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Reflection Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflection);
    DisplayTextureNames(lProperty, lConnectionString);

    //ReflectionFactor Textures
    lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sReflectionFactor);
    DisplayTextureNames(lProperty, lConnectionString);

    //Update header with material info
	bool lStringOverflow = (lConnectionString.GetLen() + 10 >= MAT_HEADER_LENGTH); // allow for string length and some padding for "%d"
	if (lStringOverflow)
	{
		// Truncate string!
		lConnectionString = lConnectionString.Left(MAT_HEADER_LENGTH - 10);
		lConnectionString = lConnectionString + "...";
	}
	sprintf(header, lConnectionString.Buffer(), pMatId, l);
	DisplayString(header);
}

void DisplayMaterialConnections(FbxMesh* pMesh)
{
    int i, l, lPolygonCount = pMesh->GetPolygonCount();
	s_polygon_material_map.resize(lPolygonCount);//
	s_polygon_material_map=0;
	gv_model * my_model=the_model; 
    //char header[MAT_HEADER_LENGTH];
	//  DisplayString("    Polygons Material Connections");
    //check whether the material maps with only one mesh
	/*
    bool lIsAllSame = true;
    for (l = 0; l < pMesh->GetLayerCount(); l++)
    {

        FbxLayerElementMaterial* lLayerMaterial = pMesh->GetLayer(l)->GetMaterials();
        if (lLayerMaterial) 
        {
            FbxSurfaceMaterial* lMaterial = NULL;
            int lMatId = -1;
            if( lLayerMaterial->GetMappingMode() == FbxLayerElement::eByPolygon) 
            {
                lIsAllSame = false;
                break;
            }
        }
    }

    //For eAllSame mapping type, just out the material and texture mapping info once
    if(lIsAllSame)
    {
        for (l = 0; l < pMesh->GetLayerCount(); l++)
        {

            FbxLayerElementMaterial* lLayerMaterial = pMesh->GetLayer(l)->GetMaterials();
            if (lLayerMaterial) 
            {
                if( lLayerMaterial->GetMappingMode() == FbxLayerElement::eAllSame) 
                {
                    FbxSurfaceMaterial* lMaterial = pMesh->GetNode()->GetMaterial(lLayerMaterial->GetIndexArray().GetAt(0));    
                    int lMatId = lLayerMaterial->GetIndexArray().GetAt(0);
					gv_material * my_material=my_model->get_material(gv_id( lMaterial->GetName() )  );
					GV_ASSERT(my_material);
                    if(lMatId >=0)
                    {
                        DisplayInt("        All polygons share the same material on layer ", l);
                        DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
                    }
					s_polygon_material_map=my_material;
                }
            }
            else
            {
                //layer 0 has no material
                //if(l == 0)
                //    DisplayString("        no material applied");
            }
        }
    }
    //For eByPolygon mapping type, just out the material and texture mapping info once
    else*/
    {
        for (i = 0; i < lPolygonCount; i++)
        {
           // DisplayInt("        Polygon ", i);

            for (l = 0; l < pMesh->GetLayerCount(); l++)
            {

                FbxLayerElementMaterial* lLayerMaterial = pMesh->GetLayer(l)->GetMaterials();
                if (lLayerMaterial) 
                {
                    FbxSurfaceMaterial* lMaterial = NULL;
                    int lMatId = -1;
                    lMaterial = pMesh->GetNode()->GetMaterial(lLayerMaterial->GetIndexArray().GetAt(i));
                    lMatId = lLayerMaterial->GetIndexArray().GetAt(i);

					gv_material * my_material=my_model->get_material(gv_id( lMaterial->GetName() )  );
					GV_ASSERT(my_material);
					s_polygon_material_map[i]=my_material;

                    //if(lMatId >= 0)
                    //{
                    //    DisplayMaterialTextureConnections(lMaterial, header, lMatId, l);
                    //}
                }
            }
        }
    }
}


void DisplayMaterialMapping(FbxMesh* pMesh, gv_static_mesh * my_mesh)
{
	int lPolygonCount = pMesh->GetPolygonCount();
	s_polygon_material_map.resize(lPolygonCount);//
	s_polygon_material_map=0;
    int lMtrlCount = 0;
	gv_model * my_model=the_model; 

    FbxNode* lNode = NULL;
    if(pMesh){
        lNode = pMesh->GetNode();
        if(lNode)
            lMtrlCount = lNode->GetMaterialCount();    
    }

    for (int l = 0; l < pMesh->GetLayerCount(); l++)
    {
        FbxLayerElementMaterial* leMat = pMesh->GetLayer(l)->GetMaterials();
        if (leMat)
        {
           // char header[100];
			GV_ASSERT(leMat->GetMappingMode()==FbxLayerElement::eByPolygon);
            int lMaterialCount = 0;
            FbxString lString;
			//WRONG!! , TODO
			gv_material * my_material=my_model->get_material(gv_id( leMat->GetName() )  );
			GV_ASSERT(my_material);

            if (leMat->GetReferenceMode() == FbxLayerElement::eDirect ||
                leMat->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            {
                lMaterialCount = lMtrlCount;
            }

            if (leMat->GetReferenceMode() == FbxLayerElement::eIndex ||
                leMat->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
            {
                int i;


                int lIndexArrayCount = leMat->GetIndexArray().GetCount(); 
                for (i = 0; i < lIndexArrayCount; i++)
                {
                    gv_int idx= leMat->GetIndexArray().GetAt(i);
					s_polygon_material_map[i]=my_material;

                }
				

            }
        }
    }

    DisplayString("");
}
