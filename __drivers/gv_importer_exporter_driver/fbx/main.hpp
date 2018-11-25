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

/////////////////////////////////////////////////////////////////////////
//
// This example illustrates how to detect if a scene is password 
// protected, import and browse the scene to access node and animation 
// information. It displays the content of the FBX file which name is 
// passed as program argument. You can try it with the various FBX files 
// output by the export examples.
//
/////////////////////////////////////////////////////////////////////////



// Local function prototypes.


int exp_main(const char* file_name)
{
    FbxManager* lSdkManager = NULL;
    FbxScene* lScene = NULL;
    bool lResult;
	gScene=lScene;

    // Prepare the FBX SDK.
    InitializeSdkObjects(lSdkManager, lScene);
    // Load the scene.
	gSdkManager=lSdkManager;
	gScene=lScene;

    // The example can take a FBX file as an argument.
	 gvt_xml_write_element_open ( the_xml,"fbx_file"  ); 

     lResult = LoadScene(lSdkManager, lScene, file_name);

    if(lResult == false)
    {
        printf("\n\nAn error occurred while loading the scene...");
        return -1;
    }
    else 
    {
        // Display the scene.
		
        //DisplayMetaData(lScene);
		/*

        printf("\n\n---------------------\nGlobal Light Settings\n---------------------\n\n");

        DisplayGlobalLightSettings(&lScene->GetGlobalSettings());

        printf("\n\n----------------------\nGlobal Camera Settings\n----------------------\n\n");

        DisplayGlobalCameraSettings(&lScene->GetGlobalSettings());
		*/
        DisplayGlobalTimeSettings(&lScene->GetGlobalSettings());

  
        DisplayHierarchy(lScene);
		/*

        printf("\n\n------------\nNode Content\n------------\n\n");

        DisplayContent(lScene);

        printf("\n\n----\nPose\n----\n\n");

        DisplayPose(lScene);

        printf("\n\n---------\nAnimation\n---------\n\n");*/

        DisplayAnimation(lScene);

        /*//now display generic information

        printf("\n\n---------\nGeneric Information\n---------\n\n");
        DisplayGenericInfo(lScene);
		*/

		gvt_xml_write_element_close( the_xml,"fbx_file" ); 
    }

    // Destroy all objects created by the FBX SDK.
	GVM_INFO_LOG(the_xml);
    DestroySdkObjects(lSdkManager);

    return 0;
}

