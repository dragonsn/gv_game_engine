#ifndef _COMMON_H
#define _COMMON_H

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


void InitializeSdkObjects(FbxManager*& pSdkManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pSdkManager);
void CreateAndFillIOSettings(FbxManager* pSdkManager);

bool SaveScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename, int pFileFormat=-1, bool pEmbedMedia=false);
bool LoadScene(FbxManager* pSdkManager, FbxDocument* pScene, const char* pFilename);


typedef FbxPropertyT<FbxBool> FbxPropertyBool1;
typedef FbxPropertyT<FbxInt> FbxPropertyInteger1;
typedef FbxPropertyT<FbxFloat> FbxPropertyFloat1;
typedef FbxPropertyT<FbxDouble> FbxPropertyDouble1;
typedef FbxPropertyT<FbxDouble2> FbxPropertyDouble2;
typedef FbxPropertyT<FbxDouble3> FbxPropertyDouble3;
typedef FbxPropertyT<FbxDouble4> FbxPropertyDouble4;
typedef FbxPropertyT<FbxAMatrix> FbxPropertyXMatrix;
typedef FbxPropertyT<FbxTime> FbxPropertyFbxTime;
typedef FbxPropertyT<FbxDateTime> FbxPropertyDateTime;
typedef FbxPropertyT<FbxEnum> FbxPropertyEnum;
typedef FbxPropertyT<FbxString> FbxPropertyString;
typedef FbxPropertyT<EFbxRotationOrder> FbxPropertyEFbxRotationOrder;
typedef FbxPropertyT<FbxTransform::EInheritType> FbxPropertyFbxTransformEInheritType;
typedef FbxPropertyT<FbxReference> FbxPropertyFbxReference;
typedef FbxPropertyT<FbxBlob> FbxPropertyBlob;
typedef FbxPropertyT<EFbxQuatInterpMode> FbxPropertyEFbxQuatInterpMode;

typedef FbxPropertyT<FbxCamera::EAspectRatioMode> FbxPropertyEAspectRatioMode;
typedef FbxPropertyT<FbxCamera::EApertureMode> FbxPropertyEApertureMode;
typedef FbxPropertyT<FbxCamera::EGateFit> FbxPropertyEGateFit;
typedef FbxPropertyT<FbxCamera::EFormat> FbxPropertyEFormat;
typedef FbxPropertyT<FbxCamera::EApertureFormat> FbxPropertyEApertureFormat;
typedef FbxPropertyT<FbxCamera::EFilmRollOrder> FbxPropertyEFilmRollOrder;
typedef FbxPropertyT<FbxCamera::EFrontBackPlaneDisplayMode> FbxPropertyEFrontBackPlaneDisplayMode;
typedef FbxPropertyT<FbxCamera::EFrontBackPlaneDistanceMode> FbxPropertyEFrontBackPlaneDistanceMode;
typedef FbxPropertyT<FbxCamera::ESafeAreaStyle> FbxPropertyESafeAreaStyle;
typedef FbxPropertyT<FbxCamera::EProjectionType> FbxPropertyEProjectionType;
typedef FbxPropertyT<FbxCamera::EFocusDistanceSource> FbxPropertyEFocusDistanceSource;
typedef FbxPropertyT<FbxCamera::EAntialiasingMethod> FbxPropertyEAntialiasingMethod;
typedef FbxPropertyT<FbxCamera::ESamplingType> FbxPropertyESamplingType;

typedef FbxPropertyT<FbxCameraStereo::EStereoType> FbxPropertyFbxCameraStereoEStereoType;

typedef FbxPropertyT<FbxCharacter::EOffAutoUser> FbxPropertyFbxCharacterEOffAutoUser;
typedef FbxPropertyT<FbxCharacter::EPostureMode> FbxPropertyFbxCharacterEPostureMode;
typedef FbxPropertyT<FbxCharacter::EAutoUser> FbxPropertyFbxCharacterEAutoUser;
typedef FbxPropertyT<FbxCharacter::EFloorPivot> FbxPropertyFbxCharacterEFloorPivot;
typedef FbxPropertyT<FbxCharacter::ERollExtractionMode> FbxPropertyFbxCharacterERollExtractionMode;
typedef FbxPropertyT<FbxCharacter::EHipsTranslationMode> FbxPropertyFbxCharacterEHipsTranslationMode;
typedef FbxPropertyT<FbxCharacter::EFootContactType> FbxPropertyFbxCharacterEFootContactType;
typedef FbxPropertyT<FbxCharacter::EHandContactType> FbxPropertyFbxCharacterEHandContactType;
typedef FbxPropertyT<FbxCharacter::EFingerContactMode> FbxPropertyFbxCharacterEFingerContactMode;
typedef FbxPropertyT<FbxCharacter::EContactBehaviour> FbxPropertyFbxCharacterEContactBehaviour;

typedef FbxPropertyT<FbxTexture::ETextureUse6> FbxPropertyETextureUse6;
typedef FbxPropertyT<FbxTexture::EUnifiedMappingType> FbxPropertyEUnifiedMappingType;
typedef FbxPropertyT<FbxTexture::EWrapMode> FbxPropertyEWrapMode;
typedef FbxPropertyT<FbxTexture::EBlendMode> FbxPropertyEBlendMode;

typedef FbxPropertyT<FbxNull::ELook> FbxPropertyFbxNullELook;

typedef FbxPropertyT<FbxLight::EType> FbxPropertyEType;
typedef FbxPropertyT<FbxLight::EDecayType> FbxPropertyEDecayType;
typedef FbxPropertyT<FbxLight::EAreaLightShape> FbxPropertyEAreaLightShape;

typedef FbxPropertyT<FbxMarker::ELook> FbxPropertyELook;

typedef FbxPropertyT<FbxVideo::EAccessMode> FbxPropertyFbxVideoEAccessMode;
typedef FbxPropertyT<FbxVideo::EInterlaceMode> FbxPropertyFbxVideoEInterlaceMode;

typedef FbxPropertyT<FbxControlSet::EType> FbxPropertyFbxControlSetEType;

#endif // #ifndef _COMMON_H


