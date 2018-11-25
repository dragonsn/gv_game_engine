
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


void DisplayProperties(FbxObject* pObject);
void DisplayGenericInfo(FbxNode* pNode, int pDepth);
void DisplayGenericInfo(FbxScene* pScene)
{
    int i;
    FbxNode* lRootNode = pScene->GetRootNode();

    for( i = 0; i < lRootNode->GetChildCount(); i++)
    {
        DisplayGenericInfo(lRootNode->GetChild(i), 0);
    }

	//Other objects directly connected onto the scene
	for( i = 0; i < pScene->GetSrcObjectCount(FBX_TYPE(FbxObject)); ++i )
	{
		DisplayProperties(pScene->GetSrcObject(FBX_TYPE(FbxObject), i));
	}
}


void DisplayGenericInfo(FbxNode* pNode, int pDepth)
{
    FbxString lString;
    int i;

    for(i = 0; i < pDepth; i++)
    {
        lString += "     ";
    }

    lString += pNode->GetName();
    lString += "\n";

    DisplayString(lString.Buffer());

    //Display generic info about that Node
    DisplayProperties(pNode);
    DisplayString("");
    for(i = 0; i < pNode->GetChildCount(); i++)
    {
        DisplayGenericInfo(pNode->GetChild(i), pDepth + 1);
    }
}

void DisplayProperties(FbxObject* pObject)
{

    DisplayString("Name: ", (char *)pObject->GetName());

    // Display all the properties
    int i,  lCount = 0;
    FbxProperty lProperty = pObject->GetFirstProperty();
    while (lProperty.IsValid())
    {
        lCount++;
        lProperty = pObject->GetNextProperty(lProperty);
    }

    FbxString lTitleStr = "    Property Count: ";

    if (lCount == 0)
        return; // there are no properties to display

    DisplayInt(lTitleStr.Buffer(), lCount);

    i=0;
	lProperty = pObject->GetFirstProperty();
    while (lProperty.IsValid())
    {
        // exclude user properties

        FbxString lString;
        DisplayInt("        Property ", i);
        lString = lProperty.GetLabel();
        DisplayString("            Display Name: ", lString.Buffer());
        lString = lProperty.GetName();
        DisplayString("            Internal Name: ", lString.Buffer());
        lString = lProperty.GetPropertyDataType().GetName();
        DisplayString("            Type: ",lString.Buffer());
        if (lProperty.HasMinLimit()) DisplayDouble("            Min Limit: ", lProperty.GetMinLimit());
        if (lProperty.HasMaxLimit()) DisplayDouble("            Max Limit: ", lProperty.GetMaxLimit());
        DisplayBool  ("            Is Animatable: ", lProperty.GetFlag(FbxProperty::eANIMATABLE));
        DisplayBool  ("            Is Temporary: ", lProperty.GetFlag(FbxProperty::eTEMPORARY));


        switch (lProperty.GetPropertyDataType().GetType())
        {
        case eBOOL1:
            DisplayBool("            Default Value: ", FbxGet<bool>(lProperty));
            break;

        case eDOUBLE1:
            DisplayDouble("            Default Value: ",FbxGet<double>(lProperty));
            break;

        case eDOUBLE4:
            {
                FbxColor lDefault;
                char      lBuf[64];

                lDefault=FbxGet <FbxColor> (lProperty);
                sprintf(lBuf, "R=%f, G=%f, B=%f, A=%f", lDefault.mRed, lDefault.mGreen, lDefault.mBlue, lDefault.mAlpha);
                DisplayString("            Default Value: ", lBuf);
            }
            break;

        case eINTEGER1:
            DisplayInt("            Default Value: ", FbxGet <int> (lProperty));
            break;

        case eDOUBLE3:
            {
                fbxDouble3 lDefault;
                char   lBuf[64];

                lDefault = FbxGet <fbxDouble3> (lProperty);
                sprintf(lBuf, "X=%f, Y=%f, Z=%f", lDefault[0], lDefault[1], lDefault[2]);
                DisplayString("            Default Value: ", lBuf);
            }
            break;

        //case DTEnum:
        //    DisplayInt("            Default Value: ", FbxGet <int> (lProperty));
        //    break;

        case eFLOAT1:
            DisplayDouble("            Default Value: ", FbxGet <float> (lProperty));
            break;
        case eSTRING:
            lString = (FbxGet <FbxString> (lProperty));
            DisplayString("            Default Value: ", lString.Buffer());
            break;

        default:
            DisplayString("            Default Value: UNIDENTIFIED");
            break;
        }
        i++;
        lProperty = pObject->GetNextProperty(lProperty);
    }
}

