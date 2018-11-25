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


void DisplayUserProperties(FbxObject* pObject)
{
    int lCount = 0;
    FbxString lTitleStr = "    Property Count: ";

    FbxProperty lProperty = pObject->GetFirstProperty();
    while (lProperty.IsValid())
    {
        if (lProperty.GetFlag(FbxProperty::eUSER))
            lCount++;

        lProperty = pObject->GetNextProperty(lProperty);
    }

    if (lCount == 0)
        return; // there are no user properties to display

    DisplayInt(lTitleStr.Buffer(), lCount);

    lProperty = pObject->GetFirstProperty();
    int i = 0;
    while (lProperty.IsValid())
    {
        if (lProperty.GetFlag(FbxProperty::eUSER))
        {
            DisplayInt("        Property ", i);
            FbxString lString = lProperty.GetLabel();
            DisplayString("            Display Name: ", lString.Buffer());
            lString = lProperty.GetName();
            DisplayString("            Internal Name: ", lString.Buffer());
            DisplayString("            Type: ", lProperty.GetPropertyDataType().GetName());
            if (lProperty.HasMinLimit()) DisplayDouble("            Min Limit: ", lProperty.GetMinLimit());
            if (lProperty.HasMaxLimit()) DisplayDouble("            Max Limit: ", lProperty.GetMaxLimit());
            DisplayBool  ("            Is Animatable: ", lProperty.GetFlag(FbxProperty::eANIMATABLE));
            DisplayBool  ("            Is Temporary: ", lProperty.GetFlag(FbxProperty::eTEMPORARY));
            
			FbxDataType lPropertyDataType=lProperty.GetPropertyDataType();

			// BOOL
			if (lPropertyDataType.GetType() == eBOOL1)
            {
                DisplayBool("            Default Value: ", FbxGet<bool>(lProperty));
			}
			// REAL
			else if (lPropertyDataType.GetType() == eDOUBLE1 || lPropertyDataType.GetType() == eFLOAT1)
			{
                DisplayDouble("            Default Value: ",FbxGet<double>(lProperty));
			}
			// COLOR
			else if (lPropertyDataType.Is(DTColor3) || lPropertyDataType.Is(DTColor4))
            {
				FbxColor lDefault;
                char      lBuf[64];

                lDefault=FbxGet <FbxColor> (lProperty);
                sprintf(lBuf, "R=%f, G=%f, B=%f, A=%f", lDefault.mRed, lDefault.mGreen, lDefault.mBlue, lDefault.mAlpha);
                DisplayString("            Default Value: ", lBuf);
            }
			// INTEGER
			else if (lPropertyDataType.GetType() == eINTEGER1)
			{
                DisplayInt("            Default Value: ", FbxGet <int> (lProperty));
			}
			// VECTOR
			else if(lPropertyDataType.GetType() == eDOUBLE3 || lPropertyDataType.GetType() == eDOUBLE4)
			{
				fbxDouble3 lDefault;
                char   lBuf[64];

                lDefault = FbxGet <fbxDouble3> (lProperty);
                sprintf(lBuf, "X=%f, Y=%f, Z=%f", lDefault[0], lDefault[1], lDefault[2]);
                DisplayString("            Default Value: ", lBuf);
            }
			// LIST
			else if (lPropertyDataType.GetType() == eENUM)
			{
                DisplayInt("            Default Value: ", FbxGet <int> (lProperty));
			}
			// UNIDENTIFIED
            else
			{
                DisplayString("            Default Value: UNIDENTIFIED");
            }
            i++;
        }

        lProperty = pObject->GetNextProperty(lProperty);
    }
}

