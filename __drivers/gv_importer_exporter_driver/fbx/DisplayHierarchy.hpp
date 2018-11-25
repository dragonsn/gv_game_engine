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


// Local functions prototype.
void DisplayHierarchy(FbxNode* pNode, int pDepth);

void DisplayHierarchy(FbxScene* pScene)
{
 //   int i;
    FbxNode* lRootNode = pScene->GetRootNode();
	DisplayHierarchy(lRootNode, 0);
/*
	gvt_xml_write_element_open	(the_xml, "node",false); 
	gvt_xml_write_attribute		(the_xml, "name", "root" ); 
	gvt_xml_write_attribute		(the_xml, "child_number", lRootNode->GetChildCount() ); 
	gvt_xml_write_element_open	(the_xml); 

    for( i = 0; i < lRootNode->GetChildCount(); i++) 
    {
        DisplayHierarchy(lRootNode->GetChild(i), 0);
    }
	gvt_xml_write_element_close (the_xml,"node" );
	*/
}

void DisplayHierarchy(FbxNode* pNode, int pDepth)
{
FbxNodeAttribute::EType lAttributeType;
	gvt_xml_write_element_open	(the_xml, "node",false); 
	gvt_xml_write_attribute		(the_xml, "name", pNode->GetName() ); 
	gvt_xml_write_attribute		(the_xml, "child_number", pNode->GetChildCount() ); 
	gvt_xml_write_attribute		(the_xml, "key", (gv_int_ptr) pNode ); 
	gvt_xml_write_element_open	(the_xml); 

	if(pNode->GetNodeAttribute() == NULL)
	{
		gvt_xml_write_element_open	(the_xml, "content",false); 
		gvt_xml_write_attribute		(the_xml, "type", "null"); 
		gvt_xml_write_element_open	(the_xml); 
	}
	else
	{
		gvt_xml_write_element_open	(the_xml, "content",false); 
		lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());

		switch (lAttributeType)
		{
		case FbxNodeAttribute::eMarker:  
			gvt_xml_write_attribute		(the_xml, "type", "eMarker"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayMarker(pNode);
			break;

		case FbxNodeAttribute::eSkeleton:  
			gvt_xml_write_attribute		(the_xml, "type", "eSkeleton"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplaySkeleton(pNode);
			break;

		case FbxNodeAttribute::eMesh:      
			gvt_xml_write_attribute		(the_xml, "type", "eMesh"); 
			gvt_xml_write_element_open	(the_xml);
			DisplayMesh(pNode);
			break;

		case FbxNodeAttribute::eNurbs:      
			gvt_xml_write_attribute		(the_xml, "type", "eNurbs"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayNurb(pNode);
			break;

		case FbxNodeAttribute::ePatch:     
			gvt_xml_write_attribute		(the_xml, "type", "ePatch"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayPatch(pNode);
			break;

		case FbxNodeAttribute::eCamera:    
			gvt_xml_write_attribute		(the_xml, "type", "eCamera"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayCamera(pNode);
			break;

		case FbxNodeAttribute::eLight:     
			gvt_xml_write_attribute		(the_xml, "type", "eLight"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayLight(pNode);
			break;

		case FbxNodeAttribute::eLODGroup:
			gvt_xml_write_attribute		(the_xml, "type", "eLODGroup"); 
			gvt_xml_write_element_open	(the_xml);
			//DisplayLodGroup(pNode);
			break;

		default:
			gvt_xml_write_attribute		(the_xml, "type", "null"); 
			gvt_xml_write_element_open	(the_xml); 
		}   
	}
	gvt_xml_write_element_close (the_xml,"content");

    for(int i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayHierarchy(pNode->GetChild(i), pDepth + 1);
    }

	gvt_xml_write_element_close (the_xml,"node");
}


