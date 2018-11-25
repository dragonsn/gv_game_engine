#pragma once

// window typedef
// has to define to use xnamath.h

typedef float FLOAT;
typedef unsigned int UINT;
typedef int INT;
typedef unsigned short USHORT;
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
typedef unsigned long DWORD;
typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef signed char INT8, *PINT8;
typedef signed short INT16, *PINT16;
typedef signed int INT32, *PINT32;
typedef signed __int64 INT64, *PINT64;
typedef unsigned char UINT8, *PUINT8;
typedef unsigned short UINT16, *PUINT16;
typedef unsigned int UINT32, *PUINT32;
typedef unsigned __int64 UINT64, *PUINT64;

typedef _W64 int INT_PTR, *PINT_PTR;
typedef _W64 unsigned int UINT_PTR, *PUINT_PTR;
typedef _W64 long LONG_PTR, *PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, *PULONG_PTR;

#ifndef CONST
#define CONST const
#endif

#ifndef VOID
#define VOID void
#endif

#ifndef DbgPrint
#define DbgPrint(...)
#endif

#ifndef __int3264
#define __int3264 __int32
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#define NO_OUTPUT_DEBUG_STRING

#include "xna/win32/xnamath.h"

namespace gv
{

/****************************************************************************
 *
 * Matrix operations
 *
 ****************************************************************************
BOOL            XMMatrixIsNaN(CXMMATRIX M);
BOOL            XMMatrixIsInfinite(CXMMATRIX M);
BOOL            XMMatrixIsIdentity(CXMMATRIX M);

XMMATRIX        XMMatrixMultiply(CXMMATRIX M1, CXMMATRIX M2);
XMMATRIX        XMMatrixMultiplyTranspose(CXMMATRIX M1, CXMMATRIX M2);
XMMATRIX        XMMatrixTranspose(CXMMATRIX M);
XMMATRIX        XMMatrixInverse(__out XMVECTOR* pDeterminant, CXMMATRIX M);
XMVECTOR        XMMatrixDeterminant(CXMMATRIX M);
BOOL            XMMatrixDecompose(__out XMVECTOR *outScale, __out XMVECTOR
*outRotQuat, __out XMVECTOR *outTrans, CXMMATRIX M);

XMMATRIX        XMMatrixIdentity();
XMMATRIX        XMMatrixSet(FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
                                                 FLOAT m10, FLOAT m11, FLOAT
m12, FLOAT m13,
                                                 FLOAT m20, FLOAT m21, FLOAT
m22, FLOAT m23,
                                                 FLOAT m30, FLOAT m31, FLOAT
m32, FLOAT m33);
XMMATRIX        XMMatrixTranslation(FLOAT OffsetX, FLOAT OffsetY, FLOAT
OffsetZ);
XMMATRIX        XMMatrixTranslationFromVector(FXMVECTOR Offset);
XMMATRIX        XMMatrixScaling(FLOAT ScaleX, FLOAT ScaleY, FLOAT ScaleZ);
XMMATRIX        XMMatrixScalingFromVector(FXMVECTOR Scale);
XMMATRIX        XMMatrixRotationX(FLOAT Angle);
XMMATRIX        XMMatrixRotationY(FLOAT Angle);
XMMATRIX        XMMatrixRotationZ(FLOAT Angle);
XMMATRIX        XMMatrixRotationRollPitchYaw(FLOAT Pitch, FLOAT Yaw, FLOAT
Roll);
XMMATRIX        XMMatrixRotationRollPitchYawFromVector(FXMVECTOR Angles);
XMMATRIX        XMMatrixRotationNormal(FXMVECTOR NormalAxis, FLOAT Angle);
XMMATRIX        XMMatrixRotationAxis(FXMVECTOR Axis, FLOAT Angle);
XMMATRIX        XMMatrixRotationQuaternion(FXMVECTOR Quaternion);
XMMATRIX        XMMatrixTransformation2D(FXMVECTOR ScalingOrigin, FLOAT
ScalingOrientation, FXMVECTOR Scaling,
                                        FXMVECTOR RotationOrigin, FLOAT
Rotation, CXMVECTOR Translation);
XMMATRIX        XMMatrixTransformation(FXMVECTOR ScalingOrigin, FXMVECTOR
ScalingOrientationQuaternion, FXMVECTOR Scaling,
                                        CXMVECTOR RotationOrigin, CXMVECTOR
RotationQuaternion, CXMVECTOR Translation);
XMMATRIX        XMMatrixAffineTransformation2D(FXMVECTOR Scaling, FXMVECTOR
RotationOrigin, FLOAT Rotation, FXMVECTOR Translation);
XMMATRIX        XMMatrixAffineTransformation(FXMVECTOR Scaling, FXMVECTOR
RotationOrigin, FXMVECTOR RotationQuaternion, CXMVECTOR Translation);
XMMATRIX        XMMatrixReflect(FXMVECTOR ReflectionPlane);
XMMATRIX        XMMatrixShadow(FXMVECTOR ShadowPlane, FXMVECTOR LightPosition);

XMMATRIX        XMMatrixLookAtLH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition,
FXMVECTOR UpDirection);
XMMATRIX        XMMatrixLookAtRH(FXMVECTOR EyePosition, FXMVECTOR FocusPosition,
FXMVECTOR UpDirection);
XMMATRIX        XMMatrixLookToLH(FXMVECTOR EyePosition, FXMVECTOR EyeDirection,
FXMVECTOR UpDirection);
XMMATRIX        XMMatrixLookToRH(FXMVECTOR EyePosition, FXMVECTOR EyeDirection,
FXMVECTOR UpDirection);
XMMATRIX        XMMatrixPerspectiveLH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT
NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveRH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT
NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveFovLH(FLOAT FovAngleY, FLOAT AspectHByW,
FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveFovRH(FLOAT FovAngleY, FLOAT AspectHByW,
FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveOffCenterLH(FLOAT ViewLeft, FLOAT ViewRight,
FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixPerspectiveOffCenterRH(FLOAT ViewLeft, FLOAT ViewRight,
FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicLH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT
NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicRH(FLOAT ViewWidth, FLOAT ViewHeight, FLOAT
NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicOffCenterLH(FLOAT ViewLeft, FLOAT ViewRight,
FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);
XMMATRIX        XMMatrixOrthographicOffCenterRH(FLOAT ViewLeft, FLOAT ViewRight,
FLOAT ViewBottom, FLOAT ViewTop, FLOAT NearZ, FLOAT FarZ);

XMMATRIX        XMLoadFloat3x3(__in CONST XMFLOAT3X3* pSource);
XMMATRIX        XMLoadFloat4x3(__in CONST XMFLOAT4X3* pSource);
XMMATRIX        XMLoadFloat4x3A(__in CONST XMFLOAT4X3A* pSource);
XMMATRIX        XMLoadFloat4x4(__in CONST XMFLOAT4X4* pSource);
XMMATRIX        XMLoadFloat4x4A(__in CONST XMFLOAT4X4A* pSource);*/

class gv_matrix44_xna
{
public:
	union {
		struct
		{
			gv_matrix44 mat_gv;
		};
		struct
		{
			XMMATRIX mat_xna;
		};
	};
	gv_matrix44_xna()
	{
	}
	// XMMatrixMultiply
	inline gv_matrix44_xna operator*(const gv_matrix44_xna& m) const
	{
		gv_matrix44_xna ret;
		ret.mat_xna = XMMatrixMultiply(mat_xna, m.mat_xna);
		return ret;
	}
};
}