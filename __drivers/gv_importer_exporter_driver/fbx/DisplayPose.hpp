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


void DisplayPose(FbxScene* pScene)
{
    int      i,j,k,lPoseCount;
    FbxString  lName;

    lPoseCount = pScene->GetPoseCount();

    for (i = 0; i < lPoseCount; i++)
    {
        FbxPose* lPose = pScene->GetPose(i);

        lName = lPose->GetName();
        DisplayString("Pose Name: ", lName.Buffer());

        DisplayBool("    Is a bind pose: ", lPose->IsBindPose());

        DisplayInt("    Number of items in the pose: ", lPose->GetCount());

        DisplayString("","");

        for (j=0; j<lPose->GetCount(); j++)
        {
            lName = lPose->GetNodeName(j).GetCurrentName();
            DisplayString("    Item name: ", lName.Buffer());

            if (!lPose->IsBindPose())
            {
                // Rest pose can have local matrix
                DisplayBool("    Is local space matrix: ", lPose->IsLocalMatrix(j));
            }

            DisplayString("    Matrix value: ","");

            FbxString lMatrixValue;

            for (k=0; k<4; k++)
            {
                FbxMatrix  lMatrix = lPose->GetMatrix(j);
                FbxVector4 lRow = lMatrix.GetRow(k);
                char        lRowValue[1024];

                sprintf(lRowValue, "%9.4f %9.4f %9.4f %9.4f\n", lRow[0], lRow[1], lRow[2], lRow[3]);
                lMatrixValue += FbxString("        ") + FbxString(lRowValue);
            }

            DisplayString("", lMatrixValue.Buffer());
        }
    }

    lPoseCount = pScene->GetCharacterPoseCount();

    for (i = 0; i < lPoseCount; i++)
    {
        FbxCharacterPose* lPose = pScene->GetCharacterPose(i);
        FbxCharacter*     lCharacter = lPose->GetCharacter();

        if (!lCharacter) break;

        DisplayString("Character Pose Name: ", lCharacter->mName.Buffer());

        FbxCharacterLink lCharacterLink;
        ECharacterNodeId  lNodeId = eCharacterHips;

        while (lCharacter->GetCharacterLink(lNodeId, &lCharacterLink))
        {
            FbxAMatrix& lGlobalPosition = pScene->GetEvaluator()->GetNodeGlobalTransform(lCharacterLink.mNode, KTIME_ZERO);

            DisplayString("    Matrix value: ","");

            FbxString lMatrixValue;

            for (k=0; k<4; k++)
            {
                FbxVector4 lRow = lGlobalPosition.GetRow(k);
                char        lRowValue[1024];

                sprintf(lRowValue, "%9.4f %9.4f %9.4f %9.4f\n", lRow[0], lRow[1], lRow[2], lRow[3]);
                lMatrixValue += FbxString("        ") + FbxString(lRowValue);
            }

            DisplayString("", lMatrixValue.Buffer());

            lNodeId = ECharacterNodeId(int(lNodeId) + 1);
        }
    }
}


