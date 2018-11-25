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


void DisplayMaterial(FbxGeometry* pGeometry,bool with_skin)
{
    gv_model * pmodel=the_model; 
	int lMaterialCount = 0;
    FbxNode* lNode = NULL;
    if(pGeometry){
        lNode = pGeometry->GetNode();
        if(lNode)
            lMaterialCount = lNode->GetMaterialCount();    
    }

    for (int l = 0; l < pGeometry->GetLayerCount(); l++)
    {
        FbxLayerElementMaterial* leMat = pGeometry->GetLayer(l)->GetMaterials();
        if (leMat)
        {
            if (leMat->GetReferenceMode() == FbxLayerElement::eIndex)
                // Materials are in an undefined external table
                continue;

            if (lMaterialCount > 0)
            {
                FbxPropertyDouble3 lFbxDouble3;
                FbxPropertyDouble1 lFbxDouble1;
                FbxColor theColor;
                

                for (int lCount = 0; lCount < lMaterialCount; lCount ++)
                {
                    FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(lCount);
					if (pmodel->get_material( gv_id(lMaterial->GetName()))) continue; 
					gv_material * material =pmodel->get_sandbox()->create_object<gv_material>(gv_id(lMaterial->GetName()), pmodel);
					if (with_skin) material->set_file_name(DEFAULT_FBX_SKIN_MATERIAL); 
					else material->set_file_name(DEFAULT_FBX_MATERIAL);
					pmodel->m_materials.push_back(material);

					//res\common\skin.material
					//res\common\box.material
					//u_diffuseTexture
					//u_normalTexture

                    /*keep it as a reference for compose shader in max
					//Get the implementation to see if it's a hardware shader.
                    const FbxImplementation* lImplementation = GetImplementation(lMaterial, ImplementationHLSL);
					FbxString lImplemenationType = "HLSL";
                    if(!lImplementation)
                    {
                        lImplementation = GetImplementation(lMaterial, ImplementationCGFX);
                        lImplemenationType = "CGFX";
                    }
                    if(lImplementation)
                    {
                        //Now we have a hardware shader, let's read it
                        printf("            Hardware Shader Type: %s\n", lImplemenationType.Buffer());
                        FbxBindingTable const* lRootTable = lImplementation->GetRootTable();
                        fbxString lFileName = lRootTable->DescAbsoluteURL.Get();
                        fbxString lTechniqueName = lRootTable->DescTAG.Get(); 


                        FbxBindingTable const* lTable = lImplementation->GetRootTable();
                        size_t lEntryNum = lTable->GetEntryCount();

                        for(int i=0;i <(int)lEntryNum; ++i)
                        {
                            const FbxBindingTableEntry& lEntry = lTable->GetEntry(i);
                            const char* lEntrySrcType = lEntry.GetEntryType(true); 
                            FbxProperty lFbxProp;


                            FbxString lTest = lEntry.GetSource();
                            printf("            Entry: %s\n", lTest.Buffer());


                            if ( strcmp( FbxPropertyEntryView::sEntryType, lEntrySrcType ) == 0 )
                            {   
                                lFbxProp = lMaterial->FindPropertyHierarchical(lEntry.GetSource()); 
                                if(!lFbxProp.IsValid())
                                {
                                    lFbxProp = lMaterial->RootProperty.FindHierarchical(lEntry.GetSource());
                                }


                            }
                            else if( strcmp( FbxConstantEntryView::sEntryType, lEntrySrcType ) == 0 )
                            {
                                lFbxProp = lImplementation->GetConstants().FindHierarchical(lEntry.GetSource());
                            }
                            if(lFbxProp.IsValid())
                            {
                                if( lFbxProp.GetSrcObjectCount( FBX_TYPE(FbxTexture) ) > 0 )
                                {
                                    //do what you want with the texture
                                    for(int j=0; j<lFbxProp.GetSrcObjectCount(FBX_TYPE(FbxTexture)); ++j)
                                    {
                                        FbxTexture *lTex = lFbxProp.GetSrcObject(FBX_TYPE(FbxTexture),j);
                                        printf("                Texture: %s\n", lTex->GetFileName());
                                    }
                                }
                                else
                                {
                                    FbxDataType lFbxType = lFbxProp.GetPropertyDataType();
                                    FbxString blah = lFbxType.GetName();
                                    if(DTBool == lFbxType)
                                    {
                                        DisplayBool("                Bool: ", FbxGet <bool> (lFbxProp) );
                                    }
                                    else if ( DTInteger == lFbxType ||  DTEnum  == lFbxType )
                                    {
                                        DisplayInt("                Int: ", FbxGet <int> (lFbxProp));
                                    }
                                    else if ( DTFloat == lFbxType)
                                    {
                                        DisplayDouble("                Float: ", FbxGet <float> (lFbxProp));

                                    }
                                    else if ( DTDouble == lFbxType)
                                    {
                                        DisplayDouble("                Double: ", FbxGet <double> (lFbxProp));
                                    }
                                    else if ( DTString == lFbxType
                                        ||  DTUrl  == lFbxType
                                        ||  DTXRefUrl  == lFbxType )
                                    {
                                        DisplayString("                String: ", (FbxGet <fbxString> (lFbxProp)).Buffer());
                                    }
                                    else if ( DTDouble2 == lFbxType)
                                    {
                                        fbxDouble2 lDouble2=FbxGet <fbxDouble2> (lFbxProp);
                                        FbxVector2 lVect;
                                        lVect[0] = lDouble2[0];
                                        lVect[1] = lDouble2[1];

                                        Display2DVector("                2D vector: ", lVect);
                                    }
                                    else if ( DTVector3D == lFbxType||DTDouble3 == lFbxType || DTColor3 == lFbxType)
                                    {
                                        fbxDouble3 lDouble3 = FbxGet <fbxDouble3> (lFbxProp);


                                        FbxVector4 lVect;
                                        lVect[0] = lDouble3[0];
                                        lVect[1] = lDouble3[1];
                                        lVect[2] = lDouble3[2];
                                        Display3DVector("                3D vector: ", lVect);
                                    }

                                    else if ( DTVector4D == lFbxType || DTDouble4 == lFbxType || DTColor4 == lFbxType)
                                    {
                                        fbxDouble4 lDouble4 = FbxGet <fbxDouble4> (lFbxProp);
                                        FbxVector4 lVect;
                                        lVect[0] = lDouble4[0];
                                        lVect[1] = lDouble4[1];
                                        lVect[2] = lDouble4[2];
                                        lVect[3] = lDouble4[3];
                                        Display4DVector("                4D vector: ", lVect);
                                    }
                                    else if ( DTDouble44 == lFbxType)
                                    {
                                        fbxDouble44 lDouble44 = FbxGet <fbxDouble44> (lFbxProp);
                                        for(int j=0; j<4; ++j)
                                        {

                                            FbxVector4 lVect;
                                            lVect[0] = lDouble44[j][0];
                                            lVect[1] = lDouble44[j][1];
                                            lVect[2] = lDouble44[j][2];
                                            lVect[3] = lDouble44[j][3];
                                            Display4DVector("                4x4D vector: ", lVect);
                                        }

                                    }
                                }

                            }   
                        }
                    }
                    else */
					if(lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) )
                    {
                        // We found a Lambert material. Display its properties.
                        // Display the Ambient Color
						lFbxDouble3 = ((FbxSurfaceLambert *)lMaterial)->Ambient;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_ambient_color=c_a;
						}

                        // Display the Diffuse Color
                        lFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Diffuse;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_diffuse_color=c_a;
						}

                        // Display the Emissive
                        lFbxDouble3 =((FbxSurfaceLambert *)lMaterial)->Emissive;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_specular_color=c_a;
						}

                        // Display the Opacity
                        lFbxDouble1 =((FbxSurfaceLambert *)lMaterial)->TransparencyFactor;
                        material->m_opacity=(float)(1.0-lFbxDouble1.Get());
                    }
                    else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
                    {
                        // We found a Phong material.  Display its properties.

                        // Display the Ambient Color
                        lFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Ambient;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_ambient_color=c_a;
						}

                        // Display the Diffuse Color
                        lFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Diffuse;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_diffuse_color=c_a;
						}

                        // Display the Specular Color (unique to Phong materials)
                        lFbxDouble3 =((FbxSurfacePhong *) lMaterial)->Specular;
						{
							gv_colorf c_a(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2] , 1.f);
							material->m_specular_color=c_a;
						}

                        // Display the Emissive Color
                        //lFbxDouble3 =((FbxSurfacePhong *) lMaterial)->GetEmissiveColor();
                        //theColor.Set(lFbxDouble3.Get()[0], lFbxDouble3.Get()[1], lFbxDouble3.Get()[2]);
                        //DisplayColor("            Emissive: ", theColor);

                        //Opacity is Transparency factor now
                        lFbxDouble1 =((FbxSurfacePhong *) lMaterial)->TransparencyFactor;
						material->m_opacity=(float)(1.0-lFbxDouble1.Get());

                        // Display the Shininess
                       // lFbxDouble1 =((FbxSurfacePhong *) lMaterial)->GetShininess();
                       // DisplayDouble("            Shininess: ", lFbxDouble1.Get());

                        // Display the Reflectivity
                        //lFbxDouble1 =((FbxSurfacePhong *) lMaterial)->GetReflectionFactor();
                        //DisplayDouble("            Reflectivity: ", lFbxDouble1.Get());
                    }
                    //else
                    //    DisplayString("Unknown type of Material");

                    //FbxPropertyString lString;
                    //lString = lMaterial->GetShadingModel();
                    //DisplayString("            Shading Model: ", lString.Get().Buffer());
                    //DisplayString("");
                }
            }
        }
    }
}
