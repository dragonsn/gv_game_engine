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

void DisplayDefaultAnimationValues(FbxLight* pLight);


void DisplayLight(FbxNode* pNode)
{
    FbxLight* lLight = (FbxLight*) pNode->GetNodeAttribute();

    DisplayString("Light Name: ", (char *) pNode->GetName());
    DisplayMetaDataConnections(lLight);

    char* lLightTypes[] = { "Point", "Directional", "Spot" };

    DisplayString("    Type: ", lLightTypes[lLight->LightType.Get()]);
    DisplayBool("    Cast Light: ", lLight->CastLight.Get());

    if (!(lLight->FileName.Get().IsEmpty()))
    {
        DisplayString("    Gobo");

        DisplayString("        File Name: \"", lLight->FileName.Get().Buffer(), "\"");
        DisplayBool("        Ground Projection: ", lLight->DrawGroundProjection.Get());
        DisplayBool("        Volumetric Projection: ", lLight->DrawVolumetricLight.Get());
        DisplayBool("        Front Volumetric Projection: ", lLight->DrawFrontFacingVolumetricLight.Get());
    }

    DisplayDefaultAnimationValues(lLight);
}


void DisplayDefaultAnimationValues(FbxLight* pLight)
{
    DisplayString("    Default Animation Values");

    fbxDouble3 c = pLight->Color.Get();
    FbxColor lColor(c[0], c[1], c[2]);
    DisplayColor("        Default Color: ", lColor);
    DisplayDouble("        Default Intensity: ", pLight->Intensity.Get());
    DisplayDouble("        Default Cone Angle: ", pLight->ConeAngle.Get());
    DisplayDouble("        Default Fog: ", pLight->Fog.Get());
}


