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

static gv_material * s_cu_my_material;

void DisplayTextureInfo(FbxFileTexture* pTexture, int pBlendMode, int index)
{
	gv_material_tex tex;
	gv_string_tmp s = pTexture->GetName();
	s.to_lower();
	if (s.find_string("diffuse")) 		tex.m_id = gv_id(DEFAULT_FBX_TEX_DIFFUSE);
	else if (s.find_string("normal")) 	tex.m_id = gv_id(DEFAULT_FBX_TEX_NORMAL);
	else if (s.find_string("env")) 		tex.m_id = gv_id(DEFAULT_FBX_TEX_ENV);
	else {
		switch (index)
		{
		case 0: tex.m_id = gv_id(DEFAULT_FBX_TEX_DIFFUSE ); break;
		case 2: tex.m_id = gv_id(DEFAULT_FBX_TEX_ENV);  break;
		default: tex.m_id = gv_id(DEFAULT_FBX_TEX_NORMAL);
		}
	}

	gv_model * my_model=the_model; 
	gv_texture * my_texture=NULL;
	my_texture=my_model->get_texture_by_file_name(pTexture->GetFileName() );
	if (!my_texture)
	{
		gv_string s=gv_global::fm->get_main_name(pTexture->GetFileName() );
		gv_string_tmp s_tmp="tex_"; 
		s_tmp<<s; 
		my_texture=my_model->get_sandbox()->create_object<gv_texture>(gv_id(*s_tmp),my_model);
		//s=the_path;
		s=pTexture->GetFileName();
		my_texture->set_file_name(*s);
		my_model->add_texture(my_texture);
	}
	tex.m_texture=my_texture;
    tex.m_use_tiling=pTexture->GetSwapUV();
	//s_cu_my_material->add_material_texture(tex);
	s_cu_my_material->m_material_textures.push_back(tex);

}

void FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex,int TexIndex){

    if( pProperty.IsValid() )
    {

        FbxTexture * lTexture= NULL;
        //Here we have to check if it's layeredtextures, or just textures:
        int lLayeredTextureCount = pProperty.GetSrcObjectCount(FbxLayeredTexture::ClassId);
        if(lLayeredTextureCount > 0)
        {
            for(int j=0; j<lLayeredTextureCount; ++j)
            {
                //DisplayInt("    Layered Texture: ", j);
                FbxLayeredTexture *lLayeredTexture = FbxCast <FbxLayeredTexture>(pProperty.GetSrcObject(FbxLayeredTexture::ClassId, j));
                int lNbTextures = lLayeredTexture->GetSrcObjectCount(FbxTexture::ClassId);
                for(int k =0; k<lNbTextures; ++k)
                {
					FbxFileTexture* lTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(k);
                    if(lTexture)
                    {
                        FbxLayeredTexture::EBlendMode lBlendMode;
                        lLayeredTexture->GetTextureBlendMode(k, lBlendMode);

                        //DisplayString("    Textures for ", pProperty.GetName());
                        //DisplayInt("        Texture ", k);  
						DisplayTextureInfo(lTexture, (int)lBlendMode, TexIndex);
                    }

                }
            }
        }
        else
        {
            //no layered texture simply get on the property
            int lNbTextures = pProperty.GetSrcObjectCount(FbxTexture::ClassId);
            for(int j =0; j<lNbTextures; ++j)
            {

				FbxFileTexture* lTexture = pProperty.GetSrcObject<FbxFileTexture>(j);
                if(lTexture)
                {
                    //display connectMareial header only at the first time
                    //if(pDisplayHeader){                    
                    //    DisplayInt("    Textures connected to Material ", pMaterialIndex);
                   //     pDisplayHeader = false;
                    //}             

                    //DisplayString("    Textures for ", pProperty.GetName());
                    //DisplayInt("        Texture ", j);  
					DisplayTextureInfo(lTexture, -1, TexIndex);
                }
            }
        }
    }//end if pProperty

}


void DisplayTexture(FbxGeometry* pGeometry)
{

	FbxProperty Property;
	int NbMat = pGeometry->GetNode()->GetMaterialCount();
	FbxNode * Node = pGeometry->GetNode();
	int MaterialIndex;
	gv_model * my_model = the_model;
	for (MaterialIndex = 0; MaterialIndex < NbMat; MaterialIndex++)
	{
		FbxSurfaceMaterial *Material = Node->GetMaterial(MaterialIndex);
		//go through all the possible textures
		if (Material)
		{
			s_cu_my_material = my_model->get_material(gv_id(Material->GetName()));
			GV_ASSERT(s_cu_my_material);
			int TextureIndex;
			FBXSDK_FOR_EACH_TEXTURE(TextureIndex)
			{
				bool lDisplayHeader = true;
				Property = Material->FindProperty(FbxLayerElement::sTextureChannelNames[TextureIndex]);
				FindAndDisplayTextureInfoByProperty(Property, lDisplayHeader, MaterialIndex, TextureIndex);
			}

		}//end if(Material)

	}// end for Mate
}
