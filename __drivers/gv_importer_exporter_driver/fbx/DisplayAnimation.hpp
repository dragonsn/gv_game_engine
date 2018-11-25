/**************************************************************************************

 Copyright (C) 2001 - 2010 Autodesk, Inc. and/or its licensors.
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


void DisplayAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, bool isSwitcher = false);
void DisplayAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, bool isSwitcher = false);
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, 
                     void (*DisplayCurve) (FbxAnimCurve *pCurve), 
                     void (*DisplayListCurve) (FbxAnimCurve *pCurve, FbxProperty* pProperty), 
                     bool isSwitcher);
void DisplayCurveKeys(FbxAnimCurve *pCurve);
void FillCurveKeys(FbxAnimCurve *pCurve, gv_float * target,gv_int stride);
void DisplayCurveDefault(FbxAnimCurve *pCurve);
void DisplayListCurveKeys(FbxAnimCurve *pCurve, FbxProperty* pProperty);
void DisplayListCurveDefault(FbxAnimCurve *pCurve, FbxProperty* pProperty);
void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, gv_ani_sequence * pseq,  gv_int father_track_id);
FbxNode *  ani_root_node=NULL;

bool  is_skeletal_node(FbxNode * pnode )
{
	if (pnode==ani_root_node) return true; 
	else if (pnode->GetParent() ) return  is_skeletal_node(pnode->GetParent() );
	return false;
}
int g_nb_frame=0;
FbxTime g_period ; 
void DisplayAnimation(FbxScene* pScene)
{
    int i;
	gScene=pScene;
	gv_model * pmodel	 = the_model; 
	//if (!pmodel->get_nb_skeletal_mesh() ) return ; 
	//gv_skeletal_mesh * pmesh=pmodel->get_skeletal_mesh(0); 
	ani_root_node = gScene->GetRootNode();// (FbxNode *)pmesh->m_skeletal->m_bones[0].m_index;

	gv_ani_set * pani_set= pmodel->get_sandbox()->create_object<gv_ani_set>(pmodel);
	pmodel->m_skeletal_ani_set.push_back( pani_set);

	for (i = 0; i < pScene->GetSrcObjectCount(FbxCriteria::ObjectType(FbxAnimStack::ClassId)); i++)
    {
		FbxAnimStack* lAnimStack = FbxCast<FbxAnimStack>(pScene->GetSrcObject(FbxCriteria::ObjectType(FbxAnimStack::ClassId), i));

		pScene->SetCurrentAnimationStack(lAnimStack);
		FbxTakeInfo* lCurrentTakeInfo = pScene->GetTakeInfo(lAnimStack->GetName());
		if (lCurrentTakeInfo)
		{
			gStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			gStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			// Take the time line value
			FbxTimeSpan lTimeLineTimeSpan;
			pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
			gStart = lTimeLineTimeSpan.GetStart();
			gStop  = lTimeLineTimeSpan.GetStop();
		}
		
		g_period.SetMilliSeconds(30);
		g_nb_frame= (gv_int)((gStop.GetSecondDouble()-gStart.GetSecondDouble() )/g_period.GetSecondDouble());
		static int max_frame=200;
		if (g_nb_frame>max_frame)
		{
			g_nb_frame=max_frame;
		}
		
		gv_ani_sequence * pani_seq=pmodel->get_sandbox()->create_object<gv_ani_sequence>(gv_id ((char*)lAnimStack->GetName()), pani_set );
		pani_set->add_sequence(pani_seq);
		int nbAnimLayers = lAnimStack->GetMemberCount(FbxCriteria::ObjectType(FbxAnimLayer::ClassId));

		for (int l = 0; l < nbAnimLayers; l++)
		{
			if (l==1) break; //only one layer is support.
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>( l);
			pani_seq->set_duration(0);
			DisplayChannels(ani_root_node,lAnimLayer, pani_seq,-1);
		}
		pani_set->compress_all();
    }
}

void DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, gv_ani_sequence * pseq, gv_int father_track_id)
{
	FbxAnimCurve* lAnimCurve = NULL;
	gv_model * pmodel=the_model; 
	gv_string s=pNode->GetName();
	if ( s.find_string("Bone_Eye_L"))
	{
		//[2016-04-08 21:41:01][main][EDITOR][EXEC]: update  (5)nba_player:0/ddd3:0/gv_ani_set:340/Take_001:0/Bone_Eye_L:0.m_name "Bone_Eye_L:0"
	 //  GV_DEBUG_BREAK;
	}
	if (!use_key_frame)
	{
		gv_ani_track_raw * pani_track=pmodel->get_sandbox()->create_object<gv_ani_track_raw>(gv_id((char *)pNode->GetName()), pseq);
		pani_track->set_frequency(gFps);
		pseq->set_duration((float)g_nb_frame/gFps);
		pseq->add_track(pani_track);
		
		gCurrentTime = gStart;
		pani_track->m_pos.resize(g_nb_frame); 
		pani_track->m_rot.resize(g_nb_frame); 
		for ( int i=0; i< g_nb_frame ; i++)
		{

			FbxMatrix matrix= gScene->GetAnimationEvaluator()->GetNodeLocalTransform(pNode,gCurrentTime); 
			gv_matrix44 my_matrix; 
			copy(my_matrix ,matrix);
			gv_vector3 v;
			my_matrix.get_trans(v); 
			gv_quat q; 
			gv_math::convert(q,my_matrix); 
			pani_track->m_pos[i] = v*fbx_ani_scale;
			pani_track->m_rot[i]=q; 
			gCurrentTime+=g_period;
		}
	}
	
	else
	{
		gv_ani_track_with_key * pani_track_kf=pmodel->get_sandbox()->create_object<gv_ani_track_with_key>(gv_id((char *)pNode->GetName()), pseq);
		pani_track_kf->set_frequency(gFps);
		pani_track_kf->m_father_track = father_track_id;
		pseq->add_track(pani_track_kf);
		gv_string s=pNode->GetName();
		gvt_array_cached<gv_float,2048 >t_key_time;
		gvt_array_cached<gv_float,2048 >r_key_time;
		gvt_array_cached<gv_float,2048 >s_key_time;

		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		FbxTime   lKeyTime;
		float   lKeyValue;
		int     lCount;
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				t_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				t_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				t_key_time.insert_in_sorted_array(cu_time);
			}
		}
		
		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				r_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				r_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for(lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime  = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time=(float)lKeyTime.GetSecondDouble();
				r_key_time.insert_in_sorted_array(cu_time);
			}
		}

	
		//SCALE
		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for (lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time = (float)lKeyTime.GetSecondDouble();
				s_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for (lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time = (float)lKeyTime.GetSecondDouble();
				s_key_time.insert_in_sorted_array(cu_time);
			}
		}

		lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			int lKeyCount = lAnimCurve->KeyGetCount();
			for (lCount = 0; lCount < lKeyCount; lCount++)
			{
				lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				lKeyTime = lAnimCurve->KeyGetTime(lCount);
				gv_float cu_time = (float)lKeyTime.GetSecondDouble();
				s_key_time.insert_in_sorted_array(cu_time);
			}
		}
		
		//TRANS
		for ( int i=0; i<t_key_time.size(); i++)
		{
			gv_float cu_time=t_key_time[i];
			FbxTime t;
			t.SetSecondDouble(cu_time);
			FbxVector4 ani_v = gScene->GetAnimationEvaluator()->GetNodeLocalTranslation(pNode, t);
			gv_vector3 v;
			copy(v, ani_v);
			pani_track_kf->m_pos_track.insert_key(cu_time, v*fbx_ani_scale);
		}
		//ROT		
		for ( int i=0; i<r_key_time.size(); i++)
		{
			gv_float cu_time=r_key_time[i];
			gv_matrix44 my_matrix; 
			FbxTime t;
			t.SetSecondDouble(cu_time);
			FbxMatrix matrix3 = gScene->GetAnimationEvaluator()->GetNodeLocalTransform(pNode, t);
			copy(my_matrix ,matrix3);
			gv_quat q3; 
			gv_math::convert(q3,my_matrix); 
			pani_track_kf->m_rot_track.insert_key(cu_time,q3);
		}
		//SCALE
		for (int i = 0; i<s_key_time.size(); i++)
		{
			gv_float cu_time = s_key_time[i];
			gv_matrix44 my_matrix;
			FbxTime t;
			t.SetSecondDouble(cu_time);
			FbxVector4 s = gScene->GetAnimationEvaluator()->GetNodeLocalScaling(pNode, t);
			gv_vector3 v;
			copy(v, s);
			pani_track_kf->m_scale_track.insert_key(cu_time, v);
		}

		if (t_key_time.size())		pani_track_kf->set_duration(*t_key_time.last());
		if (r_key_time.size())		pani_track_kf->set_duration(gvt_max(*r_key_time.last(),pani_track_kf->get_duration() ));
	
		if (!t_key_time.size() || !r_key_time.size())
		{//in case no key at all give a trans at least..
			FbxTime t;
			t.SetSecondDouble(0);
			FbxMatrix matrix = gScene->GetAnimationEvaluator()->GetNodeLocalTransform(pNode, t);
			gv_matrix44 my_matrix; 
			copy(my_matrix ,matrix);
			gv_vector3 v, s;
			gv_quat q;
			gv_math::decompose(my_matrix, v, q, s);
			pani_track_kf->m_pos_track.insert_key(0, v*fbx_ani_scale);
			pani_track_kf->m_rot_track.insert_key(0,q); 
			pani_track_kf->m_scale_track.insert_key(0, s);
		}
		
		pseq->set_duration(gvt_max(pseq->get_duration(), pani_track_kf->get_duration()));
	}

	for(int lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		DisplayChannels( pNode->GetChild(lModelCount),pAnimLayer,pseq, pseq->get_track_number() );
	}
	
}

