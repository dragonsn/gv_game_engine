/**************************************************************************************

 Copyright (C) 2010 Autodesk, Inc. and/or its licensors.
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

#include "GeometryUtility.h"

FbxNode * CreatePyramid(FbxScene * pScene, const char * pName, double pBottomWidth, double pHeight)
{
    FbxMesh * lPyramid = FbxMesh::Create(pScene, pName);

    // Calculate the vertices of the pyramid
    const double lBottomWidthHalf = pBottomWidth / 2;
    const FbxVector4 PyramidControlPointArray[] = 
    {
        FbxVector4(0, pHeight, 0),
        FbxVector4(lBottomWidthHalf, 0, lBottomWidthHalf),
        FbxVector4(lBottomWidthHalf, 0, -lBottomWidthHalf),
        FbxVector4(-lBottomWidthHalf, 0, -lBottomWidthHalf),
        FbxVector4(-lBottomWidthHalf, 0, lBottomWidthHalf)
    };

    // Initialize and set the control points of the mesh
    const int lControlPointCount = sizeof(PyramidControlPointArray) / sizeof(FbxVector4);
    lPyramid->InitControlPoints(lControlPointCount);
    for (int lIndex = 0; lIndex < lControlPointCount; ++lIndex)
    {
        lPyramid->SetControlPointAt(PyramidControlPointArray[lIndex], lIndex);
    }

    // Set the control point indices of the bottom side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the front side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(1);
    lPyramid->AddPolygon(2);
    lPyramid->EndPolygon();

    // Set the control point indices of the left side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(2);
    lPyramid->AddPolygon(3);
    lPyramid->EndPolygon();

    // Set the control point indices of the back side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(3);
    lPyramid->AddPolygon(4);
    lPyramid->EndPolygon();

    // Set the control point indices of the right side of the pyramid
    lPyramid->BeginPolygon();
    lPyramid->AddPolygon(0);
    lPyramid->AddPolygon(4);
    lPyramid->AddPolygon(1);
    lPyramid->EndPolygon();

    // Attach the mesh to a node
    FbxNode * lPyramidNode = FbxNode::Create(pScene, pName);
    lPyramidNode->SetNodeAttribute(lPyramid);

    // Set this node as a child of the root node
    pScene->GetRootNode()->AddChild(lPyramidNode);

    return lPyramidNode;
}
